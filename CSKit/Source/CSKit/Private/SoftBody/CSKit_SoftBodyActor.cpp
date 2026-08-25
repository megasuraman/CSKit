// Copyright 2020 megasuraman.
#include "SoftBody/CSKit_SoftBodyActor.h"

ACSKit_SoftBodyActor::ACSKit_SoftBodyActor()
	:Super()
{
	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	RootComponent = SceneComponent;
	
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

/**
 * @brief 
 */
void ACSKit_SoftBodyActor::BeginPlay()
{
	Super::BeginPlay();
	
#if 1//サンプル
	DebugCreateSampleTriangle();
#endif
}
/**
 * @brief 
 */
void ACSKit_SoftBodyActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}
/**
 * @brief 
 */
void ACSKit_SoftBodyActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	mSolver.SetBaseTransform(GetActorTransform());
	mSolver.Update(GetWorld(), DeltaSeconds);
}
/**
 * @brief 
 */
void ACSKit_SoftBodyActor::AddForce(const FVector &Force)
{
	mSolver.AddForce(Force);
}

#if !UE_BUILD_SHIPPING
/**
 * @brief デバッグ表示
 */
void ACSKit_SoftBodyActor::DebugDraw(UCanvas *InCanvas) const
{
	mSolver.DebugDraw(GetWorld(), InCanvas);
}
/**
 * @brief サンプル形状として三角形を構築
 */
void ACSKit_SoftBodyActor::DebugCreateSampleTriangle()
{
	const FTransform& BaseTransform = GetActorTransform();
	TArray<FCSKit_SoftBodyPoint> Points;
	Points.SetNum(3);
	Points[0] = FCSKit_SoftBodyPoint(FVector(100.f, 0.f, 0.f), BaseTransform);
	//Points[0].mbIsPinned = true;
	Points[1] = FCSKit_SoftBodyPoint(FVector(-50.0f, 86.f, 0.f), BaseTransform);
	Points[2] = FCSKit_SoftBodyPoint(FVector(-50.0f, -86.f, 0.f), BaseTransform);
	
	TArray<FCSKit_SoftBodyPointLink> Links;
	Links.SetNum(3);
	auto SetupLink = [&](int32 LinkIdx, int32 NodeA, int32 NodeB)
	{
		Links[LinkIdx].mIndexA = NodeA;
		Links[LinkIdx].mIndexB = NodeB;
		// 初期状態の距離をバネの自然長（RestLength）として記録
		Links[LinkIdx].mBaseLength = FVector::Distance(Points[NodeA].mPos, Points[NodeB].mPos);
	};

	SetupLink(0, 0, 1);
	SetupLink(1, 1, 2);
	SetupLink(2, 2, 0);
	
	mSolver.SetPointList(Points, Links);
}
#endif