// Copyright 2020 megasuraman.
/**
 * @file CSKit_Config.h
 * @brief 
 * @author megasuraman
 * @date 2025/05/06
 */
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "CSKit_Config.generated.h"


class UDataTable;

UCLASS(config = CSKit, defaultconfig)
class UCSKit_Config : public UObject
{
	GENERATED_BODY()

	UCSKit_Config();

public:
	UPROPERTY(EditAnywhere, config, Category = CSKit_NeedLevel, meta = (MetaClass = "/Script/CSKit.CSKit_NeedLevelManager") )
	FSoftClassPath mNeedLevelClass;
	UPROPERTY(EditAnywhere, config, Category = CSKit_NeedLevel, meta = (MetaClass = "/Script/CSKit.CSKit_WorrySourceManager") )
	FSoftClassPath mWorrySourceManagerClass;
	UPROPERTY(EditAnywhere, config, Category = CSKit_NeedLevel )
	FGameplayTag mAIFlowNodeTag;
	UPROPERTY(EditAnywhere, config, Category = CSKit_NeedLevel )
	float mNeedLevelDistanceBorderHigh = 2000.f;
	UPROPERTY(EditAnywhere, config, Category = CSKit_NeedLevel )
	float mNeedLevelDistanceBorderLow = 3000.f;
	UPROPERTY(EditAnywhere, config, Category = CSKit_NeedLevel )
	float mNeedLevelDistanceBorderZero = 4000.f;
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, config, Category = CSKit_BrainQuery )
	FString mEditorBrainQueryTestNameSelectorDataTablePath;
	UPROPERTY(EditAnywhere, config, Category = CSKit_BrainQuery )
	TSoftObjectPtr<UDataTable> mEditorBrainQuery_DataTable;
#endif
};
