// Fill out your copyright notice in the Description page of Project Settings.


#include "FocusableCharacter.h"

bool AFocusableCharacter::SetFocus(const EFocusKey& Key, const bool& IsSimulated)
{
	if (IsFocussed(Key, IsSimulated)) return false;
	(IsSimulated ? SimulatedFocusSpace : FocusSpace).FindOrAdd(Key) = true;
	return true;
}

void AFocusableCharacter::ReleaseFocus(const EFocusKey& Key, const bool& IsSimulated)
{
	auto& Map = GetFocusSpace(IsSimulated);
	if (Map.Contains(Key)) Map[Key] = false;
}

bool AFocusableCharacter::IsFocussed(const EFocusKey& Key, const bool& IsSimulated)
{
	auto& Map = GetFocusSpace(IsSimulated);
	return Map.Contains(Key) && Map[Key];
}
