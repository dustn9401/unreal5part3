// Fill out your copyright notice in the Description page of Project Settings.


#include "Prop/ABFountain.h"

#include "ArenaBattleSample.h"
#include "EngineUtils.h"
#include "Components/PointLightComponent.h"
#include "Net/UnrealNetwork.h"

int32 num = 90;

// Sets default values
AABFountain::AABFountain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 유니티에서 프리팹을 코드로 생성하는 과정이라고 보면 될듯
	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));	// 유니티의 AddComponent와 유사
	Water = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Water"));

	RootComponent = Body;
	Water->SetupAttachment(Body);		// 유니티의 SetParent와 유사
	Water->SetRelativeLocation(FVector(.0f, .0f, 132.0f));

	// 유니티의 Resources.Load와 유사
	// 생성자에서 LoadObject 대신 이렇게 사용하는것 잊지 말기
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BodyMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/ArenaBattle/Environment/Props/SM_Plains_Castle_Fountain_01.SM_Plains_Castle_Fountain_01'"));
	if (BodyMeshRef.Object)
	{
		Body->SetStaticMesh(BodyMeshRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> WaterMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/ArenaBattle/Environment/Props/SM_Plains_Fountain_02.SM_Plains_Fountain_02'"));
	if (WaterMeshRef.Object)
	{
		Water->SetStaticMesh(WaterMeshRef.Object);
	}

	bReplicates = true;
	NetUpdateFrequency = 1.0f;
	NetCullDistanceSquared = static_cast<float>(2000 * 2000);
	NetDormancy = DORM_Initial;
}

// Called when the game starts or when spawned
void AABFountain::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		FlushNetDormancy();
		
		{
			num = 80;
			FTimerHandle Handle;
			GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
			{
				ServerLightColor = FLinearColor(FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f), 1.0f);
				OnRep_ServerLightColor();
				// FLinearColor NewColor(FLinearColor(FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f), 1.0f));
				// MulticastRPCChangeLightColor(NewColor);
				// ClientRPCFunction(num++);	// Owner를 갖기 전에는 Server만 업데이트 되고, 갖은 후에는 클라이언트만 업데이트 될것임
			}), 1.0f, true, 0.0f);
		}
		
		{
			// FTimerHandle Handle;
			// GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
			// {
			// 	AB_LOG(LogABNetwork, Log, TEXT("Timer Called!!"));
			//
			// 	// FIXED: No owning connection for actor BP_Fountain_C_2. Function ServerRPCChangeLightColor will not be processed.
			// 	// 이 분수대의 Owner만 Server RPC 함수를 호출할 수 있다.
			// 	for(APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
			// 	{
			// 		if (PlayerController && !PlayerController->IsLocalPlayerController())
			// 		{
			// 			AB_LOG(LogABNetwork, Log, TEXT("SetOwner: %s"), *PlayerController->GetName());
			// 			SetOwner(PlayerController);
			// 			// SetRole(ROLE_AutonomousProxy);
			// 			break;
			// 		}
			// 	}
			//
			// }), 10.0f, false, -1.0f);
		}
	}
	else
	{
		// FTimerHandle Handle;
		// GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
		// {
		// }), 1.0f, true, 0.0f);
	}
}

// Called every frame
void AABFountain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		AddActorLocalRotation(FRotator(0.0f, RotationRate * DeltaTime, 0.0f));
		ServerRotationYaw = RootComponent->GetComponentRotation().Yaw;
	}
	else
	{
		ClientTimeSinceUpdate += DeltaTime;
		if (ClientTimeBetweenLastUpdate < KINDA_SMALL_NUMBER) return;
		
		const float EstimateRotationYaw = ServerRotationYaw + RotationRate * ClientTimeBetweenLastUpdate;
		const float LerpRatio = ClientTimeSinceUpdate / ClientTimeBetweenLastUpdate;
		
		FRotator ClientRotator = RootComponent->GetComponentRotation();
		const float ClientNewYaw = FMath::Lerp(ServerRotationYaw, EstimateRotationYaw, LerpRatio);
		ClientRotator.Yaw = ClientNewYaw;
		RootComponent->SetWorldRotation(ClientRotator);
	}
}

void AABFountain::OnRep_ServerRotationYaw()
{
	FRotator NewRotator = RootComponent->GetComponentRotation();
	NewRotator.Yaw = ServerRotationYaw;
	RootComponent->SetWorldRotation(NewRotator);

	ClientTimeBetweenLastUpdate = ClientTimeSinceUpdate;
	ClientTimeSinceUpdate = 0.0f;
}

void AABFountain::OnRep_ServerLightColor()
{
	// 프로퍼티 리플리케이션은 NetConnection 및 Owner 유무에 상관 없이 수행된다.
	// if (GetNetConnection())
	// {
	// 	AB_LOG(LogABNetwork, Log, TEXT("NetCOnnection: %s"), *GetNetConnection()->GetName());
	// }
	// else
	// {
	// 	AB_LOG(LogABNetwork, Log, TEXT("No NetConnection"));
	// }
	
	if (!HasAuthority())
	{
		AB_LOG(LogABNetwork, Log, TEXT("Color: %s"), *ServerLightColor.ToString());
	}

	UPointLightComponent* PointLight = Cast<UPointLightComponent>(GetComponentByClass(UPointLightComponent::StaticClass()));
	if (PointLight)
	{
		PointLight->SetLightColor(ServerLightColor);
	}
}

void AABFountain::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	AB_LOG(LogABNetwork, Log, TEXT("Called"));
	DOREPLIFETIME_CONDITION(AABFountain, ServerRotationYaw, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(AABFountain, ServerLightColor, COND_SimulatedOnly);
	// DOREPLIFETIME(AABFountain, BigData);
}

// void AABFountain::OnActorChannelOpen(FInBunch& InBunch, UNetConnection* Connection)
// {
// 	AB_LOG(LogABNetwork, Log, TEXT("Start"))
// 	
// 	Super::OnActorChannelOpen(InBunch, Connection);
// 	
// 	AB_LOG(LogABNetwork, Log, TEXT("End"))
// }
//
// bool AABFountain::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const
// {
// 	auto Ret = Super::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
// 	// AB_LOG(LogABNetwork, Log, TEXT("IsNetRelevantFor=%d, RealViewer=%s, ViewTarget=%s"), Ret, *RealViewer->GetName(), *ViewTarget->GetName())
// 	return Ret;
// }
//
// bool AABFountain::ServerRPCChangeLightColor_Validate()
// {
// 	return true;
// }
//
// void AABFountain::ServerRPCChangeLightColor_Implementation()
// {
// 	// FLinearColor NewColor(FLinearColor(FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f), 1.0f));
// 	// AB_LOG(LogABNetwork, Log, TEXT("Called: %s"), *NewColor.ToString());
// 	// MulticastRPCChangeLightColor(NewColor);
// }
//
// void AABFountain::MulticastRPCChangeLightColor_Implementation(const FLinearColor& NewLightColor)
// {
// 	AB_LOG(LogABNetwork, Log, TEXT("Called: %s"), *NewLightColor.ToString());
// 	UPointLightComponent* PointLight = Cast<UPointLightComponent>(GetComponentByClass(UPointLightComponent::StaticClass()));
// 	if (PointLight)
// 	{
// 		PointLight->SetLightColor(NewLightColor);
// 	}
// }
//
// void AABFountain::ClientRPCFunction_Implementation(int32 IntParam)
// {
// 	FLinearColor NewColor(FLinearColor(FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f), 1.0f));
// 	AB_LOG(LogABNetwork, Log, TEXT("Called: %s"), *NewColor.ToString());
// 	UPointLightComponent* PointLight = Cast<UPointLightComponent>(GetComponentByClass(UPointLightComponent::StaticClass()));
// 	if (PointLight)
// 	{
// 		PointLight->SetLightColor(NewColor);
// 	}
// }
