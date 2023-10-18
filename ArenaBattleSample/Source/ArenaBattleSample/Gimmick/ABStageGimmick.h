// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ABStageGimmick.generated.h"

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
};
