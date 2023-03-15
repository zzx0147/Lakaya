// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovableCharacter.h"
#include "FocusableCharacter.generated.h"

enum class EFocusKey : uint8
{
	MainHand
};

UCLASS()
class LAKAYA_API AFocusableCharacter : public AMovableCharacter
{
	GENERATED_BODY()

protected:
	TMap<EFocusKey, bool> FocusSpace;

	inline bool IsFocussed(const EFocusKey& Key) { return FocusSpace.Contains(Key) && FocusSpace[Key]; }
};
