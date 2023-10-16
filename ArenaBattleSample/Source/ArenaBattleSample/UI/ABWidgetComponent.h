// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "ABWidgetComponent.generated.h"

/**
 * 위젯 초기화 관련 편의성을 위한 클래스
 */
UCLASS()
class ARENABATTLESAMPLE_API UABWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

protected:
	virtual void InitWidget() override;
};
