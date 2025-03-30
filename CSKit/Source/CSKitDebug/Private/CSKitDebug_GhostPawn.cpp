// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_GhostPawn.h
 * @brief 任意のActorを間接的に操作可能にするControllerのためのダミーPawn
 * @author megasuraman
 * @date 2025/03/30
 */
#include "CSKitDebug_GhostPawn.h"

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/PlayerInput.h"
#include "GameFramework/SpringArmComponent.h"

ACSKitDebug_GhostPawn::ACSKitDebug_GhostPawn()
{
	BaseEyeHeight = 0.0f;
	bCollideWhenPlacing = false;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	CollisionComponent->InitSphereRadius(35.0f);
	CollisionComponent->SetCollisionProfileName(TEXT("NoCollision"));
	RootComponent = CollisionComponent;
	
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bInheritYaw = false;
	CameraBoom->TargetArmLength = 600.0f; // The camera follows at this distance behind the character
	CameraBoom->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f));
	//CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	//FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}


void InitializeCSKitDebugGhostPawnInputBindings()
{
	static bool bBindingsAdded = false;
	if (!bBindingsAdded)
	{
		bBindingsAdded = true;

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("CSKitDebugGhostPawn_TurnRate", EKeys::Gamepad_RightX, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("CSKitDebugGhostPawn_LookUpRate", EKeys::Gamepad_RightY, -1.f));
		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("CSKitDebugGhostPawn_ShortenArmLength", EKeys::Gamepad_DPad_Up));
		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("CSKitDebugGhostPawn_GrowArmLength", EKeys::Gamepad_DPad_Down));
	}
}

void ACSKitDebug_GhostPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	InitializeCSKitDebugGhostPawnInputBindings();

	PlayerInputComponent->BindAxis("CSKitDebugGhostPawn_TurnRate", this, &ACSKitDebug_GhostPawn::TurnAtRate);
	PlayerInputComponent->BindAxis("CSKitDebugGhostPawn_LookUpRate", this, &ACSKitDebug_GhostPawn::LookUpAtRate);
	PlayerInputComponent->BindAction("CSKitDebugGhostPawn_ShortenArmLength", EInputEvent::IE_Repeat, this, &ACSKitDebug_GhostPawn::ShortenArmLength);
	PlayerInputComponent->BindAction("CSKitDebugGhostPawn_GrowArmLength", EInputEvent::IE_Repeat, this, &ACSKitDebug_GhostPawn::GrowArmLength);
}

void ACSKitDebug_GhostPawn::OnPostAttachedActor(const AActor* InTarget) const
{
	CameraBoom->SetRelativeRotation(FRotator(-30.f, InTarget->GetActorRotation().Yaw, 0.f));
}


void ACSKitDebug_GhostPawn::TurnAtRate(float Rate)
{
	const float AddAngle = Rate * 180.f * GetWorld()->GetDeltaSeconds() * CustomTimeDilation;
	CameraBoom->AddRelativeRotation(FRotator(0.f, AddAngle , 0.f));
}

void ACSKitDebug_GhostPawn::LookUpAtRate(float Rate)
{
	const float AddAngle = Rate * 180.f * GetWorld()->GetDeltaSeconds() * CustomTimeDilation;
	CameraBoom->AddRelativeRotation(FRotator(AddAngle , 0.f, 0.f));
}

void ACSKitDebug_GhostPawn::ShortenArmLength()
{
	const float AddValue = 1500.f * GetWorld()->GetDeltaSeconds() * CustomTimeDilation;
	CameraBoom->TargetArmLength -= AddValue;
	CameraBoom->TargetArmLength = FMath::Max(CameraBoom->TargetArmLength, 200.f);
}
void ACSKitDebug_GhostPawn::GrowArmLength()
{
	const float AddValue = 1500.f * GetWorld()->GetDeltaSeconds() * CustomTimeDilation;
	CameraBoom->TargetArmLength += AddValue;
}
