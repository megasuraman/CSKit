// Copyright 2020 megasuraman.
/**
 * @file CSKit_TerritoryVolume.h
 * @brief 活動範囲
 * @author megasuraman
 * @date 2025/05/05
 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Volume.h"
#include "CSKit_TerritoryVolume.generated.h"

UCLASS(Abstract)
class CSKIT_API ACSKit_TerritoryVolume : public AVolume
{
	GENERATED_BODY()
	
public:
	ACSKit_TerritoryVolume();
#if ENGINE_MAJOR_VERSION == 5
	virtual void PreSave(FObjectPreSaveContext ObjectSaveContext) override;
#endif
	void SetUserName(const FName& InName){mUserName = InName;}
	void SetManualExtentV(const FVector& InExtentV){mManualExtentV = InExtentV;}
	FName GetUserName() const{return mUserName;}
	bool IsTouch(const FVector& InPos, const float InRadius) const;
	bool IsManualBox() const{return !mManualExtentV.IsZero();}
	const FVector& GetManualExtentV() const{return mManualExtentV;}
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	UPROPERTY(EditInstanceOnly, Category = "Territory", meta=(DisplayName = "利用者", GetOptions="EditorGetKindNameList"))
	FName mUserName;

private:
	FVector mManualExtentV = FVector::ZeroVector;//自前のBox処理用
	
#if USE_CSKIT_DEBUG
public:
	void DebugDraw(UCanvas* InCanvas, const bool bInWireFrame) const;
	void DebugDrawInfo(UCanvas* InCanvas) const;
	virtual FColor DebugGetColor() const;
#endif

protected:
	//mKindNameのプロパティのmeta情報用
	UFUNCTION()
	virtual TArray<FName> EditorGetKindNameList();
#if WITH_EDITOR
protected:
	void EditorCheckError() const;
#endif
};
