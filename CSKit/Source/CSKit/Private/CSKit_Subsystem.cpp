// Copyright 2020 megasuraman.
/**
 * @file CSKit_SubSystem.h
 * @brief CSKit用のSubSystem
 * @author megasuraman
 * @date 2025/05/06
 */
#include "CSKit_Subsystem.h"

#include "CSKit_Config.h"
#include "AI/Experience/CSKit_ExperienceManager.h"
#include "AI/CSKit_NeedLevelManager.h"
#include "AI/Notice/CSKit_NoticeTargetManager.h"
#include "AI/Community/CSKit_CommunityManager.h"
#include "Debug/DebugDrawService.h"
#include "Engine/Canvas.h"
#include "GameFramework/Actor.h"
#include "AI/Territory/CSKit_TerritoryVolume.h"
#include "AI/Worry/CSKit_WorrySourceManager.h"

#if CSKIT_USE_ENVIRONMENT_MAP
#include "AI/EnvironmentMap/CSKit_EnvironmentMap.h"
#endif

DEFINE_LOG_CATEGORY_STATIC(LogCSKit_SubSystem, Warning, All);

CSKit_XorShift UCSKit_Subsystem::msRand;

/**
 * @brief 
 */
bool UCSKit_Subsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer))
	{
		return false;
	}

	const UWorld* World = Cast<UWorld>(Outer);
	check(World);
	if (World->WorldType == EWorldType::Editor)
	{
		return false;
	}

// 	UWorld* World = Cast<UWorld>(Outer);
// 	check(World);
// 	if (!World->IsServer())このタイミングだとまだ取れないっぽい
// 	{
// 		return false;
// 	}
	return true;
}

/**
 * @brief 
 */
void UCSKit_Subsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	//if (GetWorld()->IsServer())このタイミングだと判定できない
	{
		mCommunityManager = NewObject<UCSKit_CommunityManager>(this);
		
		const UCSKit_Config* CSKitConfig = GetDefault<UCSKit_Config>();
		if (const TSubclassOf<UCSKit_NeedLevelManager> NeedLevelManagerClass = CSKitConfig->mNeedLevelClass.IsValid() ? LoadClass<UCSKit_NeedLevelManager>(nullptr, *CSKitConfig->mNeedLevelClass.ToString(), nullptr, LOAD_None, nullptr) : nullptr)
		{
			mNeedLevelManager = NewObject<UCSKit_NeedLevelManager>(this, NeedLevelManagerClass, TEXT("NeedLevelManager"));
		}
		else
		{
			mNeedLevelManager = NewObject<UCSKit_NeedLevelManager>(this);
		}
		
		mNoticeTargetManager = NewObject<UCSKit_NoticeTargetManager>(this);

		if (const TSubclassOf<UCSKit_WorrySourceManager> WorrySourceManagerClass = CSKitConfig->mWorrySourceManagerClass.IsValid() ? LoadClass<UCSKit_WorrySourceManager>(nullptr, *CSKitConfig->mWorrySourceManagerClass.ToString(), nullptr, LOAD_None, nullptr) : nullptr)
		{
			mWorrySourceManager = NewObject<UCSKit_WorrySourceManager>(this, WorrySourceManagerClass, TEXT("WorrySourceManager"));
		}
		else
		{
			mWorrySourceManager = NewObject<UCSKit_WorrySourceManager>(this);
		}
		
		mExperienceManager = NewObject<UCSKit_ExperienceManager>(this);
	}

	const FDateTime& CurDateTime = FDateTime::Now();
	const uint32 SeedValue = CurDateTime.GetSecond() * 1000 + CurDateTime.GetMillisecond();
	InitXorShiftRand(SeedValue);

#if USE_CSKIT_DEBUG
	DebugRequestDraw(true);
#endif
}

/**
 * @brief 
 */
void UCSKit_Subsystem::Deinitialize()
{
	Super::Deinitialize();
#if USE_CSKIT_DEBUG
	DebugRequestDraw(false);
#endif
}

void UCSKit_Subsystem::Tick(float DeltaTime)
{
	if (!GetWorld()->IsNetMode(NM_Client))
	{
		if (mCommunityManager)
		{
			mCommunityManager->Update(DeltaTime);
		}
		if (mNeedLevelManager)
		{
			mNeedLevelManager->Update(DeltaTime);
		}
		if (mNoticeTargetManager)
		{
			mNoticeTargetManager->Update(DeltaTime);
		}
		if (mWorrySourceManager)
		{
			mWorrySourceManager->Update(DeltaTime);
		}
	}
}
TStatId UCSKit_Subsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UCSKit_Subsystem, STATGROUP_Tickables);
}

/* ------------------------------------------------------------
   !認識対象追加
------------------------------------------------------------ */
void UCSKit_Subsystem::EntryRecognitionTarget(AActor* InTarget)
{
	if (!GetWorld()->IsNetMode(NM_Client))
	{
		mRecognitionTargetList.AddUnique(InTarget);
	}
}

/* ------------------------------------------------------------
   !認識対象除外
------------------------------------------------------------ */
void UCSKit_Subsystem::ExitRecognitionTarget(AActor* InTarget)
{
	if (!GetWorld()->IsNetMode(NM_Client))
	{
		mRecognitionTargetList.RemoveSingle(InTarget);
	}
}

#if CSKIT_USE_ENVIRONMENT_MAP
/* ------------------------------------------------------------
   !ACSKit_EnvironmentMapの登録と解除
------------------------------------------------------------ */
void UCSKit_Subsystem::EntryEnvironmentMap(ACSKit_EnvironmentMap* InMap)
{
	if (!GetWorld()->IsNetMode(NM_Client))
	{
		mAIEnvironmentMapList.AddUnique(InMap);
	}
}
void UCSKit_Subsystem::ExitEnvironmentMap(ACSKit_EnvironmentMap* InMap)
{
	if (!GetWorld()->IsNetMode(NM_Client))
	{
		mAIEnvironmentMapList.AddUnique(InMap);
	}
}

/* ------------------------------------------------------------
   !指定座標のACSKit_EnvironmentMapを取得
------------------------------------------------------------ */
ACSKit_EnvironmentMap* UCSKit_Subsystem::FindOwnEnvironmentMapMinVolume(const FVector& InPos) const
{
	float MinVolume = FLT_MAX;
	ACSKit_EnvironmentMap* SelectEnvironmentMap = nullptr;
	for (auto& WeakPtr : mAIEnvironmentMapList)
	{
		if (ACSKit_EnvironmentMap* EnvironmentMap = WeakPtr.Get())
		{
			const float Volume = EnvironmentMap->CalcVolume();
			if (Volume < MinVolume
				&& EnvironmentMap->IsInsideAroundBox(InPos))
			{
				MinVolume = Volume;
				SelectEnvironmentMap = EnvironmentMap;
			}
		}
	}
	return SelectEnvironmentMap;
}
ACSKit_EnvironmentMap* UCSKit_Subsystem::FindOwnEnvironmentMapMaxVolume(const FVector& InPos) const
{
	float MaxVolume = 0.f;
	ACSKit_EnvironmentMap* SelectEnvironmentMap = nullptr;
	for (auto& WeakPtr : mAIEnvironmentMapList)
	{
		if (ACSKit_EnvironmentMap* EnvironmentMap = WeakPtr.Get())
		{
			const float Volume = EnvironmentMap->CalcVolume();
			if (Volume > MaxVolume
				&& EnvironmentMap->IsInsideAroundBox(InPos))
			{
				MaxVolume = Volume;
				SelectEnvironmentMap = EnvironmentMap;
			}
		}
	}
	return SelectEnvironmentMap;
}
#endif

/* ------------------------------------------------------------
   !ACSKit_TerritoryVolumeの登録と解除
------------------------------------------------------------ */
void UCSKit_Subsystem::EntryTerritoryVolume(ACSKit_TerritoryVolume* InVolume)
{
	if (!GetWorld()->IsNetMode(NM_Client))
	{
		mTerritoryVolumeList.AddUnique(InVolume);
	}
}
void UCSKit_Subsystem::ExitTerritoryVolume(ACSKit_TerritoryVolume* InVolume)
{
	if (!GetWorld()->IsNetMode(NM_Client))
	{
		mTerritoryVolumeList.AddUnique(InVolume);
	}
}

/**
 * @brief 
 */
const ACSKit_TerritoryVolume* UCSKit_Subsystem::FindTouchTerritoryVolume(const FVector& InPos, const float InRadius, const FName& InUserName) const
{
	for(const auto& WeakPtr : mTerritoryVolumeList)
	{
		const ACSKit_TerritoryVolume* Volume = WeakPtr.Get();
		if(Volume == nullptr)
		{
			continue;
		}
		if(Volume->GetUserName() == InUserName
			&& Volume->IsTouch(InPos, InRadius))
		{
			return Volume;
		}
	}

	return nullptr;
}

/**
 * @brief 
 */
void UCSKit_Subsystem::InitXorShiftRand(const uint32 InSeed)
{
	UE_LOG(LogCSKit_SubSystem, Log, TEXT("UCSKit_Subsystem::sRand Seed : %d"), InSeed);
	msRand = CSKit_XorShift(InSeed);
}

#if USE_CSKIT_DEBUG
void UCSKit_Subsystem::sDebugSetFlag(const FName& InName, const bool bInFlag)
{
	if(UCSKit_Subsystem* CSKit_Subsystem = UCSKit_Subsystem::StaticClass()->GetDefaultObject<UCSKit_Subsystem>())
	{
		CSKit_Subsystem->DebugSetFlag(InName, bInFlag);
	}
}
bool UCSKit_Subsystem::sDebugIsFlag(const FName& InName)
{
	if(UCSKit_Subsystem* CSKit_Subsystem = UCSKit_Subsystem::StaticClass()->GetDefaultObject<UCSKit_Subsystem>())
	{
		return CSKit_Subsystem->DebugIsFlag(InName);
	}
	return false;
}
void UCSKit_Subsystem::DebugSetFlag(const FName& InName, const bool bInFlag)
{
	bool& Flag = mDebugFlagMap.FindOrAdd(InName);
	Flag = bInFlag;
}
bool UCSKit_Subsystem::DebugIsFlag(const FName& InName) const
{
	if(const bool* Flag = mDebugFlagMap.Find(InName))
	{
		return *Flag;
	}
	return false;
}

/* ------------------------------------------------------------
   !デバッグ描画on/off
------------------------------------------------------------ */
void UCSKit_Subsystem::DebugRequestDraw(const bool bInActive)
{
	if (bInActive)
	{
		if (!mDebugDrawHandle.IsValid())
		{
			const auto DebugDrawDelegate = FDebugDrawDelegate::CreateUObject(this, &UCSKit_Subsystem::DebugDraw);
			if (DebugDrawDelegate.IsBound())
			{
				mDebugDrawHandle = UDebugDrawService::Register(TEXT("GameplayDebug"), DebugDrawDelegate);
			}
		}
	}
	else
	{
		if (mDebugDrawHandle.IsValid())
		{
			UDebugDrawService::Unregister(mDebugDrawHandle);
			mDebugDrawHandle.Reset();
		}
	}
}

/* ------------------------------------------------------------
   !デバッグ表示
------------------------------------------------------------ */
void UCSKit_Subsystem::DebugDraw(UCanvas* InCanvas, APlayerController* ) const
{
	if(const AActor* ViewActor = InCanvas->SceneView->ViewActor)
	{
		if(GetWorld() != ViewActor->GetWorld())
		{
			return;
		}
	}
	
	if (GetWorld()->IsNetMode(NM_Client))
	{
		return;
	}
	if (mCommunityManager)
	{
		mCommunityManager->DebugDraw(InCanvas);
	}
	if (mNeedLevelManager)
	{
		mNeedLevelManager->DebugDraw(InCanvas);
	}
	if (mNoticeTargetManager)
	{
		mNoticeTargetManager->DebugDraw(InCanvas);
	}
	if (mWorrySourceManager)
	{
		mWorrySourceManager->DebugDraw(InCanvas);
	}
	if (mExperienceManager)
	{
		mExperienceManager->DebugDraw(InCanvas);
	}

	if(mbDebugDrawTerritoryVolumeList)
	{
		for(const auto& WeakPtr : mTerritoryVolumeList)
		{
			if(const ACSKit_TerritoryVolume* TerritoryVolume = WeakPtr.Get())
			{
				TerritoryVolume->DebugDraw(InCanvas, false);
			}
		}
	}
}
#endif