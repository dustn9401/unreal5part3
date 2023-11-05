// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterMovementComponent.h"

#include "ArenaBattleSample.h"
#include "GameFramework/Character.h"

void FABSavedMove_Character::Clear()
{
	FSavedMove_Character::Clear();

	bPressedTeleport = false;
	bDidTeleport = false;
}

void FABSavedMove_Character::SetInitialPosition(ACharacter* C)
{
	FSavedMove_Character::SetInitialPosition(C);

	if (const auto ABMovement = Cast<UABCharacterMovementComponent>(C->GetCharacterMovement()))
	{
		bPressedTeleport = ABMovement->bPressedTeleport;
		bDidTeleport = ABMovement->bDidTeleport;
	}
}

uint8 FABSavedMove_Character::GetCompressedFlags() const
{
	uint8 Ret = FSavedMove_Character::GetCompressedFlags();

	if (bPressedTeleport) Ret |= FLAG_Custom_0;
	if (bDidTeleport) Ret |= FLAG_Custom_1;
	
	return Ret;
}

FABNetworkPredictionData_Client_Character::FABNetworkPredictionData_Client_Character(const UCharacterMovementComponent& MovementComponent)
	: FNetworkPredictionData_Client_Character(MovementComponent)
{
}

FSavedMovePtr FABNetworkPredictionData_Client_Character::AllocateNewMove()
{
	// return FNetworkPredictionData_Client_Character::AllocateNewMove();
	return MakeShared<FABSavedMove_Character>();
}

UABCharacterMovementComponent::UABCharacterMovementComponent(): bPressedTeleport(0), bDidTeleport(0)
{
	TeleportOffset = 600.0f;
	TeleportCooltime = 3.0f;
}

void UABCharacterMovementComponent::SetTeleportCommand()
{
	bPressedTeleport = true;
}

FNetworkPredictionData_Client* UABCharacterMovementComponent::GetPredictionData_Client() const
{
	if (ClientPredictionData == nullptr)
	{
		UABCharacterMovementComponent* MutableThis = const_cast<UABCharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FABNetworkPredictionData_Client_Character(*this);
	}

	return ClientPredictionData;
}

void UABCharacterMovementComponent::ABTeleport()
{
	if (!CharacterOwner) return;

	FVector DestLoc = CharacterOwner->GetActorLocation() + CharacterOwner->GetActorForwardVector() * TeleportOffset;

	CharacterOwner->TeleportTo(DestLoc, CharacterOwner->GetActorRotation());
	bDidTeleport = true;

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
	{
		bDidTeleport = false;
	}), TeleportCooltime, false, -1.0f);
}

void UABCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);

	// 텔레포트 키가 눌렸고, 쿨타임이 아니라면, 텔레포트 수행
	if (bPressedTeleport && !bDidTeleport)
	{
		ABTeleport();
	}

	// 텔레포트 수행 여부 상관 없이, 키가 눌렸다면 초기화
	if (bPressedTeleport)
	{
		bPressedTeleport = false;
	}
}

void UABCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	bPressedTeleport = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
	bDidTeleport = (Flags & FSavedMove_Character::FLAG_Custom_1) != 0;

	if (CharacterOwner && CharacterOwner->GetLocalRole() == ROLE_Authority)
	{
		if (bPressedTeleport && !bDidTeleport)
		{
			AB_SUB_LOG(LogABNetwork, Log, TEXT("Teleport begin"));
			ABTeleport();
		}
	}
}
