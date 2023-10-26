// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ABUserWidget.generated.h"

/**
 * UABWidgetComponent 전용 위젯
 * 액터의 BeginPlay() 시점에 OwningActor 값 세팅됨
 */
UCLASS()
class ARENABATTLESAMPLE_API UABUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FORCEINLINE void SetOwningActor(AActor* NewOwner) {OwningActor = NewOwner;}

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Actor")
	TObjectPtr<AActor> OwningActor;	// 현재 위젯을 소유하고 있는 액터
};
