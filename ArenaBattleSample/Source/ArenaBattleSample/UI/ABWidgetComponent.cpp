// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ABWidgetComponent.h"

#include "ABUserWidget.h"

void UABWidgetComponent::InitWidget()
{
	Super::InitWidget();

	if (UABUserWidget* ABUserWidget = Cast<UABUserWidget>(GetWidget()))
	{
		ABUserWidget->SetOwningActor(GetOwner());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ABWidgetComponent를 사용하는데, ABUserWidget이 아님!!"))
	}
}
