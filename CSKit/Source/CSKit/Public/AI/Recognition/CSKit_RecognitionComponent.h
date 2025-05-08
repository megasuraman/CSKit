// Copyright 2020 megasuraman.
/**
 * @file CSKit_RecognitionComponent.h
 * @brief Actor認識管理Component(AIPerceptionComponentの代わり)
 * @author megasuraman
 * @date 2025/05/05
 */
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AI/Recognition/CSKit_RecognitionDataTable.h"
#include "CSKit_RecognitionComponent.generated.h"

class ACSKit_AIController;
class UCSKit_TerritoryComponent;
class ACSKit_EnvironmentMap;
class UCanvas;
struct FCSKit_RecognitionTableRow;

struct FCSKitRecognitionTarget
{
	FVector mLastRecognitionPos = FVector::ZeroVector;
	TWeakObjectPtr<AActor> mActor = nullptr;
	float mLostSec = 0.f;
	float mTotalDamage = 0.f;
	float mRecentDamage = 0.f;
	float mRecentDamageDownInterval = 0.f;
	float mTouchTerritoryGauge = 0.f;
	union
	{
		uint8 mFlags = 0;
		struct
		{
			uint8 mbRecognize : 1;
			uint8 mbCompletelyLost : 1;
			uint8 mbHideTarget : 1;
			uint8 mbTouchTerritory : 1;
			uint8 : 4;
		};
	};
	union
	{
		uint8 mLastRecognitionFlags = 0;
		struct
		{
			uint8 mbLastRecognitionInsideSight : 1;
			uint8 mbLastRecognitionInsidePresence : 1;
			uint8 mbLastRecognitionTakeDamage : 1;
			uint8 mbLastRecognitionTakeMessage : 1;
			uint8 : 5;
		};
	};
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CSKIT_API UCSKit_RecognitionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCSKit_RecognitionComponent();
	virtual void BeginPlay() override;

	virtual void Setup(const FCSKit_RecognitionTableRow& InData);
	void SetTerritoryComponent(const UCSKit_TerritoryComponent* InComponent);
	void Update(const float InDeltaTime);
	void OnTakeDamage(AActor* InDamageCauser, const float InDamage);
	void OnTakeMessage(AActor* InTarget);

	const TArray<FCSKitRecognitionTarget>& GetRecognitionTargetList() const
	{
		return mRecognitionTargetList;
	}
	const FCSKitRecognitionTarget* FindRecognitionTarget(const AActor* InTarget) const;
	float GetCompletelyLostBorderTime() const { return mRecognizeAbility.mCompletelyLostBorderTime; }
	static void SendMessage(AActor* InTarget, const TArray<ACSKit_AIController*>& InReceiverList);
	void SetIgnoreMessage(const bool bInIgnore){mbIgnoreMessage = bInIgnore;}
	
protected:
	virtual AActor* ReplaceTarget(AActor* InTarget) const;
	virtual bool IsFriend(const AActor* InTarget) const;
	virtual bool IsIgnoreTarget(const AActor* InTarget) const{return false;};
	virtual bool IsHideTarget(const AActor* InTarget) const{return false;};

	void SetRecognizeAbility(const FCSKit_RecognizeAbility& InAbility)
	{
		mRecognizeAbility = InAbility;
	}
	void UpdateRecognizeTarget(const float InDeltaTime);
	FCSKitRecognitionTarget* FindRecognitionTarget(const AActor* InTarget);
	void AddRecognitionTarget(AActor* InTarget);
	void AddRecognitionTarget(const FCSKitRecognitionTarget& InTarget);
	virtual void OnFirstRecognition(AActor* InTarget);
	float CalcPrice(const FCSKitRecognitionTarget& InTarget) const;
	virtual bool CheckCollisionVisible(const FVector& InBasePos, AActor* InTarget, const bool bInOldRecognitionTarget) const;
	bool IsOwnRecognitionTarget() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "CSKit_RecognitionComponent")
	float mOffsetFrontYaw = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = "CSKit_RecognitionComponent")
	bool mbIgnoreMessage = false;

private:
	FCSKit_RecognizeAbility mRecognizeAbility;
	TArray<FCSKitRecognitionTarget> mRecognitionTargetList;
	TWeakObjectPtr<const UCSKit_TerritoryComponent> mTerritoryComponent;
	int32 mMaxRecognitionTargetListSize = 8;

#if USE_CSKIT_DEBUG
public:
	virtual FString DebugDrawSelectedActorWatcher(UCanvas* InCanvas) const;
protected:
	void DebugDrawAbilityRange() const;
	void DebugDrawRecognitionTarget(UCanvas* InCanvas) const;
	void DebugDrawRecognitionTarget(UCanvas* InCanvas, const FCSKitRecognitionTarget& InTarget) const;
	
	FCSKit_RecognizeAbility& DebugGetRecognizeAbility() {return mRecognizeAbility;}
#endif
};
