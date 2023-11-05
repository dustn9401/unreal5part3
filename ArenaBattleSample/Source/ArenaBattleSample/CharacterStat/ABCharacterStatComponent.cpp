// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStat/ABCharacterStatComponent.h"

#include "ArenaBattleSample.h"
#include "GameData/ABGameSingleton.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UABCharacterStatComponent::UABCharacterStatComponent(): CurrentHp(-1.0f), MaxHp(-1.0f), CurrentLevelNumber(1), AttackRadius(50.0f)
{
	bWantsInitializeComponent = true;
	
	SetIsReplicatedByDefault(true);
}

void UABCharacterStatComponent::InitializeComponent()
{
	Super::InitializeComponent();


	// 이 함수는 PostNetInit 이후에 호출되기 때문에, 아래 조건문은 유효함
	if (GetOwner()->HasAuthority())
	{
		// 순서 중요, 첫 스텟 초기화보다 앞에 있어야 함
		OnStatChanged.AddUObject(this, &UABCharacterStatComponent::UpdateMaxHp);
		
		// 위젯, HUD 생성보다 먼저 호출되어야 함
		SetLevelStat(CurrentLevelNumber);
		SetCurrentHp(MaxHp);
	}
}

void UABCharacterStatComponent::SetLevelStat(int32 InNewLevelNumber)
{
	CurrentLevelNumber = FMath::Clamp(InNewLevelNumber, 1, UABGameSingleton::Get().CharacterMaxLevel);
	SetBaseStat(UABGameSingleton::Get().GetCharacterStatTable(CurrentLevelNumber));
	check(MaxHp > 0.0f);
}

float UABCharacterStatComponent::ApplyDamage(float InDamage)
{
	// 서버에서만 호출될 수 있는 함수
	const float PrevHp = CurrentHp;
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0, InDamage);
	
	SetCurrentHp(PrevHp - ActualDamage);

	return ActualDamage;
}

void UABCharacterStatComponent::SetCurrentHp(float NewHp)
{
	if (FMath::IsNearlyEqual(CurrentHp, NewHp)) return;
	
	// 서버에서만 호출될 수 있는 함수
	check(MaxHp > 0.0f);
	CurrentHp = FMath::Clamp<float>(NewHp, 0.0f, MaxHp);
	OnRep_CurrentHp();
}

void UABCharacterStatComponent::UpdateMaxHp(const FABCharacterStat& InBaseStat, const FABCharacterStat& InModifierStat)
{
	ensureMsgf(GetOwner()->HasAuthority(), TEXT("GetOwner()->HasAuthority() == false!!!"));
	
	const float NewMaxHp = GetTotalStat().MaxHp;
	if (FMath::IsNearlyEqual(MaxHp, NewMaxHp)) return;
	
	// 최대 체력이 낮아지는 경우, 현재 Hp값도 체크해줘야함
	if (CurrentHp > NewMaxHp)
	{
		SetCurrentHp(NewMaxHp);
	}

	MaxHp = NewMaxHp;
	OnRep_MaxHp();
}

void UABCharacterStatComponent::OnRep_BaseStat()
{
	AB_SUB_LOG(LogABNetwork, Log, TEXT("Start"));
	OnStatChanged.Broadcast(BaseStat, ModifierStat);
}

void UABCharacterStatComponent::OnRep_ModifierStat()
{
	AB_SUB_LOG(LogABNetwork, Log, TEXT("Start"));
	OnStatChanged.Broadcast(BaseStat, ModifierStat);
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
	DOREPLIFETIME(UABCharacterStatComponent, MaxHp);
	DOREPLIFETIME_CONDITION(UABCharacterStatComponent, BaseStat, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UABCharacterStatComponent, ModifierStat, COND_OwnerOnly);
}

void UABCharacterStatComponent::OnRep_CurrentHp()
{
	AB_SUB_LOG(LogABNetwork, Log, TEXT("Start"));
	OnHpChanged.Broadcast(CurrentHp, MaxHp);
	
	if (CurrentHp <= KINDA_SMALL_NUMBER)
	{
		OnHpZero.Broadcast();
	}
}

void UABCharacterStatComponent::OnRep_MaxHp()
{
	AB_SUB_LOG(LogABNetwork, Log, TEXT("Start"));
	OnHpChanged.Broadcast(CurrentHp, MaxHp);
	
	if (CurrentHp <= KINDA_SMALL_NUMBER)
	{
		OnHpZero.Broadcast();
	}
}
