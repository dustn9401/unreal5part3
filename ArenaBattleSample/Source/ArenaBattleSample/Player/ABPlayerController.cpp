// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ABPlayerController.h"

#include "ABSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ABHUDWidget.h"

AABPlayerController::AABPlayerController()
{
	// static ConstructorHelpers::FClassFinder<UABHUDWidget> HUDWidgetClassRef(TEXT("/Game/ArenaBattle/UI/WBP_ABHUD.WBP_ABHUD_C"));
	// if (HUDWidgetClassRef.Class)
	// {
	// 	HUDWidgetClass = HUDWidgetClassRef.Class;
	// }
}

void AABPlayerController::GameScoreChanged(int32 NewScore)
{
	K2_OnGameScoreChanged(NewScore);
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
	// Super::BeginPlay() 에서 블루프린트의 BeginPlay가 호출되기 때문에, SaveGameInstance 변수 초기화는 여기에 있어야 안전함
	SaveGameInstance = Cast<UABSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("Player0"), 0));
	if (SaveGameInstance == nullptr)
	{
		SaveGameInstance = NewObject<UABSaveGame>();
	}
	
	Super::BeginPlay();
	
	UE_LOG(LogTemp, Log, TEXT("=========================== AABPlayerController::BeginPlay() =============================="));

	/*
	블루프린트에서 호출하도록 수정됨

	// 게임 실행 시 포커스 시키는 코드 
	const FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);

	// HUD 위젯(Overlay UI)은 컴포넌트가 따로 없고, 이렇게 초기화 해서 사용
	HUDWidget = CreateWidget<UABHUDWidget>(this, HUDWidgetClass);
	if (HUDWidget)
	{
		HUDWidget->AddToViewport();
	}
	*/
}

