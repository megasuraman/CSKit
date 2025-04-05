// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_ActorSelectComponent.h
 * @brief デバッグ選択対象Component
 * @author megasuraman
 * @date 2020/05/27
 */

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ScreenWindow/CSKitDebug_ScreenWindowText.h"
#include "CSKitDebug_ActorSelectComponent.generated.h"

class UCSKitDebug_ActorSelectManager;
class UCanvas;
class AAIController;

DECLARE_DELEGATE_RetVal_OneParam(FString, FCSKitDebug_ActorSelectDrawDelegate, UCanvas*);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CSKITDEBUG_API UCSKitDebug_ActorSelectComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCSKitDebug_ActorSelectComponent();

#if USE_CSKIT_DEBUG
public:	

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void SetColor(FLinearColor InColor){mColor = InColor;}

public:
	void    AddText(const FString& InString);
    void    SetDrawDelegate(const FCSKitDebug_ActorSelectDrawDelegate& InDelegate);

	void	DebugDraw(UCanvas* InCanvas);
	void	DrawMark(class UCanvas* InCanvas) const;

	void	SetSelect(bool bInSelect);
	bool	IsSelect() const { return !!mbSelect; }
	const FLinearColor&	GetColor() const { return mColor; }

private:
	FCSKitDebug_ScreenWindowText    mScreenWindow;
	FCSKitDebug_ActorSelectDrawDelegate mDrawDelegate;
	TWeakObjectPtr<UCSKitDebug_ActorSelectManager>	mManager;
	FLinearColor mColor = FLinearColor(0.1f, 0.9f, 0.1f, 1.f);
	bool mbSelect = false;//選択済み
#endif//USE_CSKIT_DEBUG
};
