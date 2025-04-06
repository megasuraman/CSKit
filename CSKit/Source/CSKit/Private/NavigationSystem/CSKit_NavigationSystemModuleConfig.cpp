// Copyright 2020 megasuraman
/**
 * @file CSKit_NavigationSystemModuleConfig.cpp
 * @brief 
 * @author megasuraman
 * @date 2025/04/06
 */
#include "NavigationSystem/CSKit_NavigationSystemModuleConfig.h"

#include "NavigationSystem/CSKit_NavigationSystem.h"

UCSKit_NavigationSystemModuleConfig::UCSKit_NavigationSystemModuleConfig(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NavigationSystemClass = FSoftClassPath(UCSKit_NavigationSystem::StaticClass());
	bStrictlyStatic = false;
}

// void UCSKit_NavigationSystemModuleConfig::PostInitProperties()
// {
// 	Super::PostInitProperties();
//
// 	bStrictlyStatic = false;//ホントはWorldSettingでの設定に沿いたいが、基底側で上書きされるので強制的に指定
// 	UCSKit_NavigationSystem::SetStaticRuntimeNavigation(bStrictlyStatic);
// }

void UCSKit_NavigationSystemModuleConfig::PostLoad()
{
	Super::PostLoad();

	NavigationSystemClass = FSoftClassPath(UCSKit_NavigationSystem::StaticClass());
}