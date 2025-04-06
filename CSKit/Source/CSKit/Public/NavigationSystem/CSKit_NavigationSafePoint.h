// Copyright 2020 megasuraman.
/**
 * @file CSKit_NavigationSafePoint.h
 * @brief 有効なNavMesh領域を求めるためプレイエリア内を示す座標
 * @author megasuraman
 * @date 2025/04/06
 */
#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "CSKit_NavigationSafePoint.generated.h"

UCLASS()
class CSKIT_API ACSKit_NavigationSafePoint : public ATargetPoint
{
	GENERATED_BODY()

public:
	ACSKit_NavigationSafePoint(const FObjectInitializer& ObjectInitializer);
};