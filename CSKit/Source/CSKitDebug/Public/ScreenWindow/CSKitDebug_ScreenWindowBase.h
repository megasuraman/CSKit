// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_ScreenWindowBase.h
 * @brief デバッグ情報表示用WindowBase
 * @author megasuraman
 * @date 2021/12/27
 */

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CSKitDebug_ScreenWindowBase.generated.h"

/**
 * 
 */
USTRUCT(Blueprintable)
struct CSKITDEBUG_API FCSKitDebug_ScreenWindowBase
{
    GENERATED_USTRUCT_BODY()
 
    FCSKitDebug_ScreenWindowBase(){}
    virtual ~FCSKitDebug_ScreenWindowBase(){}

public:
	FVector2D	Draw(class UCanvas* InCanvas, const FVector2D& InPos2D) const;
    FVector2D	Draw(class UCanvas* InCanvas, const float InPosRatioX, const float InPosRatioY) const;
    FVector2D	Draw(class UCanvas* InCanvas, const FVector& InPos, const float InBorderDistance=-1.f) const;

	virtual void    FittingWindowExtent(class UCanvas* InCanvas) {}
    void    SetWindowName(const FString& InName) { mWindowName = InName; }
    void    SetWindowExtent(const FVector2D& InExtent) { mWindowExtent = InExtent; }
    void    SetWindowBackColor(const FLinearColor& InColor) { mWindowBackColor = InColor; }
    void    SetWindowFrameColor(const FLinearColor& InColor) { mWindowFrameColor = InColor; }

    const FVector2D& GetWindowExtent() const { return mWindowExtent; }

protected:
	virtual void    DrawAfterBackground(class UCanvas* InCanvas, const FVector2D& InPos2D) const {}

    float    DrawWindowName(class UCanvas* InCanvas, const FVector2D& InPos2D) const;
    class UFont* GetUseFont() const;
    void    CalcTextDispWidthHeight(float& OutWidth, float& OutHeight, UCanvas* InCanvas, const FString& InText) const;
 
private:
    FVector2D	mWindowExtent = FVector2D::ZeroVector;
    FString		mWindowName;
    FLinearColor	mWindowBackColor = FLinearColor(0.01f, 0.01f, 0.01f, 0.5f);
	FLinearColor	mWindowFrameColor = FLinearColor(0.1f, 0.9f, 0.1f, 1.f);
	FLinearColor	mWindowNameColor = FLinearColor(0.1f, 0.9f, 0.1f, 1.f);
    float       mFontScale = 1.f;
};