// Copyright 2020 megasuraman.
/**
 * @file CSKit_ExperienceElementSample.h
 * @brief ExperienceComponentで扱う経験値要素のサンプル
 * @author megasuraman
 * @date 2025/05/05
 */
#pragma once

#include "CoreMinimal.h"
#include "AI/Experience/CSKit_ExperienceElementBase.h"


class CSKIT_API CSKit_ExperienceElementSample : public CSKit_ExperienceElementBase
{
public:
	//素直にtypeid()使ってもいいかも
	static FName StaticGetTypeId() { static FName Type("Sample"); return Type; }
	virtual FName GetTypeId() const override { return StaticGetTypeId(); }

	CSKit_ExperienceElementSample() {};
	virtual ~CSKit_ExperienceElementSample() override {}

	virtual void	Update(const float InDeltaSec) override;
	virtual void	OnChangeTarget(AActor* InTarget) override;
	virtual void	OnTakeDamage(AActor* InDamageCauser, const float InDamage, const uint16 InParamBit) override;
	virtual void	OnApplyDamage(AActor* InDamageCauser, const float InDamage, const uint16 InParamBit) override;

protected:

private:
	float	mCountSec = 0.f;
	bool	mbCountUp = false;
};
