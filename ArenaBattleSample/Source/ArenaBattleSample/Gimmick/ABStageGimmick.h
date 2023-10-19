// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ABStageGimmick.generated.h"

DECLARE_DELEGATE(FOnStageChangedDelegate);
USTRUCT(BlueprintType)
struct FStageChangedDelegateWrapper
{
	GENERATED_BODY()
	FStageChangedDelegateWrapper() {}
	FStageChangedDelegateWrapper(const FOnStageChangedDelegate& InDelegate) : StageDelegate(InDelegate) {}
	FOnStageChangedDelegate StageDelegate;
};

UENUM(BlueprintType)
enum class EStageState : uint8
{
	Ready=0,
	Fight,
	Reward,
	Next
};

UCLASS()
class ARENABATTLESAMPLE_API AABStageGimmick : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AABStageGimmick();

// Stage Section
protected:
	UPROPERTY(VisibleAnywhere, Category=Stage, meta=(AllowPrivateAccess="true"))
	TObjectPtr<class UStaticMeshComponent> StageMesh;

	UPROPERTY(VisibleAnywhere, Category=Stage, meta=(AllowPrivateAccess="true"))
	TObjectPtr<class UBoxComponent> StageTrigger;

	UFUNCTION()
	void OnStageTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult);

// Gate Section
protected:
	UPROPERTY(VisibleAnywhere, Category=Gate, meta=(AllowPrivateAccess="true"))
	TMap<FName, TObjectPtr<class UStaticMeshComponent>> Gates;
	
	UPROPERTY(VisibleAnywhere, Category=Gate, meta=(AllowPrivateAccess="true"))
	TArray<TObjectPtr<class UBoxComponent>> GateTriggers;

	UFUNCTION()
	void OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult);

	void OpenAllGates();
	void CloseAllGates();

// State Section
protected:
	UPROPERTY(EditAnywhere, Category=Stage, meta=(AllowPrivateAccess="true"))
	EStageState CurrentState;

	UPROPERTY()
	TMap<EStageState, FStageChangedDelegateWrapper> StateChangeActions;

	void SetState(EStageState InNewState);
	void SetReady();
	void SetFight();
	void SetChooseReward();
	void SetChooseNext();
};
