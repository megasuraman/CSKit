// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_GhostPawn.h
 * @brief 任意のActorを間接的に操作可能にするControllerのためのダミーPawn
 * @author megasuraman
 * @date 2025/03/30
 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CSKitDebug_GhostPawn.generated.h"


UCLASS()
class CSKITDEBUG_API ACSKitDebug_GhostPawn : public APawn
{
	GENERATED_BODY()
public:
	ACSKitDebug_GhostPawn();
	void OnPostAttachedActor(const AActor* InTarget) const;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	void ShortenArmLength();
	void GrowArmLength();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* CollisionComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UPawnMovementComponent* MovementComponent;

private:
};
