// Copyright 2020 megasuraman.
/**
 * @file CSKit_NavigationSafePoint.cpp
 * @brief 有効なNavMesh領域を求めるためプレイエリア内を示す座標
 * @author megasuraman
 * @date 2025/04/06
 */
#include "NavigationSystem/CSKit_NavigationSafePoint.h"

ACSKit_NavigationSafePoint::ACSKit_NavigationSafePoint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetActorTickEnabled(false);
	bIsEditorOnlyActor = true;
	bNetLoadOnClient = false;
}