// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_GhostController.h
 * @brief 任意のActorを間接的に操作可能にするController
 * @author megasuraman
 * @date 2025/03/30
 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CSKitDebug_GhostController.generated.h"


DECLARE_DELEGATE_OneParam(FCSKitDebug_GhostInputDelegate, APlayerController*)
DECLARE_DELEGATE_OneParam(FCSKitDebug_GhostInputDrawDelegate, UCanvas*)

UCLASS()
class CSKITDEBUG_API ACSKitDebug_GhostController : public APlayerController
{
	GENERATED_UCLASS_BODY()
public:
	virtual void AddCheats(bool bForce) override;
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* aPawn) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void DebugRequestDraw(const bool bInActive);
	void DebugDraw(UCanvas* InCanvas, class APlayerController* InPlayerController) const;

private:
	FDelegateHandle mDebugDrawHandle;
};
