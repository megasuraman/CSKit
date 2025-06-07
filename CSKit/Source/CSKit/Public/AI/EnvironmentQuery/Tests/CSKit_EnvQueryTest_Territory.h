// Copyright 2020 megasuraman.
/**
 * @file CSKit_EnvQueryTest_Territory.h
 * @brief EnvQueryTest Territory内かどうか
 * @author megasuraman
 * @date 2025/06/07
 */
#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "CSKit_EnvQueryTest_Territory.generated.h"

UCLASS(DisplayName = "CSKit Territory")
class CSKIT_API UCSKit_EnvQueryTest_Territory : public UEnvQueryTest
{
	GENERATED_UCLASS_BODY()

	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;

	virtual FText GetDescriptionTitle() const override;
	virtual FText GetDescriptionDetails() const override;
	
protected:
};
