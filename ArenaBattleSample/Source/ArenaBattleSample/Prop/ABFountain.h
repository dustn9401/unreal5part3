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

	UPROPERTY(ReplicatedUsing=OnRep_ServerLightColor)
	FLinearColor ServerLightColor;

	// UPROPERTY(Replicated)
	// TArray<float> BigData;

	UFUNCTION()
	void OnRep_ServerRotationYaw();

	UFUNCTION()
	void OnRep_ServerLightColor();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void OnActorChannelOpen(FInBunch& InBunch, UNetConnection* Connection) override;

	float RotationRate = 30.0f;
	float ClientTimeSinceUpdate = 0.0f;		// 서버로부터 마지막으로 업데이트 받은 뒤에 얼마만큼의 시간이 지났는지
	float ClientTimeBetweenLastUpdate = 0.0f;	// 마지막으로 업데이트 받았을 때의 인터벌 캐싱용
	
	float BigDataElement = 0.0f;

	virtual bool IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const override;

	UFUNCTION(Client, Unreliable)
	void ClientRPCFunction(int32 IntParam);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCChangeLightColor(const FLinearColor& NewLightColor);

	UFUNCTION(Server, Unreliable, WithValidation)
	void ServerRPCChangeLightColor();
};
