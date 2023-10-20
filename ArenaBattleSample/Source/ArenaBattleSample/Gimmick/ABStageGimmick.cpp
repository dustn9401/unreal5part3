// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/ABStageGimmick.h"

#include "Character/ABCharacterNonPlayer.h"
#include "Components/BoxComponent.h"
#include "Item/ABItemBox.h"
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
			Gates.Add(GateSocket, Gate);

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

	// Fight Section
	OpponentSpawnTime = 2.0f;
	OpponentClass = AABCharacterNonPlayer::StaticClass();

	// RewardSection
	RewardBoxClass = AABItemBox::StaticClass();
	for(FName GateSocket : GateSockets)
	{
		FVector BoxLocation = StageMesh->GetSocketLocation(GateSocket) * .5f;
		RewardBoxLocations.Add(GateSocket, BoxLocation);
	}
}

void AABStageGimmick::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	SetState(CurrentState);
}


void AABStageGimmick::OnStageTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                                 const FHitResult& SweepHitResult)
{
	UE_LOG(LogTemp, Log, TEXT("AABStageGimmick::OnStageTriggerBeginOverlap(%s, %s, %s, %d, %d, %s)"), *OverlappedComponent->GetName(), *OtherActor->GetName(), *OtherComp->GetName(), OtherBodyIndex, bFromSweep, *SweepHitResult.BoneName.ToString());

	SetState(EStageState::Fight);
}

void AABStageGimmick::OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepHitResult)
{
	UE_LOG(LogTemp, Log, TEXT("AABStageGimmick::OnGateTriggerBeginOverlap(%s, %s, %s, %d, %d, %s)"), *OverlappedComponent->GetName(), *OtherActor->GetName(), *OtherComp->GetName(), OtherBodyIndex, bFromSweep, *SweepHitResult.BoneName.ToString());

	check(OverlappedComponent->ComponentTags.Num() == 1);	// GateTrigger의 컴포넌트 태그는 1개여야만 함 (위에서 1개만 추가함)
	const FName ComponentTag = OverlappedComponent->ComponentTags[0];
	const FName SocketName = FName(*ComponentTag.ToString().Left(2));	// +X/-X/+Y/-Y
	check(StageMesh->DoesSocketExist(SocketName));

	FVector NewLocation = StageMesh->GetSocketLocation(SocketName);
	TArray<FOverlapResult> OverlapResults;

	// 방문하지 않았던 장소일 경우에만 새로운 스테이지 생성
	FCollisionQueryParams CollisionQueryParam(SCENE_QUERY_STAT(GateTrigger), false, this);
	const bool bResult = GetWorld()->OverlapMultiByObjectType(
		OverlapResults,
		NewLocation,
		FQuat::Identity,
		FCollisionObjectQueryParams::InitType::AllStaticObjects,
		FCollisionShape::MakeSphere(775.0f),
		CollisionQueryParam);

	if (!bResult)
	{
		GetWorld()->SpawnActor<AABStageGimmick>(NewLocation, FRotator::ZeroRotator);
	}
}

void AABStageGimmick::OpenAllGates()
{
	for(const auto Gate : Gates)
	{
		Gate.Value->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));	
	}
}

void AABStageGimmick::CloseAllGates()
{
	for(const auto Gate : Gates)
	{
		Gate.Value->SetRelativeRotation(FRotator::ZeroRotator);
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

	OpenAllGates();
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

	// 문 닫힌 뒤 딜레이 후 적 스폰
	GetWorld()->GetTimerManager().SetTimer(OpponentTimerHandle, this, &AABStageGimmick::OnOpponentSpawn, OpponentSpawnTime, false);
}

void AABStageGimmick::SetChooseReward()
{
	UE_LOG(LogTemp, Log, TEXT("AABStageGimmick::SetChooseReward"));
	StageTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	for(const auto GateTrigger : GateTriggers)
	{
		GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	}

	CloseAllGates();
	SpawnRewardBoxes();
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

void AABStageGimmick::OnOpponentDestroyed(AActor* DestroyedActor)
{
	SetState(EStageState::Reward);
}

void AABStageGimmick::OnOpponentSpawn()
{
	const FVector SpawnLocation = GetActorLocation() + FVector::UpVector * 88.0f;
	AActor* OpponentCharacter = GetWorld()->SpawnActor(OpponentClass, &SpawnLocation, &FRotator::ZeroRotator);
	AABCharacterNonPlayer* ABOpponentCharacter = Cast<AABCharacterNonPlayer>(OpponentCharacter);
	if (ABOpponentCharacter)
	{
		ABOpponentCharacter->OnDestroyed.AddDynamic(this, &AABStageGimmick::OnOpponentDestroyed);
	}
}

void AABStageGimmick::OnRewardTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult)
{
	for(const auto& RewardBox : RewardBoxes)
	{
		if (RewardBox.IsValid())
		{
			AABItemBox* ValidItemBox = RewardBox.Get();
			AActor* OverlappedBox = OverlappedComponent->GetOwner();
			if (OverlappedBox != ValidItemBox)
			{
				// 선택한 상자만 남기고 모두 파괴
				ValidItemBox->Destroy();
			}
		}
	}
	
	UE_LOG(LogTemp, Log, TEXT("Boxes Destroyed, Num()=%d"), RewardBoxes.Num());	// 4, 직접 비워줘야 함
	SetState(EStageState::Next);
}

void AABStageGimmick::SpawnRewardBoxes()
{
	for(const auto& Pair : RewardBoxLocations)
	{
		FVector WorldSpawnLocation = GetActorLocation() + Pair.Value + FVector::ZAxisVector * 30.0f;
		AActor* BoxActor = GetWorld()->SpawnActor(RewardBoxClass, &WorldSpawnLocation, &FRotator::ZeroRotator);
		AABItemBox* ItemBoxActor = Cast<AABItemBox>(BoxActor);
		if (ItemBoxActor)
		{
			ItemBoxActor->Tags.Add(Pair.Key);
			ItemBoxActor->GetTrigger()->OnComponentBeginOverlap.AddDynamic(this, &AABStageGimmick::OnRewardTriggerBeginOverlap);
			RewardBoxes.Add(ItemBoxActor);
		}
	}
}