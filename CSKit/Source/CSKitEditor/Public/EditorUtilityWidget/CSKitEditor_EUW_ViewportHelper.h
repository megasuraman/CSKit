// Copyright 2020 megasuraman
/**
 * @file CSKitEditor_EUW_ViewportHelper.h
 * @brief UObject‚ÌProperty’l•\Ž¦
 * @author megasuraman
 * @date 2022/08/07
 */

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget/CSKitEditor_EUW_Base.h"
#include "CSKitEditor_EUW_ViewportHelper.generated.h"

/**
 * 
 */
UCLASS()
class CSKITEDITOR_API UCSKitEditor_EUW_ViewportHelper : public UCSKitEditor_EUW_Base
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void	LookAt(const FString& InString);
	
	UFUNCTION(BlueprintCallable)
	void	RequestDraw(bool bInDraw);

protected:
	virtual void Draw(UCanvas* InCanvas, APlayerController* InPlayerController) override;

private:
	FVector		mLastLookAtLocation = FVector::ZeroVector;
};
