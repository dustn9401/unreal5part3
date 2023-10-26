// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameData/ABCharacterStat.h"
#include "ABCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHpZeroDelegate)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHpChangedDelegate, float /*CurrentHp*/)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStatChangedDelegate, const FABCharacterStat& /*BaseStat*/, const FABCharacterStat& /*ModifierStat*/)


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARENABATTLESAMPLE_API UABCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UABCharacterStatComponent();

protected:
	virtual void InitializeComponent() override;

public:
	FOnHpZeroDelegate OnHpZero;
	FOnHpChangedDelegate OnHpChanged;
	FOnStatChangedDelegate OnStatChanged;

	// level
	FORCEINLINE float GetCurrentLevel() const {return CurrentLevelNumber;}
	void SetLevelStat(int32 InNewLevelNumber);	// 레벨 및 레벨에 해당하는 스텟 데이터 로드하여 세팅

	// base stat
	FORCEINLINE const FABCharacterStat& GetBaseStat() const {return BaseStat;}
	FORCEINLINE void SetBaseStat(const FABCharacterStat& InBaseStat)
	{
		BaseStat = InBaseStat;
		OnStatChanged.Broadcast(BaseStat, ModifierStat);
	}
	
	// modifier stat
	FORCEINLINE const FABCharacterStat& GetModifierStat() const {return ModifierStat;}
	FORCEINLINE void SetModifierStat(const FABCharacterStat& InModifierStat)
	{
		ModifierStat = InModifierStat;
		OnStatChanged.Broadcast(BaseStat, ModifierStat);
	}
	
	FORCEINLINE FABCharacterStat GetTotalStat() const {return BaseStat + ModifierStat;}
	
	FORCEINLINE float GetCurrentHp() const {return CurrentHp;}
	
	float GetAttackRadius() const {return AttackRadius;}
	float ApplyDamage(float InDamage);

protected:
	void SetHp(float NewHp);

	// Transient: 디스크에 저장하지 않음
	UPROPERTY(Transient, VisibleInstanceOnly, Category=Stat)
	float CurrentHp;

	UPROPERTY(Transient, VisibleInstanceOnly, Category=Stat)
	float CurrentLevelNumber;

	UPROPERTY(VisibleInstanceOnly, Category=Stat, meta=(AllowPrivateAccess="true"))
	float AttackRadius;		// Sweep에 사용되는 sphere의 반지름

protected:
	UPROPERTY(Transient, VisibleInstanceOnly, Category=Stat, meta=(AllowPrivateAccess="true"))
	FABCharacterStat BaseStat;

	UPROPERTY(Transient, VisibleInstanceOnly, Category=Stat, meta=(AllowPrivateAccess="true"))
	FABCharacterStat ModifierStat;
};
