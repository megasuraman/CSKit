// Copyright 2020 megasuraman.
/**
 * @file CSKit_NavLinkProxyDoor.h
 * @brief 
 * @author megasuraman
 * @date 2025/04/06
 */
#pragma once

#include "CoreMinimal.h"
#include "CSKit_NavLinkProxySimpleLink.h"
#include "CSKit_NavLinkProxyDoor.generated.h"

UCLASS()
class CSKIT_API ACSKit_NavLinkProxyDoor : public ACSKit_NavLinkProxySimpleLink
{
	GENERATED_BODY()

#if USE_CSKIT_DEBUG
protected:
	virtual UClass* DebugGetAssignNavArea() const override;
#endif
};
