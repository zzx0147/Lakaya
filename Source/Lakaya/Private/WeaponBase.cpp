// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"

void UWeaponBase::ApplyEvent(const uint8& EventNumber, const float& RequestTime)
{
	auto CurrentTime = GetServerTime();
	auto ExecutionTime = RequestTime + LockstepDelay;

	if (ExecutionTime < CurrentTime)
	{
		ExecuteLateEvent(EventNumber, RequestTime, CurrentTime);
		return;
	}

	PriorityQueue.push({ExecutionTime, EventNumber});
	GetWorld()->GetGameState()->GetWorldTimerManager().SetTimer(EventTimer, this, &UWeaponBase::EventTimerCallback,
	                                                            ExecutionTime - CurrentTime);
}

void UWeaponBase::EventTimerCallback()
{
	ExecuteEvent(PriorityQueue.top().EventNumber);
	PriorityQueue.pop();
	if (PriorityQueue.empty()) return;
	GetWorld()->GetGameState()->GetWorldTimerManager().SetTimer(EventTimer, this, &UWeaponBase::EventTimerCallback,
	                                                            PriorityQueue.top().ExecutionTime - GetServerTime());
}
