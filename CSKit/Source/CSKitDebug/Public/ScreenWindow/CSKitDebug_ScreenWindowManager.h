// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_ScreenWindowManager.h
 * @brief ScreenWindow管理
 * @author SensyuGames
 * @date 2023/02/04
 */
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CSKitDebug_ScreenWindowText.h"
#include "CSKitDebug_ScreenWindowManager.generated.h"

class APlayerController;
class UCanvas;

struct CSKITDEBUG_API FCSKitDebug_ScreenWindowOption
{
	FLinearColor	mFrameColor = FLinearColor(0.1f, 0.9f, 0.1f, 1.f);
	float	mDispTime = -1.f;
	float	mDispBorderDistance = -1.f;
};

/**
 * 
 */
UCLASS(BlueprintType)
class CSKITDEBUG_API UCSKitDebug_ScreenWindowManager : public UObject
{
	GENERATED_BODY()

public:
	UCSKitDebug_ScreenWindowManager();

	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSKitDebug"))
	void	AddWindowBP(const FName InTag, const FText InMessage, const AActor* InFollowActor, float InDispTime);

#if USE_CSKIT_DEBUG
public:
	static	UCSKitDebug_ScreenWindowManager* Get(UObject* InOwner);
	void	Init();
	bool	DebugTick(float InDeltaSecond);
	void	DebugDraw(UCanvas* InCanvas);

	void	AddWindow(const FName InTag, const FString& InMessage, const AActor* InFollowActor=nullptr, const FCSKitDebug_ScreenWindowOption& InOption= FCSKitDebug_ScreenWindowOption());

protected:
	void	OnAddWindow(const FName InTag, const FString& InMessage, const AActor* InFollowActor, const FCSKitDebug_ScreenWindowOption& InOption);

	void	UpdateLifeTime(const float InDeltaSecond);

	void	DrawWindow(UCanvas* InCanvas);

private:
	struct FTempWindowData
	{
		FCSKitDebug_ScreenWindowText	mWindow;
		FName	mTagName;
		TWeakObjectPtr<const AActor>	mFollowTarget;
		float	mLifeTime = -1.f;
		bool	mbActive = false;
	};
	TArray<FTempWindowData>	mTempWindowDataList;
#endif//USE_CSKIT_DEBUG
};
