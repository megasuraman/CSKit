// Copyright 2022 megasuraman
/**
 * @file CSKit_CppCoroutine.h
 * @brief c++でコルーチンを書ける
 * @author megasuraman
 * @date 2025/03/30
 */
#pragma once

#include "CoreMinimal.h"

class CSKIT_API CSKit_CppCoroutine
{
public:
	static constexpr uint32 msStartLineNumber = 0;
	static constexpr uint32 msBreakLineNumber = UINT32_MAX;

	CSKit_CppCoroutine();
	~CSKit_CppCoroutine();

	void	Reset() { SetLine(msStartLineNumber); }

	void	SetLine(const uint32 InLine) { mLine = InLine; }
	void	SetSleepSec(const float InSec) { mSleepSec = InSec; }

	void	UpdateSleepSec(const float InDeltaSec)
	{
		if (mSleepSec > 0.f)
		{
			mSleepSec -= InDeltaSec;
		}
	}

	uint32	GetLine() const { return mLine; }
	float	GetSleepSec() const { return mSleepSec; }
	bool	IsSleepSec() const { return (mSleepSec > 0.f); }

private:
	uint32		mLine = 0;
	float		mSleepSec = 0.f;
};

enum class ECSKit_CCResult : uint8
{
	Finish = 0,
	Yield,
};

//-- コルーチン処理関数宣言 --//
#define		DECL_CC_FUNC(_func_name_)	ECSKit_CCResult	_func_name_(CSKit_CppCoroutine& _cc, const float InDeltaSec)
#define		DECL_CC_FUNC_V(_func_name_, ...)	ECSKit_CCResult	_func_name_(CSKit_CppCoroutine& _cc, const float InDeltaSec, __VA_ARGS__)

//-- コルーチン処理関数定義 --//
#define		CC_FUNC(_func_name_)	ECSKit_CCResult	_func_name_(CSKit_CppCoroutine& _cc, const float InDeltaSec)
#define		CC_FUNC_V(_func_name_, ...)	ECSKit_CCResult	_func_name_(CSKit_CppCoroutine& _cc, const float InDeltaSec, __VA_ARGS__)

//-- コルーチン制御開始 --//
#define		CC_BEGIN()			switch (_cc.GetLine()){case CSKit_CppCoroutine::msStartLineNumber:

//-- コルーチン制御終了 --//
#define		CC_END()				default: break;}

//-- 一時停止 --//
#define		CC_YIELD()			{_cc.SetLine(__LINE__); return ECSKit_CCResult::Yield; case __LINE__:;}

//-- 先頭に戻る --//
#define		CC_RESTART()			_cc.SetLine(CSKit_CppCoroutine::msStartLineNumber); return ECSKit_CCResult::Yield;

//-- もうコルーチンに来ないようにする --//
#define		CC_BREAK()			{_cc.SetLine(CSKit_CppCoroutine::msBreakLineNumber);return ECSKit_CCResult::Finish;}

//-- 条件を満たす間一時停止 --//
#define		CC_WAIT(_continue_)	{while(_continue_){CC_YIELD()};}

//-- 条件を満たす間一時停止(停止中の処理も指定可) --//
#define		CC_WAIT_ACT(_continue_,_act_)	{while(_continue_){_act_;CC_YIELD()};}

//-- ずっと停止 --//
#define		CC_WAIT_EVER()	{while_const(TRUE){CC_YIELD()};}

//-- ずっと停止(停止中の処理も指定可) --//
#define		CC_WAIT_EVER_ACT(_act_)	{while_const(true){_act_;CC_YIELD()};}

//-- 一定時間停止 --//
#define		CC_SLEEP_SEC(_sec_)	{_cc.SetSleepSec(_sec_);CC_WAIT_ACT(_cc.IsSleepSec(),_cc.UpdateSleepSec(InDeltaSec));}

//-- 一定時間停止(停止中の処理も指定可) --//
#define		CC_SLEEP_SEC_ACT(_sec_, _act_)	{_cc.SetSleepSec(_sec_);while(_cc.IsSleepSec()){_cc.UpdateSleepSec(InDeltaSec);_act_;CC_YIELD()};}

/*** 使用例 

class SampleClass
{
public:
	void	Tick(float DeltaSec);
protected:
	DECL_CC_FUNC( UpdateCC );

private:
	CSKit_CppCoroutine	mCC;
};


void	SampleClass::Tick(float DeltaSec)
{
	ECSKit_CCResult Result = UpdateCC( mCC, DeltaSec );
}

CC_FUNC( SampleClass::UpdateCC )
{
	uint32 test = 0;

	CC_BEGIN();
	{
		Test();//なにか処理する
		CC_SLEEP_SEC( 3.f );//3秒待つ
		Test2();//なにか処理する
		CC_WAIT(!IsEndTest2());//IsEndTest2()がtrueになるまで待つ
		Test3();//なにか処理する
		CC_YIELD();//1フレーム待つ
		if(IsFinish())
		{
			CC_BREAK();//終わり
		}
		else
		{
			CC_RESTART();//最初に戻る
		}
	}
	CC_END();

	return ECSKit_CCResult::Yield;
}

***/