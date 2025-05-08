// Copyright 2020 megasuraman.
/**
 * @file CSKit_ExperienceElementBase.cpp
 * @brief ExperienceComponentで扱う経験値要素の基礎クラス
 * @author megasuraman
 * @date 2025/05/05
 */
#include "AI/Experience/CSKit_ExperienceElementBase.h"

#include "AI/CSKit_AIController.h"
#include "AI/Experience/CSKit_ExperienceManager.h"
#include "CSKit_Subsystem.h"


void CSKit_ExperienceElementBase::OnChangeTarget(AActor* InTarget)
{
	AActor* OldTargetActor = mTargetActor.Get();
	mTargetActor = InTarget;
	
	SaveCommonExperience(OldTargetActor);
	LoadCommonExperience(InTarget);
}


void CSKit_ExperienceElementBase::AddScore(const float InAddValue)
{
	SetScore(GetScore() + InAddValue);
}

void CSKit_ExperienceElementBase::SetOwner(ACSKit_AIController* InOwner)
{
	mOwner = InOwner;
}

ACSKit_AIController* CSKit_ExperienceElementBase::GetOwner() const
{
	return mOwner.Get();
}

const AActor* CSKit_ExperienceElementBase::GetTargetActor() const
{
	return mTargetActor.Get();
}

void CSKit_ExperienceElementBase::SetAddIntervalSec(const float InValue)
{
	mAddIntervalSec = FMath::Clamp(InValue, 0.f,GetScoreElement().mAutoAddIntervalTime);
}

void CSKit_ExperienceElementBase::SaveCommonExperience(AActor* InTargetActor)
{
	ACSKit_AIController*	AIController = GetOwner();
	if(AIController == nullptr)
	{
		return;
	}
	UCSKit_ExperienceManager* ExperienceManager = UCSKit_ExperienceManager::sGet(AIController->GetWorld());
	if(ExperienceManager == nullptr)
	{
		return;
	}
	const FName AIKindName = AIController->GetAIKindName();
	
	FCSKit_ExperienceSaveParameter SaveParameter;
	SaveParameter.mScore = GetScore();
	SaveParameter.mAddIntervalSec = mAddIntervalSec;
	OnPreSaveCommonExperience(SaveParameter);
	ExperienceManager->AddCommonExperience(InTargetActor, AIKindName, GetElementName(), SaveParameter);
}

void CSKit_ExperienceElementBase::LoadCommonExperience(AActor* InTargetActor)
{
	ACSKit_AIController*	AIController = GetOwner();
	if(AIController == nullptr)
	{
		return;
	}
	UCSKit_ExperienceManager* ExperienceManager = UCSKit_ExperienceManager::sGet(AIController->GetWorld());
	if(ExperienceManager == nullptr)
	{
		return;
	}
	const FName AIKindName = AIController->GetAIKindName();
	
	const FCSKit_ExperienceSaveParameter* SaveParameter = ExperienceManager->FindCommonExperience(InTargetActor, AIKindName, GetElementName());
	if(SaveParameter == nullptr)
	{
		SetScore(UCSKit_Subsystem::msRand.FRandRange());
		SetAddIntervalSec(UCSKit_Subsystem::msRand.FRandRange(0.f, GetScoreElement().mAutoAddIntervalTime));
	}
	else
	{
		SetScore(SaveParameter->mScore);
		SetAddIntervalSec(SaveParameter->mAddIntervalSec);
	}
	OnPostLoadCommonExperience(*SaveParameter, InTargetActor);
}

void CSKit_ExperienceElementBase::OnPostLoadCommonExperience(const FCSKit_ExperienceSaveParameter& InSaveParameter, AActor* InTargetActor)
{
}

void CSKit_ExperienceElementBase::OnPreSaveCommonExperience(FCSKit_ExperienceSaveParameter& OutSaveParameter)
{
}

#if USE_CSKIT_DEBUG
/* ------------------------------------------------------------
  !ActorWatcher選択時
------------------------------------------------------------ */
FString CSKit_ExperienceElementBase::DebugDrawSelectedActorWatcher(UCanvas* InCanvas) const
{
	FString DebugInfo;
	return DebugInfo;
}
#endif