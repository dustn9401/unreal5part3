// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ABHUDWidget.h"

#include "ABCharacterStatWidget.h"
#include "ABHpBarWidget.h"
#include "Interface/ABCharacterHUDInterface.h"

UABHUDWidget::UABHUDWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UABHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HpBar = Cast<UABHpBarWidget>(GetWidgetFromName(TEXT("WidgetHpBar")));
	ensure(HpBar);

	CharacterStat = Cast<UABCharacterStatWidget>(GetWidgetFromName(TEXT("WidgetCharacterStat")));
	ensure(CharacterStat);

	if (IABCharacterHUDInterface* HUDPawn = Cast<IABCharacterHUDInterface>(GetOwningPlayerPawn()))
	{
		HUDPawn->SetupHUDWidget(this);
	}
}

void UABHUDWidget::UpdateStat(const FABCharacterStat& BaseStat, const FABCharacterStat& ModifierStat)
{
	auto TotalStat = BaseStat + ModifierStat;
	HpBar->SetMaxHp(TotalStat.MaxHp);
	CharacterStat->UpdateStat(BaseStat, ModifierStat);
}

void UABHUDWidget::UpdateHpBar(float NewCurrentHp)
{
	HpBar->UpdateHpBar(NewCurrentHp);
}
