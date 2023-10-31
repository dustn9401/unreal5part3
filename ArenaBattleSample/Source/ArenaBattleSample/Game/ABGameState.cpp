// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ABGameState.h"

#include "ArenaBattleSample.h"

// void AABGameState::HandleBeginPlay()
// {
// 	// 호스트가 호출한 GameMode::StartPlay() 에 의해서 호출되는 함수. 호스트에서만 호출됨
// 	AB_LOG(LogABNetwork, Log, TEXT("Super Start"))
// 	Super::HandleBeginPlay();
// 	AB_LOG(LogABNetwork, Log, TEXT("Super End"))
// }
//
// void AABGameState::OnRep_ReplicatedHasBegunPlay()
// {
// 	// 클라이언트 에서만 호출되는, 로그인 완료 후 호출되는 함수. (서버에서 GameMode::PostLogin()이 호출된 후 타이밍) 
// 	// 내부에서 플레이어 컨트롤러 생성 및 BeginPlay() 가 호출된다.
// 	AB_LOG(LogABNetwork, Log, TEXT("Super Start"))
// 	Super::OnRep_ReplicatedHasBegunPlay();
// 	AB_LOG(LogABNetwork, Log, TEXT("Super End"))
// }
