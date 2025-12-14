// Copyright 2020 megasuraman.
/**
 * @file CSKit_BrainQueryComponent.h
 * @brief 思考判断用Component
 *			ユーティリティベースAIの構築
 * @author megasuraman
 * @date 2025/05/05
 */
#pragma once

#include "CoreMinimal.h"
#include "CSKit_DataTableRowSelector.h"
#include "Components/ActorComponent.h"
#include "CSKit_BrainQueryComponent.generated.h"

class ACSKit_AIController;
class UBlackboardComponent;
struct FCSKit_BrainQueryTableRow;
struct FCSKit_BrainQueryTest;
struct FCSKit_BrainQueryTestNode;
struct FCSKitDebug_ScreenWindowGraph;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CSKIT_API UCSKit_BrainQueryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCSKit_BrainQueryComponent();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void	Update(const float InDeltaSec);

protected:
	virtual float CalcTestScoreRatio(const FName& InName) const { return 1.f; }
	void	CalcBrainQuery(const FCSKit_BrainQueryTableRow& InBrainQueryTableRow);
	float	CalcBrainQueryTestScore(const FCSKit_BrainQueryTest& InBrainQueryTest, const ACSKit_AIController& InOwner);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCSKit_DataTableRowSelector mBrainQueryRowNameSelector;
	
private:
	TWeakObjectPtr<UBlackboardComponent>	mBlackboardComponent;

#if USE_CSKIT_DEBUG
public:
	virtual FString DebugDrawSelectedActorWatcher(UCanvas* InCanvas) const;

	void DebugRequestOutputLog(const bool bInOutputLog);
	uint32 DebugGetOutputLogCount() const { return mDebugOutputLogCount; }
	const FString& DebugGetOutputLog() const { return mDebugOutputLog; }
	void DebugAddForceSelectTestName(const FName& InName);
	void DebugResetForceSelectTestName(){mDebugForceSelectTestNameList.Empty();}
	const TMap<FName,FName>& DebugGetLastSelectTestMap() const{return mDebugLastSelectTestMap;}

protected:
	void	DebugBeginOutputLog();
	void	DebugEndOutputLog();
	void	DebugOutputLog_BeginCalcScore(const FCSKit_BrainQueryTest& InTest);
	void	DebugOutputLog_EndCalcScore(const FCSKit_BrainQueryTest& InTest, const float InScore);
	void	DebugOutputLog_BeginCalcTestNodeScore(const FCSKit_BrainQueryTestNode& InTest);
	void	DebugOutputLog_EndCalcTestNodeScore(const FCSKit_BrainQueryTestNode& InTest, const bool bInPassFilter);
	void	DebugOutputLog_EndCalcTestNodeScore(const FCSKit_BrainQueryTestNode& InTest, const float InScore);

private:
	struct DebugTestResult
	{
		FName mTestName;
		float mScore = 0.f;
		uint8 mLevel = 0;
		bool mbSelect = false;
	};
	TArray<DebugTestResult> mDebugTestResultList;
	FString mDebugOutputLog;
	TArray<FName> mDebugForceSelectTestNameList;
	TMap<FName,FName> mDebugLastSelectTestMap;
	uint32 mDebugOutputLogCount = 0;
	bool mbDebugOutputLog = false;
	mutable bool mbDebugCollectTestResult = false;
#endif

#if WITH_EDITOR
public:
	void EditorBeginModifyTestNode() { mbEditorModifyTestNode = true; }

	FCSKit_BrainQueryTableRow* EditorGetModifyData() const { return mEditorModifyData; }
protected:
	void EditorCreateModifyData(const FCSKit_BrainQueryTableRow& InBrainQueryTableRow);
	void EditorCheckDataSafety(const FCSKit_BrainQueryTableRow& InBrainQueryTableRow);
	void EditorDrawScreenWindowGraph(UCanvas* InCanvas, const float InDeltaSec) const;
#endif

#if WITH_EDITORONLY_DATA
protected:
	mutable TArray<FCSKitDebug_ScreenWindowGraph*> mEditorScreenWindowGraphList;
	FCSKit_BrainQueryTableRow* mEditorModifyData = nullptr;
	bool mbEditorModifyTestNode = false;
	bool mbEditorCheckedDataSafety = false;
#endif
};
