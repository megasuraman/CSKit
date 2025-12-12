// Copyright 2022 megasuraman

#pragma once

#include "CoreMinimal.h"
#include "DebugMenu/CSKitDebug_DebugMenuNodeBase.h"
#include "DebugMenu/CSKitDebug_DebugMenuSave.h"
#include "CSKitDebug_DebugMenuManager.generated.h"

class CSKitDebug_DebugMenuNodeBase;

UCLASS()
class CSKITDEBUG_API UCSKitDebug_DebugMenuManager : public UObject
{
	GENERATED_BODY()

	struct FFolder;
	
public:
	static UCSKitDebug_DebugMenuManager* sGet(const UObject* InObject);
	static bool sGetNodeValue_Bool(const UObject* InObject, const FString& InPath);

	UCSKitDebug_DebugMenuManager();
	virtual void BeginDestroy() override;

	void Init();
	void DebugTick(const float InDeltaTime);
	void DebugDraw(UCanvas* InCanvas);
	CSKitDebug_DebugMenuNodeBase* AddNode(const FString& InFolderPath, const FCSKitDebug_DebugMenuNodeData& InNodeData);
	CSKitDebug_DebugMenuNodeBase* AddNode_Bool(const FString& InFolderPath, const FString& InDisplayName, const bool InInitValue);
	CSKitDebug_DebugMenuNodeBase* AddNode_Button(const FString& InFolderPath, const FString& InDisplayName, const FCSKitDebug_DebugMenuNodeActionDelegate& InDelegate);
	bool GetNodeValue_Bool(const FString& InPath) const;
	void SetNodeValue_Bool(const FString& InPath, const bool InValue);
	void SetNodeActionDelegate(const FString& InPath, const FCSKitDebug_DebugMenuNodeActionDelegate& InDelegate);
	void SetMainFolder(const FString& InPath);
	void BackMainFolder();
	void SetActive(const bool bInActive);
	bool IsActive() const {return mbActive;}

protected:
	void SetupMenuByDataTable();
	void SetupDefaultMenu();
	void ClearNode();
	APlayerController* FindPlayerController() const;
	void ChangeSelectNode(const bool bInDown);
	void DrawMainFolderPath(UCanvas* InCanvas, const FVector2D& InPos) const;
	FFolder& FindOrAddFolder(const FString& InPath);
	CSKitDebug_DebugMenuNodeBase* FindOrAddDebugMenuNodeFolder(const FString& InPath);
	CSKitDebug_DebugMenuNodeBase* FindDebugMenuNode(const FString& InPath);
	void AssignNodeToFolder(CSKitDebug_DebugMenuNodeBase* InNode);
	FString CheckPathString(const FString& InPath) const;
	void Save(const FCSKitDebug_DebugMenuNodeActionParameter& InParameter);
	void Load(const FCSKitDebug_DebugMenuNodeActionParameter& InParameter);
	void OnSetAutoLoadByDebugMenu(const FCSKitDebug_DebugMenuNodeActionParameter& InParameter) const;
	static bool IsAutoLoad();

private:
	struct FFolder
	{
		TArray<CSKitDebug_DebugMenuNodeBase*> mNodeList;
		FString mPath;
	};
	TMap<FString, CSKitDebug_DebugMenuNodeBase*> mNodeMap;
	TMap<FString, FFolder> mFolderMap;
	FCSKitDebug_DebugMenuSaveData mSaveData;
	FString mMainFolderPath;
	FString mRootPath = FString(TEXT("~"));
	CSKitDebug_DebugMenuNodeBase* mSelectNode = nullptr;
	bool mbActive = false;
	bool mbDoneAutoLoad = false;
};
