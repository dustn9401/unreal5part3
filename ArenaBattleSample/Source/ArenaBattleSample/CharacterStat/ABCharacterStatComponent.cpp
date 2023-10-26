// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStat/ABCharacterStatComponent.h"

#include "GameData/ABGameSingleton.h"

// Sets default values for this component's properties
UABCharacterStatComponent::UABCharacterStatComponent(): CurrentHp(0), CurrentLevelNumber(1), AttackRadius(50.0f)
{
	bWantsInitializeComponent = true;
}

void UABCharacterStatComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// 위젯, HUD 생성보다 먼저 호출되어야 함
	SetLevelStat(CurrentLevelNumber);
	SetHp(BaseStat.MaxHp);
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
	CurrentHp = FMath::Clamp<float>(NewHp, 0.0f, BaseStat.MaxHp);
	OnHpChanged.Broadcast(CurrentHp);
}
