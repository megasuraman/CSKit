// Copyright 2020 megasuraman
/**
 * @file CSKit_NavigationSystemModuleConfig.h
 * @brief 
 * @author megasuraman
 * @date 2025/04/06
 */
#pragma once

#include "CoreMinimal.h"
#include "NavigationSystem.h"
#include "CSKit_NavigationSystemModuleConfig.generated.h"


UCLASS()
class CSKIT_API UCSKit_NavigationSystemModuleConfig : public UNavigationSystemModuleConfig
{
	GENERATED_BODY()
	
public:
	UCSKit_NavigationSystemModuleConfig(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void PostLoad() override;
};
