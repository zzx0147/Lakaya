// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"

void UWeaponBase::ApplyEvent(const uint8& EventNumber, const float& RequestTime)
{
	auto CurrentTime = GetServerTime();
	auto ExecutionTime = RequestTime + LockstepDelay;

	if (ExecutionTime < CurrentTime) ExecuteLateEvent(EventNumber, RequestTime, CurrentTime);
	else if (ExecutionTime == CurrentTime) ExecuteEvent(EventNumber);
	else EnqueueEventSetTimer({ExecutionTime, EventNumber});
}

void UWeaponBase::EventTimerCallback()
{
	float NextEventRemainTime;
	do
	{
		ExecuteEvent(PriorityQueue.top().EventNumber);
		PriorityQueue.pop();

		if (PriorityQueue.empty())
		{
			CurrentEventExecutionTime = MAX_flt;
			return;
		}

		NextEventRemainTime = PriorityQueue.top().ExecutionTime - GetServerTime();
	}
	while (NextEventRemainTime <= 0.f);

	GetWorld()->GetGameState()->GetWorldTimerManager().SetTimer(EventTimer, this, &UWeaponBase::EventTimerCallback,
	                                                            NextEventRemainTime);
	CurrentEventExecutionTime = PriorityQueue.top().ExecutionTime;
}

void UWeaponBase::EnqueueEventSetTimer(const FEventInfoStruct&& EventInfo)
{
	PriorityQueue.push(EventInfo);
	// Don't set timer when recently added event is not closer than current waiting event
	if (CurrentEventExecutionTime <= PriorityQueue.top().ExecutionTime) return;
	GetWorld()->GetGameState()->GetWorldTimerManager().SetTimer(EventTimer, this, &UWeaponBase::EventTimerCallback,
	                                                            EventInfo.ExecutionTime - GetServerTime());
	CurrentEventExecutionTime = EventInfo.ExecutionTime;
}
