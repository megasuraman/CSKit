// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_Config.h
 * @brief CSKitDebug�p��Config
 * @author megasuraman
 * @date 2020/5/26
 */

#pragma once

#include "CoreMinimal.h"
#include "CSKitDebug_Config.generated.h"

USTRUCT(BlueprintType)
struct FCSKitDebugKey
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere, config, Category = CSKitDebugCommand)
	FKey	mKeyboard;
	UPROPERTY(EditAnywhere, config, Category = CSKitDebugCommand)
	FKey	mPad;

	bool	IsPressed(const class UPlayerInput& InInput) const;
	bool	IsJustPressed(const class UPlayerInput& InInput) const;
	bool	IsJustReleased(const class UPlayerInput& InInput) const;
};

USTRUCT(BlueprintType)
struct FCSKitDebugSecretCommand
{
	GENERATED_USTRUCT_BODY();
	
	UPROPERTY(EditAnywhere, config, Category = CSKitDebugCommand)
	TArray<FKey> mKeyList;
};

/**
 * 
 */
UCLASS(config = CSKitDebug, defaultconfig)
class CSKITDEBUG_API UCSKitDebug_Config : public UObject
{
	GENERATED_BODY()

	UCSKitDebug_Config();

public:
	UPROPERTY(EditAnywhere, config, Category = CSKitDebug)
	bool	mbActiveCSKitDebug = true;

	UPROPERTY(EditAnywhere, config, Category = CSKitDebugCommand)
	FCSKitDebugKey	mDebugCommand_ReadyKey;
	
	UPROPERTY(EditAnywhere, config, Category = CSKitDebugCommand)
	FCSKitDebugKey	mDebugCommand_DebugMenuKey;
	
	UPROPERTY(EditAnywhere, config, Category = CSKitDebugCommand)
	FCSKitDebugKey	mDebugCommand_DebugStopKey;
	
	UPROPERTY(EditAnywhere, config, Category = CSKitDebugCommand)
	FCSKitDebugKey	mDebugCommand_DebugCameraKey;

	UPROPERTY(EditAnywhere, config, Category = CSKitDebugCommand)
	TMap<FString, FCSKitDebugSecretCommand> mDebugSecretCommand;
	
	UPROPERTY(EditAnywhere, config, Category = CSKitDebugSelect)
	FCSKitDebugKey	mDebugSelect_SelectKey;

	UPROPERTY(EditAnywhere, config, Category = CSKitDebugMenu)
	TSubclassOf<class UCSKitDebug_DebugMenuManager> mDebugMenuManagerClass;
	UPROPERTY(EditAnywhere, config, Category = CSKitDebugMenu)
	TSoftObjectPtr<class UDataTable> mDebugMenuDataTable = nullptr;
	UPROPERTY(EditAnywhere, config, Category = CSKitDebugMenu)
	FCSKitDebugKey	mDebugMenu_SelectKey;
	UPROPERTY(EditAnywhere, config, Category = CSKitDebugMenu)
	FCSKitDebugKey	mDebugMenu_CancelKey;
	UPROPERTY(EditAnywhere, config, Category = CSKitDebugMenu)
	FCSKitDebugKey	mDebugMenu_UpKey;
	UPROPERTY(EditAnywhere, config, Category = CSKitDebugMenu)
	FCSKitDebugKey	mDebugMenu_DownKey;
	UPROPERTY(EditAnywhere, config, Category = CSKitDebugMenu)
	FCSKitDebugKey	mDebugMenu_RightKey;
	UPROPERTY(EditAnywhere, config, Category = CSKitDebugMenu)
	FCSKitDebugKey	mDebugMenu_LeftKey;
};
