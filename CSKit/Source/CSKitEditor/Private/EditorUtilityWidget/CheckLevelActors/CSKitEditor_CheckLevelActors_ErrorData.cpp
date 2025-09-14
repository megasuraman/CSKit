// Copyright 2020 megasuraman
/**
* @file CSKitEditor_CheckLevelActors_ErrorData.cpp
 * @brief 配置Actorチェックのエラー情報
 * @author megasuraman
 * @date 2025/09/13
 */


#include "EditorUtilityWidget/CheckLevelActors/CSKitEditor_CheckLevelActors_ErrorData.h"
#include "CSKitDebug_Utility.h"

FCSKitEditor_CheckLevelActors_ErrorDataNode::FCSKitEditor_CheckLevelActors_ErrorDataNode(const AActor& InActor)
{
	mActor = &InActor;
	mLevelName = UCSKitDebug_Utility::GetActorLevelName(&InActor);
	mLabelName = InActor.GetActorLabel();
	mClassName = InActor.GetClass()->GetName();
}