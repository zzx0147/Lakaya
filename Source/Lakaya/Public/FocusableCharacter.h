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

public:
	bool SetFocus(const EFocusKey& Key, const bool& IsSimulated = false);
	void ReleaseFocus(const EFocusKey& Key, const bool& IsSimulated = false);
	bool IsFocussed(const EFocusKey& Key, const bool& IsSimulated = false);

protected:
	inline TMap<EFocusKey, bool>& GetFocusSpace(const bool& IsSimulated = false)
	{
		return IsSimulated ? SimulatedFocusSpace : FocusSpace;
	}

	TMap<EFocusKey, bool> FocusSpace;
	TMap<EFocusKey, bool> SimulatedFocusSpace;
};
