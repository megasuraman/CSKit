// Copyright 2020 megasuraman.
/**
 * @file CSKit_ExperienceManager.cpp
 * @brief 全AIのExperience情報の取りまとめ
 * @author megasuraman
 * @date 2025/05/05
 */
#include "AI/Experience/CSKit_ExperienceManager.h"

#include "CSKit_Subsystem.h"

#if USE_CSKIT_DEBUG
#include "ScreenWindow/CSKitDebug_ScreenWindowManager.h"
#endif

UCSKit_ExperienceManager* UCSKit_ExperienceManager::sGet(const UWorld* InWorld)
{
	if(InWorld
		&& !InWorld->IsNetMode(NM_Client))
	{
		if (const UCSKit_Subsystem* CSKitSubsystem = InWorld->GetSubsystem<UCSKit_Subsystem>())
		{
			return CSKitSubsystem->GetExperienceManager();
		}
	}
	return nullptr;
}

UCSKit_ExperienceManager::UCSKit_ExperienceManager()
{
}

void UCSKit_ExperienceManager::AddCommonExperience(AActor* InTarget, const FName& InAIKindName, const FName& InElementName, const FCSKit_ExperienceSaveParameter& InSaveParameter)
{
	FCSKit_CommonExperience& CommonExperience = mSameTargetCommonExperienceMap.FindOrAdd(InTarget);
	FCSKit_ExperienceSave& ExperienceSave = CommonExperience.mExperienceSaveMap.FindOrAdd(InAIKindName);
	FCSKit_ExperienceSaveParameter& ExperienceSaveParameter = ExperienceSave.mSaveParameterMap.FindOrAdd(InElementName);
	ExperienceSaveParameter.mScore = InSaveParameter.mScore;
	ExperienceSaveParameter.mAddIntervalSec = InSaveParameter.mAddIntervalSec;
}

const FCSKit_ExperienceSaveParameter* UCSKit_ExperienceManager::FindCommonExperience(AActor* InTarget, const FName& InAIKindName, const FName& InElementName)
{
	if(const FCSKit_CommonExperience* CommonExperience = mSameTargetCommonExperienceMap.Find(InTarget))
	{
		if(const FCSKit_ExperienceSave* ExperienceSave = CommonExperience->mExperienceSaveMap.Find(InAIKindName))
		{
			return ExperienceSave->mSaveParameterMap.Find(InElementName);
		}
	}
	return nullptr;
}

#if USE_CSKIT_DEBUG
/* ------------------------------------------------------------
   !デバッグ表示
------------------------------------------------------------ */
void UCSKit_ExperienceManager::DebugDraw(UCanvas* InCanvas) const
{
	if(mbDebugDrawInfo)
	{
		DebugDrawInfo();
	}
}
/* ------------------------------------------------------------
   !デバッグ表示
------------------------------------------------------------ */
void UCSKit_ExperienceManager::DebugDrawInfo() const
{
	if(UCSKitDebug_ScreenWindowManager* DebugWindowManager = UCSKitDebug_ScreenWindowManager::Get(this))
	{
		FString Message;
		for(const auto& MapElementSameTargetCommonExperience : mSameTargetCommonExperienceMap)
		{
			const AActor* TargetActor = MapElementSameTargetCommonExperience.Key.Get();
			if(TargetActor == nullptr)
			{
				continue;
			}
			Message += FString::Printf(TEXT("Target : %s\n"), *TargetActor->GetName());
			for(const auto& MapElementExperienceSave : MapElementSameTargetCommonExperience.Value.mExperienceSaveMap)
			{
				Message += FString::Printf(TEXT("   AIKind : %s\n"), *MapElementExperienceSave.Key.ToString());
				for(const auto& MapElementExperienceSaveParameter : MapElementExperienceSave.Value.mSaveParameterMap)
				{
					Message += FString::Printf(TEXT("      Element : %s\n"), *MapElementExperienceSaveParameter.Key.ToString());
					const FCSKit_ExperienceSaveParameter& SaveParameter = MapElementExperienceSaveParameter.Value;
					Message += FString::Printf(TEXT("         mScore : %.2f\n"), SaveParameter.mScore);
					Message += FString::Printf(TEXT("         mAddIntervalSec : %.2f\n"), SaveParameter.mAddIntervalSec);
				}
			}
		}
		DebugWindowManager->AddWindow(TEXT("ExperienceManager"), Message);
	}
}

#endif
