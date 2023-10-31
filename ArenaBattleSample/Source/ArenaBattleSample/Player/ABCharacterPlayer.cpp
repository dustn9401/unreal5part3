// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ABCharacterPlayer.h"

#include "ArenaBattleSample.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/ABCharacterControlData.h"
#include "Character/ABCharacterNonPlayer.h"
#include "Interface/ABGameInterface.h"
#include "Physics/ABCollision.h"
#include "UI/ABHpBarWidget.h"
#include "UI/ABHUDWidget.h"
#include "GameFramework/GameModeBase.h"

AABCharacterPlayer::AABCharacterPlayer()
{
	// =============================== Camera 설정 =======================================
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	
	// 이렇게 두 번째 파라미터로 위치값이 아니라 특정한 FName값을 넘겨 사용하는 경우도 잇음
	// USpringArmComponent::SocketName : The name of the socket at the end of the spring arm (looking back towards the spring arm origin)
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// =============================== Input 설정 =======================================
	// 이부분은 "InputMappingContext.h"를 include 하지 않아도 IDE가 오류로 인식하지 않는데, 빌드는 실패함
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionJumpRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_Jump.IA_Jump'"));
	if (InputActionJumpRef.Object)
	{
		JumpAction = InputActionJumpRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputChangeControlRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_Change_Control.IA_Change_Control'"));
	if (InputChangeControlRef.Object)
	{
		ChangeControlAction = InputChangeControlRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputShoulderMoveRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_ShoulderMove.IA_ShoulderMove'"));
	if (InputShoulderMoveRef.Object)
	{
		ShoulderMoveAction = InputShoulderMoveRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputShoulderLookRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_ShoulderLook.IA_ShoulderLook'"));
	if (InputShoulderLookRef.Object)
	{
		ShoulderLookAction = InputShoulderLookRef.Object;	
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputQuarterMoveRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_QuarterMove.IA_QuarterMove'"));
	if (InputQuarterMoveRef.Object)
	{
		QuarterMoveAction = InputQuarterMoveRef.Object;
	}

	CurrentCharacterControlType = ECharacterControlType::Shoulder;

	static ConstructorHelpers::FObjectFinder<UInputAction> InputAttackRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_Attack.IA_Attack'"));
	if (InputAttackRef.Object)
	{
		AttackAction = InputAttackRef.Object;
	}
}

void AABCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	SetCharacterControl(CurrentCharacterControlType);

	if (!InputEnabled())
	{
		EnableInput(CastChecked<APlayerController>(GetController()));
	}

	// SetCanBeDamaged(false);
}

void AABCharacterPlayer::PostInitializeComponents()
{
	AB_LOG(LogABNetwork, Log, TEXT("Start"));
	
	Super::PostInitializeComponents();
	
	AB_LOG(LogABNetwork, Log, TEXT("End"));
}

void AABCharacterPlayer::PostNetInit()
{
	// 플레이어 컨트롤러의 동명 함수와 마찬가지로 클라이언트에서만 호출되는 함수
	// 
	AB_LOG(LogABNetwork, Log, TEXT("Start"));

	if (const AActor* OwnerActor = GetOwner())
	{
		AB_LOG(LogABNetwork, Log, TEXT("Before Super::PostNetInit(); Owner = %s"), *OwnerActor->GetName());
	}
	else
	{
		AB_LOG(LogABNetwork, Log, TEXT("Before Super::PostNetInit(); No Owner"));
	}
	
	Super::PostNetInit();

	if (const AActor* OwnerActor = GetOwner())
	{
		AB_LOG(LogABNetwork, Log, TEXT("After Super::PostNetInit(); Owner = %s"), *OwnerActor->GetName());
	}
	else
	{
		AB_LOG(LogABNetwork, Log, TEXT("After Super::PostNetInit(); No Owner"));
	}

	AB_LOG(LogABNetwork, Log, TEXT("End"));
}

void AABCharacterPlayer::PossessedBy(AController* NewController)
{
	// 호스트에서만 호출되는 함수, 클라이언트는 PostNetInit() 을 사용해야 한다.
	AB_LOG(LogABNetwork, Log, TEXT("Start"));

	if (const AActor* OwnerActor = GetOwner())
	{
		AB_LOG(LogABNetwork, Log, TEXT("Before Super::PossessedBy, Owner = %s"), *OwnerActor->GetName());
	}
	else
	{
		AB_LOG(LogABNetwork, Log, TEXT("Before Super::PossessedBy, No Owner"));
	}
	
	Super::PossessedBy(NewController);

	if (const AActor* OwnerActor = GetOwner())
	{
		AB_LOG(LogABNetwork, Log, TEXT("After Super::PossessedBy, Owner = %s"), *OwnerActor->GetName());
	}
	else
	{
		AB_LOG(LogABNetwork, Log, TEXT("After Super::PossessedBy, No Owner"));
	}
	
	AB_LOG(LogABNetwork, Log, TEXT("End"));
}

void AABCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// CastChecked함수로 인핸스트인풋모듈 사용을 강제한다.
	// UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	EnhancedInputComponent->BindAction(ChangeControlAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::ChangeCharacterControl);
	EnhancedInputComponent->BindAction(ShoulderMoveAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::ShoulderMove);
	EnhancedInputComponent->BindAction(ShoulderLookAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::ShoulderLook);
	EnhancedInputComponent->BindAction(QuarterMoveAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::QuarterMove);
	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::Attack);
}

void AABCharacterPlayer::ChangeCharacterControl()
{
	if (CurrentCharacterControlType == ECharacterControlType::Quarter)
	{
		SetCharacterControl(ECharacterControlType::Shoulder);
	}
	else if (CurrentCharacterControlType == ECharacterControlType::Shoulder)
	{
		SetCharacterControl(ECharacterControlType::Quarter);
	}
}

void AABCharacterPlayer::SetCharacterControl(const ECharacterControlType NewCharacterControlType)
{
	if (!IsLocallyControlled()) return;
	
	UABCharacterControlData* NewCharacterControl = CharacterControlDataMap[NewCharacterControlType];
	check(NewCharacterControl);

	SetCharacterControlData(NewCharacterControl);

	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();
		Subsystem->AddMappingContext(NewCharacterControl->InputMappingContext, 0);
	}

	CurrentCharacterControlType = NewCharacterControlType;
}

void AABCharacterPlayer::ShoulderMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();

	// Yaw는 z축 회전량, 캐릭터가 어느 방향을 보고있는지를 의미함.
	// Pitch와 Roll은 앞/옆구르기한다고 생각하면 될듯
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	AddMovementInput(ForwardDirection, MovementVector.X);
	AddMovementInput(RightDirection, MovementVector.Y);
}

void AABCharacterPlayer::ShoulderLook(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void AABCharacterPlayer::QuarterMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	float InputSizeSquared = MovementVector.SquaredLength();
	float MovementVectorSize = 1.0f;
	float MovementVectorSizeSquared = MovementVector.SquaredLength();
	if (MovementVectorSizeSquared > 1.0f)
	{
		MovementVector.Normalize();
		MovementVectorSizeSquared = 1.0f;
	}
	else
	{
		MovementVectorSize = FMath::Sqrt(MovementVectorSizeSquared);
	}

	FVector MoveDirection = FVector(MovementVector.X, MovementVector.Y, 0.0f);
	GetController()->SetControlRotation(FRotationMatrix::MakeFromX(MoveDirection).Rotator());
	AddMovementInput(MoveDirection, MovementVectorSize);
}

void AABCharacterPlayer::Attack()
{
	ProcessComboCommand();
}

bool AABCharacterPlayer::CanHit(const FHitResult& HitResult)
{
	AABCharacterNonPlayer* NonPlayerCharacter = Cast<AABCharacterNonPlayer>(HitResult.GetActor());
	return NonPlayerCharacter != nullptr;	// 플레이어가 아닌 캐릭터만 공격 가능
}

void AABCharacterPlayer::SetDead()
{
	Super::SetDead();

	if (InputEnabled())
	{
		DisableInput(CastChecked<APlayerController>(GetController()));
	}

	if (IABGameInterface* ABGameMode = Cast<IABGameInterface>(GetWorld()->GetAuthGameMode()))
	{
		ABGameMode->OnPlayerDead();
	}
}

void AABCharacterPlayer::SetupHUDWidget(UABHUDWidget* InHUDWidget)
{
	if (InHUDWidget)
	{
		// 순서 중요: Stat 초기화 할 때 MaxHp값이 세팅되기 때문에, HpBar 초기화보다 먼저 호출해야함
		InHUDWidget->UpdateStat(Stat->GetBaseStat(), Stat->GetModifierStat());
		Stat->OnStatChanged.AddUObject(InHUDWidget, &UABHUDWidget::UpdateStat);
		
		// HUD의 HpBar는 캐릭터와 무관하기 때문에, NativeConstruct() 에서 AABCharacterBase::SetCharacterWidget() 가 호출되지 않음
		// 그래서 여기서 딜리게이트를 등록해줘야 함
		InHUDWidget->UpdateHpBar(Stat->GetCurrentHp());
		Stat->OnHpChanged.AddUObject(InHUDWidget, &UABHUDWidget::UpdateHpBar);
	}
}

void AABCharacterPlayer::SetCharacterControlData(const UABCharacterControlData* CharacterControlData)
{
	Super::SetCharacterControlData(CharacterControlData);

	CameraBoom->TargetArmLength = CharacterControlData->TargetArmLength;
	CameraBoom->SetRelativeRotation(CharacterControlData->RelativeRotation);
	CameraBoom->bUsePawnControlRotation = CharacterControlData->bUsePawnControlRotation;
	CameraBoom->bInheritPitch = CharacterControlData->bInheritPitch;
	CameraBoom->bInheritYaw = CharacterControlData->bInheritYaw;
	CameraBoom->bInheritRoll = CharacterControlData->bInheritRoll;
	CameraBoom->bDoCollisionTest = CharacterControlData->bDoCollisionTest;
}
