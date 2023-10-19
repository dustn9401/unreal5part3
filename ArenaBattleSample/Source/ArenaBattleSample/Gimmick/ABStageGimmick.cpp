// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/ABStageGimmick.h"
#include "Components/BoxComponent.h"
#include "Physics/ABCollision.h"

// Sets default values
AABStageGimmick::AABStageGimmick()
{
	// Stage Section
	RootComponent = StageMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StageMesh"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> StageMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/ArenaBattle/Environment/Stages/SM_SQUARE.SM_SQUARE'"));
	if (StageMeshRef.Object)
	{
		StageMesh->SetStaticMesh(StageMeshRef.Object);
	}

	StageTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("StageTrigger"));
	StageTrigger->SetBoxExtent(FVector(775.0f, 775.0f, 300.0f));
	StageTrigger->SetupAttachment(StageMesh);
	StageTrigger->SetRelativeLocation(FVector(0.0f, 0.0f, 250.0f));
	StageTrigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);
	StageTrigger->OnComponentBeginOverlap.AddDynamic(this, &AABStageGimmick::OnStageTriggerBeginOverlap);

	// Gate Section
	static FName GateSockets[] {TEXT("+XGate"),TEXT("-XGate"),TEXT("+YGate"),TEXT("-YGate")};	// Stage Static Mesh의 소켓들 중 Gate를 놓을 소켓들
	static ConstructorHelpers::FObjectFinder<UStaticMesh> GateMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/ArenaBattle/Environment/Props/SM_GATE.SM_GATE'"));
	if (GateMeshRef.Object)
	{
		for (const FName GateSocket : GateSockets)
		{
			// 메시생성
			UStaticMeshComponent* Gate = CreateDefaultSubobject<UStaticMeshComponent>(GateSocket);
			Gate->SetStaticMesh(GateMeshRef.Object);
			Gate->SetupAttachment(StageMesh, GateSocket);
			Gate->SetRelativeLocation(FVector(0.0f, -80.5f, 0.0f));
			Gate->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
			Gates.Emplace(GateSocket, Gate);

			// 박스트리거 생성
			const FName TriggerName = *GateSocket.ToString().Append(TEXT("Trigger"));	// const TCHAR* -> FName
			UBoxComponent* GateTrigger = CreateDefaultSubobject<UBoxComponent>(TriggerName);
			GateTrigger->SetBoxExtent(FVector(100.0f, 100.0f, 300.0f));
			GateTrigger->SetupAttachment(StageMesh, GateSocket);
			GateTrigger->SetRelativeLocation(FVector(70.0f, 0.0f, 250.0f));
			GateTrigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);
			GateTrigger->OnComponentBeginOverlap.AddDynamic(this, &AABStageGimmick::OnGateTriggerBeginOverlap);
			GateTrigger->ComponentTags.Add(GateSocket);
			GateTriggers.Add(GateTrigger);
		}
	}

	// State Section
	CurrentState = EStageState::Ready;
	
	StateChangeActions.Add(EStageState::Ready, FStageChangedDelegateWrapper(FOnStageChangedDelegate::CreateUObject(this, &AABStageGimmick::SetReady)));
	StateChangeActions.Add(EStageState::Fight, FStageChangedDelegateWrapper(FOnStageChangedDelegate::CreateUObject(this, &AABStageGimmick::SetFight)));
	StateChangeActions.Add(EStageState::Reward, FStageChangedDelegateWrapper(FOnStageChangedDelegate::CreateUObject(this, &AABStageGimmick::SetChooseReward)));
	StateChangeActions.Add(EStageState::Next, FStageChangedDelegateWrapper(FOnStageChangedDelegate::CreateUObject(this, &AABStageGimmick::SetChooseNext)));
}


void AABStageGimmick::OnStageTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepHitResult)
{
	UE_LOG(LogTemp, Log, TEXT("AABStageGimmick::OnStageTriggerBeginOverlap(%s, %s, %s, %d, %d, %s)"), *OverlappedComponent->GetName(), *OtherActor->GetName(), *OtherComp->GetName(), OtherBodyIndex, bFromSweep, *SweepHitResult.BoneName.ToString());
}

void AABStageGimmick::OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepHitResult)
{
	UE_LOG(LogTemp, Log, TEXT("AABStageGimmick::OnGateTriggerBeginOverlap(%s, %s, %s, %d, %d, %s)"), *OverlappedComponent->GetName(), *OtherActor->GetName(), *OtherComp->GetName(), OtherBodyIndex, bFromSweep, *SweepHitResult.BoneName.ToString());
}

void AABStageGimmick::OpenAllGates()
{
	for(const auto Gate : Gates)
	{
		FRotator TargetRot = FRotator(0.0f, -90.0f, 0.0f);
		if (Gate.Value->GetRelativeRotation().Equals(TargetRot))
			continue;
		
		Gate.Value->SetRelativeRotation(TargetRot);	
	}
}

void AABStageGimmick::CloseAllGates()
{
	for(const auto Gate : Gates)
	{
		FRotator TargetRot = FRotator::ZeroRotator;
		if (Gate.Value->GetRelativeRotation().Equals(TargetRot))
			continue;
		
		Gate.Value->SetRelativeRotation(TargetRot);
	}
}

void AABStageGimmick::SetState(EStageState InNewState)
{
	CurrentState = InNewState;

	if (StateChangeActions.Contains(InNewState))
	{
		// ReSharper disable once CppExpressionWithoutSideEffects
		StateChangeActions[CurrentState].StageDelegate.ExecuteIfBound();
	}
}

void AABStageGimmick::SetReady()
{
	UE_LOG(LogTemp, Log, TEXT("AABStageGimmick::SetReady"));
	StageTrigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);
	for(const auto GateTrigger : GateTriggers)
	{
		GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	}

	CloseAllGates();
}

void AABStageGimmick::SetFight()
{
	UE_LOG(LogTemp, Log, TEXT("AABStageGimmick::SetFight"));
	StageTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	for(const auto GateTrigger : GateTriggers)
	{
		GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	}

	CloseAllGates();
}

void AABStageGimmick::SetChooseReward()
{
	UE_LOG(LogTemp, Log, TEXT("AABStageGimmick::SetChooseReward"));
	StageTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	for(const auto GateTrigger : GateTriggers)
	{
		GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	}
}

void AABStageGimmick::SetChooseNext()
{
	UE_LOG(LogTemp, Log, TEXT("AABStageGimmick::SetChooseNext"));
	StageTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	for(const auto GateTrigger : GateTriggers)
	{
		GateTrigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);
	}

	OpenAllGates();
}
