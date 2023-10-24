// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"

#include "AIController.h"
#include "Interface/ABCharacterAIInterface.h"

UBTTask_Attack::UBTTask_Attack()
{
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto Unused = Super::ExecuteTask(OwnerComp, NodeMemory);
	
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}

	IABCharacterAIInterface* AICharacter = Cast<IABCharacterAIInterface>(ControllingPawn);
	if (!AICharacter)
	{
		return EBTNodeResult::Failed;
	}

	FAICharacterAttackFinished AttackFinishedDelegate;
	AttackFinishedDelegate.BindLambda([&]()
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	});
	
	AICharacter->SetAIAttackDelegate(AttackFinishedDelegate);
	AICharacter->AttackByAI();
	
	return EBTNodeResult::InProgress;
}
