// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/ABAIController.h"

#include "ABAI.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

AABAIController::AABAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBAssetRef(TEXT("/Script/AIModule.BlackboardData'/Game/ArenaBattle/AI/BB_ABCharacter.BB_ABCharacter'"));
	if (BBAssetRef.Object)
	{
		BBAsset = BBAssetRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTAssetRef(TEXT("/Script/AIModule.BehaviorTree'/Game/ArenaBattle/AI/BT_ABCharacter.BT_ABCharacter'"));
	if (BTAssetRef.Object)
	{
		BTAsset = BTAssetRef.Object;
	}
}

void AABAIController::RunAI()
{
	// 사용할 블랙보드 지정 및 행동트리 실행
	if (auto BlackboardPtr = Blackboard.Get(); UseBlackboard(BBAsset, BlackboardPtr))
	{
		// HomePos를 폰의 생성 위치로 지정
		Blackboard->SetValueAsVector(BBKEY_HOMEPOS, GetPawn()->GetActorLocation());
		
		const bool RunResult = RunBehaviorTree(BTAsset);
		ensure(RunResult);
	}
}

void AABAIController::StopAI()
{
	if (const auto BTComponent = Cast<UBehaviorTreeComponent>(BrainComponent))
	{
		BTComponent->StopTree();
	}
}

void AABAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	RunAI();
}
