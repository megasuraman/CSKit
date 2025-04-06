// Copyright 2020 megasuraman.
/**
 * @file CSKit_NavLinkProxySimpleLink.h
 * @brief 
 * @author megasuraman
 * @date 2025/04/06
 */
#pragma once

#include "CoreMinimal.h"
#include "Navigation/NavLinkProxy.h"
#include "CSKit_NavLinkProxySimpleLink.generated.h"

UCLASS()
class CSKIT_API ACSKit_NavLinkProxySimpleLink : public ANavLinkProxy
{
	GENERATED_BODY()

public:
	ACSKit_NavLinkProxySimpleLink(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitProperties() override;

protected:

#if USE_CSKIT_DEBUG
public:
	struct FDebugPointLink {
		FVector mLeft = FVector::ZeroVector;
		FVector mRight = FVector::ZeroVector;
	};
	static FVector DebugGetGroundPos(const UWorld* InWorld, const FVector& InPos);
	void DebugAssignLink(const TArray<FDebugPointLink>& InList);
	void DebugDraw(class UCanvas* InCanvas) const;
protected:
	virtual UClass* DebugGetAssignNavArea() const;
	virtual ENavLinkDirection::Type DebugGetAssignDirection() const;
#endif
};