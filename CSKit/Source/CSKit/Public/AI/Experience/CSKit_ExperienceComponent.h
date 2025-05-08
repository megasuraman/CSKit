// Copyright 2020 megasuraman.
/**
 * @file CSKit_ExperienceComponent.h
 * @brief 経験値要素の管理Component
 * @author megasuraman
 * @date 2025/05/05
 */
#pragma once

#include "CoreMinimal.h"
#include "CSKit_ExperienceElementBase.h"
#include "Components/ActorComponent.h"
#include "CSKit_ExperienceComponent.generated.h"

struct FCSKitDebug_ScreenWindowGraph;
class CSKit_ExperienceElementBase;
class UCanvas;
struct FCSKit_ExperienceTableRow;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CSKIT_API UCSKit_ExperienceComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCSKit_ExperienceComponent();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OnChangeTarget(AActor* InNewTarget);
	virtual void OnTakeDamage(AActor* InDamageCauser, const float InDamage, const uint16 InParamBit);
	virtual void OnApplyDamage(AActor* InHitTarget, const float InDamage, const uint16 InParamBit);
	virtual void OnCheckHitAttackCollision();

	void Update(const float InDeltaSec);

	template<class T>
	void EntryElement()
	{
		T* NewElement = new T();
		OnAddElement(NewElement, T::StaticGetTypeId());
		mElementMap.Add(T::StaticGetTypeId(), NewElement);
	}
	template<class T>
	T* EntryElement(const FName& InKeyName)
	{
		T* NewElement = new T();
		OnAddElement(NewElement, InKeyName);
		mElementMap.Add(InKeyName, NewElement);
		return NewElement;
	}

	void Setup(const FCSKit_ExperienceTableRow& InData);

	template<class T>
	float GetScore() const
	{
		return GetScoreByName(T::StaticGetTypeId());
	}
	float GetScoreByName(const FName InTypeId) const;
	float CalcAverageScoreTypeId(const FName InTypeId) const;
	
	CSKit_ExperienceElementBase* FindElement(const FName& InKeyName)
	{
		if (CSKit_ExperienceElementBase** ElementPtr = mElementMap.Find(InKeyName))
		{
			return *ElementPtr;
		}
		return nullptr;
	}

	template<class T>
	T* GetElementByName(const FName& InKeyName)
	{
		if (CSKit_ExperienceElementBase** ElementPtr = mElementMap.Find(InKeyName))
		{
			if (CSKit_ExperienceElementBase* Element = *ElementPtr)
			{
				check(CheckSameTypeId(Element, T::StaticGetTypeId()));
				if (CheckSameTypeId(Element, T::StaticGetTypeId()))
				{
					return static_cast<T*>(Element);
				}
			}
		}
		return nullptr;
	}
	
	template<class T>
	TArray<T*> GetElementList()
	{
		TArray<T*> ElementList;
		for (const auto& MapElement : mElementMap)
		{
			if (CSKit_ExperienceElementBase* Element = MapElement.Value)
			{
				if(Element->GetTypeId() == T::StaticGetTypeId())
				{
					ElementList.Add(static_cast<T*>(Element));
				}
			}
		}
		return ElementList;
	}

protected:
	void OnAddElement(CSKit_ExperienceElementBase* InElement, const FName& InElementName) const;
	static bool CheckSameTypeId(const CSKit_ExperienceElementBase* InElement, const FName& InTypeId);

private:
	TMap<FName, CSKit_ExperienceElementBase*> mElementMap;

#if USE_CSKIT_DEBUG
public:
	virtual FString DebugDrawSelectedActorWatcher(UCanvas* InCanvas) const;
#endif

#if WITH_EDITORONLY_DATA
protected:
	void EditorDrawScoreGraph(UCanvas* InCanvas, const float InDeltaSec) const;
private:
	mutable FCSKitDebug_ScreenWindowGraph* mEditorScreenWindowGraph = nullptr;
#endif
};
