// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ABHpBarWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Interface/ABCharacterWidgetInterface.h"

UABHpBarWidget::UABHpBarWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CurrentHp = 0.0f;
	MaxHp = -1.0f;
}

void UABHpBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HpProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PbHpBar")));
	ensure(HpProgressBar);

	CurrentHpText = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextCurrentHp")));
	MaxHpText = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextMaxHp")));
	
	if (IABCharacterWidgetInterface* CharacterWidget = Cast<IABCharacterWidgetInterface>(OwningActor))
	{
		CharacterWidget->SetCharacterWidget(this);
	}
}

void UABHpBarWidget::UpdateHpBar(float NewCurrentHp, float NewMaxHp)
{
	CurrentHp = NewCurrentHp;
	MaxHp = NewMaxHp;
	
	if (CurrentHpText)
	{
		CurrentHpText->SetText(FText::FromString(FString::Printf(TEXT("%d"), FMath::RoundToInt(CurrentHp))));
	}
	
	if (HpProgressBar)
	{
		HpProgressBar->SetPercent(CurrentHp / MaxHp);
	}
}
