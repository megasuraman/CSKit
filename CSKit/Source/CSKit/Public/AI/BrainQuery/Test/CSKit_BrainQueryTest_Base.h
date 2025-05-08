// Copyright 2020 megasuraman.
/**
 * @file CSKit_BrainQueryTest_Base.h
 * @brief BrainQueryで行う各種テストの基礎クラス
 * @author megasuraman
 * @date 2025/05/05
 */ 
#pragma once

#include "CoreMinimal.h"
#include "CSKit_BrainQueryTest_Base.generated.h"

class ACSKit_AIController;
struct FCSKit_BrainQueryTestNode;

UCLASS(BlueprintType)
class CSKIT_API UCSKit_BrainQueryTest_Base : public UObject
{
	GENERATED_BODY()

public:
	virtual	float	CalcScore(const ACSKit_AIController& InOwner, const FCSKit_BrainQueryTestNode& InTestNode) const
	{
		return 0.f;
	}
protected:
	static float CheckScoreRangeValue(const float InScore, const FCSKit_BrainQueryTestNode& InTestNode);
};
