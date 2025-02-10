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

DECLARE_DYNAMIC_DELEGATE(FDebugSelectPreDrawDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CSKITDEBUG_API UCSKitDebug_ActorSelectComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCSKitDebug_ActorSelectComponent();

	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSKitDebug"))
	void    AddTextBP(const FString& InString)
	{
#if USE_CSKIT_DEBUG
		mScreenWindow.AddText(InString);
#endif//USE_CSKIT_DEBUG
	}
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSKitDebug"))
	void    AddPreDrawDelegateBP(FDebugSelectPreDrawDelegate InDelegate)
	{
#if USE_CSKIT_DEBUG
		mPreDrawDelegate = InDelegate;
		mbUsePreDrawDelegate = true;
#endif//USE_CSKIT_DEBUG
	}

protected:
	UPROPERTY(EditAnywhere)
	FLinearColor	mMyColor = FLinearColor(0.1f, 0.9f, 0.1f, 1.f);


#if USE_CSKIT_DEBUG
public:	

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

public:
	void    AddText(const FString& InString)
	{
		mScreenWindow.AddText(InString);
	}
    void    AddPreDrawDelegate(FDebugSelectPreDrawDelegate InDelegate)
    {
        mPreDrawDelegate = InDelegate;
        mbUsePreDrawDelegate = true;
    }

	void	DebugDraw(UCanvas* InCanvas);
	void	DrawMark(class UCanvas* InCanvas) const;

	void	SetSelect(bool bInSelect);
	bool	IsSelect() const { return !!mbSelect; }
	const FLinearColor&	GetMyColor() const { return mMyColor; }

private:
	FCSKitDebug_ScreenWindowText    mScreenWindow;
	FDebugSelectPreDrawDelegate mPreDrawDelegate;
	TWeakObjectPtr<UCSKitDebug_ActorSelectManager>	mManager;
	uint8	mbSelect : 1;//選択済み
	uint8   mbUsePreDrawDelegate : 1;
	uint8 : 6;

#endif//USE_CSKIT_DEBUG
};
