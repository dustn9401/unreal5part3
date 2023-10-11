// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ABCharacterPlayer.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

AABCharacterPlayer::AABCharacterPlayer()
{
	// Camera 설정
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	
	// 이렇게 두 번째 파라미터로 위치값이 아니라 특정한 FName값을 넘겨 사용하는 경우도 잇음
	// USpringArmComponent::SocketName : The name of the socket at the end of the spring arm (looking back towards the spring arm origin)
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
}
