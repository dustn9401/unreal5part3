// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ABCharacterStatWidget.h"

#include "Components/TextBlock.h"

void UABCharacterStatWidget::NativeConstruct()
{
	Super::NativeConstruct();

	for(TFieldIterator<FNumericProperty> PropIt(FABCharacterStat::StaticStruct()); PropIt; ++PropIt)
	{
		const FName PropKey(PropIt->GetName());
		const FName TextBaseControlName(*FString::Printf(TEXT("Text%sBase"), *PropIt->GetName()));
		const FName TextModifierControlName(*FString::Printf(TEXT("Text%sModifier"), *PropIt->GetName()));

		if (UTextBlock* BaseTextBlock = Cast<UTextBlock>(GetWidgetFromName(TextBaseControlName)))
		{
			BaseLookup.Add(PropKey, BaseTextBlock);
		}

		if (UTextBlock* ModifierTextBlock = Cast<UTextBlock>(GetWidgetFromName(TextModifierControlName)))
		{
			ModifierLookup.Add(PropKey, ModifierTextBlock);
		}
	}
}

void UABCharacterStatWidget::UpdateStat(const FABCharacterStat& BaseStat, const FABCharacterStat& ModifierStat)
{
	for(TFieldIterator<FNumericProperty> PropIt(FABCharacterStat::StaticStruct()); PropIt; ++PropIt)
	{
		const FName PropKey(PropIt->GetName());

		float BaseData = 0.0f;
		PropIt->GetValue_InContainer(&BaseStat, &BaseData);
		float ModifierData = 0.0f;
		PropIt->GetValue_InContainer(&ModifierStat, &ModifierData);

		if (const auto BaseTextBlockPtr = BaseLookup.Find(PropKey))
		{
			(*BaseTextBlockPtr)->SetText(FText::FromString(FString::SanitizeFloat(BaseData)));
		}

		if (const auto ModifierTextBlockPtr = ModifierLookup.Find(PropKey))
		{
			(*ModifierTextBlockPtr)->SetText(FText::FromString(FString::SanitizeFloat(ModifierData)));
		}
	}
}
