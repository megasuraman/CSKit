// Copyright 2020 megasuraman.
/**
 * @file CSKit_TerritoryComponent.h
 * @brief 活動範囲管理Component
 * @author megasuraman
 * @date 2025/05/05
 */
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CSKit_TerritoryComponent.generated.h"

class ACSKit_TerritoryVolume;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CSKIT_API UCSKit_TerritoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCSKit_TerritoryComponent();
	virtual void BeginPlay() override;

	void Update(const float InDeltaTime);
	bool IsOwnTerritoryVolume() const;
	bool IsInside() const{return mbInside;}
	bool IsTouchTerritoryVolume(const FVector& InPos, const float InRadius) const;
	void SetUserName(const FName& InName){mUserName = InName;}

protected:
	void UpdateTerritoryVolume();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Territory")
	FName mUserName;

private:
	TWeakObjectPtr<const ACSKit_TerritoryVolume> mTerritoryVolume;
	float mCheckRadius = 100.f;
	bool mbInside = false;

#if USE_CSKIT_DEBUG
public:
	virtual FString DebugDrawSelectedActorWatcher(UCanvas* InCanvas) const;
#endif
};
