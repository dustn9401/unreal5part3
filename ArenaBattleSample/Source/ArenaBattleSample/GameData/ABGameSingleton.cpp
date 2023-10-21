// Fill out your copyright notice in the Description page of Project Settings.


#include "GameData/ABGameSingleton.h"

DEFINE_LOG_CATEGORY(LogABGameSingleton);

UABGameSingleton::UABGameSingleton()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> CharacterStatDataTableRef(TEXT("/Script/Engine.DataTable'/Game/ArenaBattle/GameData/ABCharacterStatTable.ABCharacterStatTable'"));
	if (CharacterStatDataTableRef.Object)
	{
		const UDataTable* Table = CharacterStatDataTableRef.Object;
		check(Table->GetRowMap().Num() > 0);

		/*
		TArray<uint8*> ValueArray;
		Table->GetRowMap().GenerateValueArray(ValueArray);
		Algo::Transform(ValueArray, CharacterStatTable, [](uint8* Value)
		{
			return *reinterpret_cast<FABCharacterStat*>(Value);
		});
		*/

		// 이렇게도 됨
		TArray<FABCharacterStat*> StatArr;
		Table->GetAllRows(TEXT("GetAllRows"), StatArr);
		for(const auto Stat : StatArr)
		{
			CharacterStatTable.Add(*Stat);
		}
	}

	CharacterMaxLevel = CharacterStatTable.Num();
	ensure(CharacterMaxLevel > 0);
}

UABGameSingleton& UABGameSingleton::Get()
{
	UABGameSingleton* Singleton = CastChecked<UABGameSingleton>(GEngine->GameSingleton);
	if (Singleton)
	{
		return *Singleton;
	}

	// 여기로 오면 안됨
	UE_LOG(LogABGameSingleton, Error, TEXT("Invalid Game Singleton"));
	return *NewObject<UABGameSingleton>();
}
