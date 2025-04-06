// Copyright 2020 megasuraman
/**
 * @file CSKit_NavAreaBase.h
 * @brief 
 * @author megasuraman
 * @date 2025/04/06
 */
#pragma once

#include "CoreMinimal.h"
#include "NavAreas/NavArea.h"
#include "NavMesh/NavMeshPath.h"
#include "CSKit_NavAreaBase.generated.h"

UENUM()
enum class ECSKit_NavAreaFlag : uint16
{
	Default,
	Jump,
	Door,
};

namespace FCSKit_NavAreaHelper
{
	FORCEINLINE bool IsSet(uint16 Flags, ECSKit_NavAreaFlag Bit) { return (Flags & (1 << static_cast<uint16>(Bit))) != 0; }
	FORCEINLINE void Set(uint16& Flags, ECSKit_NavAreaFlag Bit) { Flags |= (1 << static_cast<uint16>(Bit)); }

	FORCEINLINE bool IsNavLink(const FNavPathPoint& PathVert) { return (FNavMeshNodeFlags(PathVert.Flags).PathFlags & RECAST_STRAIGHTPATH_OFFMESH_CONNECTION) != 0; }
	FORCEINLINE bool HasJumpFlag(const FNavPathPoint& PathVert) { return IsSet(FNavMeshNodeFlags(PathVert.Flags).AreaFlags, ECSKit_NavAreaFlag::Jump); }
	FORCEINLINE bool HasDoorFlag(const FNavPathPoint& PathVert) { return IsSet(FNavMeshNodeFlags(PathVert.Flags).AreaFlags, ECSKit_NavAreaFlag::Door); }
}

UCLASS(Abstract)
class CSKIT_API UCSKit_NavAreaBase : public UNavArea
{
	GENERATED_BODY()
	
};