// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ABItemBox.generated.h"

UCLASS()
class ARENABATTLESAMPLE_API AABItemBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AABItemBox();

protected:
	UPROPERTY(EditAnywhere, Category=Item)
	TObjectPtr<class UABItemData> ItemData;
	
	UPROPERTY(VisibleAnywhere, Category=Box)
	TObjectPtr<class UBoxComponent> Trigger;	// 루트컴포넌트

	UPROPERTY(VisibleAnywhere, Category=Box)
	TObjectPtr<class UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, Category=Effect)
	TObjectPtr<class UParticleSystemComponent> Effect;	// 획득 시 출력할 이펙트

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult);

	UFUNCTION()
	void OnEffectFinished(class UParticleSystemComponent* ParticleSystem);
};