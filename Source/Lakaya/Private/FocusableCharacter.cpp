// Fill out your copyright notice in the Description page of Project Settings.


#include "FocusableCharacter.h"

bool AFocusableCharacter::SetFocus(const EFocusContext& Context, const EFocusSpace& Space, const EFocusState& State)
{
	if (IsFocussed(Context, Space)) return false;
	GetFocusState(Context, Space) = State;
	return true;
}

bool AFocusableCharacter::ReleaseFocus(const EFocusContext& Context, const EFocusSpace& Space, const EFocusState& State)
{
	auto& CurrentState = GetFocusState(Context, Space);
	if (CurrentState == EFocusState::None || CurrentState != State && State != EFocusState::Force) return false;
	CurrentState = EFocusState::None;
	return true;
}

bool AFocusableCharacter::IsFocussed(const EFocusContext& Context, const EFocusSpace& Space,
                                     const EFocusState& State) const
{
	if (!FocusMap.Contains(Context) || !FocusMap[Context].Contains(Space)) return false;
	auto& CurrentState = FocusMap[Context][Space];
	return CurrentState != EFocusState::None && CurrentState != State;
}

bool AFocusableCharacter::IsFocussedBy(const EFocusContext& Context, const EFocusSpace& Space,
                                       const EFocusState& State) const
{
	if (!FocusMap.Contains(Context) || !FocusMap[Context].Contains(Space)) return State == EFocusState::None;
	return FocusMap[Context][Space] == State;
}
