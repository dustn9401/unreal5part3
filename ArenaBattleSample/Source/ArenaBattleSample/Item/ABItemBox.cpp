// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ABItemBox.h"

#include "ABItemData.h"
#include "Components/BoxComponent.h"
#include "Engine/AssetManager.h"
#include "Interface/ABCharacterItemInterface.h"
#include "Particles/ParticleSystemComponent.h"
#include "Physics/ABCollision.h"

// Sets default values
AABItemBox::AABItemBox()
{
 	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Effect"));

	RootComponent = Trigger;
	Mesh->SetupAttachment(Trigger);
	Effect->SetupAttachment(Trigger);

	Trigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);
	Trigger->SetBoxExtent(FVector(40.0f, 42.0f, 30.0f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshRef(TEXT("/Script/Engine.StaticMesh'/Game/ArenaBattle/Environment/Props/SM_Env_Breakables_Box1.SM_Env_Breakables_Box1'"));
	if (MeshRef.Object)
	{
		Mesh->SetStaticMesh(MeshRef.Object);
	}
	
	Mesh->SetRelativeLocation(FVector(0.0f, -3.5f, -30.0f));
	Mesh->SetCollisionProfileName(TEXT("NoCollision"));

	static ConstructorHelpers::FObjectFinder<UParticleSystem> EffectRef(TEXT("/Script/Engine.ParticleSystem'/Game/ArenaBattle/Effect/P_TreasureChest_Open_Mesh.P_TreasureChest_Open_Mesh'"));
	if (EffectRef.Object)
	{
		Effect->SetTemplate(EffectRef.Object);
		Effect->bAutoActivate = false;
	}

	bReplicates = true;
}

void AABItemBox::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!IsValid(ItemData))
	{
		// 에셋매니저로부터 데이터 로드
		const UAssetManager& AM = UAssetManager::Get();
	
		TArray<FPrimaryAssetId> AssetIds;
		ensure(AM.GetPrimaryAssetIdList(TEXT("ABItemData"), AssetIds));
	
		const int32 RIdx = FMath::RandRange(0, AssetIds.Num() - 1);
		const FSoftObjectPtr AssetPtr(AM.GetPrimaryAssetPath(AssetIds[RIdx]));

		UObject* LoadedAsset = AssetPtr.LoadSynchronous();
		ItemData = CastChecked<UABItemData>(LoadedAsset);
	}

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AABItemBox::OnOverlapBegin);
}

void AABItemBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult)
{
	UE_LOG(LogTemp, Log, TEXT("ABItemBox.OnOverlapBegin(%s, %s, %s, %d, %d, %s)"), *OverlappedComponent->GetName(), *OtherActor->GetName(), *OtherComp->GetName(), OtherBodyIndex, bFromSweep, *SweepHitResult.BoneName.ToString());
	if (ItemData == nullptr)
	{
		Destroy();
		return;
	}
	
	if (const auto ICharacterItemInst = Cast<IABCharacterItemInterface>(OtherActor))
	{
		ICharacterItemInst->TakeItem(ItemData);
	}
	
	Effect->Activate(true);
	Mesh->SetHiddenInGame(true);
	SetActorEnableCollision(false);
	Effect->OnSystemFinished.AddDynamic(this, &AABItemBox::OnEffectFinished);
}

void AABItemBox::OnEffectFinished(UParticleSystemComponent* ParticleSystem)
{
	Destroy();
}
