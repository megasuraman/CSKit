// Copyright 2020 megasuraman.
/**
 * @file CSKit_BinarySerializable.cpp
 * @brief BinaryデータでのSave,Loadを提供する構造体
 * @author megasuraman
 * @date 2026/01/11
 */
#include "CSKit_BinarySerializable.h"

#include "Serialization/ArchiveLoadCompressedProxy.h"
#include "Serialization/ArchiveSaveCompressedProxy.h"

/**
 * @brief	
 */
void FCSKit_BinarySerializable::Save()
{
	const FString FilePath = GetFilePath();
	if (FilePath.IsEmpty())
	{
		ensureMsgf(false, TEXT("FBinarySerializable FilePath Empty"));
		return;
	}
	//圧縮
	TArray<uint8> UncompressedBuffer;
#if ENGINE_MAJOR_VERSION == 4
	FArchiveSaveCompressedProxy Compressor(UncompressedBuffer, NAME_Gzip);
#else
	FArchiveSaveCompressedProxy Compressor(UncompressedBuffer, NAME_Oodle);
#endif
	Compressor << *this;
	Compressor.Flush();
	
	if (FArchive* Writer = IFileManager::Get().CreateFileWriter(*FilePath))
	{
		*Writer << UncompressedBuffer;
		Writer->Close();
		delete Writer;
	}
}

/**
 * @brief	
 */
void FCSKit_BinarySerializable::Load()
{
	const FString FilePath = GetFilePath();
	if (FilePath.IsEmpty())
	{
		ensureMsgf(false, TEXT("FBinarySerializable FilePath Empty"));
		return;
	}
	TArray<uint8> CompressedBuffer;
	if (FArchive* FileReader = IFileManager::Get().CreateFileReader(*FilePath))
	{
		*FileReader << CompressedBuffer;
		FileReader->Close();
		delete FileReader;
	}

	//解凍
	if (CompressedBuffer.Num() > 0)
	{
#if ENGINE_MAJOR_VERSION == 4
		FArchiveLoadCompressedProxy Decompressor(CompressedBuffer, NAME_Gzip);
#else
		FArchiveLoadCompressedProxy Decompressor(CompressedBuffer, NAME_Oodle);
#endif
		Decompressor << *this;
		Decompressor.Flush();
	}
}