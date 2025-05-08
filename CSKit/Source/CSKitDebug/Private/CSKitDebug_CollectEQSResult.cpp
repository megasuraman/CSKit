// Copyright 2020 megasuraman.
/**
 * @file CSKitDebug_CollectEQSResult.cpp
 * @brief EQSの実行ログ収集用構造体
 * @author megasuraman
 * @date 2025/05/06
 */
#include "CSKitDebug_CollectEQSResult.h"

void FCSKitDebug_CollectEQSResultNode::SetPos(const FVector& InPos)
{
	mPosString = InPos.ToString();
}
FVector FCSKitDebug_CollectEQSResultNode::GetPos() const
{
	FVector Pos;
	Pos.InitFromString(mPosString);
	return Pos;
}

void	FCSKitDebug_CollectEQSResult::SetCenterPos(const FVector& InPos)
{
	mCenterPosString = InPos.ToString();
}
FVector FCSKitDebug_CollectEQSResult::GetCenterPos() const
{
	FVector CenterPos;
	CenterPos.InitFromString(mCenterPosString);
	return CenterPos;
}
