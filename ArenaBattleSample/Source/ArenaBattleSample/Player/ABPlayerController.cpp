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

void AABPlayerController::PostInitializeComponents()
{
	// 네트워크와 무관하게 액터 초기화
	AB_LOG(LogABNetwork, Log, TEXT("Start"));
	
	Super::PostInitializeComponents();
	
	AB_LOG(LogABNetwork, Log, TEXT("End"));
}

void AABPlayerController::PostNetInit()
{
	// 서버로부터 액터 생성에 필요한 모든 정보를 수신한 뒤에 호출되는 함수
	// 클라이언트만 호출됨
	AB_LOG(LogABNetwork, Log, TEXT("Start"));
	
	Super::PostNetInit();

	if (UNetDriver* NetDriver = GetNetDriver())
	{
		AB_LOG(LogABNetwork, Log, TEXT("Client Connection: %s"), *NetDriver->ServerConnection->GetName());
	}
	else
	{
		AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("No NetDriver"));
	}
	
	AB_LOG(LogABNetwork, Log, TEXT("End"));
}

void AABPlayerController::BeginPlay()
{
	AB_LOG(LogABNetwork, Log, TEXT("Start"));
	
	Super::BeginPlay();
	
	AB_LOG(LogABNetwork, Log, TEXT("End"));
}

void AABPlayerController::OnPossess(APawn* InPawn)
{
	AB_LOG(LogABNetwork, Log, TEXT("Start"));
	
	Super::OnPossess(InPawn);
	
	AB_LOG(LogABNetwork, Log, TEXT("End"));
}

