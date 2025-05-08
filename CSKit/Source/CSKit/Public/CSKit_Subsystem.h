// Copyright 2020 megasuraman.
/**
 * @file CSKit_SubSystem.h
 * @brief CSKit用のSubSystem
 * @author megasuraman
 * @date 2025/05/06
 */
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CSKit_XorShift.h"
#include "CSKit_Subsystem.generated.h"

class UCSKit_CommunityManager;
class ACSKit_EnvironmentMap;
class UCSKit_NeedLevelManager;
class CSKit_XorShift;
class ACSKit_TerritoryVolume;
class UCSKit_NoticeTargetManager;
class UCSKit_WorrySourceManager;
class UCSKit_ExperienceManager;

#define CSKIT_USE_ENVIRONMENT_MAP 0

UCLASS()
class CSKIT_API UCSKit_Subsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	static CSKit_XorShift msRand;
	//3次式の単純な補間関数(FMath::InterpEaseInOut<float>(0, 1, InRatio, 2.0)に近い)
	static float CalcSimpleEaseInOut(float InRatio)
	{
		InRatio = FMath::Clamp(InRatio, 0.f, 1.f);
		return (3.f - 2.f*InRatio)*FMath::Square(InRatio);
	}
	//2次ベジェ曲線の座標取得
	static FVector CalcQuadraticBezier(const FVector& InPosA, const FVector& InPosB, const FVector& InPosC, const float InRatio)
	{
		const double Ratio = static_cast<double>(InRatio);
		const FVector PosAB = InPosA + (InPosB-InPosA)*Ratio;
		const FVector PosBC = InPosB + (InPosC-InPosB)*Ratio;
		const FVector PosAB_BC = PosAB + (PosBC-PosAB)*Ratio;
		return PosAB_BC;
	}

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	
	UFUNCTION(BlueprintCallable, Category = "CSKit_Subsystem")
	void InitXorShiftBP(int32 InSeed) const
	{
		InitXorShiftRand(static_cast<uint32>(InSeed));
	}
	UFUNCTION(BlueprintCallable, Category = "CSKit_Subsystem")
	void InitRandBP(int32 InSeed) const
	{
		FMath::RandInit(InSeed);
		FMath::SRandInit(InSeed);
	}

	void EntryRecognitionTarget(AActor* InTarget);
	void ExitRecognitionTarget(AActor* InTarget);
	const TArray<TWeakObjectPtr<AActor>>& GetRecognitionTargetList() const
	{
		return mRecognitionTargetList;
	}

	UCSKit_CommunityManager*	GetCommunityManager() const { return mCommunityManager; }
	UCSKit_NeedLevelManager* GetNeedLevelManager() const { return mNeedLevelManager; }
	UCSKit_NoticeTargetManager* GetNoticeTargetManager() const { return mNoticeTargetManager; }
	UCSKit_WorrySourceManager* GetWorrySourceManager() const { return mWorrySourceManager; }
	UCSKit_ExperienceManager* GetExperienceManager() const { return mExperienceManager; }
#if CSKIT_USE_ENVIRONMENT_MAP
	void EntryEnvironmentMap(ACSKit_EnvironmentMap* InMap);
	void ExitEnvironmentMap(ACSKit_EnvironmentMap* InMap);
	const TArray<TWeakObjectPtr<ACSKit_EnvironmentMap>>& GetEnvironmentMapList() const { return mAIEnvironmentMapList; }
	ACSKit_EnvironmentMap* FindOwnEnvironmentMapMinVolume(const FVector& InPos) const;
	ACSKit_EnvironmentMap* FindOwnEnvironmentMapMaxVolume(const FVector& InPos) const;
#endif
	void EntryTerritoryVolume(ACSKit_TerritoryVolume* InVolume);
	void ExitTerritoryVolume(ACSKit_TerritoryVolume* InVolume);
	const TArray<TWeakObjectPtr<ACSKit_TerritoryVolume>>& GetTerritoryVolumeList() const{return mTerritoryVolumeList;}
	const ACSKit_TerritoryVolume* FindTouchTerritoryVolume(const FVector& InPos, const float InRadius, const FName& InUserName) const;
	static void InitXorShiftRand(uint32 InSeed);
	
private:
	TArray<TWeakObjectPtr<AActor>> mRecognitionTargetList;
	TArray<TWeakObjectPtr<ACSKit_EnvironmentMap>> mAIEnvironmentMapList;
	TArray<TWeakObjectPtr<ACSKit_TerritoryVolume>> mTerritoryVolumeList;
	UPROPERTY()
	UCSKit_CommunityManager* mCommunityManager = nullptr;
	UPROPERTY()
	UCSKit_NeedLevelManager* mNeedLevelManager = nullptr;
	UPROPERTY()
	UCSKit_NoticeTargetManager* mNoticeTargetManager = nullptr;
	UPROPERTY()
	UCSKit_WorrySourceManager* mWorrySourceManager = nullptr;
	UPROPERTY()
	UCSKit_ExperienceManager* mExperienceManager = nullptr;

#if USE_CSKIT_DEBUG
public:
	void DebugSetDrawTerritoryVolumeList(const bool bInDraw){mbDebugDrawTerritoryVolumeList=bInDraw;}
	static void sDebugSetFlag(const FName& InName, const bool bInFlag);
	static bool sDebugIsFlag(const FName& InName);
	void DebugSetFlag(const FName& InName, const bool bInFlag);
	bool DebugIsFlag(const FName& InName) const;
protected:
	void DebugRequestDraw(const bool bInActive);
	void DebugDraw(UCanvas* InCanvas, class APlayerController* InPlayerController) const;
private:
	FDelegateHandle mDebugDrawHandle;
	TMap<FName,bool> mDebugFlagMap;
	bool mbDebugDrawTerritoryVolumeList = false;
#endif
};
