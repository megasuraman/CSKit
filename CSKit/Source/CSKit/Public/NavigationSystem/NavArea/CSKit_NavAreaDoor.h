// Copyright 2020 megasuraman
/**
 * @file CSKit_NavAreaDoor.h
 * @brief 
 * @author megasuraman
 * @date 2025/04/06
 */
#pragma once

#include "CoreMinimal.h"
#include "CSKit_NavAreaBase.h"
#include "CSKit_NavAreaDoor.generated.h"

UCLASS()
class CSKIT_API UCSKit_NavAreaDoor : public UCSKit_NavAreaBase
{
	GENERATED_BODY()

public:
	UCSKit_NavAreaDoor(const class FObjectInitializer& ObjectInitializer)
		: Super(ObjectInitializer)
	{
		FCSKit_NavAreaHelper::Set(AreaFlags, ECSKit_NavAreaFlag::Door);
		DefaultCost = 1.f;
	}
};
