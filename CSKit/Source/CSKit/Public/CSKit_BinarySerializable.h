#pragma once
// Copyright 2020 megasuraman.
/**
 * @file CSKit_BinarySerializable.h
 * @brief BinaryデータでのSave,Loadを提供する構造体
 * @author megasuraman
 * @date 2026/01/11
 */
#include "CoreMinimal.h"

struct CSKIT_API FCSKit_BinarySerializable
{
	// FArchive用
	friend FArchive& operator<<(FArchive& Ar, FCSKit_BinarySerializable& Data)
	{
		//Ar << Data.mHoge;
		Data.Serializable(Ar);
		return Ar;
	}
	
	virtual ~FCSKit_BinarySerializable(){}
	virtual FString GetFilePath() const = 0;
	virtual void Serializable(FArchive& Ar) = 0;
	// {
	// 	Ar << mHoge;//Save,Loadしたいメンバ変数を流し込む
	//	自前の構造体を渡す場合はFArchive用に oprator<< を定義する
	// }
	void Save();
	void Load();
};
