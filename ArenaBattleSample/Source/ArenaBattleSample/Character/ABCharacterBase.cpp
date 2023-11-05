// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterBase.h"

#include "ABCharacterControlData.h"
#include "ABComboActionData.h"
#include "ArenaBattleSample.h"
#include "EngineUtils.h"
#include "CharacterStat/ABCharacterStatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Item/ABItemData.h"
#include "Item/ABItems.h"
#include "Net/UnrealNetwork.h"
#include "Physics/ABCollision.h"
#include "Player/ABCharacterPlayer.h"
#include "UI/ABHpBarWidget.h"
#include "UI/ABUserWidget.h"
#include "UI/ABWidgetComponent.h"

DEFINE_LOG_CATEGORY(LogABCharacter)

// Sets default values
AABCharacterBase::AABCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	TeamType = ECharacterTeamType::Red;
	
	// Pawn
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Capsule
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);
	GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_ABCAPSULE);

	// Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 700.0f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;

	// Mesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f), FRotator(0.0f, 270.0f, 0.0f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));	// Mesh 대신에 Capsule 사용

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard'"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

	// Animation
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Game/ArenaBattle/Animation/ABP_AB_Character.ABP_AB_Character_C"));
	if (AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}

	// ControlData
	static ConstructorHelpers::FObjectFinder<UABCharacterControlData> CharacterControlDataQuarterRef(TEXT("/Script/ArenaBattleSample.ABCharacterControlData'/Game/ArenaBattle/CharacterControl/ABC_Quarter.ABC_Quarter'"));
	if (CharacterControlDataQuarterRef.Object)
	{
		CharacterControlDataMap.Emplace(ECharacterControlType::Quarter, CharacterControlDataQuarterRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UABCharacterControlData> CharacterControlDataShoulderRef(TEXT("/Script/ArenaBattleSample.ABCharacterControlData'/Game/ArenaBattle/CharacterControl/ABC_Shoulder.ABC_Shoulder'"));
	if (CharacterControlDataShoulderRef.Object)
	{
		CharacterControlDataMap.Emplace(ECharacterControlType::Shoulder, CharacterControlDataShoulderRef.Object);
	}

	if (ComboActionMontage == nullptr)
	{
		static ConstructorHelpers::FObjectFinder<UAnimMontage> ComboActionMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/ArenaBattle/Animation/AM_Combo_Attack.AM_Combo_Attack'"));
		if (ComboActionMontageRef.Object)
		{
			ComboActionMontage = ComboActionMontageRef.Object;
		}
	}

	if (ComboActionData == nullptr)
	{
		static ConstructorHelpers::FObjectFinder<UABComboActionData> ComboActionDataRef(TEXT("/Script/ArenaBattleSample.ABComboActionData'/Game/ArenaBattle/CharacterAction/ABA_ComboAttack.ABA_ComboAttack'"));
		if (ComboActionDataRef.Object)
		{
			ComboActionData = ComboActionDataRef.Object;
		}
	}

	if (DeadMontage == nullptr)
	{
		static ConstructorHelpers::FObjectFinder<UAnimMontage> DeadMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/ArenaBattle/Animation/AM_Dead.AM_Dead'"));
		if (DeadMontageRef.Object)
		{
			DeadMontage = DeadMontageRef.Object;
		}
	}

	if (!Stat)
	{
		Stat = CreateDefaultSubobject<UABCharacterStatComponent>(TEXT("Stat"));
	}

	// Widget
	if (!HpBar)
	{
		HpBar = CreateDefaultSubobject<UABWidgetComponent>(TEXT("Widget"));
		HpBar->SetupAttachment(GetMesh());
		HpBar->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
		static ConstructorHelpers::FClassFinder<UABUserWidget> HpBarWidgetRef(TEXT("/Game/ArenaBattle/UI/WBP_HpBar.WBP_HpBar_C"));
		if (HpBarWidgetRef.Class)
		{
			HpBar->SetWidgetClass(HpBarWidgetRef.Class);
			HpBar->SetWidgetSpace(EWidgetSpace::Screen);
			HpBar->SetDrawSize(FVector2D(150.0f, 15.0f));
			HpBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}

	// Item Actions
	TakeItemDelegates.Emplace(FOnTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AABCharacterBase::EquipWeapon)));
	TakeItemDelegates.Emplace(FOnTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AABCharacterBase::DrinkPotion)));
	TakeItemDelegates.Emplace(FOnTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AABCharacterBase::ReadScroll)));

	// Weapon
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));

	bCanAttack = true;
}

void AABCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 우리팀의 공격만 무시
	GetCapsuleComponent()->SetMaskFilterOnBodyInstance(static_cast<uint8>(TeamType));

	Stat->OnHpZero.AddUObject(this, &AABCharacterBase::SetDead);
	Stat->OnStatChanged.AddUObject(this, &AABCharacterBase::ApplyStat);
	// 여기는 아직 위젯이 생성되기 전이라서, 위젯을 세팅할 수 없음
}

void AABCharacterBase::BeginPlay()
{
	AB_LOG(LogABNetwork, Log, TEXT("Start"))
	Super::BeginPlay();
}

void AABCharacterBase::SetCharacterControlData(const UABCharacterControlData* CharacterControlData)
{
	// Pawn
	bUseControllerRotationYaw = CharacterControlData->bUseControllerRotationYaw;

	// Movement
	GetCharacterMovement()->bOrientRotationToMovement = CharacterControlData->bOrientRotationToMovement;
	GetCharacterMovement()->bUseControllerDesiredRotation = CharacterControlData->bUseControllerDesiredRotation;
	GetCharacterMovement()->RotationRate = CharacterControlData->RotationRate;
}

void AABCharacterBase::ProcessComboCommand()
{
	UE_LOG(LogTemp, Log, TEXT("ProcessComboCommand() Called, CurrentCombo: %d, HasNextComboAction: %d, IsComboTimerValid: %d"), CurrentCombo, HasNextComboCommand, ComboTimerHandle.IsValid());
	// 순서 중요
	if (CurrentCombo == 0)
	{
		ComboActionBegin();
		return;
	}

	// 콤보 타이머가 돌고있을 때 공격 인풋이 들어왔다면, 다음 콤보로 넘어갈 수 있게 해준다.
	if (ComboTimerHandle.IsValid())
	{
		HasNextComboCommand = true;
	}
}

void AABCharacterBase::ComboActionBegin()
{
	ensure(CurrentCombo == 0);
	CurrentCombo = 1;

	// 캐릭터의 이동 기능을 막아서 콤보 기능을 온전히 수행시키기 위한 코드
	UE_LOG(LogTemp, Log, TEXT("Start ComboAction, GetMovementName(): %s"), *GetCharacterMovement()->GetMovementName())
	GetCharacterMovement()->SetMovementMode(MOVE_None);

	// 몽타주 실행
	const float AttackSpeedRate = Stat->GetTotalStat().AttackSpeed;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(ComboActionMontage, AttackSpeedRate);

	// 몽타주가 종료될 때 ComboActionEnd가 실행되도록 이벤트 추가
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &AABCharacterBase::ComboActionEnd);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, ComboActionMontage);

	ComboTimerHandle.Invalidate();
	SetComboCheckTimerIfPossible();
}

void AABCharacterBase::ComboActionEnd(UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
	ensure(CurrentCombo != 0);
	CurrentCombo = 0;
	
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	NotifyComboActionEnd();
}

void AABCharacterBase::NotifyComboActionEnd()
{
}

void AABCharacterBase::SetComboCheckTimerIfPossible()
{
	int32 ComboIndex = CurrentCombo - 1;
	ensure(ComboActionData->EffectiveFrameCount.IsValidIndex(ComboIndex));

	const float AttackSpeedRate = Stat->GetTotalStat().AttackSpeed;
	float ComboEffectiveTime = (ComboActionData->EffectiveFrameCount[ComboIndex] / ComboActionData->FrameRate) / AttackSpeedRate;
	if (ComboEffectiveTime > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &AABCharacterBase::ComboCheck, ComboEffectiveTime, false);
	}
}

void AABCharacterBase::ComboCheck()
{
	ComboTimerHandle.Invalidate();
	if (HasNextComboCommand)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, ComboActionData->MaxComboCount);
		const FName NextSectionName = *FString::Printf(TEXT("%s%d"), *ComboActionData->MontageSectionNamePrefix, CurrentCombo);

		// JumpToSection으로, 현재 콤보의 남은 애니메이션을 생략하고 다음콤보로 바로 이동
		AnimInstance->Montage_JumpToSection(NextSectionName, ComboActionMontage);
		
		SetComboCheckTimerIfPossible();
		
		HasNextComboCommand = false;
	}
}

void AABCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AABCharacterBase, bCanAttack);
	DOREPLIFETIME(AABCharacterBase, BigData);
}

void AABCharacterBase::Attack()
{
	// ProcessComboCommand();

	if (bCanAttack)
	{
		// 타이머 설정, 몽타주 재생을 클라에 맡긴다.
		if (!HasAuthority())
		{
			bCanAttack = false;
			GetCharacterMovement()->SetMovementMode(MOVE_None);
		
			FTimerHandle Handle;
			GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
			{
				bCanAttack = true;
				GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			}), AttackTime, false, -1.0f);

			PlayAttackAnimation();
		}
		
		ServerRPCAttack(GetWorld()->GetGameState()->GetServerWorldTimeSeconds());
	}
}

void AABCharacterBase::AttackHitConfirm(const FHitResult& HitResult)
{
	AB_LOG(LogABNetwork, Log, TEXT("Called: %s"), *HitResult.Component->GetName());
	ensure(HasAuthority());
	
	AActor* HitActor = HitResult.GetActor();
	ensure(HitActor);

	// BigData.Init(BigDataItem, 1000);
	// BigDataItem += 1.0f;

	const float AttackDamage = Stat->GetTotalStat().Attack;
	FDamageEvent DamageEvent;
	HitActor->TakeDamage(AttackDamage, DamageEvent, GetController(), this);
}

void AABCharacterBase::PlayAttackAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->StopAllMontages(0.0f);
	AnimInstance->Montage_Play(ComboActionMontage);
}

void AABCharacterBase::ServerRPCAttack_Implementation(float AttackStartTime)
{
	AB_LOG(LogABNetwork, Log, TEXT("Called"));
	
	bCanAttack = false;
	OnRep_CanAttack();

	AttackTimeDifference = GetWorld()->GetTimeSeconds() - AttackStartTime;
	AB_LOG(LogABNetwork, Log, TEXT("AttackTimeDiff: %f"), AttackTimeDifference);
	AttackTimeDifference = FMath::Clamp(AttackTimeDifference, 0.0f, AttackTime - 0.01f);
		
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
	{
		bCanAttack = true;
		OnRep_CanAttack();
	}), AttackTime - AttackTimeDifference, false, -1.0f);

	PlayAttackAnimation();
	
	LastAttackStartTime = AttackStartTime;
	
	// MulticastRPCAttack();
	
	for(const APlayerController* PC : TActorRange<APlayerController>(GetWorld()))
	{
		if (!PC) continue;
		if (PC == GetController()) continue;	// 이 캐릭터의 주인에게는 보내지 않음
		if (PC->IsLocalController()) continue;	// 서버 자신에게는 보내지 않음

		if (AABCharacterPlayer* OtherPlayerCharacter = Cast<AABCharacterPlayer>(PC->GetPawn()))
		{
			OtherPlayerCharacter->ClientRPCPlayAnimation(this);
		}
	}
}

bool AABCharacterBase::ServerRPCAttack_Validate(float AttackStartTime)
{
	if (LastAttackStartTime == 0.0f) return true;

	if (AttackStartTime - LastAttackStartTime < AttackTime) return false;
	
	return true;
}

void AABCharacterBase::MulticastRPCAttack_Implementation()
{
	if (!IsLocallyControlled())
	{
		PlayAttackAnimation();
	}
}


void AABCharacterBase::ClientRPCPlayAnimation_Implementation(AABCharacterBase* CharacterToPlay)
{
	AB_LOG(LogABNetwork, Log, TEXT("Called"));
	if (CharacterToPlay)
	{
		CharacterToPlay->PlayAttackAnimation();
	}
}

void AABCharacterBase::OnRep_CanAttack()
{
	if (!bCanAttack)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_None);
	}
	else
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}

void AABCharacterBase::ServerRPCNotifyHit_Implementation(const TArray<FHitResult>& OutHitResults, float HitCheckTime)
{
	for(const auto& HitResult : OutHitResults)
	{
		AActor* HitActor = HitResult.GetActor();
		if (!::IsValid(HitActor)) continue;

		const FVector HitLocation = HitResult.Location;
		const FBox HitBox = HitActor->GetComponentsBoundingBox();
		const FVector ActorBoxCenter = HitBox.GetCenter();
		if (FVector::DistSquared(HitLocation, ActorBoxCenter) <= AcceptCheckDistance * AcceptCheckDistance)
		{
			AttackHitConfirm(HitResult);
		}
		else
		{
			AB_LOG(LogABNetwork, Warning, TEXT("HitTest Rejected!"));
		}

#if ENABLE_DRAW_DEBUG
		DrawDebugPoint(GetWorld(), ActorBoxCenter, 50.0f, FColor::Cyan, false, 5.0f);
		DrawDebugPoint(GetWorld(), HitLocation, 50.0f, FColor::Magenta, false, 5.0f);
#endif
		
	}
}

bool AABCharacterBase::ServerRPCNotifyHit_Validate(const TArray<FHitResult>& OutHitResults, float HitCheckTime)
{
	return true;
}

void AABCharacterBase::ServerRPCNotifyMiss_Implementation(FVector_NetQuantize TraceStart, FVector_NetQuantize TraceEnd, FVector_NetQuantizeNormal TraceDir, float HitCheckTime)
{
}

bool AABCharacterBase::ServerRPCNotifyMiss_Validate(FVector_NetQuantize TraceStart, FVector_NetQuantize TraceEnd, FVector_NetQuantizeNormal TraceDir, float HitCheckTime)
{
	return true;
}

void AABCharacterBase::AttackHitCheck()
{
	// 자신이 조종중인 캐릭터의 공격일 경우
	if (IsLocallyControlled())
	{
		AB_LOG(LogABNetwork, Log, TEXT("Called"))
		TArray<FHitResult> OutHitResults;
		FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);
		// Params.IgnoreMask = static_cast<FMaskFilter>(TeamType);

		const auto TotalStat = Stat->GetTotalStat();
		const float AttackRange = TotalStat.AttackRange;
		const float AttackRadius = Stat->GetAttackRadius();
		const float AttackDamage = TotalStat.Attack;
	
		const FVector Forward = GetActorForwardVector();
		const FVector Start = GetActorLocation() + Forward * GetCapsuleComponent()->GetScaledCapsuleRadius();
		const FVector End = Start + Forward * AttackRange;

		bool HitDetected = GetWorld()-> SweepMultiByChannel(OutHitResults, Start, End, FQuat::Identity, CCHANNEL_ABACTION, FCollisionShape::MakeSphere(AttackRadius), Params);
		float HitCheckTime = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
		
		// 클라이언트인 경우, 서버에 검증을 받기 위해 Server RPC 함수 호출
		if (!HasAuthority())
		{
			if (HitDetected)
			{
				ServerRPCNotifyHit(OutHitResults, HitCheckTime);
			}
			else
			{
				ServerRPCNotifyMiss(Start, End, Forward, HitCheckTime);
			}
		}
		// 서버의 경우, 바로 Confirm 함수 호출
		else
		{
			if (HitDetected)
			{
				for(const auto& HitResult : OutHitResults)
				{
					AttackHitConfirm(HitResult);
				}
			}
			else
			{
				
			}
		}

// #if ENABLE_DRAW_DEBUG
// 		FVector CapsuleOrigin = Start + (End - Start) * .5f;
// 		float CapsuleHalfHeight = AttackRange * .5f;
// 		FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;
// 		DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, AttackRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 5.0f);
// #endif
	}
}

float AABCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	Stat->ApplyDamage(DamageAmount);
	
	return DamageAmount;
}

bool AABCharacterBase::CanHit(const FHitResult& HitResult)
{
	return true;
}

void AABCharacterBase::SetDead()
{
	GetCharacterMovement()->SetMovementMode(MOVE_None);
	PlayDeadAnimation();
	SetActorEnableCollision(false);		// 시체가 걸리적거리지 않도록 설정
	
	// SetActorHiddenInGame(true);		// ok, 캐릭터 및 위젯 모두 숨겨짐
	HpBar->SetHiddenInGame(true);	// ok
	// HpBar->SetVisibleFlag(false);	// ok
	// HpBar->SetVisibility(false);		// ok
	// HpBar->SetActive(false);			// no, 틱만 끄는듯
}

void AABCharacterBase::PlayDeadAnimation()
{
	const auto AnimInst = GetMesh()->GetAnimInstance();
	AnimInst->StopAllMontages(0.0f);
	AnimInst->Montage_Play(DeadMontage);
}

void AABCharacterBase::SetCharacterWidget(UABUserWidget* InUserWidget)
{
	if (UABHpBarWidget* HpBarWidget = Cast<UABHpBarWidget>(InUserWidget))
	{
		HpBarWidget->UpdateStat(Stat->GetBaseStat(), Stat->GetModifierStat());
		HpBarWidget->UpdateHpBar(Stat->GetCurrentHp());
		Stat->OnStatChanged.AddUObject(HpBarWidget, &UABHpBarWidget::UpdateStat);
		Stat->OnHpChanged.AddUObject(HpBarWidget, &UABHpBarWidget::UpdateHpBar);
	}
}

void AABCharacterBase::TakeItem(UABItemData* InItemData)
{
	// 여기에서 아이템 종류에 따라 분기하여 효과 적용
	if (InItemData)
	{
		TakeItemDelegates[static_cast<uint8>(InItemData->Type)].ItemDelegate.ExecuteIfBound(InItemData);
	}
}

void AABCharacterBase::DrinkPotion(UABItemData* InItemData)
{
	// 스텟 적용은 서버와 습득한 클라이언트만 수행
	if (HasAuthority())
	{
		if (const UABPotionItemData* PotionItemData = Cast<UABPotionItemData>(InItemData))
		{
			Stat->HealHp(PotionItemData->HealAmount);
		}
	}
}

void AABCharacterBase::EquipWeapon(UABItemData* InItemData)
{
	if (const UABWeaponItemData* WeaponItemData = Cast<UABWeaponItemData>(InItemData))
	{
		const auto WeaponMesh = WeaponItemData->WeaponMesh.IsPending()
			                        ? WeaponItemData->WeaponMesh.LoadSynchronous()
			                        : WeaponItemData->WeaponMesh.Get();
		Weapon->SetSkeletalMesh(WeaponMesh);

		// 스텟 적용은 서버와 습득한 클라이언트만 수행
		if (HasAuthority())
		{
			Stat->SetModifierStat(WeaponItemData->ModifierStat);
		}
	}
}

void AABCharacterBase::ReadScroll(UABItemData* InItemData)
{
	// 스텟 적용은 서버와 습득한 클라이언트만 수행
	if (HasAuthority())
	{
		if (const UABScrollItemData* ScrollItemData = Cast<UABScrollItemData>(InItemData))
		{
			Stat->AddBaseStat(ScrollItemData->BaseStat);
		}
	}
}

int32 AABCharacterBase::GetLevel() const
{
	return Stat->GetCurrentLevel();
}

void AABCharacterBase::SetLevel(const int32 InNewLevel) const
{
	Stat->SetLevelStat(InNewLevel);
}

void AABCharacterBase::ApplyStat(const FABCharacterStat& BaseStat, const FABCharacterStat& ModifierStat)
{
	const FABCharacterStat TotalStat = BaseStat + ModifierStat;
	GetCharacterMovement()->MaxWalkSpeed = TotalStat.MovementSpeed;
}
