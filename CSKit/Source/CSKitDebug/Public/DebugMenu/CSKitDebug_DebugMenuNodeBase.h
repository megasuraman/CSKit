// Copyright 2022 megasuraman

#pragma once

#include "CoreMinimal.h"
#include "DebugMenu/CSKitDebug_DebugMenuTableRow.h"

class UCSKitDebug_DebugMenuManager;

struct FCSKitDebug_DebugMenuNodeActionParameter
{
	TWeakObjectPtr<APlayerController> mPlayerController;
};
DECLARE_DELEGATE_OneParam(FCSKitDebug_DebugMenuNodeActionDelegate, const FCSKitDebug_DebugMenuNodeActionParameter&);


class CSKITDEBUG_API CSKitDebug_DebugMenuNodeBase
{
public:
	CSKitDebug_DebugMenuNodeBase();
	virtual ~CSKitDebug_DebugMenuNodeBase();

	void Init(const FString& InPath, const FCSKitDebug_DebugMenuNodeData& InData, UCSKitDebug_DebugMenuManager* InManager);
	virtual void OnBeginAction();
	virtual void OnEndAction(const FCSKitDebug_DebugMenuNodeActionParameter& InParameter);
	virtual void OnJustPressedUpKey() {}
	virtual void OnJustPressedDownKey() {}
	virtual void OnJustPressedLeftKey() {}
	virtual void OnJustPressedRightKey() {}
	void Draw(UCanvas* InCanvas, const FVector2D& InPos, const bool bInSelect) const;
	const FString& GetPath() const { return mPath; }
	FString GetValueString() const { return mValueString; }
	FString GetDrawValueString() const;
	bool GetBool() const;
	int32 GetInt() const;
	float GetFloat() const;
	int32 GetSelectIndex() const;
	FString GetSelectString() const;
	void SetNodeAction(const FCSKitDebug_DebugMenuNodeActionDelegate& InDelegate);
	const FCSKitDebug_DebugMenuNodeData& GetNodeData() const{return mNodeData;}
	void Load(const FString& InValueString, const FCSKitDebug_DebugMenuNodeActionParameter& InParameter);

protected:
	void SetValueString(const FString& InString){mValueString = InString;}
	virtual void SetInitValue();
	void SetValueBool(const bool InValue);
	void SetValueInt(const int32 InValue);
	void SetValueFloat(const float InValue);
	void SetValueList(const int32 InSelectIndex);
	virtual void DrawValue(UCanvas* InCanvas, const FVector2D& InPos, const FLinearColor InColor) const;
	virtual void DrawEditValue(UCanvas* InCanvas, const FVector2D& InValuePos, const FVector2D& InValueExtent) const;
	bool IsEditMode() const { return mbEditMode; }
	FLinearColor GetWindowBackColor() const { return FLinearColor(0.01f, 0.01f, 0.01f, 0.5f); }
	FLinearColor GetWindowFrameColor() const { return FLinearColor(0.1f, 0.9f, 0.1f, 1.f); }
	FLinearColor GetFontColor() const { return FLinearColor(0.1f, 0.9f, 0.1f, 1.f); }
	FLinearColor GetSelectColor() const{return FLinearColor(0.1f, 0.9f, 0.9f, 1.f);}
	UCSKitDebug_DebugMenuManager* GetManager() const;

private:
	FCSKitDebug_DebugMenuNodeData mNodeData;
	FCSKitDebug_DebugMenuNodeActionDelegate mActionDelegate;
	FString mValueString;
	FString mPath;
	TWeakObjectPtr<UCSKitDebug_DebugMenuManager> mManager;
	bool mbEditMode = false;
};
