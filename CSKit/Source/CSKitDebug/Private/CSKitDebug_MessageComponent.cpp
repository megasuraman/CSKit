// Copyright 2020 megasuraman.
/**
 * @file CSKitDebug_MessageComponent.cpp
 * @brief デバッグ用にDGS情報の収集ややりとりをするComponent
 *			GameStateに持たせることで、同期対象でない情報も得られる
 * @author megasuraman
 * @date 2025/03/30
 */
#include "CSKitDebug_MessageComponent.h"

#include "CSKitDebug_Subsystem.h"

UCSKitDebug_MessageComponent::UCSKitDebug_MessageComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

/**
 * @brief 更新
 */
void UCSKitDebug_MessageComponent::Update(const float InDeltaSec)
{
	if(GetWorld()->IsNetMode(NM_DedicatedServer))
	{
		UpdateBroadcastDGSInfo(InDeltaSec);
	}
}

/**
 * @brief メッセージ受け取り(ServerCommand実行できる奴から呼ぶ)
 */
void UCSKitDebug_MessageComponent::ReceiveMessageOnServer(const FName& InTag, const FString& InMessage, AActor* InTarget)
{
	if(GetWorld()->IsNetMode(NM_Client))
	{
		return;
	}

	if(InTag == FName(TEXT("__OutputDetailLog__"))
		&& InTarget != nullptr)
	{
		ResetFinishOutputDetailLog();
		RequestOutputDetailLog(InTarget);
	}
}

/**
 * @brief DGS情報をClientに定期的に発信するイベント登録
 */
void UCSKitDebug_MessageComponent::EntryBroadcastDGSInfo(const FName& InTag, UObject* InRequester, const float InIntervalTime)
{
	if(!GetWorld()->IsNetMode(NM_DedicatedServer))
	{
		UCSKitDebug_Subsystem::sOneShotWarning(
			GetWorld(),
			false,
			TEXT("CSKitDebug_MessageComponent"),
			FString::Printf(TEXT("UCSKitDebug_MessageComponent::EntryBroadcastDGSInfo() RunWithDGS"))
		);
		return;
	}
	FCSKitDebug_BroadcastDGSInfoNode& Node = mBroadcastDGSInfoMap.FindOrAdd(InTag);
	Node.mIntervalTime = FMath::Max(InIntervalTime, 0.1f);
	Node.mRequester = InRequester;
}

/**
 * @brief DGS情報をClientに定期的に発信するイベント解除
 */
void UCSKitDebug_MessageComponent::ExitBroadcastDGSInfo(const FName& InTag)
{
	mBroadcastDGSInfoMap.Remove(InTag);
}

/**
 * @brief 
 */
FString UCSKitDebug_MessageComponent::FindBroadcastDGSInfo(const FName& InTag) const
{
	if(const FCSKitDebug_BroadcastDGSInfoNode* Node = mBroadcastDGSInfoMap.Find(InTag))
	{
		return Node->mMessage;
	}
	
	return FString();
}

/**
 * @brief 
 */
void UCSKitDebug_MessageComponent::BeginPlay()
{
	Super::BeginPlay();
}

/**
 * @brief 
 */
void UCSKitDebug_MessageComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

/**
 * @brief 詳細ログ出力
 */
void UCSKitDebug_MessageComponent::Multicast_OutputDetailLog_Implementation(const FString& InLog, AActor* InTarget)
{
	FString LogString(TEXT("/*** CSKitDebug OutputDetailLog ***/\n"));
	LogString += FString(TEXT("=== Server ===\n"));
	LogString += InLog;
	LogString += FString(TEXT("======\n"));
	if(GetWorld()->IsNetMode(NM_Client))
	{
		LogString += FString(TEXT("=== Client ===\n"));
		LogString += GetDetailLog(InTarget);
		LogString += FString(TEXT("======\n"));
	}
	LogString += FString(TEXT("/******/\n"));
	UE_LOG(LogTemp, Log, TEXT("%s"), *LogString);

	mbFinishOutputDetailLog = true;
}

/**
 * @brief DGS情報共有
 */
void UCSKitDebug_MessageComponent::Multicast_BroadcastDGSInfo_Implementation(const FName InTag, const FString& InMessage)
{
	if(GetWorld()->GetNetMode() != NM_Client)
	{
		return;
	}
	FCSKitDebug_BroadcastDGSInfoNode& Node = mBroadcastDGSInfoMap.FindOrAdd(InTag);
	Node.mMessage = InMessage;
}

/**
 * @brief 詳細ログ出力
 */
void UCSKitDebug_MessageComponent::RequestOutputDetailLog(AActor* InTarget)
{
	const FString LogString = GetDetailLog(InTarget);
	Multicast_OutputDetailLog(LogString, InTarget);
}

/**
 * @brief 詳細ログ取得
 */
FString UCSKitDebug_MessageComponent::GetDetailLog(AActor* InTarget)
{
	FString LogString;
	const FName OriginalFuncName(TEXT("CSKitDebug_GetDetailLog"));
	FCSKitDebug_OutputDetailLogDelegate OwnerActorDebugInfo;
	OwnerActorDebugInfo.BindUFunction(InTarget, OriginalFuncName);
	if (OwnerActorDebugInfo.IsBound())
	{
		LogString = OwnerActorDebugInfo.Execute();
	}
	return LogString;
}

/**
 * @brief 詳細ログ取得
 */
void UCSKitDebug_MessageComponent::UpdateBroadcastDGSInfo(const float InDeltaSec)
{
	for(auto& MapElement : mBroadcastDGSInfoMap)
	{
		FCSKitDebug_BroadcastDGSInfoNode& Node = MapElement.Value;
		Node.mSleepSec += InDeltaSec;
		if(Node.mSleepSec < Node.mIntervalTime)
		{
			continue;
		}

		UObject* Requester = Node.mRequester.Get();
		if(Requester == nullptr)
		{
			continue;
		}

		Node.mSleepSec = 0.f;
		const FName OriginalFuncName(TEXT("CSKitDebug_BroadcastDGSInfo"));
		FCSKitDebug_BroadcastDGSInfoDelegate OwnerActorDebugInfo;
		OwnerActorDebugInfo.BindUFunction(Requester, OriginalFuncName);
		if (OwnerActorDebugInfo.IsBound())
		{
			Node.mMessage = OwnerActorDebugInfo.Execute();
			Multicast_BroadcastDGSInfo(MapElement.Key, Node.mMessage);
		}
	}
}