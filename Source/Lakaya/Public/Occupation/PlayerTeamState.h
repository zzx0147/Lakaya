// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerTeamState.generated.h"

/**
 * 
 */
UENUM()
enum class EPlayerTeamState : uint8
{
	// 현재 팀 배정을 받지 못한 상태
	None UMETA(DisplayerName = "None"),

	// A팀인 상태
	A UMETA(DisplayerName = "A"),

	// B팀인 상태
	B UMETA(DisplayerName = "B"),
};
