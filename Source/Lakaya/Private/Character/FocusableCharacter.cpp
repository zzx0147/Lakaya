// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/FocusableCharacter.h"

bool AFocusableCharacter::SetFocus(const EFocusContext& Context, const EFocusSpace& Space, const EFocusState& State)
{
	if (IsFocussed(Context, Space)) return false;
	InternalGetFocusState(Context, Space) = State;
	BroadcastFocusEvent(Context, Space, State);
	return true;
}

void AFocusableCharacter::SetFocusForce(const EFocusContext& Context, const EFocusSpace& Space,
                                        const EFocusState& State)
{
	auto& CurrentState = InternalGetFocusState(Context, Space);
	if (CurrentState != EFocusState::None)
		UE_LOG(LogActor, Warning, TEXT("FocusState was not None! It was %d"), CurrentState);
	CurrentState = State;
	BroadcastFocusEvent(Context, Space, State);
}

bool AFocusableCharacter::ReleaseFocus(const EFocusContext& Context, const EFocusSpace& Space, const EFocusState& State)
{
	auto& CurrentState = InternalGetFocusState(Context, Space);
	if (CurrentState == EFocusState::None || CurrentState != State) return false;
	CurrentState = EFocusState::None;
	BroadcastFocusEvent(Context, Space, CurrentState);
	return true;
}

void AFocusableCharacter::ReleaseFocusForce(const EFocusContext& Context, const EFocusSpace& Space,
                                            const EFocusState& State)
{
	auto& CurrentState = InternalGetFocusState(Context, Space);
	if (State != EFocusState::None && State != CurrentState)
		UE_LOG(LogActor, Error, TEXT("Current state matching error on ReleaseFocusForce!"));
	CurrentState = EFocusState::None;
	BroadcastFocusEvent(Context, Space, CurrentState);
}

bool AFocusableCharacter::IsFocussed(const EFocusContext& Context, const EFocusSpace& Space,
                                     const EFocusState& State) const
{
	if (FocusMap.Contains(Context) && FocusMap[Context].Contains(Space))
	{
		auto& CurrentState = FocusMap[Context][Space].State;
		return CurrentState != EFocusState::None && CurrentState != State;
	}
	return false;
}

bool AFocusableCharacter::IsFocussedBy(const EFocusContext& Context, const EFocusSpace& Space,
                                       const EFocusState& State) const
{
	if (!FocusMap.Contains(Context) || !FocusMap[Context].Contains(Space)) return State == EFocusState::None;
	return FocusMap[Context][Space].State == State;
}

void AFocusableCharacter::BroadcastFocusEvent(const EFocusContext& Context, const EFocusSpace& Space,
                                              const EFocusState& State)
{
	if (FocusMap.Contains(Context) && FocusMap[Context].Contains(Space))
		FocusMap[Context][Space].Event.Broadcast(State);
}
