// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_Utility.cpp
 * @brief 便利関数等のまとめ(分類しにくかった奴)
 * @author megasuraman
 * @date 2023/02/24
 */
#include "CSKitDebug_Utility.h"

#include "EnvironmentQuery/EnvQueryManager.h"
#include "NavigationSystem.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/DebugCameraController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"

#if WITH_EDITOR
#include "Editor.h"
#include "UnrealEdGlobals.h"
#include "Editor/UnrealEdEngine.h"
#endif

DEFINE_LOG_CATEGORY_STATIC(LogCSKitDebug_ScopeTime, Log, All);

/**
 * @brief	デバッグ表示用のObject名取得
 */
FString UCSKitDebug_Utility::GetObjectDisplayName(const UObject* InObject)
{
	if (const AActor* Actor = Cast<AActor>(InObject))
	{
#if WITH_EDITOR
		return Actor->GetActorLabel();
#else
		return Actor->GetName();
#endif
	}
	else if (const UActorComponent* ActorComponent = Cast<UActorComponent>(InObject))
	{
		if (const AActor* OwnerActor = ActorComponent->GetOwner())
		{
#if WITH_EDITOR
			return FString::Printf(TEXT("%s - %s"), *OwnerActor->GetActorLabel(), *ActorComponent->GetName());
#else
			return FString::Printf(TEXT("%s - %s"), *OwnerActor->GetName(), *ActorComponent->GetName());
#endif
		}
		return ActorComponent->GetName();
	}
	return FString(TEXT(""));
}

/**
 * @brief	最終のEQS情報取得
 */
FEnvQueryInstance* UCSKitDebug_Utility::FindLastEnvQueryInstance(float& OutLastTimeStamp, const APawn* InOwner)
{
	if (InOwner == nullptr)
	{
		return nullptr;
	}
#if USE_EQS_DEBUGGER
	UWorld* World = InOwner->GetWorld();
	UEnvQueryManager* QueryManager = UEnvQueryManager::GetCurrent(World);
	if (QueryManager == nullptr)
	{
		return nullptr;
	}

	const TArray<FEQSDebugger::FEnvQueryInfo>& QueryDataList = QueryManager->GetDebugger().GetAllQueriesForOwner(InOwner);
	int32 LastQueryDataIndex = INDEX_NONE;
	float MaxTimeStamp = 0.f;
	for (int32 i = 0; i < QueryDataList.Num(); ++i)
	{
		const FEQSDebugger::FEnvQueryInfo& Info = QueryDataList[i];
		if (Info.Timestamp > MaxTimeStamp)
		{
			MaxTimeStamp = Info.Timestamp;
			LastQueryDataIndex = i;
		}
	}

	if (LastQueryDataIndex == INDEX_NONE)
	{
		return nullptr;
	}

	OutLastTimeStamp = MaxTimeStamp;
	return QueryDataList[LastQueryDataIndex].Instance.Get();
#else
	return nullptr;
#endif
}

/**
 * @brief	プレイヤーキャラクターを非表示状態で指定Actorへ憑依させる
 */
void UCSKitDebug_Utility::MakeGhostPlayer(ACharacter* InPlayer, AActor* InTarget)
{
	FAttachmentTransformRules Rules(EAttachmentRule::KeepRelative, true);
	InPlayer->AttachToActor(InTarget, Rules);

	InPlayer->SetHidden(true);
	InPlayer->SetActorEnableCollision(false);
	InPlayer->GetCharacterMovement()->SetActive(false);
}

/**
 * @brief	デバッグ表示止めるべきか
 */
bool	UCSKitDebug_Utility::IsNeedStopDebugDraw(const UWorld* InWorld)
{
#if WITH_EDITOR
	if (InWorld
		&& InWorld->WorldType == EWorldType::Editor)
	{//自分がEditorのときにGame動いてたら表示しない
		const TIndirectArray<FWorldContext>& WorldContextList = GUnrealEd->GetWorldContexts();
		for (auto& WorldContext : WorldContextList)
		{
			if (WorldContext.WorldType == EWorldType::PIE)
			{
				return true;
			}
		}
	}
#endif
	return false;
}

/**
 * @brief	Actorが所属するレベル名を取得
 *			(サブレベル配置だったらサブレベル名)
 */
FString UCSKitDebug_Utility::GetActorLevelName(const AActor* InActor)
{
	if (InActor == nullptr)
	{
		return FString();
	}
	if (InActor->GetWorld()->WorldType == EWorldType::Editor)
	{
		return GetActorLevelNameInEditor(InActor);
	}
	return GetActorLevelNameInGame(InActor);
}
FString UCSKitDebug_Utility::GetActorLevelNameInGame(const AActor* InActor)
{
	if (InActor == nullptr)
	{
		return FString();
	}
	const ULevel* ActorLevel = InActor->GetLevel();
	for (ULevelStreaming* StreamingLevel : InActor->GetWorld()->GetStreamingLevels())
	{
		if (StreamingLevel
			&& StreamingLevel->GetLoadedLevel() == ActorLevel)
		{
			const FString PathString = StreamingLevel->PackageNameToLoad.ToString();
			TArray<FString> StringArray;
			PathString.ParseIntoArray(StringArray, TEXT("/"));
			if (StringArray.Num() > 0)
			{
				return StringArray[StringArray.Num() - 1];
			}
			return PathString;
		}
	}
	return FString();
}
FString UCSKitDebug_Utility::GetActorLevelNameInEditor(const AActor* InActor)
{
	if (InActor == nullptr)
	{
		return FString();
	}
	return GetLevelName(InActor->GetLevel());
}
FString UCSKitDebug_Utility::GetLevelName(const ULevel* InLevel)
{
	if (InLevel == nullptr)
	{
		return FString();
	}
	const FString LevelPathName = InLevel->GetPathName();
	TArray<FString> PathList;
	LevelPathName.ParseIntoArray(PathList, TEXT("/"));
	FString LastPath = LevelPathName;
	if (PathList.Num() > 0)
	{
		LastPath = PathList[PathList.Num() - 1];
	}
	FString LevelName;
	FString PersistentLevel;
	LastPath.Split(FString(TEXT(".")), &LevelName, &PersistentLevel);
	return LevelName;
}

/**
 * @brief	操作中のPlayerController
 */
APlayerController* UCSKitDebug_Utility::FindMainPlayerController(const UWorld* InWorld)
{
	for (FConstPlayerControllerIterator It = InWorld->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PlayerController = It->Get();
		if (PlayerController == nullptr)
		{
			continue;
		}
		if (Cast<ULocalPlayer>(PlayerController->Player) == nullptr)
		{
			continue;
		}
		return PlayerController;
	}
	return nullptr;
}

/**
 * @brief	操作中のPlayerControllerに応じたデバッグ用座標情報
 */
bool UCSKitDebug_Utility::CalcLocalPlayerDebugPosition(FVector& OutPos, FRotator& OutRot, const UWorld* InWorld)
{
	const APlayerController* PlayerController = FindMainPlayerController(InWorld);
	if (PlayerController == nullptr)
	{
		return false;
	}

	if (const ADebugCameraController* DebugCameraController = Cast<ADebugCameraController>(PlayerController))
	{
		FVector CamLoc;
		FRotator CamRot;
		DebugCameraController->GetPlayerViewPoint(CamLoc, CamRot);

		const FVector CameraTipPos = CamLoc + CamRot.RotateVector(FVector(2000.f, 0.f, 0.f));
		FHitResult HitResult;
		if (InWorld->LineTraceSingleByChannel(
			HitResult,
			CamLoc,
			CameraTipPos,
			ECC_WorldStatic)
			)
		{
			OutPos = HitResult.ImpactPoint;
		}
		else
		{
			OutPos = CameraTipPos;
		}
		OutRot = CamRot;
		OutRot.Pitch = 0.f;
		return true;
	}
	else if (const APawn* ControllerPawn = PlayerController->GetPawn())
	{
		OutPos = ControllerPawn->GetActorLocation();
		OutRot = ControllerPawn->GetActorRotation();
		return false;
	}
	return false;
}

/**
 * @brief	地面や天井のコリジョンに問題ないかチェック
 */
FString UCSKitDebug_Utility::CheckFitCollision(const UWorld* InWorld, const FVector& InPos, const FVector& InCheckNV, const ECollisionChannel InCollisionChannel, const float InCheckHeightSpace, const float InSafeDiffHeight, const float InSafeFloorAngle)
{
	const FVector OutsidePos = InPos + InCheckNV * InCheckHeightSpace;
	const FVector InsidePos = InPos - InCheckNV * InCheckHeightSpace;
	FHitResult HitResult;
	//すぐ上に何かあったらアウト
	if (InWorld->LineTraceSingleByChannel(
		HitResult,
		InPos,
		OutsidePos,
		InCollisionChannel)
		)
	{
		return FString(TEXT("障害物アリ"));
	}

	//上から下に何もなかったらアウト
	if (!InWorld->LineTraceSingleByChannel(
		HitResult,
		OutsidePos,
		InsidePos,
		InCollisionChannel)
		)
	{
		return FString(TEXT("離れすぎ"));
	}

	if (FVector::DistSquared(InPos, HitResult.ImpactPoint) > FMath::Square(InSafeDiffHeight))
	{
		//埋まってたらアウト
		const FVector Pos2ImpactNV = FVector(HitResult.ImpactPoint - InPos).GetSafeNormal();
		if (FVector::DotProduct(Pos2ImpactNV, InCheckNV) >= 0.f)
		{
			return FString(TEXT("埋まってる"));
		}
		return FString(TEXT("離れすぎ"));
	}

	//斜面過ぎるのはアウト
	const float SlopeAngle = FMath::RadiansToDegrees(FMath::Abs(FMath::Acos(FVector::DotProduct(HitResult.ImpactNormal, InCheckNV))));
	if (SlopeAngle > InSafeFloorAngle)
	{
		return FString(TEXT("斜面過ぎる"));
	}
	return FString();
}

/**
 * @brief	地面コリジョンにフィットしてるかチェック
 */
FString UCSKitDebug_Utility::CheckFitGround(
	const UWorld* InWorld,
	const FVector& InPos,
	const ECollisionChannel InCollisionChannel,
	const float InCheckHeightSpace,
	const float InSafeDiffHeight,
	const float InSafeFloorAngle
)
{
	const FVector UpPos = InPos + FVector(0.f, 0.f, InCheckHeightSpace);
	const FVector DownPos = InPos - FVector(0.f, 0.f, InCheckHeightSpace);
	FHitResult HitResult;
	//すぐ上に何かあったらアウト
	if (InWorld->LineTraceSingleByChannel(
		HitResult,
		InPos + FVector(0.f, 0.f, 1.f),
		UpPos,
		InCollisionChannel)
		)
	{
		return FString(TEXT("頭上に障害物アリ"));
	}

	//上から下に何もなかったらアウト
	if (!InWorld->LineTraceSingleByChannel(
		HitResult,
		UpPos,
		DownPos,
		InCollisionChannel)
		)
	{
		return FString(TEXT("空中に浮いてる"));
	}
	if (InPos.Z > HitResult.ImpactPoint.Z + InSafeDiffHeight)
	{
		return FString(TEXT("空中に浮いてる"));
	}

	//埋まってたらアウト
	if (HitResult.ImpactPoint.Z > InPos.Z + InSafeDiffHeight)
	{
		return FString(TEXT("地面に埋まってる"));
	}

	//斜面過ぎるのはアウト
	const float SlopeAngle = FMath::RadiansToDegrees(FMath::Abs(FMath::Acos(FVector::DotProduct(HitResult.ImpactNormal, FVector::UpVector))));
	if (SlopeAngle > InSafeFloorAngle)
	{
		return FString(TEXT("斜面過ぎる"));
	}
	return FString();
}

/**
 * @brief	壁に触れてるか
 */
FString UCSKitDebug_Utility::CheckTouchWall(
	const UWorld* InWorld,
	const FVector& InPos,
	const ECollisionChannel InCollisionChannel,
	const float InCheckRadius,
	const float InCheckAngleInterval
)
{
	float CheckAngle = 0.f;
	while (CheckAngle < 360.f)
	{
		const FRotator CheckRot(0.f, CheckAngle, 0.f);
		const FVector CheckV = CheckRot.RotateVector(FVector(InCheckRadius, 0.f, 0.f));
		FHitResult HitResult;
		if (InWorld->LineTraceSingleByChannel(
			HitResult,
			InPos,
			InPos + CheckV,
			InCollisionChannel)
			)
		{
			return FString(TEXT("壁に触れてる"));
		}

		CheckAngle += InCheckAngleInterval;
	}
	return FString();
}

/**
 * @brief	Navmeshに触れてるか
 */
bool UCSKitDebug_Utility::IsTouchNavmesh(
	const UWorld* InWorld,
	const FVector& InPos,
	const FVector& InExtent
)
{
	const UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(InWorld);
	if (NavSys == nullptr)
	{
		return false;
	}

	const ANavigationData* NavData = NavSys->GetNavDataForProps(FNavAgentProperties::DefaultProperties, InPos);
	if (NavData == nullptr)
	{
		return false;
	}

	FNavLocation HitPos;
	return NavData->ProjectPoint(InPos, HitPos, InExtent);
}

/**
 * @brief	この構造体の宣言するだけで簡易的にスコープ内の処理時間を計測できる
 */
UCSKitDebug_Utility::LogScopeTime::LogScopeTime(const FString& InTitle)
	:mTitle(InTitle)
{
	mBeginTime = FPlatformTime::Seconds();
}
UCSKitDebug_Utility::LogScopeTime::~LogScopeTime()
{
	UE_LOG(LogCSKitDebug_ScopeTime, Log, TEXT("%s [%.5lfs]"), *mTitle, FPlatformTime::Seconds() - mBeginTime);
}

/**
 * @brief	指定Objectが依存しているObjectを全て収集
 */
void UCSKitDebug_Utility::CollectAssetDependency(TArray<FAssetDependency>& OutList, const UObject* InObject)
{
	if(InObject == nullptr)
	{
		return;
	}
	const UPackage* CurrentPackage = InObject->GetPackage();
	if(CurrentPackage == nullptr)
	{
		return;
	}
	
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	
	TArray<FAssetDependency> NeedCheckDependencyList;
	const FName RootObjectName = CurrentPackage->GetFName();

	FAssetDependency RootData;
	RootData.mAssetPathName = RootObjectName;
	NeedCheckDependencyList.Add(RootData);
	
	while (NeedCheckDependencyList.Num() > 0)
	{
		FAssetDependency TargetDataCopy = NeedCheckDependencyList[0];
		const FName TargetName = TargetDataCopy.mAssetPathName;
		TArray<FName> DependencyNameList;
		constexpr UE::AssetRegistry::EDependencyCategory DependencyCategory = UE::AssetRegistry::EDependencyCategory::Package;
		const UE::AssetRegistry::FDependencyQuery DependencyQuery( UE::AssetRegistry::EDependencyQuery::Game );
		AssetRegistryModule.Get().GetDependencies(TargetName, DependencyNameList, DependencyCategory, DependencyQuery);
		
		NeedCheckDependencyList.RemoveAt(0);
		for(const FName& Name : DependencyNameList)
		{
			FAssetDependency TempData;
			TempData.mAssetPathName = Name;
			if(OutList.Find(TempData) == INDEX_NONE)
			{
				FAssetDependency ChildData;
				ChildData.mAssetPathName = Name;
				ChildData.mRoot = TargetDataCopy.mRoot;
				ChildData.mRoot.Add(TargetName);
				NeedCheckDependencyList.AddUnique(ChildData);
			}
		}

		OutList.AddUnique(TargetDataCopy);
	}

	if(OutList.Num() > 0)
	{//先頭はInObjectなのでいらない
		OutList.RemoveAt(0);
	}
}

/**
 * @brief	指定Objectを参照しているObjectを全て収集
 */
void UCSKitDebug_Utility::CollectAssetReferencer(TArray<FAssetDependency>& OutList, const UObject* InObject)
{
	if(InObject == nullptr)
	{
		return;
	}
	const UPackage* CurrentPackage = InObject->GetPackage();
	if(CurrentPackage == nullptr)
	{
		return;
	}
	
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	
	TArray<FAssetDependency> NeedCheckDependencyList;
	const FName RootObjectName = CurrentPackage->GetFName();

	FAssetDependency RootData;
	RootData.mAssetPathName = RootObjectName;
	NeedCheckDependencyList.Add(RootData);
	
	while (NeedCheckDependencyList.Num() > 0)
	{
		FAssetDependency TargetDataCopy = NeedCheckDependencyList[0];
		const FName TargetName = TargetDataCopy.mAssetPathName;
		TArray<FName> DependencyNameList;
		constexpr UE::AssetRegistry::EDependencyCategory DependencyCategory = UE::AssetRegistry::EDependencyCategory::Package;
		const UE::AssetRegistry::FDependencyQuery DependencyQuery( UE::AssetRegistry::EDependencyQuery::Game );
		AssetRegistryModule.Get().GetReferencers(TargetName, DependencyNameList, DependencyCategory, DependencyQuery);
		
		NeedCheckDependencyList.RemoveAt(0);
		for(const FName& Name : DependencyNameList)
		{
			FAssetDependency TempData;
			TempData.mAssetPathName = Name;
			if(OutList.Find(TempData) == INDEX_NONE)
			{
				FAssetDependency ChildData;
				ChildData.mAssetPathName = Name;
				ChildData.mRoot = TargetDataCopy.mRoot;
				ChildData.mRoot.Add(TargetName);
				NeedCheckDependencyList.AddUnique(ChildData);
			}
		}

		OutList.AddUnique(TargetDataCopy);
	}

	if(OutList.Num() > 0)
	{//先頭はInObjectなのでいらない
		OutList.RemoveAt(0);
	}
}