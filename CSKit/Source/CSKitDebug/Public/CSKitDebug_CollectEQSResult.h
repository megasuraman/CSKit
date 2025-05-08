// Copyright 2020 megasuraman.
/**
 * @file CSKitDebug_CollectEQSResult.h
 * @brief EQSの実行ログ収集用構造体
 * @author megasuraman
 * @date 2025/05/06
 */
#pragma once

#include "CoreMinimal.h"
#include "Serialization/JsonSerializerMacros.h"

class AAIController;

struct FCSKitDebug_CollectEQSResultNode : public FJsonSerializable
{
	BEGIN_JSON_SERIALIZER
		JSON_SERIALIZE_ARRAY("mTestList", mTestList);
		JSON_SERIALIZE("mPosString", mPosString);
		JSON_SERIALIZE("mFailedLabel", mFailedLabel);
		JSON_SERIALIZE("mScore", mScore);
		JSON_SERIALIZE("mbWinner", mbWinner);
	END_JSON_SERIALIZER

	TArray<FString>	mTestList;
	FString mPosString;
	FString	mFailedLabel;
	float	mScore = 0.f;
	bool	mbWinner = false;

	CSKITDEBUG_API void SetPos(const FVector& InPos);
	CSKITDEBUG_API FVector GetPos() const;
};
struct FCSKitDebug_CollectEQSResult : public FJsonSerializable
{
	BEGIN_JSON_SERIALIZER
		JSON_SERIALIZE_ARRAY_SERIALIZABLE("mNodeList", mNodeList, FCSKitDebug_CollectEQSResultNode);
		JSON_SERIALIZE("mCenterPosString", mCenterPosString);
		JSON_SERIALIZE("mResult", mResult);
	END_JSON_SERIALIZER

	TArray< FCSKitDebug_CollectEQSResultNode>	mNodeList;
	FString	mCenterPosString;
	FString	mResult;
	bool	mbCollected = false;

	CSKITDEBUG_API void	SetCenterPos(const FVector& InPos);
	CSKITDEBUG_API FVector GetCenterPos() const;
};
