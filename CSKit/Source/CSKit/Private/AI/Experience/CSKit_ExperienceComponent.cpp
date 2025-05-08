// Copyright 2020 megasuraman.
/**
 * @file CSKit_ExperienceComponent.cpp
 * @brief 経験値要素の管理Component
 * @author megasuraman
 * @date 2025/05/05
 */
#include "AI/Experience/CSKit_ExperienceComponent.h"

#include "AI/CSKit_AIController.h"
#include "CSKit_Subsystem.h"
#include "AI/Experience/CSKit_ExperienceDataTable.h"
#include "AI/Experience/CSKit_ExperienceElementBase.h"

#if USE_CSKIT_DEBUG
#include "CSKitDebug_Subsystem.h"
#include "ScreenWindow/CSKitDebug_ScreenWindowGraph.h"
#include "ScreenWindow/CSKitDebug_ScreenWindowText.h"
#endif

UCSKit_ExperienceComponent::UCSKit_ExperienceComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UCSKit_ExperienceComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCSKit_ExperienceComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	for (const auto& MapElement : mElementMap)
	{
		const CSKit_ExperienceElementBase* Element = MapElement.Value;
		delete Element;
	}
	mElementMap.Empty();

#if WITH_EDITOR
	if (mEditorScreenWindowGraph)
	{
		delete mEditorScreenWindowGraph;
		mEditorScreenWindowGraph = nullptr;
	}
#endif
}

void UCSKit_ExperienceComponent::OnChangeTarget(AActor* InNewTarget)
{
	for (const auto& MapElement : mElementMap)
	{
		CSKit_ExperienceElementBase* Element = MapElement.Value;
		Element->OnChangeTarget(InNewTarget);
	}
}

/* ------------------------------------------------------------
	!ダメージを受けたイベント
------------------------------------------------------------ */
void UCSKit_ExperienceComponent::OnTakeDamage(AActor* InDamageCauser, const float InDamage, const uint16 InParamBit)
{
	for (const auto& MapElement : mElementMap)
	{
		CSKit_ExperienceElementBase* Element = MapElement.Value;
		Element->OnTakeDamage(InDamageCauser, InDamage, InParamBit);
	}
}

/* ------------------------------------------------------------
	!ダメージを与えたイベント
------------------------------------------------------------ */
void UCSKit_ExperienceComponent::OnApplyDamage(AActor* InHitTarget, const float InDamage, const uint16 InParamBit)
{
	for (const auto& MapElement : mElementMap)
	{
		CSKit_ExperienceElementBase* Element = MapElement.Value;
		Element->OnApplyDamage(InHitTarget, InDamage, InParamBit);
	}
}

/* ------------------------------------------------------------
	!攻撃コリジョンのヒット判定した時
------------------------------------------------------------ */
void UCSKit_ExperienceComponent::OnCheckHitAttackCollision()
{
	for (const auto& MapElement : mElementMap)
	{
		CSKit_ExperienceElementBase* Element = MapElement.Value;
		Element->OnCheckHitAttackCollision();
	}
}

void UCSKit_ExperienceComponent::Update(const float InDeltaSec)
{
	for (const auto& MapElement : mElementMap)
	{
		CSKit_ExperienceElementBase* Element = MapElement.Value;
		Element->Update(InDeltaSec);
	}
}

/* ------------------------------------------------------------
   !各Elementへの設定
------------------------------------------------------------ */
void UCSKit_ExperienceComponent::Setup(const FCSKit_ExperienceTableRow& InData)
{
	for (const auto& DataElement : InData.mExperienceScoreElement)
	{
		if (CSKit_ExperienceElementBase** ElementPtr = mElementMap.Find(DataElement.Key))
		{
			if (CSKit_ExperienceElementBase* Element = *ElementPtr)
			{
				Element->SetScoreElement(DataElement.Value);
			}
		}
		else
		{
			//UE_LOG(LogCSKit, Warning, TEXT("UCSKit_ExperienceComponent::SetupElement() Invalid Key : %s"), *DataElement.Key.ToString());
#if USE_CSKIT_DEBUG
			UCSKitDebug_Subsystem::sOneShotWarning(
				GetWorld(),
				false,
				TEXT("ExperienceSetup"),
				FString::Printf(TEXT("UCSKit_ExperienceComponent::SetupElement() Invalid Key : %s"), *DataElement.Key.ToString())
			);
#endif
		}
	}

#if WITH_EDITOR
	for (const auto& MapElement : mElementMap)
	{
		if (const CSKit_ExperienceElementBase* Element = MapElement.Value)
		{
			if (!Element->EditorIsSetScoreElement())
			{
#if USE_CSKIT_DEBUG
				UCSKitDebug_Subsystem::sOneShotWarning(
					GetWorld(),
					false,
					TEXT("ExperienceSetupIsSetScoreElement"),
					FString::Printf(TEXT("UCSKit_ExperienceComponent::SetupElement() Loss Setup : %s"), *MapElement.Key.ToString())
				);
#endif
			}
		}
	}
#endif
}

/* ------------------------------------------------------------
   !Score取得
------------------------------------------------------------ */
float UCSKit_ExperienceComponent::GetScoreByName(const FName InTypeId) const
{
	if (const CSKit_ExperienceElementBase* const* ElementPtr = mElementMap.Find(InTypeId))
	{
		if (const CSKit_ExperienceElementBase* Element = *ElementPtr)
		{
			return Element->GetScore();
		}
	}
	else
	{
#if USE_CSKIT_DEBUG
		UCSKitDebug_Subsystem::sOneShotWarning(
			GetWorld(),
			false,
			TEXT("ExperienceGetScoreByName"),
			FString::Printf(TEXT("UCSKit_ExperienceComponent::GetScoreByName() Invalid TypeId : %s"), *InTypeId.ToString())
		);
#endif
	}
	return 0.f;
}

float UCSKit_ExperienceComponent::CalcAverageScoreTypeId(const FName InTypeId) const
{
	float TotalScore = 0.f;
	int32 ElementCount = 0;
	for (const auto& ElementMap : mElementMap)
	{
		if (const CSKit_ExperienceElementBase* Element = ElementMap.Value)
		{
			if(Element->GetTypeId() == InTypeId)
			{
				++ElementCount;
				TotalScore += Element->GetScore();
			}
		}
	}

	if(ElementCount <= 0)
	{
		return 0.f;
	}
	return TotalScore / static_cast<float>(ElementCount);
}

/* ------------------------------------------------------------
   !Element追加時処理
------------------------------------------------------------ */
void UCSKit_ExperienceComponent::OnAddElement(CSKit_ExperienceElementBase* InElement, const FName& InElementName) const
{
	InElement->SetOwner(Cast<ACSKit_AIController>(GetOwner()));
	InElement->SetElementName(InElementName);
}

/* ------------------------------------------------------------
   !Element追加時処理
------------------------------------------------------------ */
bool UCSKit_ExperienceComponent::CheckSameTypeId(const CSKit_ExperienceElementBase* InElement, const FName& InTypeId)
{
	if (InElement
		&& InElement->GetTypeId() == InTypeId)
	{
		return true;
	}
	return false;
}

#if USE_CSKIT_DEBUG
/* ------------------------------------------------------------
  !ActorWatcher選択時
------------------------------------------------------------ */
FString UCSKit_ExperienceComponent::DebugDrawSelectedActorWatcher(UCanvas* InCanvas) const
{
	FString DebugInfo;

	const bool bDrawFiltering = UCSKit_Subsystem::sDebugIsFlag(FName(TEXT("DrawExperienceGraphFiltering")));

	DebugInfo += FString::Printf(TEXT("[Experience]\n"));
	for (const auto& MapElement : mElementMap)
	{
		if(bDrawFiltering)
		{
			const FString DebugFlagName = FString::Printf(TEXT("DrawExperience_%s"), *MapElement.Key.ToString());
			if(!UCSKit_Subsystem::sDebugIsFlag(FName(*DebugFlagName)))
			{
				continue;
			}
		}
		const CSKit_ExperienceElementBase* Element = MapElement.Value;
		DebugInfo += FString::Printf(TEXT("   %s : %.2f\n"), *MapElement.Key.ToString(), Element->GetScore());
	}

	for (const auto& MapElement : mElementMap)
	{
		const CSKit_ExperienceElementBase* Element = MapElement.Value;
		DebugInfo += Element->DebugDrawSelectedActorWatcher(InCanvas);
	}

#if WITH_EDITOR
	EditorDrawScoreGraph(InCanvas, GetWorld()->GetDeltaSeconds());
#endif

	return DebugInfo;
}
#endif

#if WITH_EDITOR
/* ------------------------------------------------------------
  !グラフ表示
------------------------------------------------------------ */
void UCSKit_ExperienceComponent::EditorDrawScoreGraph(UCanvas* InCanvas, const float InDeltaSec) const
{
	if (mEditorScreenWindowGraph == nullptr)
	{
		mEditorScreenWindowGraph = new FCSKitDebug_ScreenWindowGraph();
		if (mEditorScreenWindowGraph == nullptr)
		{
			return;
		}

		mEditorScreenWindowGraph->InitWindowExtent();
	}

	const bool bDrawFiltering = UCSKit_Subsystem::sDebugIsFlag(FName(TEXT("DrawExperienceGraphFiltering")));

	for (const auto& MapElement : mElementMap)
	{
		if(bDrawFiltering)
		{
			const FString DebugFlagName = FString::Printf(TEXT("DrawExperience_%s"), *MapElement.Key.ToString());
			if(!UCSKit_Subsystem::sDebugIsFlag(FName(*DebugFlagName)))
			{
				mEditorScreenWindowGraph->SetScoreLineHide(MapElement.Key, true);
				continue;
			}
		}
		const CSKit_ExperienceElementBase* Element = MapElement.Value;
		mEditorScreenWindowGraph->AddScore(MapElement.Key, Element->GetScore());
	}

	mEditorScreenWindowGraph->Update(InDeltaSec);

	const FVector2D DrawPos(30.f, 30.f);
	mEditorScreenWindowGraph->Draw(InCanvas, DrawPos);
}
#endif