// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Interface/ABGameInterface.h"
#include "ABGameMode.generated.h"

/**
 * 중요한 멀티플레이 로직이 대부분 포함된 클래스
 */
UCLASS()
class ARENABATTLESAMPLE_API AABGameMode : public AGameModeBase, public IABGameInterface
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

// IABGameInterface Impl
public:
	virtual void OnPlayerDead() override;
	virtual bool IsGameCleared() override;

// AGameModeBase override (Login)
public:
	// virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	// virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	// virtual void PostLogin(APlayerController* NewPlayer) override;
	// virtual void StartPlay() override;
};
