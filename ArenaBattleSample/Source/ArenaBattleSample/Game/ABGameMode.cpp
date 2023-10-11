// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ABGameMode.h"

AABGameMode::AABGameMode()
{
	// 에셋의 ReferencePath를 복사해서 그대로 사용하지 않고, 아래와 같이 수정해준다.
	// 끝에 _C를 추가한 것은 클래스 정보를 가져온다는 의미임
	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassRef(TEXT("/Script/ArenaBattleSample.ABCharacterPlayer"));
	if (DefaultPawnClassRef.Class)
	{
		DefaultPawnClass = DefaultPawnClassRef.Class;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ThirdPersonClassRef.Class is null!!"))
	}

	// PlayerControllerClass = AABPlayerController::StaticClass();	// 헤더를 include한다면 이렇게 사용하면 되고
	// 헤더를 include 하지 않고 ConstructorHelpers를 사용할 수도 있다. 이 경우에는 블루프린트가 아니므로 _C 를 안붙임
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassRef(TEXT("/Script/ArenaBattleSample.ABPlayerController"));
	if (PlayerControllerClassRef.Class)
	{
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}
}
