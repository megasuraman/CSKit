// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_Utility.h
 * @brief カテゴライズできなかった機能達
 * @author megasuraman
 * @date 2023/02/24
 */

#pragma once

#include "CoreMinimal.h"
#include "CSKitDebug_Utility.generated.h"

class APawn;
struct FEnvQueryInstance;


UCLASS()
class CSKITDEBUG_API UCSKitDebug_Utility : public UObject
{
	GENERATED_BODY()
	
public:
	template<typename ENUM>
	static FString	GetUEnumString(const ENUM InEnum)
	{
		const int64 EnumValue = static_cast<int64>(InEnum);
		if (const UEnum* Enum = StaticEnum<ENUM>())
		{
			return Enum->GetNameStringByValue(EnumValue);
		}
		return FString::Printf(TEXT("%lld"), EnumValue);
	}
	static FString GetObjectDisplayName(const UObject* InObject);
	static FEnvQueryInstance* FindLastEnvQueryInstance(float& OutLastTimeStamp, const APawn* InOwner);
	static void MakeGhostPlayer(ACharacter* InPlayer, AActor* InTarget);
	static bool	IsNeedStopDebugDraw(const UWorld* InWorld);
	static FString GetActorLevelName(const AActor* InActor);
	static FString GetActorLevelNameInGame(const AActor* InActor);
	static FString GetActorLevelNameInEditor(const AActor* InActor);
	static FString GetLevelName(const ULevel* InLevel);
	static APlayerController* FindMainPlayerController(const UWorld* InWorld);
	static bool CalcLocalPlayerDebugPosition(FVector& OutPos, FRotator& OutRot, const UWorld* InWorld);
	static FString CheckFitCollision(
		const UWorld* InWorld,
		const FVector& InPos,
		const FVector& InCheckNV,
		const ECollisionChannel InCollisionChannel,
		const float InCheckHeightSpace = 200.f,
		const float InSafeDiffHeight = 5.f,
		const float InSafeFloorAngle = 40.f
	);
	static FString CheckFitGround(
		const UWorld* InWorld,
		const FVector& InPos,
		const ECollisionChannel InCollisionChannel,
		const float InCheckHeightSpace = 200.f,
		const float InSafeDiffHeight = 5.f,
		const float InSafeFloorAngle = 40.f
	);
	static FString CheckTouchWall(
		const UWorld* InWorld,
		const FVector& InPos,
		const ECollisionChannel InCollisionChannel,
		const float InCheckRadius = 50.f,
		const float InCheckAngleInterval = 60.f
	);
	static bool IsTouchNavmesh(
		const UWorld* InWorld,
		const FVector& InPos,
		const FVector& InExtent = FVector(50.f, 50.f, 100.f)
	);

	//簡易的にスコープ内の処理時間計測
	struct CSKITDEBUG_API LogScopeTime
	{
		LogScopeTime() {}
		LogScopeTime(const FString& InTitle);
		~LogScopeTime();

		FString mTitle;
		double mBeginTime = 0;
	};
	
	struct CSKITDEBUG_API FAssetDependency
	{
		FName mAssetPathName;//参照アセットパス
		TArray<FName> mRoot;//参照経路
		bool operator==(const FAssetDependency& InTarget) const
		{
			return mAssetPathName == InTarget.mAssetPathName;
		}
	};
	// 指定Objectが依存しているObjectを全て収集
	static void CollectAssetDependency(TArray<FAssetDependency>& OutList, const UObject* InObject);
	// 指定Objectが参照しているObjectを全て収集
	static void CollectAssetReferencer(TArray<FAssetDependency>& OutList, const UObject* InObject);
};
