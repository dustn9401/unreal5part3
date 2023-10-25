// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ABCharacterBase.h"
#include "Engine/StreamableManager.h"
#include "Interface/ABCharacterAIInterface.h"
#include "ABCharacterNonPlayer.generated.h"

/**
 * 
 */
UCLASS(Config=ArenaBattle)
class ARENABATTLESAMPLE_API AABCharacterNonPlayer : public AABCharacterBase, public IABCharacterAIInterface
{
	GENERATED_BODY()

public:
	AABCharacterNonPlayer();

protected:
	virtual void PostInitializeComponents() override;

protected:
	virtual void SetDead() override;
	
	void NPCMeshLoadCompleted();

	// 이 방식은 블루프린트를 생성해 추가하거나, 코드로 배열을 채워줘야 함
	// UPROPERTY()
	// TArray<TSoftObjectPtr<USkeletalMesh>> NPCMeshes;

	// 대신에 이렇게 하면 텍스트 파일로 배열값을 관리할 수 있음
	UPROPERTY(Config)
	TArray<FSoftObjectPath> NPCMeshes;

	TSharedPtr<FStreamableHandle> NPCMeshHandle;	// 메쉬 비동기 로드용 변수

// IABCharacterAIInterface impl
public:
	virtual float GetAIPatrolRadius() override;
	virtual float GetAIDetectRange() override;
	virtual float GetAIAttackRange() override;
	virtual float GetAITurnSpeed() override;

	FAICharacterAttackFinished OnAttackFinished;
	virtual void AttackByAI() override;
	virtual void SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished) override;

// AABCharacterBase override
protected:
	virtual void NotifyComboActionEnd() override;
	
	virtual bool CanHit(const FHitResult& HitResult) override;
};
