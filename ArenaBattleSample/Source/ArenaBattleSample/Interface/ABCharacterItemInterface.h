// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ABCharacterItemInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UABCharacterItemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * ItemBox가 자신과 충돌한 캐릭터에 호출
 */
class ARENABATTLESAMPLE_API IABCharacterItemInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void TakeItem(class UABItemData* InItemData) = 0;
};
