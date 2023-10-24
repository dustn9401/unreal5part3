// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterNonPlayer.h"

#include "AI/ABAIController.h"
#include "Engine/AssetManager.h"

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
	throw "this function is not implemented";
}

float AABCharacterNonPlayer::GetAIDetectRange()
{
	throw "this function is not implemented";
}

float AABCharacterNonPlayer::GetAIAttackRange()
{
	throw "this function is not implemented";
}

float AABCharacterNonPlayer::GetAITurnSpeed()
{
	throw "this function is not implemented";
}
