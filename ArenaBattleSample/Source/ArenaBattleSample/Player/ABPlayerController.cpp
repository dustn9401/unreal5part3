// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ABPlayerController.h"

#include "ABSaveGame.h"
#include "ArenaBattleSample.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ABHUDWidget.h"

AABPlayerController::AABPlayerController()
{

}

void AABPlayerController::GameClear()
{
	K2_OnGameClear();
}

void AABPlayerController::GameOver()
{
	K2_OnGameOver();
}

void AABPlayerController::OnClickRetry()
{
	ensure(SaveGameInstance);
	SaveGameInstance->RetryCount++;
	
	if (!UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("Player0"), 0))
	{
		UE_LOG(LogTemp, Error, TEXT("Save Game Error!"));
	}
}

void AABPlayerController::BeginPlay()
{
	AB_LOG(LogTemp, Log, TEXT("=========================== Super::BeginPlay() Start =============================="));
	
	Super::BeginPlay();
	
	AB_LOG(LogTemp, Log, TEXT("=========================== Super::BeginPlay() End =============================="));
}

