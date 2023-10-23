// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ABItemData.h"
#include "GameData/ABCharacterStat.h"
#include "ABWeaponItemData.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLESAMPLE_API UABWeaponItemData : public UABItemData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category=Weapon)
	TSoftObjectPtr<USkeletalMesh> WeaponMesh;

	UPROPERTY(EditAnywhere, Category=Stat)
	FABCharacterStat ModifierStat;
};
