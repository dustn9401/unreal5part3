// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ABFountain.generated.h"

UCLASS()
class ARENABATTLESAMPLE_API AABFountain : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AABFountain();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Mesh)
	TObjectPtr<class UStaticMeshComponent> Body;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Mesh)
	TObjectPtr<class UStaticMeshComponent> Water;

public:
	UPROPERTY(ReplicatedUsing=OnRep_ServerRotationYaw)
	float ServerRotationYaw;

	UFUNCTION()
	void OnRep_ServerRotationYaw();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void OnActorChannelOpen(FInBunch& InBunch, UNetConnection* Connection) override;

	float RotationRate = 30.0f;
	float ClientTimeSinceUpdate = 0.0f;		// 서버로부터 마지막으로 업데이트 받은 뒤에 얼마만큼의 시간이 지났는지
	float ClientTimeBetweenLastUpdate = 0.0f;	// 마지막으로 업데이트 받았을 때의 인터벌 캐싱용
};
