#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ABCharacterStat.generated.h"

// 구조체의 크기를 float 크기로 나눈 값으로 멤버변수 개수를 구하기 때문에 float 고정할것
USTRUCT(BlueprintType)
struct FABCharacterStat : public FTableRowBase
{
	GENERATED_BODY()

public:
	FABCharacterStat() : MaxHp(0.0f), Attack(0.0f), AttackRange(0.0f), AttackSpeed(0.0f), MovementSpeed(0.0f)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float MaxHp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float Attack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float AttackSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float MovementSpeed;

	FABCharacterStat operator+(const FABCharacterStat& Other) const
	{
		const float* const ThisPtr = reinterpret_cast<const float* const>(this);
		const float* const OtherPtr = reinterpret_cast<const float* const>(&Other);

		FABCharacterStat Result;
		float* ResultPtr = reinterpret_cast<float*>(&Result);
		constexpr int32 StatNum = sizeof(FABCharacterStat) / sizeof(float);
		for (int32 i = 0; i < StatNum; i++)
		{
			ResultPtr[i] = ThisPtr[i] + OtherPtr[i];
		}

		return Result;
	}
	
	FABCharacterStat& operator+=(const FABCharacterStat& Other)
	{
		float* const ThisPtr = reinterpret_cast<float* const>(this);
		const float* const OtherPtr = reinterpret_cast<const float* const>(&Other);
		
		constexpr int32 StatNum = sizeof(FABCharacterStat) / sizeof(float);
		for (int32 i = 0; i < StatNum; i++)
		{
			ThisPtr[i] += OtherPtr[i];
		}

		return *this;
	}
	
	FString ToString() const
	{
		return FString::Printf(TEXT("MaxHp: %f, Attack: %f, AttackRange: %f, AttackSpeed: %f, MovementSpeed: %f"), MaxHp, Attack, AttackRange, AttackSpeed, MovementSpeed);
	}
};