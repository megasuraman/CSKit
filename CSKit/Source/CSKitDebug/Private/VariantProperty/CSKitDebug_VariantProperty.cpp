// Copyright 2020 megasuraman.

#include "VariantProperty/CSKitDebug_VariantProperty.h"

void FCSKit_VariantProperty::Set(const bool InValue)
{
	mType = ECSKit_VariantPropertyType::Bool;
	mDataString = InValue ? TEXT("true") : TEXT("false");;
}
void FCSKit_VariantProperty::Set(const int32 InValue)
{
	mType = ECSKit_VariantPropertyType::Int;
	mDataString = FString::FromInt(InValue);
}
void FCSKit_VariantProperty::Set(const float InValue)
{
	mType = ECSKit_VariantPropertyType::Float;
	mDataString = FString::SanitizeFloat(InValue);
}
void FCSKit_VariantProperty::Set(const FVector& InValue)
{
	mType = ECSKit_VariantPropertyType::Vector;
#if ENGINE_MAJOR_VERSION == 4
	mDataString = InValue.ToString();
#else
	mDataString = InValue.ToCompactString();
#endif
}
void FCSKit_VariantProperty::Set(const FTransform& InValue)
{
	mType = ECSKit_VariantPropertyType::Transform;
	mDataString = InValue.ToString();
}
void FCSKit_VariantProperty::Set(const FString& InValue)
{
	mType = ECSKit_VariantPropertyType::String;
	mDataString = InValue;
}
void FCSKit_VariantProperty::Set(const TArray<FString> &InList,
                        const int32 InSelectIndex)
{
	mType = ECSKit_VariantPropertyType::StringList;
	ensure(InSelectIndex < InList.Num());
	mDataString = FString::Printf(TEXT("%d"), InSelectIndex);
	for (const FString& String : InList)
	{
		mDataString += FString::Printf(TEXT(",%s"), *String);
	}
}
void FCSKit_VariantProperty::SetStringListSelectIndex(const int32 InSelectIndex)
{
	TArray<FString> List;
	int32 SelectIndex = 0;
	GetStringList(List, SelectIndex);
	Set( List, InSelectIndex);
}
bool FCSKit_VariantProperty::GetBool() const
{
	ensure(mType == ECSKit_VariantPropertyType::Bool);
	return mDataString.ToBool();
}
int32 FCSKit_VariantProperty::GetInt() const
{
	ensure(mType == ECSKit_VariantPropertyType::Int);
	return FCString::Atoi(*mDataString);;
}
float FCSKit_VariantProperty::GetFloat() const
{
	ensure(mType == ECSKit_VariantPropertyType::Float);
	return FCString::Atof(*mDataString);;
}
FVector FCSKit_VariantProperty::GetVector() const
{
	ensure(mType == ECSKit_VariantPropertyType::Vector);
	FVector Vector;
#if ENGINE_MAJOR_VERSION == 4
	Vector.InitFromString(mDataString);
#else
	Vector.InitFromCompactString(mDataString);
#endif
	return Vector;
}
FTransform FCSKit_VariantProperty::GetTransform() const
{
	ensure(mType == ECSKit_VariantPropertyType::Transform);
	FTransform Transform;
	Transform.InitFromString(mDataString);
	return Transform;
}
FString FCSKit_VariantProperty::GetString() const
{
	ensure(mType == ECSKit_VariantPropertyType::String);
	return mDataString;
}
void FCSKit_VariantProperty::GetStringList(TArray<FString>& OutList, int32& OutSelectIndex) const
{
	ensure(mType == ECSKit_VariantPropertyType::StringList);
	mDataString.ParseIntoArray(OutList, TEXT(","));
	OutSelectIndex = FCString::Atoi(*OutList[0]);
	OutList.RemoveAt(0);
}
