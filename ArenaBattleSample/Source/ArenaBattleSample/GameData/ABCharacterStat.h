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

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		{
			uint32 MaxHpInt = static_cast<uint32>(MaxHp);
			Ar.SerializeIntPacked(MaxHpInt);
			MaxHp = static_cast<float>(MaxHpInt);
		}

		{
			uint32 AttackInt = static_cast<uint32>(Attack);
			Ar.SerializeIntPacked(AttackInt);
			Attack = static_cast<float>(AttackInt);
		}

		{
			uint32 AttackRangeInt = static_cast<uint32>(AttackRange);
			Ar.SerializeIntPacked(AttackRangeInt);
			AttackRange = static_cast<float>(AttackRangeInt);
		}

		{
			uint32 AttackSpeedInt = static_cast<uint32>(AttackSpeed);
			Ar.SerializeIntPacked(AttackSpeedInt);
			AttackSpeed = static_cast<float>(AttackSpeedInt);
		}

		{
			uint32 MovementSpeedInt = static_cast<uint32>(MovementSpeed);
			Ar.SerializeIntPacked(MovementSpeedInt);
			MovementSpeed = static_cast<float>(MovementSpeedInt);
		}
		
		return true;
	}
};

template<>
struct TStructOpsTypeTraits<FABCharacterStat> : public TStructOpsTypeTraitsBase2<FABCharacterStat>
{
	enum
	{
		WithNetSerializer = true,
	};
};