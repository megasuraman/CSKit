// Copyright 2020 megasuraman.
/**
 * @file CSKit_Config.cpp
 * @brief 
 * @author megasuraman
 * @date 2025/05/06
 */
#include "CSKit_Config.h"

#include "AI/CSKit_NeedLevelManager.h"
#include "InputCoreTypes.h"
#include "AI/Worry/CSKit_WorrySourceManager.h"


UCSKit_Config::UCSKit_Config()
{
	mNeedLevelClass = UCSKit_NeedLevelManager::StaticClass();
	mWorrySourceManagerClass = UCSKit_WorrySourceManager::StaticClass();
}
