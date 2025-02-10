// Copyright 2022 megasruaman
#pragma once

#include "CoreMinimal.h"
#include "Serialization/JsonSerializerMacros.h"

struct FCSKitDebug_DebugMenuSaveDataNode : public FJsonSerializable
{
	BEGIN_JSON_SERIALIZER
	JSON_SERIALIZE("mPath", mPath);
	JSON_SERIALIZE("mValueString", mValueString);
	END_JSON_SERIALIZER

	FString mPath;
	FString mValueString;
};

struct FCSKitDebug_DebugMenuSaveData : public FJsonSerializable
{
	BEGIN_JSON_SERIALIZER
	JSON_SERIALIZE_ARRAY_SERIALIZABLE("mSaveNodeList", mSaveNodeList, FCSKitDebug_DebugMenuSaveDataNode);
	END_JSON_SERIALIZER

public:
	void Clear();
	void Save();
	void Load();
	void WriteValue(const FString& InPath, const FString& InValue);
	FString GetValueString(const FString& InPath) const;
	const TMap<FString, FString>& GetValueMap() const{return mValueMap;}

protected:
	static FString GetSaveFilePath();

private:
	TArray<FCSKitDebug_DebugMenuSaveDataNode> mSaveNodeList;
	TMap<FString, FString> mValueMap;
	bool mbLoaded = false;
};