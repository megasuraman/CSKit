// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_CollisionInspectorError.h
 * @brief コリジョンチェック用のエラー情報
 * @author megasuraman
 * @date 2025/12/12
 */

#pragma once

#include "CoreMinimal.h"
#include "Serialization/JsonSerializerMacros.h"

struct FCSKitDebug_ScreenWindowText;
struct FCollisionInspectorErrorNode;

//エラー種類
enum class ECollisionInspectorErrorType : uint8
{
	Invalid,
	PassThrough,//コリジョン抜け
	Stuck,//スタック
	TooTight,//狭すぎる隙間
	Penetrating,//めり込んでる
	NoGround,//地面なし
};
//エラー検知時のコリジョン情報
struct FCollisionInspectorError_CollisionHitData : public FJsonSerializable
{
	BEGIN_JSON_SERIALIZER
	JSON_SERIALIZE("mObjectName", mObjectName);
	JSON_SERIALIZE("mImpactPointString", mImpactPointString);
	JSON_SERIALIZE("mImpactNormalString", mImpactNormalString);
	END_JSON_SERIALIZER

	FString mObjectName;
	FString mImpactPointString;
	FString mImpactNormalString;
	FCollisionInspectorError_CollisionHitData(){}
	FCollisionInspectorError_CollisionHitData(const FHitResult& InHitResult)
	{
		Set(InHitResult);
	}
	void Set(const FHitResult& InHitResult);
	FVector GetImpactPoint() const
	{
		FVector Pos;Pos.InitFromString(mImpactPointString);
		return Pos;
	}
	FVector GetImpactNormal() const
	{
		FVector NormalV;NormalV.InitFromString(mImpactNormalString);
		return NormalV;
	}
};
//コリジョン抜け情報
struct FCollisionInspectorErrorData_PassThrough : public FJsonSerializable
{
	static ECollisionInspectorErrorType sGetType(){return ECollisionInspectorErrorType::PassThrough;}
	
	BEGIN_JSON_SERIALIZER
	JSON_SERIALIZE("mLastSafePosString", mLastSafePosString);
	JSON_SERIALIZE("mHitCollisionInfo", mHitCollisionInfo);
	END_JSON_SERIALIZER

	FString mLastSafePosString;
	FString mHitCollisionInfo;
	
	FVector GetLastSafePos() const
	{
		FVector Pos;Pos.InitFromString(mLastSafePosString);
		return Pos;
	}
	void Draw(const UWorld* InWorld, UCanvas* InCanvas, FCSKitDebug_ScreenWindowText& InWindowText, const FCollisionInspectorErrorNode& InErrorNode) const;
};
//コリジョンハマり情報
struct FCollisionInspectorErrorData_Stuck : public FJsonSerializable
{
	static ECollisionInspectorErrorType sGetType(){return ECollisionInspectorErrorType::Stuck;}
	
	BEGIN_JSON_SERIALIZER
	JSON_SERIALIZE("mLastSafePosString", mLastSafePosString);
	JSON_SERIALIZE("mHitCollisionInfo", mHitCollisionInfo);
	JSON_SERIALIZE("mHitImpactPosString", mHitImpactPosString);
	JSON_SERIALIZE("mCheckPosBeginString", mCheckPosBeginString);
	JSON_SERIALIZE("mCheckPosEndString", mCheckPosEndString);
	JSON_SERIALIZE("mCheckRadius", mCheckRadius);
	END_JSON_SERIALIZER

	FString mLastSafePosString;
	FString mHitCollisionInfo;
	FString mHitImpactPosString;
	FString mCheckPosBeginString;
	FString mCheckPosEndString;
	float mCheckRadius = 0.f;
	
	FVector GetLastSafePos() const
	{
		FVector Pos;Pos.InitFromString(mLastSafePosString);
		return Pos;
	}
	void Draw(const UWorld* InWorld, UCanvas* InCanvas, FCSKitDebug_ScreenWindowText& InWindowText, const FCollisionInspectorErrorNode& InErrorNode) const;
};
//狭すぎコリジョン情報
struct FCollisionInspectorErrorData_TooTight : public FJsonSerializable
{
	static ECollisionInspectorErrorType sGetType(){return ECollisionInspectorErrorType::TooTight;}
	
	BEGIN_JSON_SERIALIZER
	JSON_SERIALIZE_ARRAY_SERIALIZABLE("mCollisionHitDataList", mCollisionHitDataList, FCollisionInspectorError_CollisionHitData);
	JSON_SERIALIZE("mLength", mLength);
	END_JSON_SERIALIZER

	TArray<FCollisionInspectorError_CollisionHitData> mCollisionHitDataList;
	float mLength = 0.f;
	
	void Draw(const UWorld* InWorld, UCanvas* InCanvas, FCSKitDebug_ScreenWindowText& InWindowText, const FCollisionInspectorErrorNode& InErrorNode) const;
};
//めり込み情報
struct FCollisionInspectorErrorData_Penetrating : public FJsonSerializable
{
	static ECollisionInspectorErrorType sGetType(){return ECollisionInspectorErrorType::Penetrating;}
	
	BEGIN_JSON_SERIALIZER
	JSON_SERIALIZE("mHitCollisionInfo", mHitCollisionInfo);
	JSON_SERIALIZE("mHitImpactPosString", mHitImpactPosString);
	JSON_SERIALIZE("mCheckPosBeginString", mCheckPosBeginString);
	JSON_SERIALIZE("mCheckPosEndString", mCheckPosEndString);
	JSON_SERIALIZE("mCheckRadius", mCheckRadius);
	END_JSON_SERIALIZER

	FString mHitCollisionInfo;
	FString mHitImpactPosString;
	FString mCheckPosBeginString;
	FString mCheckPosEndString;
	float mCheckRadius = 0.f;
	
	void Draw(const UWorld* InWorld, UCanvas* InCanvas, FCSKitDebug_ScreenWindowText& InWindowText, const FCollisionInspectorErrorNode& InErrorNode) const;
};
//地面なし情報
struct FCollisionInspectorErrorData_NoGround : public FJsonSerializable
{
	static ECollisionInspectorErrorType sGetType(){return ECollisionInspectorErrorType::NoGround;}
	
	BEGIN_JSON_SERIALIZER
	JSON_SERIALIZE("mPosString", mPosString);
	END_JSON_SERIALIZER

	FString mPosString;
	FVector GetPos() const
	{
		FVector Pos;Pos.InitFromString(mPosString);
		return Pos;
	}
	void Draw(const UWorld* InWorld, UCanvas* InCanvas, FCSKitDebug_ScreenWindowText& InWindowText, const FCollisionInspectorErrorNode& InErrorNode) const;
};
//個々のエラー情報
struct CSKITDEBUG_API FCollisionInspectorErrorNode : public FJsonSerializable
{
	BEGIN_JSON_SERIALIZER
	JSON_SERIALIZE("mPosString", mPosString);
	JSON_SERIALIZE("mErrorTypeInfoJsonString", mErrorTypeInfoJsonString);
	JSON_SERIALIZE("mErrorType", mErrorType);
	END_JSON_SERIALIZER

	FString mPosString;
	FString mErrorTypeInfoJsonString;//Error種類毎のjson情報
	int32 mErrorType = 0;//ECollisionInspectorErrorType
	
	FVector GetPos() const
	{
		FVector Pos;Pos.InitFromString(mPosString);
		return Pos;
	}
	ECollisionInspectorErrorType GetErrorType() const{return static_cast<ECollisionInspectorErrorType>(mErrorType);}
	FString GetErrorTypeString() const{return sGetErrorTypeString(GetErrorType());}
	static FString sGetErrorTypeString(const ECollisionInspectorErrorType InErrorType);
	bool IsSame(const FCollisionInspectorErrorNode& InNode) const;
	
	void Draw(UWorld* InWorld, UCanvas* InCanvas) const;
	template<typename T>
	void DrawErrorTypeInfo(const UWorld* InWorld, UCanvas* InCanvas, FCSKitDebug_ScreenWindowText& InWindowText) const
	{
		if (T::sGetType() == GetErrorType())
		{
			T ErrorTypeInfo;
			if (ErrorTypeInfo.FromJson(mErrorTypeInfoJsonString))
			{
				ErrorTypeInfo.Draw(InWorld, InCanvas, InWindowText, *this);
			}
		}
	}
};
//エラー情報
struct CSKITDEBUG_API FCollisionInspectorError : public FJsonSerializable
{
	BEGIN_JSON_SERIALIZER
	JSON_SERIALIZE_ARRAY_SERIALIZABLE("mNodeList", mNodeList, FCollisionInspectorErrorNode);
	END_JSON_SERIALIZER

	TArray<FCollisionInspectorErrorNode> mNodeList;
	
	template<typename T>
	void AddErrorNode(const FVector& InErrorPos, const T& InErrorTypeData)
	{
		FCollisionInspectorErrorNode ErrorNode;
		ErrorNode.mPosString = InErrorPos.ToString();
		ErrorNode.mErrorTypeInfoJsonString = InErrorTypeData.ToJson();
		ErrorNode.mErrorType = static_cast<int32>(InErrorTypeData.sGetType());
		if (IsOwnSameError(ErrorNode))
		{
			return;
		}
		mNodeList.Add(ErrorNode);
	}
	bool IsOwnSameError(const FCollisionInspectorErrorNode& InNode) const;
	void Merge(const FCollisionInspectorError& InError);
	void Draw(UWorld* InWorld, UCanvas* InCanvas) const;
};