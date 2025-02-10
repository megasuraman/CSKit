// Copyright 2020 megasuraman
/**
 * @file CSKitEditor_EUW_PropertyViewer.h
 * @brief UObject‚ÌProperty•\Ž¦
 * @author megasuraman
 * @date 2022/08/07
 */
#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget/CSKitEditor_EUW_Base.h"
#include "Engine/EngineTypes.h"
#include "CSKitEditor_EUW_PropertyViewer.generated.h"

class UBlueprint;
class UCanvas;
class APlayerController;

DECLARE_DELEGATE_RetVal(FString, FPropertyWatchInfo)

UCLASS()
class CSKITEDITOR_API UCSKitEditor_EUW_PropertyViewer : public UCSKitEditor_EUW_Base
{
	GENERATED_BODY()

public:
	UCSKitEditor_EUW_PropertyViewer(const FObjectInitializer& ObjectInitializer);
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
	UFUNCTION(BlueprintCallable, Category = "PropertyViewer")
	FString	GetPropertyInfo(UObject* InTarget, UClass* InTargetClass);
	UFUNCTION(BlueprintCallable, Category = "PropertyViewer")
	bool	UpdatePropertyInfo(UObject* InTarget, UClass* InTargetClass);
	
	UFUNCTION(BlueprintCallable, Category = "PropertyViewer")
	bool	SetupTargetObjectList();
	UFUNCTION(BlueprintCallable, Category = "PropertyViewer")
	FString	GetDispObjectName(UObject* InTarget) const;
	UFUNCTION(BlueprintCallable, Category = "PropertyViewer")
	void	SetUpdateRealTime(bool bInRealTime);
	UFUNCTION(BlueprintCallable, Category = "PropertyViewer")
	void	Clear();
	UFUNCTION(BlueprintCallable, Category = "PropertyViewer")
	const TMap<FName, FString>&	GetPropertyInfoMap() const{return mPropertyInfo;}
	
	UFUNCTION(BlueprintImplementableEvent, Category = "PropertyViewer")
	void	UpdateRealTime();
	UFUNCTION(BlueprintImplementableEvent, Category = "PropertyViewer")
	void	OnChangeTargetClass();
	
protected:
	virtual void NativeDestruct() override;
	virtual void FakeTick() override;

	bool	FindDebuggingData(UBlueprint* Blueprint, UObject* ActiveObject, FProperty* InProperty, void*& OutData);
	FString	GetDebugInfo_InContainer(int32 Index, FProperty* Property, const void* Data);
	FString	GetDebugInfoInternal(FProperty* Property, const void* PropertyValue);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PropertyViewer")
	UClass*	mTargetClass = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PropertyViewer")
	TArray<UClass*>	mBookmarkClassList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PropertyViewer")
	TArray<TSoftObjectPtr<UObject>> mTargetObjectList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PropertyViewer")
	TWeakObjectPtr<UObject> mTargetObject;

private:
	TMap<FName, FString>	mPropertyInfo;
};