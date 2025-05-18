// Copyright 2020 megasuraman.
/**
 * @file CSKit_TerritoryVolume.cpp
 * @brief 活動範囲
 * @author megasuraman
 * @date 2025/05/05
 */
#include "AI/Territory/CSKit_TerritoryVolume.h"

#include "CSKit_Subsystem.h"
#include "DrawDebugHelpers.h"
#include "Components/BrushComponent.h"
#include "Misc/UObjectToken.h"
#if ENGINE_MAJOR_VERSION == 5
#include "UObject/ObjectSaveContext.h"
#endif

#if USE_CSKIT_DEBUG
#include "CSKitDebug_Draw.h"
#include "CSKitDebug_Utility.h"
#include "ScreenWindow/CSKitDebug_ScreenWindowText.h"
#endif

ACSKit_TerritoryVolume::ACSKit_TerritoryVolume()
{
	GetBrushComponent()->SetGenerateOverlapEvents(false);
	//ゲーム中に交差判定する場合はNoCollisionにできない
	//GetBrushComponent()->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	GetBrushComponent()->Mobility = EComponentMobility::Static;

	bNetLoadOnClient = false;
}

#if ENGINE_MAJOR_VERSION == 5
void ACSKit_TerritoryVolume::PreSave(FObjectPreSaveContext ObjectSaveContext)
{
	Super::PreSave(ObjectSaveContext);
	
#if WITH_EDITOR
	EditorCheckError();
#endif
}
#endif

/**
 * @brief 
 */
bool ACSKit_TerritoryVolume::IsTouch(const FVector& InPos, const float InRadius) const
{
	if(!mManualExtentV.IsZero())
	{//自前のbox判定で行う
		const FVector LocalPos = GetActorTransform().InverseTransformPosition(InPos);
		return (FMath::Abs(LocalPos.X) < mManualExtentV.X+InRadius
			&& FMath::Abs(LocalPos.Y) < mManualExtentV.Y+InRadius
			&& FMath::Abs(LocalPos.Z) < mManualExtentV.Z+InRadius
			);
	}
	FVector ClosestPos;
	float DistanceSq;
	if(GetBrushComponent()->GetSquaredDistanceToCollision(InPos, DistanceSq, ClosestPos))
	{
		return DistanceSq <= FMath::Square(InRadius);
	}
	return false;
}

/**
 * @brief 
 */
void ACSKit_TerritoryVolume::BeginPlay()
{
	Super::BeginPlay();
	
	if (UCSKit_Subsystem* CSKitSubsystem = GetWorld()->GetSubsystem<UCSKit_Subsystem>())
	{
		CSKitSubsystem->EntryTerritoryVolume(this);
	}
}

/**
 * @brief 
 */
void ACSKit_TerritoryVolume::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (UCSKit_Subsystem* CSKitSubsystem = GetWorld()->GetSubsystem<UCSKit_Subsystem>())
	{
		CSKitSubsystem->ExitTerritoryVolume(this);
	}
}

/* ------------------------------------------------------------
  !mKindNameのプロパティのmeta情報用
------------------------------------------------------------ */
TArray<FName> ACSKit_TerritoryVolume::EditorGetKindNameList()
{
	TArray<FName> NameList;

	NameList.Add(FName());

	return NameList;
}

#if USE_CSKIT_DEBUG

/**
 * @brief 
 */
void ACSKit_TerritoryVolume::DebugDraw(UCanvas* InCanvas, const bool bInWireFrame) const
{
	DebugDrawInfo(InCanvas);

	if(IsManualBox())
	{
		if(bInWireFrame)
		{
			DrawDebugBox(
				GetWorld(),
				GetActorLocation(),
				mManualExtentV,
				GetActorRotation().Quaternion(),
				DebugGetColor(),
				false,
				-1.f,
				0,
				3.f
				);
		}
		else
		{
			DrawDebugSolidBox(
				GetWorld(),
				GetActorLocation(),
				mManualExtentV,
				GetActorRotation().Quaternion(),
				DebugGetColor(),
				false,
				-1.f,
				0
				);
		}
	}
	else
	{
		if(bInWireFrame)
		{
			UCSKitDebug_Draw::DrawBrushWire(GetWorld(), this, DebugGetColor());
		}
		else
		{
			UCSKitDebug_Draw::DrawBrushMesh(GetWorld(), this, DebugGetColor());
		}
	}
}
/**
 * @brief 
 */
void ACSKit_TerritoryVolume::DebugDrawInfo(UCanvas* InCanvas) const
{
	FCSKitDebug_ScreenWindowText ScreenWindowText;
	ScreenWindowText.SetWindowName(FString(TEXT("TerritoryVolume")));
	ScreenWindowText.SetWindowFrameColor(DebugGetColor());
	ScreenWindowText.AddText(FString::Printf(TEXT("KindName : %s"), *mUserName.ToString()));
	ScreenWindowText.AddText(FString::Printf(TEXT("IsManualBox : %d"), IsManualBox()));
	ScreenWindowText.AddText(FString::Printf(TEXT("%s"), *UCSKitDebug_Utility::GetActorLevelName(this)));
	ScreenWindowText.Draw(InCanvas, GetActorLocation());
}
/**
 * @brief 
 */
FColor ACSKit_TerritoryVolume::DebugGetColor() const
{
	return FColor::Blue;
}
#endif

#if WITH_EDITOR
/* ------------------------------------------------------------
  !エラーチェック
------------------------------------------------------------ */
void ACSKit_TerritoryVolume::EditorCheckError() const
{
	if(!GetWorld()->IsEditorWorld())
	{
		return;
	}

	if(mUserName.IsNone())
	{
		const FString ErrorMessage = FString::Printf(TEXT("「利用者」設定がNoneです"));
		FMessageLog EnemyCheck("CSKit");
		EnemyCheck.Warning()
			->AddToken(FUObjectToken::Create(this))
			->AddToken(FTextToken::Create(FText::FromString(ErrorMessage)))
			;
		EnemyCheck.Open();
	}
}
#endif