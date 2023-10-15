// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/ABAnimationAttackInterface.h"
#include "ABCharacterBase.generated.h"

UENUM()
enum class ECharacterControlType : uint8
{
	Shoulder,
	Quarter
};

UCLASS()
class ARENABATTLESAMPLE_API AABCharacterBase : public ACharacter, public IABAnimationAttackInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AABCharacterBase();

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
	void SetComboCheckTimerIfPossible();	// 다음 콤보가 있는 경우 타이머를 등록해주는 함수
	void ComboCheck();

	// 콤보중이 아닐때 0, 콤보중인경우 1/2/3/4
	int32 CurrentCombo = 0;

	FTimerHandle ComboTimerHandle;
	bool HasNextComboCommand = false;

// Attack Hit
protected:
	virtual void AttackHitCheck() override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
// Dead
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Stat, meta=(AllowPrivateAccess="true"))
	TObjectPtr<class UAnimMontage> DeadMontage;
	
	virtual void SetDead();
	void PlayDeadAnimation();
};
