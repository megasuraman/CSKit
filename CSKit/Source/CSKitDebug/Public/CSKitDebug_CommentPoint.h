// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_CommentPoint.h
 * @brief 指定位置にコメントを書くActor
 * @author megasuraman
 * @date 2025/03/29
 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CSKitDebug_CommentPoint.generated.h"

UENUM(BlueprintType)
enum class ECSKitDebug_CommentPriority : uint8
{
	Invalid,
	Low,
	Middle,
	High,
};

USTRUCT(BlueprintType)
struct FCSKitDebug_CommentData
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, Category = CommentData, Meta = (DisplayName = "メッセージ", DisplayPriority = 1))
	FString mMessage;
	UPROPERTY(VisibleAnywhere, Category = CommentData, Meta = (DisplayName = "記入者", DisplayPriority = 1))
	FString mOwner;
	UPROPERTY(EditAnywhere, Category = CommentData, Meta = (DisplayName = "既読", DisplayPriority = 4))
	bool mbAlreadyRead = false;
};

UCLASS()
class CSKITDEBUG_API ACSKitDebug_CommentPoint : public AActor
{
	GENERATED_BODY()
	
public:
	ACSKitDebug_CommentPoint(const FObjectInitializer& ObjectInitializer);

	void DebugDraw(UCanvas* InCanvas) const;
	ECSKitDebug_CommentPriority GetPriority() const{return mPriority;}
	FString GetTitle() const{return mTitle;}
	FString GetLastCommentOwner() const;
	bool IsAlreadyReadAllComment() const;
	bool IsDrawAlways() const{return mbDrawAlways;}

protected:
	virtual void PostInitProperties() override;
	virtual void PostLoad() override;
	virtual void BeginDestroy() override;
	
	void DebugRequestDraw(const bool bInActive);
	void DebugDraw(UCanvas* InCanvas, class APlayerController* InPlayerController) const;

protected:
	UPROPERTY(EditAnywhere, Meta = (DisplayName = "タイトル", DisplayPriority = 1))
	FString mTitle;
	UPROPERTY(VisibleAnywhere, Meta = (DisplayName = "作成者", DisplayPriority = 1))
	FString mOwner;
	UPROPERTY(EditAnywhere, Meta = (DisplayName = "コメント", DisplayPriority = 2))
	TArray<FCSKitDebug_CommentData> mCommentDataList;
	UPROPERTY(EditAnywhere, Meta = (DisplayName = "重要度", DisplayPriority = 1))
	ECSKitDebug_CommentPriority mPriority = ECSKitDebug_CommentPriority::Middle;
	//UCSKitEditor_EUW_CommentPointなしで単体で描画可能
	UPROPERTY(EditAnywhere, Meta = (DisplayName = "常に表示", DisplayPriority = 3))
	bool mbDrawAlways = false;
	
private:
	FDelegateHandle mDebugDrawHandle;

#if WITH_EDITOR
public:
	virtual bool CanEditChange(const FProperty* InProperty) const override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditImport() override;
protected:
	void EditorOnAttached();
private:
	bool mbEditorCalledEditImport = false;
#endif
	
#if WITH_EDITORONLY_DATA
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Display, meta = (AllowPrivateAccess = "true"))
	class UBillboardComponent* SpriteComponent;
#endif
};
