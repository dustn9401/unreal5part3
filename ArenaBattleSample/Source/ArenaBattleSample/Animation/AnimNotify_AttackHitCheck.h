// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_AttackHitCheck.generated.h"

/**
 * 애니메이터 -> 코드 이벤트 전용 클래스
 */
UCLASS()
class ARENABATTLESAMPLE_API UAnimNotify_AttackHitCheck : public UAnimNotify
{
	GENERATED_BODY()

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
