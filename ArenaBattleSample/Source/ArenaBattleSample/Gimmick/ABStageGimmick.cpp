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
	CurrentStageNum = 0;
	CurrentState = EStageState::Ready;
	
	StateChangeActions.Add(EStageState::Ready, FStageChangedDelegateWrapper(FOnStageChangedDelegate::CreateUObject(this, &AABStageGimmick::SetReady)));
	StateChangeActions.Add(EStageState::Fight, FStageChangedDelegateWrapper(FOnStageChangedDelegate::CreateUObject(this, &AABStageGimmick::SetFight)));
	StateChangeActions.Add(EStageState::Reward, FStageChangedDelegateWrapper(FOnStageChangedDelegate::CreateUObject(this, &AABStageGimmick::SetChooseReward)));
	StateChangeActions.Add(EStageState::Next, FStageChangedDelegateWrapper(FOnStageChangedDelegate::CreateUObject(this, &AABStageGimmick::SetChooseNext)));

	// Fight Section
	OpponentSpawnTime = 2.0f;
	OpponentClass = AABCharacterNonPlayer::StaticClass();

	// square
	int w = 1, h = 10;
	float space = 100.0f;
	float xs = -w * .5f * space;
	float ys = -h * .5f * space;
	for(int i=0;i<w;i++)
	{
		for(int j=0;j<h;j++)
		{
			OpponentSpawnLocations.Add(FVector(xs + i * space, ys + j * space, 0.0f));
		}
	}

	// spiral
	// double a = 0.0;
	// double b = 50;
	// int NumPoints = 100;
	// for(int i=0;i<=NumPoints;i++)
	// {
	// 	double Theta = 0.2 * i;
	// 	double r = a + b * Theta;
	// 	double x = r * FMath::Cos(Theta);
	// 	double y = r * FMath::Sin(Theta);
	// 	OpponentSpawnLocations.Add(FVector(x, y, 0));
	// 

	CurrentOpponentCount = 0;

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
		const FTransform NewTr(NewLocation);
		if (const auto NewGimmick = GetWorld()->SpawnActorDeferred<AABStageGimmick>(StaticClass(), NewTr))
		{
			NewGimmick->SetStageNum(CurrentStageNum + 1);
			NewGimmick->FinishSpawning(NewTr);
		}
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

	// StageTrigger는 여기서 NoCollision으로 변한 뒤 다시 활성화 되지 않는다. (한번 Fight가 일어난 스테이지에서는 다시 전투가 발생하면 안되니까)
	// 그래서 Stage를 재사용 하려는 경우, 방문하지 않은 위치의 Stage 일 경우에만 다시 활성화 시켜주는 코드 추가가 필요함
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
	CurrentOpponentCount--;
	ensure(CurrentOpponentCount >= 0);
	
	if (CurrentOpponentCount == 0)
	{
		SetState(EStageState::Reward);
	}
}

void AABStageGimmick::OnOpponentSpawn()
{
	for(auto SpawnLoc : OpponentSpawnLocations)
	{
		const FTransform SpawnTr = FTransform(GetActorLocation() + SpawnLoc + FVector::UpVector * 88.0f);

		// SpawnActor를 사용하면 즉시 BeginPlay()함수가 호출되기 때문에, SetLevel함수 호출 후 초기화 코드를 또 호출해줘야함
		// 그래서 BeginPlay 등의 액터 생성 이벤트 함수들이 자동으로 호출되지 않는  SpawnActorDeferred 라는 함수를 사용한다.
		AActor* OpponentCharacter = GetWorld()->SpawnActorDeferred<AABCharacterNonPlayer>(OpponentClass, SpawnTr, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		AABCharacterNonPlayer* ABOpponentCharacter = Cast<AABCharacterNonPlayer>(OpponentCharacter);
		if (ABOpponentCharacter)
		{
			CurrentOpponentCount++;
			ABOpponentCharacter->OnDestroyed.AddDynamic(this, &AABStageGimmick::OnOpponentDestroyed);
			ABOpponentCharacter->SetLevel(CurrentStageNum);	// 현재 스테이지 레벨로 적을 세팅한다.
			ABOpponentCharacter->FinishSpawning(SpawnTr);	// SpawnActorDeferred 사용 시 반드시 호출해줘야함
		}
	}
}

void AABStageGimmick::OnRewardTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult)
{
	// 이 함수 먼저 호출 후 ABItemBox::OnOverlapBegin() 이 호출되어야 함!!
	UE_LOG(LogTemp, Log, TEXT("AABStageGimmick::OnRewardTriggerBeginOverlap"));
	
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
	
	SetState(EStageState::Next);
}

void AABStageGimmick::SpawnRewardBoxes()
{
	for(const auto& Pair : RewardBoxLocations)
	{
		FTransform BoxTr(GetActorLocation() + Pair.Value + FVector::ZAxisVector * 30.0f);
		AABItemBox* BoxActor = GetWorld()->SpawnActorDeferred<AABItemBox>(RewardBoxClass, BoxTr);
		if (BoxActor)
		{
			BoxActor->Tags.Add(Pair.Key);
			
			// 캐릭터가 미리 상자 위치에 서있는 경우 주의할것. 아래에서 FinishSpawning함수가 호출됨과 동시에 OnComponentBeginOverlap이 트리거됨
			BoxActor->GetTrigger()->OnComponentBeginOverlap.AddDynamic(this, &AABStageGimmick::OnRewardTriggerBeginOverlap);
			
			RewardBoxes.Add(BoxActor);
		}
	}

	for(const auto& RewardBox : RewardBoxes)
	{
		if (RewardBox.IsValid())
		{
			RewardBox.Get()->FinishSpawning(RewardBox.Get()->GetActorTransform());
		}
	}
}
