// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ABPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLESAMPLE_API AABPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AABPlayerController();

	UFUNCTION(BlueprintImplementableEvent, Category=Game, meta=(DisplayName="OnGameClearCpp"))
	void K2_OnGameClear();
	
	UFUNCTION(BlueprintImplementableEvent, Category=Game, meta=(DisplayName="OnGameOverCpp"))
	void K2_OnGameOver();

	// GameMode에 의해 호출되는 함수들
	void GameClear();
	void GameOver();

	UFUNCTION(BlueprintCallable, Category=Game)
	void OnClickRetry();

protected:
	// virtual void PostInitializeComponents() override;
	// virtual void PostNetInit() override;
	// virtual void BeginPlay() override;
	// virtual void OnPossess(APawn* InPawn) override;

// HUD Section: 블루프린트에서 관리하도록 수정됨 (BP_ABPlayerController)
protected:
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=HUD)
	// TSubclassOf<class UABHUDWidget> HUDWidgetClass;	// 위젯 로드하려면 클래스 정보가 필요

	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=HUD)
	// TObjectPtr<class UABHUDWidget> HUDWidget;	// 로드한 hud 위젯 보관용

// Save Game Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=SaveGame)
	TObjectPtr<class UABSaveGame> SaveGameInstance;
};
