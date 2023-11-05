// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameData/ABCharacterStat.h"
#include "ABCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHpZeroDelegate)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHpChangedDelegate, float /*CurrentHp*/, float /*MaxHp*/)
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
	FORCEINLINE void AddBaseStat(const FABCharacterStat& InAddBaseStat)
	{
		ensureMsgf(GetOwner()->HasAuthority(), TEXT("GetOwner()->HasAuthority() false!!"));
		BaseStat += InAddBaseStat;
		OnRep_BaseStat();
	}
	
	FORCEINLINE void SetBaseStat(const FABCharacterStat& InBaseStat)
	{
		ensureMsgf(GetOwner()->HasAuthority(), TEXT("GetOwner()->HasAuthority() false!!"));
		BaseStat = InBaseStat;
		OnRep_BaseStat();
	}
	
	// modifier stat
	FORCEINLINE const FABCharacterStat& GetModifierStat() const {return ModifierStat;}
	FORCEINLINE void SetModifierStat(const FABCharacterStat& InModifierStat)
	{
		ensureMsgf(GetOwner()->HasAuthority(), TEXT("GetOwner()->HasAuthority() false!!"));
		ModifierStat = InModifierStat;
		OnRep_ModifierStat();
	}

	// total stat
	FORCEINLINE FABCharacterStat GetTotalStat() const {return BaseStat + ModifierStat;}
	
	// HP
	FORCEINLINE float GetCurrentHp() const {return CurrentHp;}
	FORCEINLINE float GetMaxHp() const {return MaxHp;}
	
	void SetCurrentHp(float NewHp);
	void UpdateMaxHp(const FABCharacterStat& InBaseStat, const FABCharacterStat& InModifierStat);
	
	FORCEINLINE void HealHp(float InHealAmount) { SetCurrentHp(CurrentHp + InHealAmount); }
	
	// attack
	float GetAttackRadius() const {return AttackRadius;}
	float ApplyDamage(float InDamage);

protected:

	// Transient: 디스크에 저장하지 않음
	UPROPERTY(ReplicatedUsing=OnRep_CurrentHp, Transient, VisibleInstanceOnly, Category=Stat)
	float CurrentHp;

	UPROPERTY(ReplicatedUsing=OnRep_MaxHp, Transient, VisibleInstanceOnly, Category=Stat)
	float MaxHp;

	UPROPERTY(Transient, VisibleInstanceOnly, Category=Stat)
	float CurrentLevelNumber;

	UPROPERTY(VisibleInstanceOnly, Category=Stat, meta=(AllowPrivateAccess="true"))
	float AttackRadius;		// Sweep에 사용되는 sphere의 반지름

protected:
	UPROPERTY(ReplicatedUsing=OnRep_BaseStat, Transient, VisibleInstanceOnly, Category=Stat, meta=(AllowPrivateAccess="true"))
	FABCharacterStat BaseStat;

	UPROPERTY(ReplicatedUsing=OnRep_ModifierStat, Transient, VisibleInstanceOnly, Category=Stat, meta=(AllowPrivateAccess="true"))
	FABCharacterStat ModifierStat;

// Replication
protected:
	virtual void ReadyForReplication() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()
	void OnRep_CurrentHp();

	UFUNCTION()
	void OnRep_MaxHp();

	UFUNCTION()
	void OnRep_BaseStat();

	UFUNCTION()
	void OnRep_ModifierStat();
};
