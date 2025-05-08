// Copyright 2020 megasuraman.
/**
 * @file CSKit_WorrySourceManager.h
 * @brief 全AIのWorrySource情報の取りまとめ
 * @author megasuraman
 * @date 2025/05/05
 */
#pragma once

#include "CoreMinimal.h"
#include "CSKit_WorryDataTable.h"
#include "CSKit_WorrySourceManager.generated.h"

union FCSKit_WorryGridIndex
{
	uint32 mUID = 0;
	struct IndexXY
	{
		int16 mX;
		int16 mY;
	};
	IndexXY	mIndex;

	FCSKit_WorryGridIndex()
	{}
	FCSKit_WorryGridIndex(const int32 InUID)
	{
		mUID = InUID;
	}
	FCSKit_WorryGridIndex(const int32 InX, const int32 InY)
	{
		check(InX <= INT16_MAX && InY <= INT16_MAX
			&& InX >=INT16_MIN && InY >= INT16_MIN);
		mIndex.mX = InX;
		mIndex.mY = InY;
	}
	bool IsValid() const{return mUID!=0;}
};

struct FCSKit_WorrySource
{
	FCSKit_WorrySourceTableRow mData;
	FVector mPos = FVector::ZeroVector;
	TWeakObjectPtr<const AActor> mSourceActor;
	float mRadius = 0.f;
	float mLifeSec = 0.f;
	FName mName;
	bool mbActive = false;
};

struct FCSKit_WorrySourceGridElement
{
	TArray<FCSKit_WorrySource> mWorrySourceList;
};

UCLASS()
class CSKIT_API UCSKit_WorrySourceManager : public UObject
{
	GENERATED_BODY()
	
public:
	static UCSKit_WorrySourceManager* sGet(const UWorld* InWorld);
	static void sEntrySource(const UWorld* InWorld, const FName& InName, const FVector& InPos, const AActor* InSourceActor);
	
	UCSKit_WorrySourceManager();

	void Update(const float InDeltaSec);
	void EntryWorrySource(const FName& InName, const FVector& InPos, const AActor* InSourceActor);
	FCSKit_WorryGridIndex CalcGridIndex(const FVector& InPos) const;
	void CollectTouchSource(TArray<FCSKit_WorrySource>& OutList, const FVector& InPos, const float InRadius, const float InHeight) const;

protected:
	virtual const FCSKit_WorrySourceTableRow* FindWorrySourceTableRow(const FName& InName) const;

private:
	TMap<uint32, FCSKit_WorrySourceGridElement> mGridMap;
	float mGridLength = 2000.f;

#if USE_CSKIT_DEBUG
public:
	void DebugDraw(UCanvas* InCanvas) const;

	void DebugSetDrawInfo(const bool bInDraw){mbDebugDrawInfo=bInDraw;}
protected:
	void DebugDrawInfo(UCanvas* InCanvas) const;
	void DebugDrawSource(UCanvas* InCanvas) const;
private:
	bool mbDebugDrawInfo = false;
#endif
};
