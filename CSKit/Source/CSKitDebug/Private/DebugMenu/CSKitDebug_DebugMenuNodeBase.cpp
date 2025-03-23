// Copyright 2022 megasuraman


#include "DebugMenu/CSKitDebug_DebugMenuNodeBase.h"

#include "CanvasItem.h"
#include "CSKitDebug_Subsystem.h"
#include "DrawDebugHelpers.h"
#include "Engine/Canvas.h"


CSKitDebug_DebugMenuNodeBase::CSKitDebug_DebugMenuNodeBase()
{
}

CSKitDebug_DebugMenuNodeBase::~CSKitDebug_DebugMenuNodeBase()
{
}

void CSKitDebug_DebugMenuNodeBase::Init(const FString& InPath, const FCSKitDebug_DebugMenuNodeData& InData, UCSKitDebug_DebugMenuManager* InManager)
{
	mNodeData = InData;
	if (mNodeData.mKind == ECSKitDebug_DebugMenuValueKind::Folder)
	{
		mPath = InPath;
	}
	else
	{
		mPath = FString::Printf(TEXT("%s/%s"), *InPath, *mNodeData.mDisplayName);
	}
	mManager = InManager;
	switch (mNodeData.mKind)
	{
	case ECSKitDebug_DebugMenuValueKind::Bool:
		SetValueBool(false);
		SetInitValue();
		break;
	case ECSKitDebug_DebugMenuValueKind::Int:
		SetValueInt(0);
		SetInitValue();
		break;
	case ECSKitDebug_DebugMenuValueKind::Float:
		SetValueFloat(0.f);
		SetInitValue();
		break;
	case ECSKitDebug_DebugMenuValueKind::List:
		SetValueList(0);
		SetInitValue();
		break;
	case ECSKitDebug_DebugMenuValueKind::Enum:
	{
		mNodeData.mList.Empty();
		if(const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, *mNodeData.mEnumName, true))
		{
			for (int32 i = 0; i < EnumPtr->NumEnums() - 1; ++i)
			{
				FString EnumName = EnumPtr->GetNameStringByIndex(i);
				mNodeData.mList.Add(EnumName);
			}
		}
		SetValueList(0);
		SetInitValue();
		break;
	}
	default:
		break;
	}
}

void CSKitDebug_DebugMenuNodeBase::OnBeginAction()
{
	mbEditMode = true;
}

void CSKitDebug_DebugMenuNodeBase::OnEndAction(const FCSKitDebug_DebugMenuNodeActionParameter& InParameter)
{
	mActionDelegate.ExecuteIfBound(InParameter);
	mbEditMode = false;
}

void CSKitDebug_DebugMenuNodeBase::Draw(UCanvas* InCanvas, const FVector2D& InPos, const bool bInSelect) const
{
	FVector2D WindowExtent(300.f, 20.f);
	constexpr float ValueLineOffsetX = 200.f;
	const bool bFolderNode = mNodeData.mKind == ECSKitDebug_DebugMenuValueKind::Folder;
	if (bFolderNode)
	{
		WindowExtent.X = ValueLineOffsetX;
	}
	const FVector2D StringOffset(2.f, 2.f);
	const FLinearColor WindowBackColor = GetWindowBackColor();
	const FLinearColor WindowFrameColor = GetWindowFrameColor();
	const FLinearColor FontColor = GetFontColor();
	// 下敷き
	{
		FCanvasTileItem Item(InPos, WindowExtent, WindowBackColor);
		Item.BlendMode = ESimpleElementBlendMode::SE_BLEND_Translucent;
		InCanvas->DrawItem(Item);
	}
	// 枠
	{
		FCanvasBoxItem Item(InPos, WindowExtent);
		if (bInSelect
			&& !mbEditMode)
		{
			Item.SetColor(GetSelectColor());
			Item.LineThickness = 3.f;
		}
		else
		{
			Item.SetColor(WindowFrameColor);
			Item.LineThickness = 1.f;
		}
		InCanvas->DrawItem(Item);
	}
	// 項目名表示
	{
		const FVector2D StringPos = InPos + StringOffset;
		FCanvasTextItem Item(StringPos, FText::FromString(mNodeData.mDisplayName), GEngine->GetSmallFont(), FontColor);
		Item.Scale = FVector2D(1.f);
		InCanvas->DrawItem(Item);
	}
	if(!bFolderNode)
	{
		// 値表示線
		const FVector2D BorderLinePosBegin(InPos.X + ValueLineOffsetX, InPos.Y);
		const FVector2D BorderLinePosEnd(InPos.X + ValueLineOffsetX, InPos.Y + WindowExtent.Y);
		DrawDebugCanvas2DLine(InCanvas, BorderLinePosBegin, BorderLinePosEnd, WindowFrameColor);

		// 値表示
		const FVector2D StringPos = BorderLinePosBegin + FVector2D(2.f, 2.f);
		DrawValue(InCanvas, StringPos, FontColor);

		if (mbEditMode)
		{
			const FVector2D ValueFrameExtent(WindowExtent.X - ValueLineOffsetX, WindowExtent.Y);
			DrawEditValue(InCanvas, BorderLinePosBegin, ValueFrameExtent);
		}
	}
}

FString CSKitDebug_DebugMenuNodeBase::GetDrawValueString() const
{
	if (mNodeData.mKind == ECSKitDebug_DebugMenuValueKind::List
		|| mNodeData.mKind == ECSKitDebug_DebugMenuValueKind::Enum)
	{
		return GetSelectString();
	}
	return mValueString;
}

bool CSKitDebug_DebugMenuNodeBase::GetBool() const
{
	ensure(mNodeData.mKind == ECSKitDebug_DebugMenuValueKind::Bool);
	return mValueString.ToBool();
}

int32 CSKitDebug_DebugMenuNodeBase::GetInt() const
{
	ensure(mNodeData.mKind == ECSKitDebug_DebugMenuValueKind::Int);
	return FCString::Atoi(*mValueString);
}

float CSKitDebug_DebugMenuNodeBase::GetFloat() const
{
	ensure(mNodeData.mKind == ECSKitDebug_DebugMenuValueKind::Float);
	return FCString::Atof(*mValueString);
}

int32 CSKitDebug_DebugMenuNodeBase::GetSelectIndex() const
{
	ensure(mNodeData.mKind == ECSKitDebug_DebugMenuValueKind::List
			|| mNodeData.mKind == ECSKitDebug_DebugMenuValueKind::Enum);
	return FCString::Atoi(*mValueString);
}

FString CSKitDebug_DebugMenuNodeBase::GetSelectString() const
{
	ensure(mNodeData.mKind == ECSKitDebug_DebugMenuValueKind::List
		|| mNodeData.mKind == ECSKitDebug_DebugMenuValueKind::Enum);
	const int32 SelectIndex = GetSelectIndex();
	if (SelectIndex < mNodeData.mList.Num())
	{
		return mNodeData.mList[SelectIndex];
	}
	return FString();
}

void CSKitDebug_DebugMenuNodeBase::SetNodeAction(const FCSKitDebug_DebugMenuNodeActionDelegate& InDelegate)
{
	if (mActionDelegate.IsBound())
	{
		UE_LOG(CSKitDebugLog, Error, TEXT("Delegate is already set"));
		return;
	}
	mActionDelegate = InDelegate;
}

void CSKitDebug_DebugMenuNodeBase::Load(const FString& InValueString, const FCSKitDebug_DebugMenuNodeActionParameter& InParameter)
{
	SetValueString(InValueString);
	mActionDelegate.ExecuteIfBound(InParameter);
}

void CSKitDebug_DebugMenuNodeBase::SetInitValue()
{
	if (!mNodeData.mInitValue.IsEmpty())
	{
		SetValueString(mNodeData.mInitValue);
	}
}

void CSKitDebug_DebugMenuNodeBase::SetValueBool(const bool InValue)
{
	ensure(mNodeData.mKind == ECSKitDebug_DebugMenuValueKind::Bool);
	mValueString = InValue ? TEXT("true") : TEXT("false");
}

void CSKitDebug_DebugMenuNodeBase::SetValueInt(const int32 InValue)
{
	ensure(mNodeData.mKind == ECSKitDebug_DebugMenuValueKind::Int);
	mValueString = FString::FromInt(InValue);
}

void CSKitDebug_DebugMenuNodeBase::SetValueFloat(const float InValue)
{
	ensure(mNodeData.mKind == ECSKitDebug_DebugMenuValueKind::Float);
	mValueString = FString::SanitizeFloat(InValue);
}

void CSKitDebug_DebugMenuNodeBase::SetValueList(const int32 InSelectIndex)
{
	ensure(mNodeData.mKind == ECSKitDebug_DebugMenuValueKind::List
			|| mNodeData.mKind == ECSKitDebug_DebugMenuValueKind::Enum);
	if (InSelectIndex < mNodeData.mList.Num())
	{
		mValueString = FString::FromInt(InSelectIndex);
	}
}

void CSKitDebug_DebugMenuNodeBase::DrawValue(UCanvas* InCanvas, const FVector2D& InPos, const FLinearColor InColor) const
{
	FCanvasTextItem Item(InPos, FText::FromString(GetDrawValueString()), GEngine->GetSmallFont(), InColor);
	Item.Scale = FVector2D(1.f);
	InCanvas->DrawItem(Item);
}

void CSKitDebug_DebugMenuNodeBase::DrawEditValue(UCanvas* InCanvas, const FVector2D& InValuePos, const FVector2D& InValueExtent) const
{
	FCanvasBoxItem Item(InValuePos, InValueExtent);
	Item.SetColor(GetSelectColor());
	Item.LineThickness = 3.f;
	InCanvas->DrawItem(Item);
}

UCSKitDebug_DebugMenuManager* CSKitDebug_DebugMenuNodeBase::GetManager() const
{
	return mManager.Get();
}