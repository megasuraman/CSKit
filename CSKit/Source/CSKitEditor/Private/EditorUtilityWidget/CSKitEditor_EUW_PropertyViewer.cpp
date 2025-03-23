// Copyright 2020 megasuraman
/**
 * @file CSKitEditor_EUW_PropertyViewer.cpp
 * @brief UObjectのProperty値表示
 * @author megasuraman
 * @date 2022/08/07
 */
#include "EditorUtilityWidget/CSKitEditor_EUW_PropertyViewer.h"

#include "Editor.h"
#include "Engine/Engine.h"
#include "UnrealEd/Public/Editor.h"

UCSKitEditor_EUW_PropertyViewer::UCSKitEditor_EUW_PropertyViewer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}
/**
 * @brief	プロパティの変化時
 */
void UCSKitEditor_EUW_PropertyViewer::PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

/**
 * @brief	
 */
FString	UCSKitEditor_EUW_PropertyViewer::GetPropertyInfo(UObject* InTarget, UClass* InTargetClass)
{
	if (InTargetClass == nullptr)
	{
		return FString(TEXT("None"));
	}

	FString DebugInfo;

	const FName OriginalFuncName(TEXT("CSKitEditor_GetInfoPropertyViewer"));
	if (UFunction* Result = InTarget->FindFunction(OriginalFuncName))
	{
		FPropertyWatchInfo OriginalInfoFunc;
		OriginalInfoFunc.BindUFunction(InTarget, OriginalFuncName);
		if (OriginalInfoFunc.IsBound())
		{
			const FString OriginalInfo = OriginalInfoFunc.Execute();
			if (!OriginalInfo.IsEmpty())
			{
				DebugInfo += FString(TEXT("/*** User ***/\n"));
				DebugInfo += OriginalInfo;
				DebugInfo += FString(TEXT("/******/\n"));
			}
		}
	}

	FString LastOwnerName(TEXT(""));
	for (TFieldIterator<FProperty> PropIt(InTargetClass); PropIt; ++PropIt)
	{
		FProperty* TargetProperty = *PropIt;
		const FString OwnerName = TargetProperty->Owner.GetName();
		if (LastOwnerName != OwnerName)
		{
			DebugInfo += FString::Printf(TEXT("<%s>\n"), *OwnerName);
			LastOwnerName = OwnerName;
		}
		if (TargetProperty->GetCPPType() == FString(TEXT("FPointerToUberGraphFrame")))
		{
			continue;
		}
		//DebugInfo += FString::Printf(TEXT("[%s]%s : "), *TargetProperty->GetCPPType(), *TargetProperty->GetNameCPP());
		DebugInfo += FString::Printf(TEXT("    %s : "), *TargetProperty->GetNameCPP());

		void* DataPtr = nullptr;
		if (FindDebuggingData(Cast<UBlueprint>(InTargetClass), InTarget, TargetProperty, DataPtr))
		{
			const FString DebugValue = GetDebugInfo_InContainer(0, TargetProperty, DataPtr);
			DebugInfo += FString::Printf(TEXT("    %s\n"), *DebugValue);
		}
	}

	return DebugInfo;
}
/**
 * @brief	
 */
bool	UCSKitEditor_EUW_PropertyViewer::UpdatePropertyInfo(UObject* InTarget, UClass* InTargetClass)
{
	mPropertyInfo.Empty();

	if (InTargetClass == nullptr)
	{
		return false;
	}

	const FName OriginalFuncName(TEXT("CSKitEditor_GetInfoPropertyViewer"));
	if (UFunction* Result = InTarget->FindFunction(OriginalFuncName))
	{
		FPropertyWatchInfo OriginalInfoFunc;
		OriginalInfoFunc.BindUFunction(InTarget, OriginalFuncName);
		if (OriginalInfoFunc.IsBound())
		{
			const FString OriginalInfo = OriginalInfoFunc.Execute();
			if (!OriginalInfo.IsEmpty())
			{
				FString& InfoString = mPropertyInfo.FindOrAdd(TEXT("User"));
				InfoString = OriginalInfo;
			}
		}
	}

	for (TFieldIterator<FProperty> PropIt(InTargetClass); PropIt; ++PropIt)
	{
		FProperty* TargetProperty = *PropIt;
		if (TargetProperty->GetCPPType() == FString(TEXT("FPointerToUberGraphFrame")))
		{
			continue;
		}

		const FString OwnerName = TargetProperty->Owner.GetName();
		FString& InfoString = mPropertyInfo.FindOrAdd(*OwnerName);
		if (!InfoString.IsEmpty())
		{
			InfoString += FString::Printf(TEXT("\n"));
		}
		InfoString += FString::Printf(TEXT("%s : "), *TargetProperty->GetNameCPP());

		void* DataPtr = nullptr;
		if (FindDebuggingData(Cast<UBlueprint>(InTargetClass), InTarget, TargetProperty, DataPtr))
		{
			const FString DebugValue = GetDebugInfo_InContainer(0, TargetProperty, DataPtr);
			InfoString += FString::Printf(TEXT("%s"), *DebugValue);
		}
	}

	return true;
}

/**
 * @brief	
 */
bool	UCSKitEditor_EUW_PropertyViewer::SetupTargetObjectList()
{
	mTargetObjectList.Empty();

	UWorld* World = GetWorld_GameServer();
	UWorld* SecondWorld = GetWorld_GameClient();

	TArray<UObject*> ObjectList;
	GetObjectsOfClass(mTargetClass, ObjectList);
	for (UObject* Object : ObjectList)
	{
		if (Object->GetWorld() == World
			|| Object->GetWorld() == SecondWorld)
		{
			mTargetObjectList.Add(Object);
		}
	}

	return mTargetObjectList.Num() > 0;
}
/**
 * @brief	
 */
FString	UCSKitEditor_EUW_PropertyViewer::GetDispObjectName(UObject* InTarget) const
{
	if (InTarget)
	{
		FString DispName = FString::Printf(TEXT("%s(%d)"),*InTarget->GetName(),InTarget->GetUniqueID());
		UWorld* OwnWorld = InTarget->GetWorld();
		if (UActorComponent* TargetComponent = Cast<UActorComponent>(InTarget))
		{
			if (AActor* OwnerActor = TargetComponent->GetOwner())
			{
				DispName += FString::Printf(TEXT("[%s]"), *OwnerActor->GetName());
				OwnWorld = OwnerActor->GetWorld();
			}
		}
		if (OwnWorld)
		{
			if (InTarget->GetWorld()->WorldType == EWorldType::Editor)
			{
				DispName += FString(TEXT("[Editor]"));
			}
			else if (InTarget->GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer)
			{
				DispName += FString(TEXT("[DGS]"));
			}
			else if (InTarget->GetWorld()->GetNetMode() == ENetMode::NM_Client)
			{
				DispName += FString(TEXT("[Client]"));
			}
		}
		return DispName;
	}
	return FString(TEXT("None"));
}

/**
 * @brief	
 */
void	UCSKitEditor_EUW_PropertyViewer::SetUpdateRealTime(bool bInRealTime)
{
	SetActiveDraw(bInRealTime);
}
/**
 * @brief	
 */
void	UCSKitEditor_EUW_PropertyViewer::Clear()
{
	SetUpdateRealTime(false);
	mTargetClass = nullptr;
	mTargetObjectList.Empty();
	mTargetObject.Reset();
}

/**
 * @brief	Widget閉じたとき？
 */
void	UCSKitEditor_EUW_PropertyViewer::NativeDestruct()
{
	Super::NativeDestruct();
	SetUpdateRealTime(false);
}

/**
 * @brief
 */
void UCSKitEditor_EUW_PropertyViewer::FakeTick()
{
	UpdateRealTime();
}

/**
 * @brief	FKismetDebugUtilities::FindDebuggingData()を参考に情報抜き出し
 */
bool UCSKitEditor_EUW_PropertyViewer::FindDebuggingData(UBlueprint* Blueprint, UObject* ActiveObject, FProperty* InProperty, void*& OutData)
{
	if (ActiveObject == nullptr
		|| InProperty == nullptr)
	{
		return false;
	}
	
	if (!InProperty->IsValidLowLevel())
	{
		return false;
	}

	if (!ActiveObject->IsValidLowLevel())
	{
		return false;
	}

	void* PropertyBase = nullptr;

	// Walk up the stack frame to see if we can find a function scope that contains the property as a local
	//for (const FFrame* TestFrame = Data.StackFrameAtIntraframeDebugging; TestFrame != NULL; TestFrame = TestFrame->PreviousFrame)
	//{
	//	if (Property->IsIn(TestFrame->Node))
	//	{
	//		// output parameters need special handling
	//		for (FOutParmRec* OutParmRec = TestFrame->OutParms; OutParmRec != nullptr; OutParmRec = OutParmRec->NextOutParm)
	//		{
	//			if (OutParmRec->Property == Property)
	//			{
	//				if (WatchPin->Direction == EEdGraphPinDirection::EGPD_Input)
	//				{
	//					// try to use the output pin we're linked to
	//					// otherwise the output param won't show any data since the return node hasn't executed when we stop here
	//					if (WatchPin->LinkedTo.Num() == 1)
	//					{
	//						return FindDebuggingData(Blueprint, ActiveObject, WatchPin->LinkedTo[0], OutProperty, OutData, OutDelta, OutParent, SeenObjects, OutbShouldUseContainerOffset);
	//					}
	//					else if (!WatchPin->LinkedTo.Num())
	//					{
	//						// If this is an output pin with no links, then we have no debug data
	//						// so fallback to the local stack frame
	//						PropertyBase = TestFrame->Locals;
	//					}
	//				}

	//				// If this is an out container property then a different offset must be used when exporting this property
	//				// to text. Only container properties are effected by this because ExportText_InContainer adds an extra
	//				// 8 byte offset, which  would point to the container's first element, not the container itself.
	//				const bool bIsContainer = OutParmRec->Property->IsA<FArrayProperty>() || OutParmRec->Property->IsA<FSetProperty>() || OutParmRec->Property->IsA<FMapProperty>();
	//				if (PropertyBase == nullptr && OutbShouldUseContainerOffset && bIsContainer)
	//				{
	//					*OutbShouldUseContainerOffset = true;
	//					PropertyBase = OutParmRec->PropAddr;
	//				}
	//				break;
	//			}
	//		}

	//		// Fallback to the local variables if we couldn't find one
	//		if (PropertyBase == nullptr)
	//		{
	//			PropertyBase = TestFrame->Locals;
	//		}
	//		break;
	//	}
	//}

	// Try at member scope if it wasn't part of a current function scope
	UClass* PropertyClass = InProperty->GetOwner<UClass>();
	if (!PropertyBase && PropertyClass)
	{
		if (ActiveObject->GetClass()->IsChildOf(PropertyClass))
		{
			PropertyBase = ActiveObject;
		}
		else if (AActor* Actor = Cast<AActor>(ActiveObject))
		{
			// Try and locate the propertybase in the actor components
			for (UActorComponent* ComponentIter : Actor->GetComponents())
			{
				if (ComponentIter->GetClass()->IsChildOf(PropertyClass))
				{
					PropertyBase = ComponentIter;
					break;
				}
			}
		}
	}
#if USE_UBER_GRAPH_PERSISTENT_FRAME
	// Try find the propertybase in the persistent ubergraph frame
// 	UFunction* OuterFunction = Property->GetOwner<UFunction>();
// 	if (!PropertyBase && OuterFunction)
// 	{
// 		UBlueprintGeneratedClass* BPGC = Cast<UBlueprintGeneratedClass>(Blueprint->GeneratedClass);
// 		if (BPGC && ActiveObject->IsA(BPGC))
// 		{
// 			PropertyBase = BPGC->GetPersistentUberGraphFrame(ActiveObject, OuterFunction);
// 		}
// 	}
#endif // USE_UBER_GRAPH_PERSISTENT_FRAME

	// see if our WatchPin is on a animation node & if so try to get its property info
// 	UAnimBlueprintGeneratedClass* AnimBlueprintGeneratedClass = Cast<UAnimBlueprintGeneratedClass>(Blueprint->GeneratedClass);
// 	if (!PropertyBase && AnimBlueprintGeneratedClass)
// 	{
// 		// are we linked to an anim graph node?
// 		FProperty* LinkedProperty = Property;
// 		const UAnimGraphNode_Base* Node = Cast<UAnimGraphNode_Base>(WatchPin->GetOuter());
// 		if (Node == nullptr && WatchPin->LinkedTo.Num() > 0)
// 		{
// 			const UEdGraphPin* LinkedPin = WatchPin->LinkedTo[0];
// 			// When we change Node we *must* change Property, so it's still a sub-element of that.
// 			LinkedProperty = FKismetDebugUtilities::FindClassPropertyForPin(Blueprint, LinkedPin);
// 			Node = Cast<UAnimGraphNode_Base>(LinkedPin->GetOuter());
// 		}
//
// 		if (Node && LinkedProperty)
// 		{
// 			FStructProperty* NodeStructProperty = CastField<FStructProperty>(FKismetDebugUtilities::FindClassPropertyForNode(Blueprint, Node));
// 			if (NodeStructProperty)
// 			{
// 				for (const FStructPropertyPath& NodeProperty : AnimBlueprintGeneratedClass->GetAnimNodeProperties())
// 				{
// 					if (NodeProperty.Get() == NodeStructProperty)
// 					{
// 						void* NodePtr = NodeProperty->ContainerPtrToValuePtr<void>(ActiveObject);
// 						OutProperty = LinkedProperty;
// 						OutData = NodePtr;
// 						OutDelta = NodePtr;
// 						OutParent = ActiveObject;
// 						return EWTR_Valid;
// 					}
// 				}
// 			}
// 		}
// 	}
//
// 	// If we still haven't found a result, try changing the active object to whatever is passed into the self pin.
// 	if (!PropertyBase)
// 	{
// 		UEdGraphNode* WatchNode = WatchPin->GetOwningNode();
//
// 		if (WatchNode)
// 		{
// 			UEdGraphPin* SelfPin = WatchNode->FindPin(TEXT("self"));
// 			if (SelfPin && SelfPin != WatchPin)
// 			{
// 				FProperty* SelfPinProperty = nullptr;
// 				void* SelfPinData = nullptr;
// 				void* SelfPinDelta = nullptr;
// 				UObject* SelfPinParent = nullptr;
// 				SeenObjects.AddUnique(ActiveObject);
// 				FKismetDebugUtilities::EWatchTextResult Result = FindDebuggingData(Blueprint, ActiveObject, SelfPin, SelfPinProperty, SelfPinData, SelfPinDelta, SelfPinParent, SeenObjects);
// 				FObjectPropertyBase* SelfPinPropertyBase = CastField<FObjectPropertyBase>(SelfPinProperty);
// 				if (Result == EWTR_Valid && SelfPinPropertyBase != nullptr)
// 				{
// 					void* PropertyValue = SelfPinProperty->ContainerPtrToValuePtr<void>(SelfPinData);
// 					UObject* TempActiveObject = SelfPinPropertyBase->GetObjectPropertyValue(PropertyValue);
// 					if (TempActiveObject && TempActiveObject != ActiveObject)
// 					{
// 						if (!SeenObjects.Contains(TempActiveObject))
// 						{
// 							return FindDebuggingData(Blueprint, TempActiveObject, WatchPin, OutProperty, OutData, OutDelta, OutParent, SeenObjects);
// 						}
// 					}
// 				}
// 			}
// 		}
// 	}

	// Now either print out the variable value, or that it was out-of-scope
	if (PropertyBase == nullptr)
	{
		return false;
	}
	OutData = PropertyBase;
	return true;
}

FString UCSKitEditor_EUW_PropertyViewer::GetDebugInfo_InContainer(int32 Index, FProperty* Property, const void* Data)
{
	return GetDebugInfoInternal(Property, Property->ContainerPtrToValuePtr<void>(Data, Index));
}
FString UCSKitEditor_EUW_PropertyViewer::GetDebugInfoInternal(FProperty* Property, const void* PropertyValue)
{
	FString DebugValue(TEXT("None"));
	if (Property == nullptr)
	{
		return DebugValue;
	}

// 	DebugInfo.Type = UEdGraphSchema_K2::TypeToText(Property);
// 	DebugInfo.DisplayName = Property->GetDisplayNameText();

	FByteProperty* ByteProperty = CastField<FByteProperty>(Property);
	if (ByteProperty)
	{
		UEnum* Enum = ByteProperty->GetIntPropertyEnum();
		if (Enum)
		{
			if (Enum->IsValidEnumValue(*(const uint8*)PropertyValue))
			{
				DebugValue = Enum->GetDisplayNameTextByValue(*(const uint8*)PropertyValue).ToString();
			}
			else
			{
				DebugValue = FString(TEXT("(INVALID)"));
			}

			return DebugValue;
		}

		// if there is no Enum we need to fall through and treat this as a FNumericProperty
	}

	FNumericProperty* NumericProperty = CastField<FNumericProperty>(Property);
	if (NumericProperty)
	{
		DebugValue = NumericProperty->GetNumericPropertyValueToString(PropertyValue);
		return DebugValue;
	}

	FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property);
	if (BoolProperty)
	{
		//const FCoreTexts& CoreTexts = FCoreTexts::Get();
		DebugValue = BoolProperty->GetPropertyValue(PropertyValue) ? FString(TEXT("True")) : FString(TEXT("False"));
		return DebugValue;
	}

	FNameProperty* NameProperty = CastField<FNameProperty>(Property);
	if (NameProperty)
	{
		DebugValue = ((FName*)PropertyValue)->ToString();
		return DebugValue;
	}

	FTextProperty* TextProperty = CastField<FTextProperty>(Property);
	if (TextProperty)
	{
		DebugValue = TextProperty->GetPropertyValue(PropertyValue).ToString();
		return DebugValue;
	}

	FStrProperty* StringProperty = CastField<FStrProperty>(Property);
	if (StringProperty)
	{
		DebugValue = StringProperty->GetPropertyValue(PropertyValue);
		return DebugValue;
	}

	FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Property);
	if (ArrayProperty)
	{
		checkSlow(ArrayProperty->Inner);

		FScriptArrayHelper ArrayHelper(ArrayProperty, PropertyValue);

		DebugValue = FString::Printf(TEXT("ArraySize(%d)"), ArrayHelper.Num());

 		for (int32 i = 0; i < ArrayHelper.Num(); i++)
 		{
 			uint8* PropData = ArrayHelper.GetRawPtr(i);
			const FString ArrayDebugValue = GetDebugInfoInternal(ArrayProperty->Inner, PropData);
			DebugValue += FString::Printf(TEXT("\n        [%d]%s"), i, *ArrayDebugValue);
 		}

		return DebugValue;
	}

	FStructProperty* StructProperty = CastField<FStructProperty>(Property);
	if (StructProperty)
	{
		FString WatchText;
		StructProperty->ExportTextItem(WatchText, PropertyValue, PropertyValue, nullptr, PPF_PropertyWindow, nullptr);
		const FString StructTypeName = StructProperty->Struct->GetName();
		if (StructTypeName == FString(TEXT("Vector"))
			|| StructTypeName == FString(TEXT("Quat")))
		{
			DebugValue = WatchText;
		}
		else
		{
			DebugValue = FString(TEXT(""));
			//最初と最後の()を消す
			WatchText = WatchText.RightChop(1);
			WatchText = WatchText.LeftChop(1);
			
			TArray<FString> ValueList;
			WatchText.ParseIntoArray(ValueList, TEXT(","), true);
			for (const FString& ValueString : ValueList)
			{
				DebugValue += FString::Printf(TEXT("\n        %s"), *ValueString);
			}
		}

// 		for (TFieldIterator<FProperty> It(StructProperty->Struct); It; ++It)
// 		{
// 			FDebugInfo StructDebugInfo;
// 			GetDebugInfoInternal(StructDebugInfo, *It, It->ContainerPtrToValuePtr<void>(PropertyValue, 0));
//
// 			DebugInfo.Children.Add(StructDebugInfo);
// 		}

		return DebugValue;
	}

	FEnumProperty* EnumProperty = CastField<FEnumProperty>(Property);
	if (EnumProperty)
	{
		FNumericProperty* LocalUnderlyingProp = EnumProperty->GetUnderlyingProperty();
		UEnum* Enum = EnumProperty->GetEnum();

		int64 Value = LocalUnderlyingProp->GetSignedIntPropertyValue(PropertyValue);

		// if the value is the max value (the autogenerated *_MAX value), export as "INVALID", unless we're exporting text for copy/paste (for copy/paste,
		// the property text value must actually match an entry in the enum's names array)
		if (Enum)
		{
			if (Enum->IsValidEnumValue(Value))
			{
				DebugValue = Enum->GetDisplayNameTextByValue(Value).ToString();
			}
			else
			{
				DebugValue = FString(TEXT("Invalid"));
			}
		}
		else
		{
			DebugValue = FString::Printf(TEXT("%lld"), Value);
		}

		return DebugValue;
	}

	FMapProperty* MapProperty = CastField<FMapProperty>(Property);
	if (MapProperty)
	{
		FScriptMapHelper MapHelper(MapProperty, PropertyValue);
		DebugValue = FString::Printf(TEXT("MapSize(%d)"), MapHelper.Num());
		uint8* PropData = MapHelper.GetPairPtr(0);

 		int32 Index = 0;
 		for (int32 Count = MapHelper.Num(); Count; PropData += MapProperty->MapLayout.SetLayout.Size, ++Index)
 		{
 			if (MapHelper.IsValidIndex(Index))
 			{
 				const FString ChildDebugValue = GetDebugInfoInternal(MapProperty->ValueProp, PropData + MapProperty->MapLayout.ValueOffset);

 				// use the info from the ValueProp and then overwrite the name with the KeyProp data
 				FString NameStr = TEXT("[");
 				MapProperty->KeyProp->ExportTextItem(NameStr, PropData, nullptr, nullptr, PPF_PropertyWindow | PPF_Delimited, nullptr);
 				NameStr += TEXT("] ");

				DebugValue += FString::Printf(TEXT("\n        [%d]%s%s"), Index, *NameStr, *ChildDebugValue);

 				--Count;
 			}
 		}

		return DebugValue;
	}

	FSetProperty* SetProperty = CastField<FSetProperty>(Property);
	if (SetProperty)
	{
		FScriptSetHelper SetHelper(SetProperty, PropertyValue);
		DebugValue = FString::Printf(TEXT("SetSize(%d)"), SetHelper.Num());
		//uint8* PropData = SetHelper.GetElementPtr(0);

// 		int32 Index = 0;
// 		for (int32 Count = SetHelper.Num(); Count; PropData += SetProperty->SetLayout.Size, ++Index)
// 		{
// 			if (SetHelper.IsValidIndex(Index))
// 			{
// 				FDebugInfo ChildInfo;
// 				GetDebugInfoInternal(ChildInfo, SetProperty->ElementProp, PropData);
//
// 				// members of sets don't have their own names
// 				ChildInfo.DisplayName = FText::GetEmpty();
//
// 				DebugInfo.Children.Add(ChildInfo);
//
// 				--Count;
// 			}
// 		}

		return DebugValue;
	}

	FObjectPropertyBase* ObjectPropertyBase = CastField<FObjectPropertyBase>(Property);
	if (ObjectPropertyBase)
	{
		UObject* Obj = ObjectPropertyBase->GetObjectPropertyValue(PropertyValue);
		if (Obj != nullptr)
		{
			//DebugValue = Obj->GetFullName();
			DebugValue = Obj->GetName();
		}

		return DebugValue;
	}

	FDelegateProperty* DelegateProperty = CastField<FDelegateProperty>(Property);
	if (DelegateProperty)
	{
		if (DelegateProperty->SignatureFunction)
		{
			DebugValue = DelegateProperty->SignatureFunction->GetDisplayNameText().ToString();
		}
		else
		{
			DebugValue = FString(TEXT("NoFunc"));
		}

		return DebugValue;
	}

	FMulticastDelegateProperty* MulticastDelegateProperty = CastField<FMulticastDelegateProperty>(Property);
	if (MulticastDelegateProperty)
	{
		if (MulticastDelegateProperty->SignatureFunction)
		{
			DebugValue = MulticastDelegateProperty->SignatureFunction->GetDisplayNameText().ToString();
		}
		else
		{
			DebugValue = FString(TEXT("NoFunc"));
		}

		return DebugValue;
	}
	return DebugValue;
}