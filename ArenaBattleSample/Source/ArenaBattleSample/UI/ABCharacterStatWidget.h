 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameData/ABCharacterStat.h"
#include "ABCharacterStatWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLESAMPLE_API UABCharacterStatWidget : public UUserWidget
{
 GENERATED_BODY()

public:
 void UpdateStat(const FABCharacterStat& BaseStat, const FABCharacterStat& ModifierStat);
};
