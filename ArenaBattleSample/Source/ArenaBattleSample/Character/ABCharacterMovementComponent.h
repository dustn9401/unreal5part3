// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ABCharacterMovementComponent.generated.h"

class FABSavedMove_Character : public FSavedMove_Character
{
public:
	virtual void Clear() override;
	virtual void SetInitialPosition(ACharacter* C) override;
	virtual uint8 GetCompressedFlags() const override;

	uint8 bPressedTeleport : 1;
	uint8 bDidTeleport : 1;
};

class FABNetworkPredictionData_Client_Character : public FNetworkPredictionData_Client_Character
{
public:
	FABNetworkPredictionData_Client_Character(const UCharacterMovementComponent& MovementComponent);

	virtual FSavedMovePtr AllocateNewMove() override;
};

/**
 * 
 */
UCLASS()
class ARENABATTLESAMPLE_API UABCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UABCharacterMovementComponent();

	void SetTeleportCommand();

protected:
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	virtual void ABTeleport();
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;

public:
	uint8 bPressedTeleport : 1;
	uint8 bDidTeleport : 1;

protected:
	UPROPERTY()
	float TeleportOffset;

	UPROPERTY()
	float TeleportCooltime;
};
