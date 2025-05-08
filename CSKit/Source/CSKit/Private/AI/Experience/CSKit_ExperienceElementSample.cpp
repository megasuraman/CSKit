// Copyright 2020 megasuraman.
/**
 * @file CSKit_ExperienceElementSample.cpp
 * @brief ExperienceComponentで扱う経験値要素のサンプル
 * @author megasuraman
 * @date 2025/05/05
 */
#include "AI/Experience/CSKit_ExperienceElementSample.h"


void CSKit_ExperienceElementSample::Update(const float InDeltaSec)
{
	CSKit_ExperienceElementBase::Update(InDeltaSec);

	if (mbCountUp)
	{
		mCountSec += InDeltaSec;
		if (mCountSec > 1.f)
		{
			mbCountUp = false;
			mCountSec = 1.f;
		}
	}
	else
	{
		mCountSec -= InDeltaSec;
		if (mCountSec < 0.f)
		{
			mbCountUp = true;
			mCountSec = 0.f;
		}
	}
	SetScore(mCountSec);
}

void CSKit_ExperienceElementSample::OnChangeTarget(AActor* InTarget)
{
	CSKit_ExperienceElementBase::OnChangeTarget(InTarget);
}

void CSKit_ExperienceElementSample::OnTakeDamage(AActor* InDamageCauser, const float InDamage, const uint16 InParamBit)
{
	CSKit_ExperienceElementBase::OnTakeDamage(InDamageCauser, InDamage, InParamBit);
}

void CSKit_ExperienceElementSample::OnApplyDamage(AActor* InDamageCauser, const float InDamage, const uint16 InParamBit)
{
	CSKit_ExperienceElementBase::OnApplyDamage(InDamageCauser, InDamage, InParamBit);
}
