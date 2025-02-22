// Copyright 2020 megasuraman
/**
 * @file CSKitEditor_EUW_DebugMenu.h
 * @brief DebugMenu操作用のEUW
 * @author megasuraman
 * @date 2025/01/27
 */

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget/CSKitEditor_EUW_Base.h"
#include "CSKitEditor_EUW_DebugMenu.generated.h"

/**
 * 
 */
UCLASS()
class CSKITEDITOR_API UCSKitEditor_EUW_DebugMenu : public UCSKitEditor_EUW_Base
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

protected:
	virtual void OnRunGame(const UWorld& InWorld) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CSKitEditor_EUW_DebugMenu", DisplayName = "AutoLoad")
	bool mbAutoLoad = false;
};
