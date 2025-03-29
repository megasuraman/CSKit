// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_ScreenWindowGraph.h
 * @brief デバッグ情報表示用Window　グラフ表示
 * @author megasuraman
 * @date 2025/03/29
 */

#pragma once

#include "CoreMinimal.h"
#include "CSKitDebug_LoopOrderArray.h"
#include "CSKitDebug_ScreenWindowBase.h"
#include "CSKitDebug_ScreenWindowGraph.generated.h"

/**
 * 
 */
USTRUCT(Blueprintable)
struct CSKITDEBUG_API FCSKitDebug_ScreenWindowGraph : public FCSKitDebug_ScreenWindowBase
{
    GENERATED_USTRUCT_BODY()
	
    FCSKitDebug_ScreenWindowGraph(){}
    virtual ~FCSKitDebug_ScreenWindowGraph() override {}

	struct ScoreData
	{
		float mScore = 0.f;
		float mSec = 0.f;

		bool operator==(const ScoreData& InData) const
		{
			return (mSec == InData.mSec);
		}
	};
	struct LineData
	{
		FName mName;
		FString mDisplayName;
		TCSKitDebug_LoopOrderArray<ScoreData> mScoreDataList = { 64 };
		FLinearColor mColor;
		bool mbHide = false;
	};
	struct GuideScoreLineData
	{
		FName mName;
		float mScore = 0.f;
		FLinearColor mColor;
	};
 
public:
	void InitWindowExtent();
	void Update(float InDeltaTime);
	void SetAddScoreInterval(const float InInterval) { mAddScoreInterval = InInterval; }
	void AddScore(const FName& InLineName, const float InScore);
	void SetScoreLineColor(const FName& InLineName, const FLinearColor InColor);
	void SetScoreLineDisplayName(const FName& InLineName, const FString& InDisplayName);
	void SetScoreLineHide(const FName& InLineName, const bool bInHide);
	void Clear()
	{
		mLineDataList.Empty();
	}
	bool IsOwnLineData() const { return (mLineDataList.Num() > 0); }
	void AddGuideScoreLine(const GuideScoreLineData& InData);
	void SetScoreName(const FString& InName) { mScoreName = InName; }

protected:
    virtual void DrawAfterBackground(class UCanvas* InCanvas, const FVector2D& InPos2D) const override;
 
private:
	TArray<LineData> mLineDataList;
	TArray<GuideScoreLineData> mGuideScoreLineData;
	FString mScoreName = { TEXT("score") };
	FVector2D mMaxValueDrawLen = FVector2D(200.f,200.f);
	FVector2D mOffsetDrawLenOverMaxValue = FVector2D(20.f, 20.f);
	float mSecMaxLen = 3.f;
	float mScoreMaxLen = 1.f;
	float mSec = 0.f;
	float mAddScoreInterval = 0.5f;
	float mWindowSpaceLen = 30.f;
};