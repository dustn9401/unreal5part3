// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ABGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLESAMPLE_API AABGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AABGameMode();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Game)
	int32 ClearScore;	// 클리어에 필요한 점수

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category=Game)
	int32 CurrentScore;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category=Game)
	uint8 bIsCleared : 1;
};
