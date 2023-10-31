// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ABGameMode.h"

#include "ABGameState.h"
#include "ArenaBattleSample.h"
#include "Player/ABPlayerController.h"

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

	GameStateClass = AABGameState::StaticClass();
}

void AABGameMode::OnPlayerDead()
{
	if (AABPlayerController* ABPlayerController = Cast<AABPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		ABPlayerController->GameOver();
	}
}

bool AABGameMode::IsGameCleared()
{
	return bIsCleared;
}

void AABGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId,
	FString& ErrorMessage)
{
	AB_LOG(LogABNetwork, Log, TEXT("========================================================="));
	AB_LOG(LogABNetwork, Log, TEXT("Super Start"));
	
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
	// ErrorMessage = TEXT("Server is Full!");
	
	AB_LOG(LogABNetwork, Log, TEXT("Super End"));
}

APlayerController* AABGameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal,
	const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	AB_LOG(LogABNetwork, Log, TEXT("Super Start"));
	
	APlayerController* Ret = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
	
	AB_LOG(LogABNetwork, Log, TEXT("Super End"));
	
	return Ret;
}

void AABGameMode::PostLogin(APlayerController* NewPlayer)
{
	AB_LOG(LogABNetwork, Log, TEXT("Super Start"));
	
	Super::PostLogin(NewPlayer);

	if (UNetDriver* NetDriver = GetNetDriver())
	{
		if (NetDriver->ClientConnections.Num() == 0)
		{
			AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("No Client Connection"));	
		}
		else
		{
			for(const auto& Connection : NetDriver->ClientConnections)
			{
				AB_LOG(LogABNetwork, Log, TEXT("Client Connection: %s"), *Connection->GetName());
			}
		}
	}
	else
	{
		AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("No NetDriver"));
	}
	
	AB_LOG(LogABNetwork, Log, TEXT("Super End"));
}

void AABGameMode::StartPlay()
{
	// GameMode::StartPlay() -> GameState::HandleBeginPlay() -> PlayerController::BeginPlay()
	AB_LOG(LogABNetwork, Log, TEXT("Super Start"));
	Super::StartPlay();
	AB_LOG(LogABNetwork, Log, TEXT("Super End"));
}
