// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerTeam.generated.h"

/**
 * 
 */
UENUM()
enum class EPlayerTeam : uint8
{
	// 현재 팀 배정을 받지 못한 상태
	None UMETA(DisplayerName = "None"),

	// A팀인 상태
	A UMETA(DisplayerName = "A"),

	// B팀인 상태
	B UMETA(DisplayerName = "B"),

	// 개인전
	Individual
};

UENUM()
enum class EObjectTeam : uint8
{
	// 현재 어느 팀이든 소속되어 있지 않은 상태
	None UMETA(DisplayerName = "None"),

	// A팀에 소속이 된 상태
	A UMETA(DisplayerName = "A"),

	// B팀에 소속이 된 상태
	B UMETA(DisplayerName = "B")
};