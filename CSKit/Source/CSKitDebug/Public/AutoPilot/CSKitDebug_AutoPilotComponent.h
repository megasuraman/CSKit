// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_AutoPilotComponent.h
 * @brief PlayerControllerの入力をプログラムから行う
 * @author megasuraman
 * @date 2021/12/28
 */

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputCore/Classes/InputCoreTypes.h"
#include "CSKitDebug_AutoPilotComponent.generated.h"

class UCanvas;
class APlayerController;
class UCSKitDebug_AutoPilotModeBase;

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
enum class ECSKitDebug_AutoPilotMode : uint8
{
	Invalid,
	Record,
	Random,
	Command,
	AutoPlay,
	SemiAutoPlay,
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CSKITDEBUG_API UCSKitDebug_AutoPilotComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCSKitDebug_AutoPilotComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

public:
	void	PreProcessInput(float DeltaTime);
	void	PostProcessInput(float DeltaTime);
	class APlayerController* GetPlayerController();

	void	SetMode(ECSKitDebug_AutoPilotMode InMode);
	void	SetIgnoreDefaultInput(const bool InIgnore);

	void	RequestPlayInputRecord(const FString& InFileName);
	bool	IsFinishPlayRecord() const;
	void	RequestBeginRecord(const FString& InFileName);
	void	RequestEndRecord();
	void	RequestIdleRecord();

protected:
	void	RequestDebugDraw(const bool bInActive);
	void	DebugDraw(class UCanvas* InCanvas, class APlayerController* InPlayerController);

	void	OnBeginMode();
	void	OnEndMode();
	void	SetFixFrameRate(bool InFix);

private:
	UPROPERTY()
	UCSKitDebug_AutoPilotModeBase* mActiveMode = nullptr;

	FDelegateHandle	mDebugDrawHandle;
	ECSKitDebug_AutoPilotMode	mMode = ECSKitDebug_AutoPilotMode::Invalid;
	bool	mbIgnoreInput = false;

#if 0
public:


	void	RequestBeginRandom();
	void	RequestEndRandom();

	void	RequestBeginAutoPlay();
	void	RequestEndAutoPlay();

	void	RequestBeginSemiAutoPlay();
	void	RequestEndSemiAutoPlay();

	void	OnPushUIKey(const FKey& InKey);

	uint32	GetKeyId(const FKey* InKey) const;
	uint32	GetKeyId(const FKey& InKey) const;
	const FKey* GetKey(EAutoPilotKey InKey) const;
	const PadDeadZoneMap& GetPadDeadZoneMap() const { return mPadDeadZoneMap; }

	UFUNCTION(BlueprintCallable, Category = "Command", meta = (DevelopmentOnly))
		int32	RequestCommandInput(ECSKitDebug_AutoPilotKey InKey, float InInputTime, float InAxisValue);
	UFUNCTION(BlueprintCallable, Category = "Command", meta = (DevelopmentOnly))
		int32	RequestCommandMove(const FVector& InGoalPos);
	UFUNCTION(BlueprintCallable, Category = "Command", meta = (DevelopmentOnly))
		int32	RequestCommandMoveStop();
	UFUNCTION(BlueprintCallable, Category = "Command", meta = (DevelopmentOnly))
		int32	RequestCommandAttack();
	UFUNCTION(BlueprintCallable, Category = "Command", meta = (DevelopmentOnly))
		bool	IsFinishCommand(int32 InCommandId);
	UFUNCTION(BlueprintCallable, Category = "Command", meta = (DevelopmentOnly))
		bool	IsFinishLastCommand();
	UFUNCTION(BlueprintCallable, Category = "Command", meta = (DevelopmentOnly))
		bool	IsFinishAllCommand();
	UFUNCTION(BlueprintCallable, Category = "Command", meta = (DevelopmentOnly))
		bool	RequestCommandScript(FString InPath);

	UFUNCTION(BlueprintCallable, Category = "Command", meta = (DevelopmentOnly))
		FVector	GetCommandMoveGoalPos() const;

protected:

public:

private:
	FDelegateHandle	mDebugDrawDelegateHandle;
	union
	{
		uint8	mFlags = 0;
		struct
		{
			uint8 mbIgnoreInput : 1;
			uint8 : 7;
		};
	};

public:
	void	DebugUpdateSelectInfo(float DeltaTime);
	void	DebugDrawSelected(UCanvas* Canvas, const APlayerController* InPlayerController);
#endif
};