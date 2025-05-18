// Copyright 2020 megasuraman.
/**
 * @file CSKit_BBInitDataTable.cpp
 * @brief Blackboardへの初期値設定用DataTable
 * @author megasuraman
 * @date 2025/05/18
 */
#include "AI/Blackboard/CSKit_BBInitDataTable.h"

#include "CSKitDebug_Subsystem.h"
#include "BehaviorTree/BlackboardComponent.h"

/**
 * @brief 指定初期値をBlackboardに設定
 */
void FCSKit_BBInitValueTableRow::ApplyBlackboardValue(UBlackboardComponent& InBlackboard) const
{
	for (const FCSKit_BBInitDataNodeInt& Data : mIntValueList)
	{
#if USE_CSKIT_DEBUG
		DebugCheckInvalidKeyName(InBlackboard, Data.mKeyName);
#endif
		InBlackboard.SetValueAsInt(Data.mKeyName, Data.mValue);
	}
	for (const FCSKit_BBInitDataNodeFloat& Data : mFloatValueList)
	{
#if USE_CSKIT_DEBUG
		DebugCheckInvalidKeyName(InBlackboard, Data.mKeyName);
#endif
		InBlackboard.SetValueAsFloat(Data.mKeyName, Data.mValue);
	}
	for (const FCSKit_BBInitDataNodeName& Data : mNameValueList)
	{
#if USE_CSKIT_DEBUG
		DebugCheckInvalidKeyName(InBlackboard, Data.mKeyName);
#endif
		InBlackboard.SetValueAsName(Data.mKeyName, Data.mValue);
	}
	for (const FCSKit_BBInitDataNodeBool& Data : mBoolValueList)
	{
#if USE_CSKIT_DEBUG
		DebugCheckInvalidKeyName(InBlackboard, Data.mKeyName);
#endif
		InBlackboard.SetValueAsBool(Data.mKeyName, Data.mbValue);
	}
}

#if USE_CSKIT_DEBUG
/**
 * @brief 有効なKeyNameかチェック
 */
void FCSKit_BBInitValueTableRow::DebugCheckInvalidKeyName(const UBlackboardComponent& InBlackboard,	const FName& InKeyName)
{
	if (InBlackboard.GetKeyID(InKeyName) == FBlackboard::InvalidKey)
	{
		UCSKitDebug_Subsystem::sOneShotWarning(
			InBlackboard.GetWorld(),
			false,
			FName("BBInitValue"),
			FString::Printf(TEXT("Invalid Key Name: %s"), *InKeyName.ToString())
		);
	}
}
#endif