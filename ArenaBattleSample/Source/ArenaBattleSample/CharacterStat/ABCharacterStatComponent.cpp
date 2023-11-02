// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStat/ABCharacterStatComponent.h"

#include "GameData/ABGameSingleton.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UABCharacterStatComponent::UABCharacterStatComponent(): CurrentHp(0), CurrentLevelNumber(1), AttackRadius(50.0f)
{
	bWantsInitializeComponent = true;

	SetIsReplicated(true);
}

void UABCharacterStatComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// 위젯, HUD 생성보다 먼저 호출되어야 함
	SetLevelStat(CurrentLevelNumber);
	SetHp(GetTotalStat().MaxHp);
}

void UABCharacterStatComponent::SetLevelStat(int32 InNewLevelNumber)
{
	CurrentLevelNumber = FMath::Clamp(InNewLevelNumber, 1, UABGameSingleton::Get().CharacterMaxLevel);
	SetBaseStat(UABGameSingleton::Get().GetCharacterStatTable(CurrentLevelNumber));
	check(BaseStat.MaxHp > 0.0f);
}

float UABCharacterStatComponent::ApplyDamage(float InDamage)
{
	const float PrevHp = CurrentHp;
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0, InDamage);
	
	SetHp(PrevHp - ActualDamage);
	if (CurrentHp <= KINDA_SMALL_NUMBER)
	{
		OnHpZero.Broadcast();
	}

	return ActualDamage;
}

void UABCharacterStatComponent::SetHp(float NewHp)
{
	CurrentHp = FMath::Clamp<float>(NewHp, 0.0f, GetTotalStat().MaxHp);
	OnHpChanged.Broadcast(CurrentHp);
}

void UABCharacterStatComponent::ReadyForReplication()
{
	// InitializeComponent과 BeginPlay 사이에 호출되는 함수
	Super::ReadyForReplication();

}

void UABCharacterStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UABCharacterStatComponent, CurrentHp);
}

void UABCharacterStatComponent::OnRep_CurrentHp()
{
	OnHpChanged.Broadcast(CurrentHp);
	
	if (CurrentHp <= KINDA_SMALL_NUMBER)
	{
		OnHpZero.Broadcast();
	}
}
