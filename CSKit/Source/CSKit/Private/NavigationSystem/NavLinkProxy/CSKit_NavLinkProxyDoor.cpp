// Copyright 2020 megasuraman.
/**
 * @file CSKit_NavLinkProxyDoor.cpp
 * @brief 
 * @author megasuraman
 * @date 2025/04/06
 */

#include "NavigationSystem/NavLinkProxy/CSKit_NavLinkProxyDoor.h"

#include "NavigationSystem/NavArea/CSKit_NavAreaDoor.h"


#if USE_CSKIT_DEBUG
UClass* ACSKit_NavLinkProxyDoor::DebugGetAssignNavArea() const
{
	return UCSKit_NavAreaDoor::StaticClass();
}
#endif