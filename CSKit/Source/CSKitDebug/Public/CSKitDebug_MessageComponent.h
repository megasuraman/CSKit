// Copyright 2020 megasuraman.
/**
 * @file CSKitDebug_MessageComponent.h
 * @brief デバッグ用にDGS情報の収集ややりとりをするComponent
 *			GameStateに持たせることで、同期対象でない情報も得られる
 * @author megasuraman
 * @date 2025/03/30
 */
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CSKitDebug_MessageComponent.generated.h"

DECLARE_DELEGATE_RetVal(FString, FCSKitDebug_OutputDetailLogDelegate)
DECLARE_DELEGATE_RetVal(FString, FCSKitDebug_BroadcastDGSInfoDelegate)

struct FCSKitDebug_BroadcastDGSInfoNode
{
	TWeakObjectPtr<UObject> mRequester;
	FString mMessage;
	float mIntervalTime = 1.f;
	float mSleepSec = 0.f;
};

UCLASS()
class CSKITDEBUG_API UCSKitDebug_MessageComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCSKitDebug_MessageComponent();

	void Update(const float InDeltaSec);
	void ReceiveMessageOnServer(const FName& InTag, const FString& InMessage, AActor* InTarget);
	void ResetFinishOutputDetailLog(){mbFinishOutputDetailLog = false;}
	void SetFinishOutputDetailLog(){mbFinishOutputDetailLog = true;}
	bool IsFinishOutputDetailLog() const {return mbFinishOutputDetailLog;};
	void EntryBroadcastDGSInfo(const FName& InTag, UObject* InRequester, const float InIntervalTime = 1.f);
	void ExitBroadcastDGSInfo(const FName& InTag);
	FString FindBroadcastDGSInfo(const FName& InTag) const;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION(NetMulticast, reliable)
	void Multicast_OutputDetailLog(const FString& InLog, AActor* InTarget);
	UFUNCTION(NetMulticast, reliable)
	void Multicast_BroadcastDGSInfo(const FName InTag, const FString& InMessage);
	
	void RequestOutputDetailLog(AActor* InTarget);
	static FString GetDetailLog(AActor* InTarget);
	void UpdateBroadcastDGSInfo(const float InDeltaSec);

private:
	TMap<FName, FCSKitDebug_BroadcastDGSInfoNode> mBroadcastDGSInfoMap;
	bool mbFinishOutputDetailLog = false;
};
