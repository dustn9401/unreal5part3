// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/ABAnimationAttackInterface.h"
#include "Interface/ABCharacterItemInterface.h"
#include "Interface/ABCharacterWidgetInterface.h"
#include "functional"
#include "CharacterStat/ABCharacterStatComponent.h"
#include "ABCharacterBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogABCharacter, Log, All);

UENUM()
enum class ECharacterControlType : uint8
{
	Shoulder,
	Quarter
};

DECLARE_DELEGATE_OneParam(FOnTakeItemDelegate, class UABItemData* /*InItemData*/)

// 딜리게이트를 컨테이너에 담기 위한 래퍼클래스. 딜리게이트 타입 자체를 컨테이너 타입으로 지정할 수는 있으나, UPROPERTY를 붙일 수 없음
USTRUCT(BlueprintType)
struct FOnTakeItemDelegateWrapper
{
	GENERATED_BODY()
	FOnTakeItemDelegateWrapper() {}
	FOnTakeItemDelegateWrapper(const FOnTakeItemDelegate& InItemDelegate) : ItemDelegate(InItemDelegate) {}
	FOnTakeItemDelegate ItemDelegate;
};

UCLASS()
class ARENABATTLESAMPLE_API AABCharacterBase : public ACharacter,
public IABAnimationAttackInterface,
public IABCharacterWidgetInterface,
public IABCharacterItemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AABCharacterBase();

	virtual void PostInitializeComponents() override;

protected:
	virtual void SetCharacterControlData(const class UABCharacterControlData* CharacterControlData);

	UPROPERTY(EditAnywhere, Category=CharacterControl, meta=(AllowPrivateAccess="true"))
	TMap<ECharacterControlType, class UABCharacterControlData*> CharacterControlDataMap;

// Combo Action
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Animation)
	TObjectPtr<class UAnimMontage> ComboActionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Attack, meta=(AllowPrivateAccess="true"))
	TObjectPtr<class UABComboActionData> ComboActionData;
	
	void ProcessComboCommand();
	void ComboActionBegin();
	void ComboActionEnd(class UAnimMontage* TargetMontage, bool IsProperlyEnded);
	virtual void NotifyComboActionEnd();
	void SetComboCheckTimerIfPossible();	// 다음 콤보가 있는 경우 타이머를 등록해주는 함수
	void ComboCheck();

	// 콤보중이 아닐때 0, 콤보중인경우 1/2/3/4
	int32 CurrentCombo = 0;

	FTimerHandle ComboTimerHandle;
	bool HasNextComboCommand = false;

	// Attack RPCs

	void Attack();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCAttack();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCAttack();

	UPROPERTY(ReplicatedUsing=OnRep_CanAttack)
	uint8 bCanAttack : 1;
	
	float AttackTime = 1.4667f;

	UFUNCTION()
	void OnRep_CanAttack();

// Attack Hit
protected:
	virtual void AttackHitCheck() override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual bool CanHit(const FHitResult& HitResult);	// Sweep 또는 Trace로 충돌 판정이 발생했을 때, 데미지를 입을 수 있는지 여부 반환 (동맹 조건 등 체크)
	
// Dead
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Stat, meta=(AllowPrivateAccess="true"))
	TObjectPtr<class UAnimMontage> DeadMontage;
	
	virtual void SetDead();
	void PlayDeadAnimation();

	float DeadEventDelayTime = 5.0f;

// Stat
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Stat, meta=(AllowPrivateAccess="true"))
	TObjectPtr<class UABCharacterStatComponent> Stat;

// UI Widget
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Stat, meta=(AllowPrivateAccess="true"))
	TObjectPtr<class UABWidgetComponent> HpBar;

	virtual void SetCharacterWidget(UABUserWidget* InUserWidget) override;

// Item
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Equipment, meta=(AllowPrivateAccess="true"))
	TObjectPtr<class USkeletalMeshComponent> Weapon;
	
	virtual void TakeItem(UABItemData* InItemData) override;	// IABCharacterItemInterface Impl

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Item)
	TArray<FOnTakeItemDelegateWrapper> TakeItemDelegates;	// EItemType을 인덱스로 사용하기 때문에, 주의

	virtual void DrinkPotion(class UABItemData* InItemData);
	virtual void EquipWeapon(class UABItemData* InItemData);
	virtual void ReadScroll(class UABItemData* InItemData);

// Stat Section
public:
	int32 GetLevel() const;
	void SetLevel(int32 InNewLevel) const;
	void ApplyStat(const FABCharacterStat& BaseStat, const FABCharacterStat& ModifierStat);
};
