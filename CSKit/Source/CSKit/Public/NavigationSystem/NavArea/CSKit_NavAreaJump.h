// Copyright 2020 megasuraman
/**
 * @file CSKit_NavAreaJump.h
 * @brief 
 * @author megasuraman
 * @date 2025/04/06
 */
#pragma once

#include "CoreMinimal.h"
#include "CSKit_NavAreaBase.h"
#include "CSKit_NavAreaJump.generated.h"

UCLASS()
class CSKIT_API UCSKit_NavAreaJump : public UCSKit_NavAreaBase
{
	GENERATED_BODY()

public:
	UCSKit_NavAreaJump(const class FObjectInitializer& ObjectInitializer)
		: Super(ObjectInitializer)
	{
		FCSKit_NavAreaHelper::Set(AreaFlags, ECSKit_NavAreaFlag::Jump);
		DefaultCost = 1.f;
	}
};
