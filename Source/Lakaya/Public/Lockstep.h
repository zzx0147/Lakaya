// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"

#define DEFINE_LOCKSTEP(FunctionName)\
	protected:\
	UFUNCTION(Server, Reliable, WithValidation)\
	void FunctionName(const float& RequestTime, const FString& Arg);\
	FTimerHandle FunctionName##Handle;\
	public:\
	void Call_##FunctionName(const FString& Arg = "")\
	{\
		FunctionName(GetWorld()->GetGameState()->GetServerWorldTimeSeconds(), Arg);\
	}

#define DEFINE_LOCKSTEP_NOTIFY(FunctionName, NotifyType)\
	DEFINE_LOCKSTEP(FunctionName)\
	protected:\
	UFUNCTION(NotifyType, Reliable)\
	void FunctionName##Notify(const float& RequestTime, const FString& Arg);\
	FTimerHandle FunctionName##NotifyHandle;
