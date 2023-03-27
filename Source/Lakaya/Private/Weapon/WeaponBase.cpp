// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/WeaponBase.h"

void UWeaponBase::ApplyEvent(const uint8& EventNumber, const float& RequestTime)
{
	auto CurrentTime = GetServerTime();
	auto ExecutionTime = RequestTime + LockstepDelay;

	if (ExecutionTime > CurrentTime) EnqueueSetTimer({ExecutionTime, EventNumber});
	else if (ExecutionTime < CurrentTime) ExecuteLateEvent(EventNumber, RequestTime, CurrentTime);
	else ExecuteEvent(EventNumber);
}

void UWeaponBase::ExecuteLateEvent(const uint8& EventNumber, const float& RequestTime, const float& CurrentTime)
{
	UE_LOG(LogNetTraffic, Warning, TEXT("RequestedTime : %f, CurrentTime : %f. ExecuteLateEvent Invoked..."),
	       RequestTime, CurrentTime);
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

void UWeaponBase::EnqueueSetTimer(const FEventInfoStruct&& EventInfo)
{
	PriorityQueue.emplace(EventInfo);
	// Don't set timer when recently added event is not closer than current waiting event
	if (CurrentEventExecutionTime <= PriorityQueue.top().ExecutionTime) return;
	GetWorld()->GetGameState()->GetWorldTimerManager().SetTimer(EventTimer, this, &UWeaponBase::EventTimerCallback,
	                                                            EventInfo.ExecutionTime - GetServerTime());
	CurrentEventExecutionTime = EventInfo.ExecutionTime;
}
