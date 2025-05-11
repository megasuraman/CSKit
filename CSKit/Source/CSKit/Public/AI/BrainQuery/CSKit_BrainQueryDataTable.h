// Copyright 2020 megasuraman.
/**
 * @file CSKit_BrainQueryDataTable.h
 * @brief BrainQuery用DataTable
 * @author megasuraman
 * @date 2025/05/05
 */
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CSKit_BrainQueryDataTable.generated.h"

class UCSKit_BrainQueryTest_Base;

/* ------------------------------------------------------------
   !評価目的
------------------------------------------------------------ */
UENUM(BlueprintType)
enum class ECSKit_BrainQueryTestPurpose : uint8
{
	Score,
	Filter,
	FilterAndScore,
};
/* ------------------------------------------------------------
   !評価値のFiltering
------------------------------------------------------------ */
UENUM(BlueprintType)
enum class ECSKit_BrainQueryTestFilterType : uint8
{
	Invalid,
	//最小値で切る
	Minimum,
	//最大値で切る
	Maximum,
	//最小値と最大値できる
	Range,
};

/* ------------------------------------------------------------
   !評価用ノード
   (ホントはUEnvQueryTestみたいにUObjectにして継承先でテスト内容に応じたパラメータ持たせたい)
   (FInstancedStructを使うとか)
------------------------------------------------------------ */
USTRUCT(BlueprintType)
struct FCSKit_BrainQueryTestNode
{
	GENERATED_USTRUCT_BODY()

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Category = "BrainQuery", Meta = (DisplayName = "コメント", DisplayPriority = 1))
	FString mEditorComment;
#endif

	UPROPERTY(EditDefaultsOnly, Category = "BrainQuery", Meta = (DisplayName = "Scoreの重み", ClampMin = "0.0", ClampMax = "1.0", DisplayPriority = 4))
	float mScoreWeight = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "BrainQuery", Meta = (DisplayName = "Score計算時の値の最小値", DisplayPriority = 5))
	float mCalcScoreRangeValueMin = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = "BrainQuery", Meta = (DisplayName = "Score計算時の値の最大値", DisplayPriority = 6))
	float mCalcScoreRangeValueMax = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "BrainQuery", Meta = (DisplayName = "Filtering時の最小値", ClampMin = "0.0", ClampMax = "1.0", DisplayPriority = 9))
	float mFilteringScoreMin = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = "BrainQuery", Meta = (DisplayName = "Filtering時の最大値", ClampMin = "0.0", ClampMax = "1.0", DisplayPriority = 10))
	float mFilteringScoreMax = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "BrainQuery", Meta = (DisplayName = "テスト目的", DisplayPriority = 2))
	ECSKit_BrainQueryTestPurpose mTestPurpose = ECSKit_BrainQueryTestPurpose::Score;
	UPROPERTY(EditDefaultsOnly, Category = "BrainQuery", Meta = (DisplayName = "FilterType", DisplayPriority = 8))
	ECSKit_BrainQueryTestFilterType mTestFilterType = ECSKit_BrainQueryTestFilterType::Invalid;
	UPROPERTY(EditDefaultsOnly, Category = "BrainQuery", Meta = (DisplayName = "判定対象", DisplayPriority = 3))
	TSubclassOf<UCSKit_BrainQueryTest_Base> mBrainQueryTest;
	UPROPERTY(EditDefaultsOnly, Category = "BrainQuery", Meta = (DisplayName = "判定対象付加情報(Name)", DisplayPriority = 3))
	FName mBrainQueryTestOption_Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BrainQuery", Meta = (DisplayName = "Score反転", DisplayPriority = 7))
	uint8 mbInverseScore : 1;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Category = "BrainQuery", Meta = (DisplayName = "[Editor]一時的に無効化", DisplayPriority = 99))
	uint8 mbEditorTestIgnore : 1;
#endif

	FCSKit_BrainQueryTestNode()
		:mbInverseScore(false)
#if WITH_EDITORONLY_DATA
		,mbEditorTestIgnore(false)
#endif
	{}
};

/* ------------------------------------------------------------
   !評価用データ
------------------------------------------------------------ */
USTRUCT(BlueprintType)
struct FCSKit_BrainQueryTest
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category = "BrainQuery", Meta = (DisplayName = "TestName", DisplayPriority = 3))
	FName mTestName;
	UPROPERTY(EditDefaultsOnly, Category = "BrainQuery", Meta = (DisplayName = "ParrentTestName", DisplayPriority = 4))
	FName mParentTestName;
	UPROPERTY(EditDefaultsOnly, Category = "BrainQuery", Meta = (DisplayName = "結果格納するBlackboardKey", DisplayPriority = 5))
	FName mBlackboardKeyName;
	// UPROPERTY(EditDefaultsOnly, Category = "BrainQuery", Meta = (DisplayName = "選択時Scoreボーナス", DisplayPriority = 6))
	// float mScoreBonus = 0.f;
	// UPROPERTY(EditDefaultsOnly, Category = "BrainQuery", Meta = (DisplayName = "選択時Scoreボーナス減少速度", DisplayPriority = 7))
	// float mScoreBonusDownSpeed = 0.1f;
	UPROPERTY(EditDefaultsOnly, Category = "BrainQuery", Meta = (DisplayName = "同着時の優先度", DisplayPriority = 8))
	uint32 mTestPriority = 0;
	UPROPERTY(EditDefaultsOnly, Category = "BrainQuery", meta = (DisplayName = "テストリスト", TitleProperty = "mEditorComment", DisplayPriority = 9))
	TArray<FCSKit_BrainQueryTestNode> mTestNodeList;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Category = "BrainQuery", Meta = (DisplayName = "(Debug)コメント", DisplayPriority = 1))
	FString mEditorComment;
	UPROPERTY(EditAnywhere, Category = "BrainQuery", Meta = (DisplayName = "(Debug)無効化", DisplayPriority = 2))
	uint8 mbEditorSeal : 1;
#endif

	FCSKit_BrainQueryTest()
#if WITH_EDITORONLY_DATA
		:mbEditorSeal(false)
#endif
	{}
};

// BrainQuery用データ
USTRUCT(BlueprintType)
struct FCSKit_BrainQueryTableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "BrainQuery", meta = (DisplayName = "評価対象リスト", TitleProperty = "mTestName", DisplayPriority = 2))
	TArray<FCSKit_BrainQueryTest> mTest;
	
	UPROPERTY(EditDefaultsOnly, Category = "BrainQuery", meta = (DisplayName = "更新間隔(秒)", DisplayPriority = 1))
	float mUpdateInterval = 0.f;
};

// BrainQueryの外部補正用データ
USTRUCT(BlueprintType)
struct FCSKit_BrainQuerySupplementTableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "BrainQuery", meta = (DisplayName = "TestのScore補正"))
	TMap<FName, float>	mTestScoreRatio;
};

class CSKIT_API CSKit_BrainQueryTool
{
public:
#if WITH_EDITOR
	static bool EditorCheckError(const FCSKit_BrainQueryTableRow& InData);
#endif
};