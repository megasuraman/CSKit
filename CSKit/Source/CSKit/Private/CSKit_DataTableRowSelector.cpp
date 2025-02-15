// Copyright 2022 megasuraman

#include "CSKit_DataTableRowSelector.h"
#include "Engine/DataTable.h"

#if WITH_EDITOR
/**
 * @brief	有効なRowNameがセットされてるかどうか
 */
bool FCSKit_DataTableRowSelector::EditorIsValidRowName() const
{
	if (!mDataTablePath.IsEmpty())
	{
		return EditorIsValidRowName_Path();
	}
	else if (mDataTableStruct != nullptr)
	{
		return EditorIsValidRowName_Struct();
	}
	return false;
}
bool FCSKit_DataTableRowSelector::EditorIsValidRowName_Path() const
{
	if (const UDataTable* DataTable = LoadObject<UDataTable>(nullptr, (*mDataTablePath), nullptr, LOAD_None, nullptr))
	{
		if (DataTable->GetRowMap().Find(mRowName) != nullptr)
		{
			return true;
		}
	}
	return false;
}
bool FCSKit_DataTableRowSelector::EditorIsValidRowName_Struct() const
{
	for (TObjectIterator<UDataTable> It; It; ++It)
	{
		const UDataTable* DataTable = *It;
		if (DataTable == nullptr)
		{
			continue;
		}
		if (DataTable->RowStruct->IsChildOf(mDataTableStruct))
		{
			if (DataTable->GetRowMap().Find(mRowName) != nullptr)
			{
				return true;
			}
		}
	}
	return false;
}
#endif
