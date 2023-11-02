// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterNonPlayer.h"

#include "AI/ABAIController.h"
#include "Engine/AssetManager.h"
#include "Physics/ABCollision.h"
#include "Player/ABCharacterPlayer.h"

AABCharacterNonPlayer::AABCharacterNonPlayer()
{
	GetMesh()->SetHiddenInGame(true);
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AABAIController::StaticClass();
}

static int GlobalLoadIndex = 0;

void AABCharacterNonPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	ensure(NPCMeshes.Num() > 0);
	
	NPCMeshHandle = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(NPCMeshes[GlobalLoadIndex], FStreamableDelegate::CreateUObject(this, &AABCharacterNonPlayer::NPCMeshLoadCompleted));
	GlobalLoadIndex = (GlobalLoadIndex + 1) % NPCMeshes.Num();
}


void AABCharacterNonPlayer::SetDead()
{
	Super::SetDead();

	if (AABAIController* ABAIController = Cast<AABAIController>(GetController()))
	{
		ABAIController->StopAI();
	}

	FTimerHandle DeadTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda(
		[&]()
		{
			Destroy();
		}
		), DeadEventDelayTime, false);
}

void AABCharacterNonPlayer::NPCMeshLoadCompleted()
{
	if (NPCMeshHandle.IsValid())
	{
		if (const auto NPCMesh = Cast<USkeletalMesh>(NPCMeshHandle->GetLoadedAsset()))
		{
			GetMesh()->SetSkeletalMesh(NPCMesh);
			GetMesh()->SetHiddenInGame(false);
		}
	}

	NPCMeshHandle->ReleaseHandle();
}

float AABCharacterNonPlayer::GetAIPatrolRadius()
{
	return 800.0f;
}

float AABCharacterNonPlayer::GetAIDetectRange()
{
	return 400.0f;
}

float AABCharacterNonPlayer::GetAIAttackRange()
{
	return Stat->GetTotalStat().AttackRange + Stat->GetAttackRadius() * 2.0f;
}

float AABCharacterNonPlayer::GetAITurnSpeed()
{
	return 0.0f;
}

void AABCharacterNonPlayer::AttackByAI()
{
	// ProcessComboCommand();

	Attack();
}

void AABCharacterNonPlayer::SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished)
{
	OnAttackFinished = InOnAttackFinished;
}

void AABCharacterNonPlayer::NotifyComboActionEnd()
{
	Super::NotifyComboActionEnd();
	
	// ReSharper disable once CppExpressionWithoutSideEffects
	OnAttackFinished.ExecuteIfBound();
}

bool AABCharacterNonPlayer::CanHit(const FHitResult& HitResult)
{
	AABCharacterPlayer* PlayerCharacter = Cast<AABCharacterPlayer>(HitResult.GetActor());
	return PlayerCharacter != nullptr;	// 플레이어 캐릭터만 공격 가능하다.
}
