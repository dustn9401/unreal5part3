// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameData/ABCharacterStat.h"
#include "Item/ABItemData.h"
#include "ABScrollItemData.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLESAMPLE_API UABScrollItemData : public UABItemData
{
	GENERATED_BODY()

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(TEXT("ABItemData"), GetFName());
	}

public:
	UABScrollItemData();

public:
	UPROPERTY(EditAnywhere, Category=Stat)
	FABCharacterStat BaseStat;
};
