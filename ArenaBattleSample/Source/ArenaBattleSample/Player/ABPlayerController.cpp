// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ABPlayerController.h"

void AABPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 게임 실행 시 포커스 시키는 코드 
	const FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}
