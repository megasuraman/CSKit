// Copyright 2022 megasuraman
#include "DebugMenu/CSKitDebug_DebugMenuManager.h"

#include "CanvasItem.h"
#include "CSKitDebug_Subsystem.h"
#include "CSKitDebug_Config.h"
#include "DebugMenu/CSKitDebug_DebugMenuNodeBool.h"
#include "DebugMenu/CSKitDebug_DebugMenuNodeInt.h"
#include "DebugMenu/CSKitDebug_DebugMenuNodeFloat.h"
#include "DebugMenu/CSKitDebug_DebugMenuNodeFolder.h"
#include "DebugMenu/CSKitDebug_DebugMenuNodeList.h"
#include "DebugMenu/CSKitDebug_DebugMenuNodeButton.h"
#include "DebugMenu/CSKitDebug_DebugMenuTableRow.h"
#include "Engine/Canvas.h"
#include "GameFramework/PlayerController.h"

UCSKitDebug_DebugMenuManager* UCSKitDebug_DebugMenuManager::sGet(const UObject* InObject)
{
	UGameInstance* GameInstance = InObject->GetWorld()->GetGameInstance();
	UCSKitDebug_Subsystem* CSKitDebugSubsystem = GameInstance->GetSubsystem<UCSKitDebug_Subsystem>();
	return CSKitDebugSubsystem->GetDebugMenuManager();
}

UCSKitDebug_DebugMenuManager::UCSKitDebug_DebugMenuManager()
{
}

void UCSKitDebug_DebugMenuManager::BeginDestroy()
{
	Super::BeginDestroy();

	ClearNode();
}

void UCSKitDebug_DebugMenuManager::Init()
{
	ClearNode();
	
	FindOrAddFolder(mRootPath);
	
	SetupMenuByDataTable();

	SetupDefaultMenu();

	SetMainFolder(mRootPath);

	if (IsAutoLoad())
	{
		Load(FCSKitDebug_DebugMenuNodeActionParameter());
		mbDoneAutoLoad = true;
	}
}

void UCSKitDebug_DebugMenuManager::DebugTick(const float InDeltaTime)
{
	if (!mbActive)
	{
		return;
	}
	if (mSelectNode == nullptr)
	{
		return;
	}
	APlayerController* PlayerController = FindPlayerController();
	if (PlayerController == nullptr)
	{
		return;
	}

	const UPlayerInput* PlayerInput = PlayerController->PlayerInput;
	if (PlayerInput == nullptr)
	{
		return;
	}

	const UCSKitDebug_Config* CSKitDebugConfig = GetDefault<UCSKitDebug_Config>();
	if (CSKitDebugConfig->mDebugCommand_ReadyKey.IsPressed(*PlayerInput))
	{
		return;
	}

	const bool bPressedSelectKey = CSKitDebugConfig->mDebugMenu_SelectKey.IsPressed(*PlayerInput);
	if (CSKitDebugConfig->mDebugMenu_SelectKey.IsJustPressed(*PlayerInput))
	{
		mSelectNode->OnBeginAction();
	}
	else if (CSKitDebugConfig->mDebugMenu_SelectKey.IsJustReleased(*PlayerInput))
	{
		FCSKitDebug_DebugMenuNodeActionParameter ActionParameter;
		ActionParameter.mPlayerController = PlayerController;
		mSelectNode->OnEndAction(ActionParameter);
	}
	else if (CSKitDebugConfig->mDebugMenu_CancelKey.IsJustPressed(*PlayerInput))
	{
	}
	else if (CSKitDebugConfig->mDebugMenu_UpKey.IsJustPressed(*PlayerInput))
	{
		mSelectNode->OnJustPressedUpKey();
		if (!bPressedSelectKey)
		{
			ChangeSelectNode(false);
		}
	}
	else if (CSKitDebugConfig->mDebugMenu_DownKey.IsJustPressed(*PlayerInput))
	{
		mSelectNode->OnJustPressedDownKey();
		if (!bPressedSelectKey)
		{
			ChangeSelectNode(true);
		}
	}
	else if (CSKitDebugConfig->mDebugMenu_LeftKey.IsJustPressed(*PlayerInput))
	{
		mSelectNode->OnJustPressedLeftKey();
		if (!bPressedSelectKey)
		{
			BackMainFolder();
		}
	}
	else if (CSKitDebugConfig->mDebugMenu_RightKey.IsJustPressed(*PlayerInput))
	{
		mSelectNode->OnJustPressedRightKey();
	}
}

void UCSKitDebug_DebugMenuManager::DebugDraw(UCanvas* InCanvas)
{
	if (!mbActive)
	{
		return;
	}

	FVector2D DrawPos(50.f, 30.f);

	DrawMainFolderPath(InCanvas, DrawPos);
	DrawPos.Y += 20.f;

	FVector2D SelectNodeDrawPos = DrawPos;
	if (const FFolder* MainFolder = mFolderMap.Find(mMainFolderPath))
	{
		for(const CSKitDebug_DebugMenuNodeBase* Node : MainFolder->mNodeList)
		{
			if (mSelectNode == Node)
			{
				SelectNodeDrawPos = DrawPos;//選択されてるのは最後に描画したいので
			}
			else
			{
				Node->Draw(InCanvas, DrawPos, false);
			}
			DrawPos.Y += 20.f;
		}
		if (mSelectNode)
		{
			mSelectNode->Draw(InCanvas, SelectNodeDrawPos, true);
		}
	}
}

CSKitDebug_DebugMenuNodeBase* UCSKitDebug_DebugMenuManager::AddNode(const FString& InFolderPath, const FCSKitDebug_DebugMenuNodeData& InNodeData)
{
	const FString PathString = CheckPathString(InFolderPath);
	const FString NodePath = FString::Printf(TEXT("%s/%s"), *PathString, *InNodeData.mDisplayName);
	if (CSKitDebug_DebugMenuNodeBase** NodeBase = mNodeMap.Find(NodePath))
	{
		return *NodeBase;
	}

	CSKitDebug_DebugMenuNodeBase* NewNode;
	switch (InNodeData.mKind)
	{
	case ECSKitDebug_DebugMenuValueKind::Bool:
		NewNode = new CSKitDebug_DebugMenuNodeBool();
		break;
	case ECSKitDebug_DebugMenuValueKind::Int:
		NewNode = new CSKitDebug_DebugMenuNodeInt();
		break;
	case ECSKitDebug_DebugMenuValueKind::Float:
		NewNode = new CSKitDebug_DebugMenuNodeFloat();
		break;
	case ECSKitDebug_DebugMenuValueKind::List:
	case ECSKitDebug_DebugMenuValueKind::Enum:
		NewNode = new CSKitDebug_DebugMenuNodeList();
		break;
	case ECSKitDebug_DebugMenuValueKind::Button:
		NewNode = new CSKitDebug_DebugMenuNodeButton();
		break;
	default:
		NewNode = new CSKitDebug_DebugMenuNodeBase();
		break;
	}

	NewNode->Init(PathString, InNodeData, this);
	mNodeMap.Add(NewNode->GetPath(), NewNode);
	AssignNodeToFolder(NewNode);
	if (mbDoneAutoLoad)
	{
		const FString SaveValue = mSaveData.GetValueString(NewNode->GetPath());
		if(!SaveValue.IsEmpty())
		{
			NewNode->Load(SaveValue, FCSKitDebug_DebugMenuNodeActionParameter());
		}
	}
	return NewNode;
}

CSKitDebug_DebugMenuNodeBase* UCSKitDebug_DebugMenuManager::AddNode_Bool(const FString& InFolderPath, const FString& InDisplayName, const bool InInitValue)
{
	FCSKitDebug_DebugMenuNodeData NodeData;
	NodeData.mDisplayName = InDisplayName;
	NodeData.mKind = ECSKitDebug_DebugMenuValueKind::Bool;
	NodeData.mInitValue = InInitValue ? FString(TEXT("true")) : FString(TEXT("false"));
	return AddNode(InFolderPath, NodeData);
}

CSKitDebug_DebugMenuNodeBase* UCSKitDebug_DebugMenuManager::AddNode_Button(const FString& InFolderPath, const FString& InDisplayName, const FCSKitDebug_DebugMenuNodeActionDelegate& InDelegate)
{
	FCSKitDebug_DebugMenuNodeData NodeData;
	NodeData.mDisplayName = InDisplayName;
	NodeData.mKind = ECSKitDebug_DebugMenuValueKind::Button;
	if (CSKitDebug_DebugMenuNodeBase* DebugMenuNode = AddNode(InFolderPath, NodeData))
	{
		DebugMenuNode->SetNodeAction(InDelegate);
		return DebugMenuNode;
	}
	return nullptr;
}

bool UCSKitDebug_DebugMenuManager::GetNodeValue_Bool(const FString& InPath) const
{
	const FString PathString = CheckPathString(InPath);
	if (const CSKitDebug_DebugMenuNodeBase* const* NodePtrPtr = mNodeMap.Find(PathString))
	{
		if (const CSKitDebug_DebugMenuNodeBase* NodePtr = *NodePtrPtr)
		{
			return NodePtr->GetBool();
		}
	}
	return false;
}

void UCSKitDebug_DebugMenuManager::SetNodeActionDelegate(const FString& InPath, const FCSKitDebug_DebugMenuNodeActionDelegate& InDelegate)
{
	if (CSKitDebug_DebugMenuNodeBase** NodePtrPtr = mNodeMap.Find(InPath))
	{
		if (CSKitDebug_DebugMenuNodeBase* NodePtr = *NodePtrPtr)
		{
			NodePtr->SetNodeAction(InDelegate);
		}
	}
}

void UCSKitDebug_DebugMenuManager::SetMainFolder(const FString& InPath)
{
	mMainFolderPath = InPath;

	FFolder& RootFolder = FindOrAddFolder(mMainFolderPath);
	if (RootFolder.mNodeList.Num() > 0)
	{
		mSelectNode = RootFolder.mNodeList[0];
	}
}

void UCSKitDebug_DebugMenuManager::BackMainFolder()
{
	TArray<FString> FolderList;
	mMainFolderPath.ParseIntoArray(FolderList, TEXT("/"));
	const int32 FolderNum = FolderList.Num();
	if (FolderNum <= 1)
	{
		return;
	}

	FString BackFolderPath;
	for (int32 i = 0; i < FolderNum - 1; ++i)
	{
		if (i != 0)
		{
			BackFolderPath += FString::Printf(TEXT("/"));
		}
		BackFolderPath += FolderList[i];
	}
	SetMainFolder(BackFolderPath);
}

void UCSKitDebug_DebugMenuManager::SetActive(const bool bInActive)
{
	mbActive = bInActive;
}

void UCSKitDebug_DebugMenuManager::SetupMenuByDataTable()
{
	const UCSKitDebug_Config* CSKitDebugConfig = GetDefault<UCSKitDebug_Config>();
	const UDataTable* DataTable = CSKitDebugConfig->mDebugMenuDataTable.LoadSynchronous();
	if (DataTable == nullptr)
	{
		return;
	}

	TArray<FName> RowNameList = DataTable->GetRowNames();
	for (const FName& RowName : RowNameList)
	{
		const FCSKitDebug_DebugMenuTableRow* DebugMenuTableRow = DataTable->FindRow<FCSKitDebug_DebugMenuTableRow>(RowName, FString());
		if (DebugMenuTableRow == nullptr)
		{
			continue;
		}

		FFolder NodeFolder;
		for(const FCSKitDebug_DebugMenuNodeData& NodeData : DebugMenuTableRow->mNodeList)
		{
			AddNode(RowName.ToString(), NodeData);
		}
	}
}

void UCSKitDebug_DebugMenuManager::SetupDefaultMenu()
{
	const FString BaseDebugMenuPath(TEXT("CSKitDebug/DebugMenu"));
	{
		const auto& Delegate = FCSKitDebug_DebugMenuNodeActionDelegate::CreateUObject(this, &UCSKitDebug_DebugMenuManager::Save);
		AddNode_Button(BaseDebugMenuPath, FString(TEXT("Save")), Delegate);
	}
	{
		const auto& Delegate = FCSKitDebug_DebugMenuNodeActionDelegate::CreateUObject(this, &UCSKitDebug_DebugMenuManager::Load);
		AddNode_Button(BaseDebugMenuPath, FString(TEXT("Load")), Delegate);
	}
	{
		const auto& Delegate = FCSKitDebug_DebugMenuNodeActionDelegate::CreateUObject(this, &UCSKitDebug_DebugMenuManager::OnSetAutoLoadByDebugMenu);
		CSKitDebug_DebugMenuNodeBase* DebugMenuNodeBase = AddNode_Bool(BaseDebugMenuPath, FString(TEXT("AutoLoad")), IsAutoLoad());
		DebugMenuNodeBase->SetNodeAction(Delegate);
	}
}

void UCSKitDebug_DebugMenuManager::ClearNode()
{
	for (auto& MapElement : mNodeMap)
	{
		CSKitDebug_DebugMenuNodeBase* Node = MapElement.Value;
		delete Node;
	}
	mNodeMap.Empty();
}

APlayerController* UCSKitDebug_DebugMenuManager::FindPlayerController() const
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (APlayerController* PlayerController = Iterator->Get())
		{
			if (PlayerController->Player)
			{
				return PlayerController;
			}
		}
	}
	return nullptr;
}

void UCSKitDebug_DebugMenuManager::ChangeSelectNode(const bool bInDown)
{
	const FFolder* NodeFolder = mFolderMap.Find(mMainFolderPath);
	if (NodeFolder == nullptr)
	{
		return;
	}
	const TArray<CSKitDebug_DebugMenuNodeBase*>& NodeList = NodeFolder->mNodeList;
	const int32 NodeListSize = NodeList.Num();
	if (NodeListSize <= 0)
	{
		return;
	}
	int32 SelectIndex = NodeList.Find(mSelectNode);
	if (SelectIndex == INDEX_NONE)
	{
		return;
	}
	if (bInDown)
	{
		SelectIndex = (SelectIndex + 1) % NodeListSize;
	}
	else
	{
		if (SelectIndex == 0)
		{
			SelectIndex = NodeListSize - 1;
		}
		else
		{
			SelectIndex = SelectIndex - 1;
		}
	}
	mSelectNode = NodeList[SelectIndex];
}

void UCSKitDebug_DebugMenuManager::DrawMainFolderPath(UCanvas* InCanvas, const FVector2D& InPos) const
{
	const FVector2D WindowExtent(200.f, 20.f);
	//constexpr float ValueLineOffsetX = 150.f;
	const FVector2D StringOffset(2.f, 2.f);
	constexpr FLinearColor WindowBackColor(0.01f, 0.01f, 0.01f, 0.5f);
	constexpr FLinearColor WindowFrameColor(0.1f, 0.9f, 0.1f, 1.f);
	constexpr FLinearColor FontColor(0.1f, 0.9f, 0.1f, 1.f);
	// 下敷き
	{
		FCanvasTileItem Item(InPos, WindowExtent, WindowBackColor);
		Item.BlendMode = ESimpleElementBlendMode::SE_BLEND_Translucent;
		InCanvas->DrawItem(Item);
	}
	// 枠
	{
		FCanvasBoxItem Item(InPos, WindowExtent);
		Item.SetColor(WindowFrameColor);
		Item.LineThickness = 1.f;
		InCanvas->DrawItem(Item);
	}
	// パス表示
	{
		const FVector2D StringPos = InPos + StringOffset;
		FCanvasTextItem Item(StringPos, FText::FromString(mMainFolderPath), GEngine->GetSmallFont(), FontColor);
		Item.Scale = FVector2D(1.f);
		InCanvas->DrawItem(Item);
	}
}

UCSKitDebug_DebugMenuManager::FFolder& UCSKitDebug_DebugMenuManager::FindOrAddFolder(const FString& InPath)
{
	if (FFolder* NodeFolder = mFolderMap.Find(InPath))
	{
		return *NodeFolder;
	}

	FFolder NodeFolder;
	NodeFolder.mPath = InPath;

	return mFolderMap.Add(InPath, NodeFolder);
}

CSKitDebug_DebugMenuNodeBase* UCSKitDebug_DebugMenuManager::FindOrAddDebugMenuNodeFolder(const FString& InPath)
{
	TArray<FString> PathList;
	InPath.ParseIntoArray(PathList, TEXT("/"));
	if (PathList.Num() <= 1)
	{
		return nullptr;
	}

	if (CSKitDebug_DebugMenuNodeBase** NodePtrPtr = mNodeMap.Find(InPath))
	{
		CSKitDebug_DebugMenuNodeBase* NodePtr = *NodePtrPtr;
		check(NodePtr->GetNodeData().mKind == ECSKitDebug_DebugMenuValueKind::Folder);
		return NodePtr;
	}

	FCSKitDebug_DebugMenuNodeData NodeData;
	NodeData.mKind = ECSKitDebug_DebugMenuValueKind::Folder;
	NodeData.mDisplayName = PathList[PathList.Num() - 1];

	CSKitDebug_DebugMenuNodeFolder* NodeFolder = new CSKitDebug_DebugMenuNodeFolder();
	NodeFolder->Init(InPath, NodeData, this);
	mNodeMap.Add(InPath, NodeFolder);

	FString ParentFolderPath = PathList[0];
	for (int32 i = 1; i < PathList.Num() - 1; ++i)
	{
		ParentFolderPath += FString(TEXT("/")) + PathList[i];
	}
	FFolder& Folder = FindOrAddFolder(ParentFolderPath);
	Folder.mNodeList.Add(NodeFolder);

	return NodeFolder;
}

CSKitDebug_DebugMenuNodeBase* UCSKitDebug_DebugMenuManager::FindDebugMenuNode(const FString& InPath)
{
	if (CSKitDebug_DebugMenuNodeBase** NodePtr = mNodeMap.Find(InPath))
	{
		if (NodePtr != nullptr)
		{
			return *NodePtr;
		}
	}
	return nullptr;
}

void UCSKitDebug_DebugMenuManager::AssignNodeToFolder(CSKitDebug_DebugMenuNodeBase* InNode)
{
	const FString NodePath = InNode->GetPath();
	FString FolderPath;
	TArray<FString> PathList;
	NodePath.ParseIntoArray(PathList, TEXT("/"));
	int32 LastIndex = PathList.Num();
	if (InNode->GetNodeData().mKind != ECSKitDebug_DebugMenuValueKind::Folder)
	{
		--LastIndex;
	}
	for (int32 i = 0; i < LastIndex; ++i)
	{
		if (i != 0)
		{
			FolderPath += FString(TEXT("/"));
		}
		FolderPath += PathList[i];
		FindOrAddDebugMenuNodeFolder(FolderPath);
	}

	FFolder& LastFolder = FindOrAddFolder(FolderPath);
	LastFolder.mNodeList.Add(InNode);
}

FString UCSKitDebug_DebugMenuManager::CheckPathString(const FString& InPath) const
{
	FString PathString = InPath;
	if (PathString.Left(1) != mRootPath)
	{
		PathString = mRootPath + FString(TEXT("/")) + PathString;
	}
	return PathString;
}

void UCSKitDebug_DebugMenuManager::Save(const FCSKitDebug_DebugMenuNodeActionParameter& InParameter)
{
	for (const auto& MapElement : mNodeMap)
	{
		const FString& Path = MapElement.Key;
		if (const CSKitDebug_DebugMenuNodeBase* Node = MapElement.Value)
		{
			const FCSKitDebug_DebugMenuNodeData& NodeData = Node->GetNodeData();
			if (NodeData.mKind == ECSKitDebug_DebugMenuValueKind::Button
				|| NodeData.mKind == ECSKitDebug_DebugMenuValueKind::Folder)
			{
				continue;
			}
			const FString ValueString = Node->GetValueString();
			mSaveData.WriteValue(Path, ValueString);
		}
	}
	mSaveData.Save();
}

void UCSKitDebug_DebugMenuManager::Load(const FCSKitDebug_DebugMenuNodeActionParameter& InParameter)
{
	mSaveData.Load();
	for (const auto& MapElement : mSaveData.GetValueMap())
	{
		if (CSKitDebug_DebugMenuNodeBase* Node = FindDebugMenuNode(MapElement.Key))
		{
			Node->Load(MapElement.Value, InParameter);
		}
	}
}

void UCSKitDebug_DebugMenuManager::OnSetAutoLoadByDebugMenu(const FCSKitDebug_DebugMenuNodeActionParameter& InParameter) const
{
	const bool AutoLoad = GetNodeValue_Bool(FString(TEXT("CSKitDebug/DebugMenu/AutoLoad")));
	UCSKitDebug_Subsystem::sGetSaveData().SetBool(FString(TEXT("DebugMenu_AutoLoad")), AutoLoad);
}

bool UCSKitDebug_DebugMenuManager::IsAutoLoad()
{
	return UCSKitDebug_Subsystem::sGetSaveData().GetBool(FString(TEXT("DebugMenu_AutoLoad")));
}