// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ABCharacterStat.h"
#include "UObject/NoExportTypes.h"
#include "ABGameSingleton.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogABGameSingleton, Error, All);

/**
 * 
 */
UCLASS()
class ARENABATTLESAMPLE_API UABGameSingleton : public UObject
{
	GENERATED_BODY()

public:
	UABGameSingleton();

	static UABGameSingleton& Get();


private:
	UPROPERTY()
	TArray<FABCharacterStat> CharacterStatTable;

public:
	UPROPERTY()
	int32 CharacterMaxLevel;
	
	FORCEINLINE FABCharacterStat GetCharacterStatTable(int32 InLevelIndex) const
	{
		return CharacterStatTable.IsValidIndex(InLevelIndex)
		? CharacterStatTable[InLevelIndex]
		: FABCharacterStat();
	}
};
