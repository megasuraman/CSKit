// Copyright 2020 megasuraman.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CSKit_SoftBodySolver.h"
#include "CSKit_SoftBodyActor.generated.h"

//SoftBodyを扱うActor
UCLASS(BlueprintType, Blueprintable)
class CSKIT_API ACSKit_SoftBodyActor : public AActor
{
	GENERATED_BODY()

public:
	ACSKit_SoftBodyActor();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;
	
	void AddForce(const FVector& Force);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Soft Body")
	FCSKit_SoftBodySpringSetting mSpringSetting;
	
private:
	CSKit_SoftBodySolver mSolver;
	
	
#if !UE_BUILD_SHIPPING
public:
	void DebugDraw(UCanvas* InCanvas) const;
	void DebugCreateSampleTriangle();
#endif
};
