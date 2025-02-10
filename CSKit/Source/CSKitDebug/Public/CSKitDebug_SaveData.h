// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_SaveData.h
 * @brief Debug用の簡易セーブ情報(UE5.4からGConfig->Set系で.iniの書き込みがうまく行かなくなったので代わり)
 * @author megasuraman
 * @date 2025/01/27
 */
#pragma once

#include "CoreMinimal.h"
#include "Serialization/JsonSerializerMacros.h"

enum class ECSKitDebug_SaveDataValueType : uint8
{
	Invalid,
	Bool,
	Int,
	Float,
	String,
};
struct FCSKitDebug_SaveDataValue : public FJsonSerializable
{
	BEGIN_JSON_SERIALIZER
	JSON_SERIALIZE("mTag", mTag);
	JSON_SERIALIZE("mValueString", mValueString);
	JSON_SERIALIZE("mValueType", mValueType);
	END_JSON_SERIALIZER

	FString mTag;
	FString mValueString;
	int32 mValueType;

	ECSKitDebug_SaveDataValueType GetValueType() const{return static_cast<ECSKitDebug_SaveDataValueType>(mValueType); }
};

struct FCSKitDebug_SaveData : public FJsonSerializable
{
	BEGIN_JSON_SERIALIZER
	JSON_SERIALIZE_ARRAY_SERIALIZABLE("mValueList", mValueList, FCSKitDebug_SaveDataValue);
	END_JSON_SERIALIZER
public:
	CSKITDEBUG_API void Save();
	CSKITDEBUG_API void Load();
	CSKITDEBUG_API void SetBool(const FString& InTag, const bool bInValue);
	CSKITDEBUG_API void SetInt(const FString& InTag, const int32 InValue);
	CSKITDEBUG_API void SetFloat(const FString& InTag, const float InValue);
	CSKITDEBUG_API void SetString(const FString& InTag, const FString& InValue);
	CSKITDEBUG_API bool GetBool(const FString& InTag) const;
	CSKITDEBUG_API int32 GetInt(const FString& InTag) const;
	CSKITDEBUG_API float GetFloat(const FString& InTag) const;
	CSKITDEBUG_API FString GetString(const FString& InTag) const;

private:
	TArray<FCSKitDebug_SaveDataValue> mValueList;
	TMap<FString, FCSKitDebug_SaveDataValue> mValueMap;
	bool mbLoaded = false;
};