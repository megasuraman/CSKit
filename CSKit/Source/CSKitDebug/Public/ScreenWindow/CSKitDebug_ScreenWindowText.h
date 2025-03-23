// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_ScreenWindowText.h
 * @brief デバッグ情報表示用Window　テキスト表示
 * @author megasuraman
 * @date 2021/12/27
 */

#pragma once

#include "CoreMinimal.h"
#include "CSKitDebug_LoopOrderArray.h"
#include "CSKitDebug_ScreenWindowBase.h"
#include "CSKitDebug_ScreenWindowText.generated.h"

/**
 * 
 */
USTRUCT(Blueprintable)
struct CSKITDEBUG_API FCSKitDebug_ScreenWindowText : public FCSKitDebug_ScreenWindowBase
{
    GENERATED_USTRUCT_BODY()

    FCSKitDebug_ScreenWindowText(){}
    virtual ~FCSKitDebug_ScreenWindowText() override {}
 
public:
	virtual void    FittingWindowExtent(class UCanvas* InCanvas) override;

    void    AddText(const FString& InString);
    void    ClearString()
    {
        mStringList.Clear();
        SetWindowExtent(FVector2D::ZeroVector);
    }
    void    SetFontColor(const FLinearColor& InColor) { mFontColor = InColor; }

protected:
    virtual void    DrawAfterBackground(class UCanvas* InCanvas, const FVector2D& InPos2D) const override;
 
private:
    TCSKitDebug_LoopOrderArray<FString> mStringList{64};
    FLinearColor	mFontColor = FLinearColor(0.1f, 0.9f, 0.1f, 1.f);
    float   mWidthInterval = 5.f;
    float   mHeightInterval = 2.f;//文字の上下に空ける長さ
    float   mFontWidth = 9.f;
    float   mFontHeight = 15.f;
};