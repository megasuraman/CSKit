// Copyright 2022 megasuraman
/**
 * @file CSKitEditor_EUW_Base.h
 * @brief ShortcutCommand用EUW
 * @author megasuraman
 * @date 2023/03/30
 */
#pragma once

#include "CoreMinimal.h"
#include "CSKitEditor_EUW_Base.h"
#include "CSKitDebug_ShortcutCommand.h"
#include "Serialization/JsonSerializerMacros.h"
#include "CSKitEditor_EUW_ShortcutCommand.generated.h"

struct FCSKitEditor_PersonalShortcutCommandSaveNode : public FJsonSerializable
{
	BEGIN_JSON_SERIALIZER
	JSON_SERIALIZE("mConsoleCommand", mConsoleCommand);
	JSON_SERIALIZE("mKeyboardString", mKeyboardString);
	JSON_SERIALIZE("mPadString", mPadString);
	END_JSON_SERIALIZER

	FString mConsoleCommand;
	FString mKeyboardString;
	FString mPadString;
};
struct FCSKitEditor_PersonalShortcutCommand : public FJsonSerializable
{
	BEGIN_JSON_SERIALIZER
	JSON_SERIALIZE_ARRAY_SERIALIZABLE("mNodeList", mNodeList, FCSKitEditor_PersonalShortcutCommandSaveNode);
	END_JSON_SERIALIZER

	TArray<FCSKitEditor_PersonalShortcutCommandSaveNode> mNodeList;
};

class UCSKitDebug_ShortcutCommand;

UCLASS()
class CSKITEDITOR_API UCSKitEditor_EUW_ShortcutCommand : public UCSKitEditor_EUW_Base
{
	GENERATED_BODY()
public:
	virtual void PostInitProperties() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
protected:
	virtual void OnRunGame(const UWorld& InWorld) override;
	static UCSKitDebug_ShortcutCommand* GetShortcutCommand(const UWorld& InWorld);
	void Save();
	void Load();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EUW_ShortcutCommand", Meta = (DisplayName = "自分専用コマンド"))
	TMap<FString, FCSKitDebugKey> mPersonalShortcutCommand;
};
