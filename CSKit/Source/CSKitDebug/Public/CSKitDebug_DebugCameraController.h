// Copyright 2020 megasuraman.
/**
 * @file CSKitDebug_DebugCameraController.h
 * @brief 操作方法や機能を変更したDebugCamera
 * @author megasuraman
 * @date 2025/05/05
 */
#pragma once

#include "CoreMinimal.h"
#include "Engine/DebugCameraController.h"
#include "NavigationData.h"
#include "CSKitDebug_DebugCameraController.generated.h"

class ASpectatorPawn;
struct FHitResult;
enum ECollisionChannel : int;

UCLASS()
class CSKITDEBUG_API ACSKitDebug_DebugCameraController : public ADebugCameraController
{
	GENERATED_BODY()

public:
	virtual ASpectatorPawn* SpawnSpectatorPawn() override;
	virtual void OnActivate(class APlayerController* OriginalPC) override;
	virtual void OnDeactivate(class APlayerController* RestoredPC) override;
protected:
	virtual void SetupInputComponent() override;

	void MoveForward(float Val);
	void MoveRight(float Val);
	void MoveUp_World(float Val);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	void SpeedLevelUp();
	void SpeedLevelDown();
	void WarpPlayer();
	void SwitchDrawViewPointInfo();
	void StepTestPathFindingState();

	bool LineTraceChannelToViewPoint(FHitResult& OutResult, const ECollisionChannel InTraceChannel) const;
	void TestPathFinding();

	void DebugRequestDraw(const bool bInActive);
	void DebugDraw(UCanvas* InCanvas, class APlayerController* InPlayerController) const;
	void DebugDrawViewPointInfo(UCanvas* InCanvas) const;
	void DebugDrawTestPathFinding(UCanvas* InCanvas) const;

private:
	FDelegateHandle mDebugDrawHandle;
	FPathFindingResult TestPathFindingResult;
	FVector TestPathFindingBeginPos = FVector::ZeroVector;
	FVector TestPathFindingEndPos = FVector::ZeroVector;
	enum class ETestPathFindingState : uint8 
	{
		Standby,
		FindBeginPos,
		FindEndPos,
		KeepResult,
	};
	ETestPathFindingState TestPathFindingState = ETestPathFindingState::Standby;
	bool bTestPathfindingMode = false;
	bool bDrawViewPointInfo = false;
};
