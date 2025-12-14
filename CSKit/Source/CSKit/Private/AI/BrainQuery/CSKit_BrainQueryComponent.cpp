// Copyright 2020 megasuraman.
/**
 * @file CSKit_BrainQueryComponent.cpp
 * @brief 思考判断用Component
 *			ユーティリティベースAIの構築
 * @author megasuraman
 * @date 2025/05/05
 */
#include "AI/BrainQuery/CSKit_BrainQueryComponent.h"

#include "CSKit_Config.h"
#include "AI/CSKit_AIController.h"
#include "AI/BrainQuery/CSKit_BrainQueryDataTable.h"
#include "AI/BrainQuery/Test/CSKit_BrainQueryTest_Base.h"
#include "BehaviorTree/BlackboardComponent.h"

#if USE_CSKIT_DEBUG
#include "CSKitDebug_Subsystem.h"
#include "ScreenWindow/CSKitDebug_ScreenWindowGraph.h"
#include "ScreenWindow/CSKitDebug_ScreenWindowText.h"
#endif

DEFINE_LOG_CATEGORY_STATIC(LogCSKit_BrainQueryComponent, Warning, All);

UCSKit_BrainQueryComponent::UCSKit_BrainQueryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

#if WITH_EDITOR
	if (const UCSKit_Config* CSKitConfig = GetDefault<UCSKit_Config>())
	{
		mBrainQueryRowNameSelector.mDataTablePath = CSKitConfig->mEditorBrainQuery_DataTable.ToSoftObjectPath().GetAssetPathString();
	}
#endif
}

void UCSKit_BrainQueryComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ACSKit_AIController* Owner = Cast<ACSKit_AIController>(GetOwner()))
	{
		mBlackboardComponent = Owner->GetBlackboardComponent();
	}
}
void UCSKit_BrainQueryComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

#if WITH_EDITOR
	if (mEditorModifyData)
	{
		delete mEditorModifyData;
		mEditorModifyData = nullptr;
	}
	for(const FCSKitDebug_ScreenWindowGraph* Graph : mEditorScreenWindowGraphList)
	{
		if (Graph)
		{
			delete Graph;
		}
	}
	mEditorScreenWindowGraphList.Empty();
#endif
}

/* ------------------------------------------------------------
   !更新
------------------------------------------------------------ */
void UCSKit_BrainQueryComponent::Update(const float InDeltaSec)
{
	const UCSKit_Config* CSKitConfig = GetDefault<UCSKit_Config>();
	if (CSKitConfig == nullptr)
	{
		return;
	}
	const UDataTable* BrainQueryDataTable = CSKitConfig->mEditorBrainQuery_DataTable.LoadSynchronous();
	if (BrainQueryDataTable == nullptr)
	{
		ensureMsgf(false, TEXT("Need Assign mEditorBrainQuery_DataTable"));
		return;
	}
	if (const FCSKit_BrainQueryTableRow* TableRow = BrainQueryDataTable->FindRow<FCSKit_BrainQueryTableRow>(mBrainQueryRowNameSelector.mRowName, TEXT("")))
	{
		CalcBrainQuery(*TableRow);
	}
}

/* ------------------------------------------------------------
   !スコア計算と選択したTest結果をBlackboardへ格納
------------------------------------------------------------ */
void UCSKit_BrainQueryComponent::CalcBrainQuery(const FCSKit_BrainQueryTableRow& InBrainQueryTableRow)
{
#if USE_CSKIT_DEBUG
	mDebugLastSelectTestMap.Empty();
	if (mbDebugCollectTestResult)
	{
		mDebugTestResultList.Empty();
	}
	DebugBeginOutputLog();
#endif

	const FCSKit_BrainQueryTableRow* BrainQueryData = &InBrainQueryTableRow;
#if WITH_EDITOR
	EditorCheckDataSafety(InBrainQueryTableRow);
	if (mbEditorModifyTestNode)
	{
		if (mEditorModifyData)
		{
			BrainQueryData = mEditorModifyData;
		}
		else
		{
			EditorCreateModifyData(InBrainQueryTableRow);
		}
	}
#endif

	const ACSKit_AIController* Owner = Cast<ACSKit_AIController>(GetOwner());
	if (Owner == nullptr)
	{
		return;
	}

#if USE_CSKIT_DEBUG
	uint8 DebugTestLevelCount = 0;
#endif

	FName LastSelectTestName;
	bool bFinish = false;
	while(!bFinish)
	{
		float MaxScore = 0.f;
		FName SelectTest;
		FName AssignBlackboardKeyName;
		uint32 SelectTestPriority = 0;
		for (const FCSKit_BrainQueryTest& BrainQueryTest : BrainQueryData->mTest)
		{
			if (LastSelectTestName != BrainQueryTest.mParentTestName)
			{
				continue;
			}
#if WITH_EDITOR
			if(BrainQueryTest.mbEditorSeal)
			{
				continue;
			}
#endif

#if USE_CSKIT_DEBUG
			DebugOutputLog_BeginCalcScore(BrainQueryTest);
#endif
			const float TestScore = CalcBrainQueryTestScore(BrainQueryTest, *Owner);
#if USE_CSKIT_DEBUG
			DebugOutputLog_EndCalcScore(BrainQueryTest, TestScore);
			if (mbDebugCollectTestResult)
			{
				DebugTestResult DebugResult;
				DebugResult.mTestName = BrainQueryTest.mTestName;
				DebugResult.mScore = TestScore;
				DebugResult.mLevel = DebugTestLevelCount;
				mDebugTestResultList.Add(DebugResult);
			}
#endif
			bool bNewRecord = false;
			if (TestScore > MaxScore)
			{
				bNewRecord = true;
			}
			else if (TestScore == MaxScore
					&& BrainQueryTest.mTestPriority > SelectTestPriority)
			{
				bNewRecord = true;
			}

			if(bNewRecord)
			{
				MaxScore = TestScore;
				SelectTest = BrainQueryTest.mTestName;
				AssignBlackboardKeyName = BrainQueryTest.mBlackboardKeyName;
				SelectTestPriority = BrainQueryTest.mTestPriority;
#if USE_CSKIT_DEBUG
				if (AssignBlackboardKeyName.IsNone())
				{
					UE_LOG(LogCSKit_BrainQueryComponent, Error, TEXT("Invalid BlackboardKeyName : %s"), *BrainQueryTest.mTestName.ToString());
				}
#endif
			}
		}

		if (SelectTest.IsNone())
		{
			bFinish = true;
		}
		else
		{
			LastSelectTestName = SelectTest;
			if (UBlackboardComponent* BlackboardComponent = mBlackboardComponent.Get())
			{
#if USE_CSKIT_DEBUG
				if (BlackboardComponent->GetKeyID(AssignBlackboardKeyName) == FBlackboard::InvalidKey)
				{
					UCSKitDebug_Subsystem::sOneShotWarning(
						GetWorld(),
						false,
						FName(TEXT("CSKit_BrainQuery")),
						FString::Printf(TEXT("UCSKit_BrainQueryComponent::CalcBrainQuery() Invalid AssignBlackboardKeyName : %s"), *AssignBlackboardKeyName.ToString())
					);
				}
				mDebugLastSelectTestMap.Add(AssignBlackboardKeyName, SelectTest);
#endif
				BlackboardComponent->SetValueAsName(AssignBlackboardKeyName, SelectTest);
			}
#if USE_CSKIT_DEBUG
			if (mbDebugCollectTestResult)
			{
				for (DebugTestResult& Result : mDebugTestResultList)
				{
					if (Result.mTestName == SelectTest)
					{
						Result.mbSelect = true;
						break;
					}
				}
			}
			++DebugTestLevelCount;
#endif
		}
	}

#if USE_CSKIT_DEBUG
	DebugEndOutputLog();
#endif
}

/* ------------------------------------------------------------
   !スコア計算
------------------------------------------------------------ */
float UCSKit_BrainQueryComponent::CalcBrainQueryTestScore(const FCSKit_BrainQueryTest& InBrainQueryTest, const ACSKit_AIController& InOwner)
{
#if USE_CSKIT_DEBUG
	if(mDebugForceSelectTestNameList.Num() > 0)
	{
		if(mDebugForceSelectTestNameList.Find(InBrainQueryTest.mTestName) != INDEX_NONE)
		{
			return 1.f;
		}
		return 0.f;
	}
#endif

	struct ScoreData
	{
		float	mScore = 0.f;
		float	mWeight = 0.f;
		ScoreData(){}
		ScoreData(const float InScore, const float InWeight)
			:mScore(InScore)
			,mWeight(InWeight)
		{}
	};
	TArray<ScoreData> ScoreDataList;

	for (const FCSKit_BrainQueryTestNode& TestNode : InBrainQueryTest.mTestNodeList)
	{
#if WITH_EDITORONLY_DATA
		if(TestNode.mbEditorTestIgnore)
		{
			continue;
		}
#endif
#if USE_CSKIT_DEBUG
		DebugOutputLog_BeginCalcTestNodeScore(TestNode);
#endif
		float Score;
		if (const UClass* BrainQueryTestClass = TestNode.mBrainQueryTest.Get())
		{
			Score = BrainQueryTestClass->GetDefaultObject<UCSKit_BrainQueryTest_Base>()->CalcScore(InOwner, TestNode);
		}
		else
		{
			UE_LOG(LogCSKit_BrainQueryComponent, Error, TEXT("Invalid BrainQueryTest : %s"), *InBrainQueryTest.mTestName.ToString());
			continue;
		}
		Score = FMath::Clamp(Score, 0.f, 1.f);
		if (TestNode.mbInverseScore)
		{
			Score = 1.f - Score;
		}

		if (TestNode.mTestPurpose == ECSKit_BrainQueryTestPurpose::Filter
			|| TestNode.mTestPurpose == ECSKit_BrainQueryTestPurpose::FilterAndScore)
		{
			switch (TestNode.mTestFilterType)
			{
			case ECSKit_BrainQueryTestFilterType::Minimum:
				if (Score <= TestNode.mFilteringScoreMin)
				{
#if USE_CSKIT_DEBUG
					DebugOutputLog_EndCalcTestNodeScore(TestNode, false);
#endif
					return 0.f;
				}
				break;
			case ECSKit_BrainQueryTestFilterType::Maximum:
				if (Score > TestNode.mFilteringScoreMax)
				{
#if USE_CSKIT_DEBUG
					DebugOutputLog_EndCalcTestNodeScore(TestNode, false);
#endif
					return 0.f;
				}
				break;
			case ECSKit_BrainQueryTestFilterType::Range:
				if (Score <= TestNode.mFilteringScoreMin
					|| Score > TestNode.mFilteringScoreMax)
				{
#if USE_CSKIT_DEBUG
					DebugOutputLog_EndCalcTestNodeScore(TestNode, false);
#endif
					return 0.f;
				}
				break;
			default:
				UE_LOG(LogCSKit_BrainQueryComponent, Error, TEXT("Invalid TestFilterType : %s"), *InBrainQueryTest.mTestName.ToString());
				break;
			}

			if (TestNode.mTestPurpose == ECSKit_BrainQueryTestPurpose::Filter)
			{
#if USE_CSKIT_DEBUG
				DebugOutputLog_EndCalcTestNodeScore(TestNode, true);
#endif
				continue;
			}
		}

#if USE_CSKIT_DEBUG
		DebugOutputLog_EndCalcTestNodeScore(TestNode, Score);
#endif
		ScoreDataList.Add(ScoreData(Score, TestNode.mScoreWeight));
	}

	const int32 ScoreDataListNum = ScoreDataList.Num();
	if (ScoreDataListNum <= 0)
	{
		return 0.f;
	}

	//加重平均をScoreとする
	float TotalScore = 0.f;
	float TotalWeight = 0.f;
	for (const ScoreData& Data : ScoreDataList)
	{
		TotalScore += Data.mScore * Data.mWeight;
		TotalWeight += Data.mWeight;
	}
	float ResultScore = 0.f;
	if (TotalWeight > 0.f)
	{
		ResultScore = TotalScore / TotalWeight;
	}

	const float ScoreRatio = CalcTestScoreRatio(InBrainQueryTest.mTestName);
	ResultScore *= ScoreRatio;
	ResultScore = FMath::Clamp(ResultScore, 0.f, 1.f);
	return ResultScore;
}

#if USE_CSKIT_DEBUG
/* ------------------------------------------------------------
  !ActorWatcher選択時
------------------------------------------------------------ */
FString UCSKit_BrainQueryComponent::DebugDrawSelectedActorWatcher(UCanvas* InCanvas) const
{
	FString DebugInfo;

	DebugInfo += FString::Printf(TEXT("[BrainQuery]\n"));
	for (const DebugTestResult& Result : mDebugTestResultList)
	{
		if (Result.mbSelect)
		{
			DebugInfo += FString::Printf(TEXT("   [%d]%s : %.2f\n"), Result.mLevel, *Result.mTestName.ToString(), Result.mScore);
		}
	}

#if WITH_EDITOR
	EditorDrawScreenWindowGraph(InCanvas, GetWorld()->GetDeltaSeconds());
#endif

	mbDebugCollectTestResult = true;

	return DebugInfo;
}

/* ------------------------------------------------------------
  !デバッグ用ログ出力有無
------------------------------------------------------------ */
void UCSKit_BrainQueryComponent::DebugRequestOutputLog(const bool bInOutputLog)
{
	if (mbDebugOutputLog == bInOutputLog)
	{
		return;
	}
	mbDebugOutputLog = bInOutputLog;
	mDebugOutputLogCount = 0;
}

/**
 * @brief 
 */
void UCSKit_BrainQueryComponent::DebugAddForceSelectTestName(const FName& InName)
{
	mDebugForceSelectTestNameList.Add(InName);
}

/* ------------------------------------------------------------
   !詳細ログ出力開始
------------------------------------------------------------ */
void UCSKit_BrainQueryComponent::DebugBeginOutputLog()
{
	if (!mbDebugOutputLog)
	{
		return;
	}
	mDebugOutputLog.Empty();
	mDebugOutputLog = FString::Printf(TEXT("/***** BeginBrainQueryLog(%d) *****/\n"), mDebugOutputLogCount);
}
/* ------------------------------------------------------------
   !詳細ログ出力終了
------------------------------------------------------------ */
void UCSKit_BrainQueryComponent::DebugEndOutputLog()
{
	if (!mbDebugOutputLog)
	{
		return;
	}
	mDebugOutputLog += FString::Printf(TEXT("/***** EndBrainQueryLog(%d) *****/\n"), mDebugOutputLogCount);
	++mDebugOutputLogCount;
}
/**
 * @brief 
 */
void UCSKit_BrainQueryComponent::DebugOutputLog_BeginCalcScore(const FCSKit_BrainQueryTest& InTest)
{
	if (!mbDebugOutputLog)
	{
		return;
	}
	mDebugOutputLog += FString::Printf(TEXT("\t[BeginCalcScore(%s)]\n"), *InTest.mTestName.ToString());
}
/**
 * @brief 
 */
void UCSKit_BrainQueryComponent::DebugOutputLog_EndCalcScore(const FCSKit_BrainQueryTest& InTest, const float InScore)
{
	if (!mbDebugOutputLog)
	{
		return;
	}
	mDebugOutputLog += FString::Printf(TEXT("\t[EndCalcScore %s(%.3f)]\n"), *InTest.mTestName.ToString(), InScore);
}
/**
 * @brief 
 */
void UCSKit_BrainQueryComponent::DebugOutputLog_BeginCalcTestNodeScore(const FCSKit_BrainQueryTestNode& InTest)
{
	if (!mbDebugOutputLog)
	{
		return;
	}

	FString TestNodeName(TEXT("---"));
#if WITH_EDITOR
	TestNodeName = InTest.mEditorComment;
#else
	if (const UClass* BrainQueryTestClass = InTest.mBrainQueryTest.Get())
	{
		TestNodeName = BrainQueryTestClass->GetName();
	}
#endif
	mDebugOutputLog += FString::Printf(TEXT("\t\t[BeginCalcTestNodeScore(%s)]\n"), *TestNodeName);
}
/**
 * @brief 
 */
void UCSKit_BrainQueryComponent::DebugOutputLog_EndCalcTestNodeScore(const FCSKit_BrainQueryTestNode& InTest, const bool bInPassFilter)
{
	if (!mbDebugOutputLog)
	{
		return;
	}

	FString TestNodeName(TEXT("---"));
#if WITH_EDITOR
	TestNodeName = InTest.mEditorComment;
#else
	if (const UClass* BrainQueryTestClass = InTest.mBrainQueryTest.Get())
	{
		TestNodeName = BrainQueryTestClass->GetName();
	}
#endif
	mDebugOutputLog += FString::Printf(TEXT("\t\t[EndCalcTestNodeScore(%s) Filter(%d)]\n"), *TestNodeName, bInPassFilter);
}
/**
 * @brief 
 */
void UCSKit_BrainQueryComponent::DebugOutputLog_EndCalcTestNodeScore(const FCSKit_BrainQueryTestNode& InTest, const float InScore)
{
	if (!mbDebugOutputLog)
	{
		return;
	}

	FString TestNodeName(TEXT("---"));
#if WITH_EDITOR
	TestNodeName = InTest.mEditorComment;
#else
	if (const UClass* BrainQueryTestClass = InTest.mBrainQueryTest.Get())
	{
		TestNodeName = BrainQueryTestClass->GetName();
	}
#endif
	mDebugOutputLog += FString::Printf(TEXT("\t\t[EndCalcTestNodeScore(%s) : %.3f]\n"), *TestNodeName, InScore);
}
#endif


#if WITH_EDITOR
/**
 * @brief 
 */
void UCSKit_BrainQueryComponent::EditorCreateModifyData(const FCSKit_BrainQueryTableRow& InBrainQueryTableRow)
{
	if (mEditorModifyData == nullptr)
	{
		mEditorModifyData = new FCSKit_BrainQueryTableRow();
		*mEditorModifyData = InBrainQueryTableRow;
	}
}

/**
 * @brief 
 */
void UCSKit_BrainQueryComponent::EditorCheckDataSafety(const FCSKit_BrainQueryTableRow& InBrainQueryTableRow)
{
	if (!mbEditorCheckedDataSafety)
	{
		CSKit_BrainQueryTool::EditorCheckError(InBrainQueryTableRow);
		mbEditorCheckedDataSafety = true;
	}
}

/**
 * @brief 
 */
void UCSKit_BrainQueryComponent::EditorDrawScreenWindowGraph(UCanvas* InCanvas, const float InDeltaSec) const
{
	for (const DebugTestResult& Result : mDebugTestResultList)
	{
		if (Result.mLevel < mEditorScreenWindowGraphList.Num())
		{
			if (FCSKitDebug_ScreenWindowGraph* Graph = mEditorScreenWindowGraphList[Result.mLevel])
			{
				Graph->AddScore(Result.mTestName, Result.mScore);
			}
		}
		else
		{
			if (FCSKitDebug_ScreenWindowGraph* Graph = new FCSKitDebug_ScreenWindowGraph())
			{
				Graph->InitWindowExtent();
				Graph->AddScore(Result.mTestName, Result.mScore);
				mEditorScreenWindowGraphList.Add(Graph);//Result.mLevelは小さい順に積まれてるので
			}
		}
	}

	FVector2D DrawPos(30.f, 30.f);
	for (FCSKitDebug_ScreenWindowGraph* Graph : mEditorScreenWindowGraphList)
	{
		if (Graph)
		{
			Graph->Update(InDeltaSec);
			Graph->Draw(InCanvas, DrawPos);
			DrawPos.X += Graph->GetWindowExtent().X + 10.f;
		}
	}
}

#endif