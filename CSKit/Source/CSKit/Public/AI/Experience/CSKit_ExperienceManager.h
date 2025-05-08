// Copyright 2020 megasuraman.
/**
 * @file CSKit_ExperienceManager.h
 * @brief 全AIのExperience情報の取りまとめ
 * @author megasuraman
 * @date 2025/05/05
 */
#pragma once

#include "CoreMinimal.h"
#include "CSKit_ExperienceManager.generated.h"

struct FCSKit_ExperienceSaveParameter
{
	float mScore = 0.f;
	float mAddIntervalSec = 0.f;
};
struct FCSKit_ExperienceSave
{
	TMap<FName, FCSKit_ExperienceSaveParameter> mSaveParameterMap;//Key:ElementName
};
struct FCSKit_CommonExperience
{
	TMap<FName, FCSKit_ExperienceSave> mExperienceSaveMap;//Key:AIの種別
};

UCLASS()
class CSKIT_API UCSKit_ExperienceManager : public UObject
{
	GENERATED_BODY()
	
public:
	static UCSKit_ExperienceManager* sGet(const UWorld* InWorld);
	UCSKit_ExperienceManager();

	void AddCommonExperience(AActor* InTarget, const FName& InAIKindName, const FName& InElementName, const FCSKit_ExperienceSaveParameter& InSaveParameter);
	const FCSKit_ExperienceSaveParameter* FindCommonExperience(AActor* InTarget, const FName& InAIKindName, const FName& InElementName);

private:
	TMap<TWeakObjectPtr<AActor>, FCSKit_CommonExperience> mSameTargetCommonExperienceMap;

#if USE_CSKIT_DEBUG
public:
	void DebugDraw(UCanvas* InCanvas) const;
	void DebugSetDrawInfo(const bool bInDraw){mbDebugDrawInfo=bInDraw;}
protected:
	void DebugDrawInfo() const;
private:
	bool mbDebugDrawInfo = false;
#endif
};
