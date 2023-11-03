// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStat/ABCharacterStatComponent.h"

#include "ArenaBattleSample.h"
#include "GameData/ABGameSingleton.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UABCharacterStatComponent::UABCharacterStatComponent(): CurrentHp(0), CurrentLevelNumber(1), AttackRadius(50.0f)
{
	bWantsInitializeComponent = true;

	// SetIsReplicated(true);
	SetIsReplicatedByDefault(true);
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
	// 서버에서만 호출될 수 있는 함수
	const float PrevHp = CurrentHp;
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0, InDamage);
	
	SetHp(PrevHp - ActualDamage);

	return ActualDamage;
}

void UABCharacterStatComponent::SetHp(float NewHp)
{
	// 서버에서만 호출될 수 있는 함수
	CurrentHp = FMath::Clamp<float>(NewHp, 0.0f, GetTotalStat().MaxHp);
	OnRep_CurrentHp();
}

void UABCharacterStatComponent::ReadyForReplication()
{
	// InitializeComponent과 BeginPlay 사이에 호출되는 함수
	AB_SUB_LOG(LogABNetwork, Log, TEXT("Start"));
	Super::ReadyForReplication();

}

void UABCharacterStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UABCharacterStatComponent, CurrentHp);
}

void UABCharacterStatComponent::OnRep_CurrentHp()
{
	AB_SUB_LOG(LogABNetwork, Log, TEXT("Start"));
	OnHpChanged.Broadcast(CurrentHp);
	
	if (CurrentHp <= KINDA_SMALL_NUMBER)
	{
		OnHpZero.Broadcast();
	}
}
