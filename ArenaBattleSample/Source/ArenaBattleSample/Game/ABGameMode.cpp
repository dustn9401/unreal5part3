// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ABGameMode.h"

AABGameMode::AABGameMode(): ClearScore(3), CurrentScore(0), bIsCleared(false)
{
	// 블루프린트 클래스를 사용할 경우, 아래처럼 작은따옴표 지우고 _C 를 붙여준다.
	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassRef(TEXT("/Game/ArenaBattle/Blueprint/BP_ABCharacterBase.BP_ABCharacterBase_C"));
	if (DefaultPawnClassRef.Class)
	{
		DefaultPawnClass = DefaultPawnClassRef.Class;
	}

	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassRef(TEXT("/Script/ArenaBattleSample.ABPlayerController"));
	if (PlayerControllerClassRef.Class)
	{
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}
}

void AABGameMode::OnPlayerScoreChanged(int32 NewPlayerScore)
{
	CurrentScore = NewPlayerScore;

	if (CurrentScore >= ClearScore)
	{
		bIsCleared = true;
	}
}

void AABGameMode::OnPlayerDead()
{
}

bool AABGameMode::IsGameCleared()
{
	return bIsCleared;
}
