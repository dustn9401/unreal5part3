// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ABUserWidget.h"
#include "ABHpBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLESAMPLE_API UABHpBarWidget : public UABUserWidget
{
	GENERATED_BODY()

public:
	// UserWidget에서는 아래 생성자를 사용해야 함
	UABHpBarWidget(const FObjectInitializer& ObjectInitializer);

protected:
	// UserWidget에서는 아래 함수에서 오브젝트 포인터 초기화하는것이 좋음
	virtual void NativeConstruct() override;

public:
	FORCEINLINE void SetMaxHp(float NewMaxHp) {MaxHp = NewMaxHp;}
	void UpdateHpBar(float NewCurrentHp);

protected:
	UPROPERTY()
	TObjectPtr<class UProgressBar> HpProgressBar;

	UPROPERTY()
	float MaxHp;
};
