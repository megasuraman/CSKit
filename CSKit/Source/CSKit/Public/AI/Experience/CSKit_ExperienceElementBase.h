// Copyright 2020 megasuraman.
/**
 * @file CSKit_ExperienceElementBase.h
 * @brief ExperienceComponentで扱う経験値要素の基礎クラス
 * @author megasuraman
 * @date 2025/05/05
 */
#pragma once

#include "CoreMinimal.h"
#include "CSKit_ExperienceDataTable.h"

struct FCSKit_ExperienceSaveParameter;
class ACSKit_AIController;
class UCanvas;

class CSKIT_API CSKit_ExperienceElementBase
{
public:
	//素直にtypeid()使ってもいいかも
	static FName StaticGetTypeId() { static FName Type("Base"); return Type; }
	virtual FName GetTypeId() const { return StaticGetTypeId(); }

	CSKit_ExperienceElementBase(){}
	virtual ~CSKit_ExperienceElementBase(){}

	virtual void Update(const float InDeltaSec) {}
	virtual void OnChangeTarget(AActor* InTarget);
	virtual void OnTakeDamage(AActor* InDamageCauser, const float InDamage, const uint16 InParamBit) {}
	virtual void OnApplyDamage(AActor* InHitTarget, const float InDamage, const uint16 InParamBit) {}
	virtual void OnCheckHitAttackCollision() {}

	float GetScore() const { return mScore; }
	void DirectSetScore(const float InScore){SetScore(InScore);}
	void AddScore(const float InAddValue);
	void SetOwner(ACSKit_AIController* InOwner);
	void SetScoreElement(const FCSKit_ExperienceScoreElement& InElement)
	{
		mScoreElement = InElement;
#if WITH_EDITOR
		mbEditorSetScoreElement = true;
#endif
	}
	void SetElementName(const FName& InName){mElementName = InName;}
	const FName& GetElementName() const{return mElementName;}

protected:
	ACSKit_AIController*	GetOwner() const;
	void SetScore(const float InScore) { mScore = FMath::Clamp(InScore,0.f,1.f); }
	const AActor* GetTargetActor() const;
	const FCSKit_ExperienceScoreElement& GetScoreElement() const { return mScoreElement; }
	void SetAddIntervalSec(const float InValue);
	float GetAddIntervalSec() const{return mAddIntervalSec;}
	void SaveCommonExperience(AActor* InTargetActor);
	void LoadCommonExperience(AActor* InTargetActor);
	virtual void OnPostLoadCommonExperience(const FCSKit_ExperienceSaveParameter& InSaveParameter, AActor* InTargetActor);
	virtual void OnPreSaveCommonExperience(FCSKit_ExperienceSaveParameter& OutSaveParameter);

private:
	FCSKit_ExperienceScoreElement mScoreElement;
	TWeakObjectPtr<ACSKit_AIController> mOwner;
	TWeakObjectPtr<AActor> mTargetActor;
	float mScore = 0.f;
	float mAddIntervalSec = 0.f;
	FName mElementName;

#if WITH_EDITOR
public:
	bool EditorIsSetScoreElement() const { return mbEditorSetScoreElement; }
private:
	bool mbEditorSetScoreElement = false;
#endif

#if USE_CSKIT_DEBUG
public:
	virtual FString DebugDrawSelectedActorWatcher(UCanvas* InCanvas) const;
#endif
};
