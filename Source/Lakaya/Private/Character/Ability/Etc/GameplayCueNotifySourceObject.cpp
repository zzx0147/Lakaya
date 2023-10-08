// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/Etc/GameplayCueNotifySourceObject.h"

#include "RemoteCallableSubObject.h"

UObject* UGameplayCueNotifySourceObject::GetGameplayCueNotifySourceObject(const int32& Index)
{
	return GetDefault<UGameplayCueNotifySourceObject>()->SourceObjects[Index];
}

UGameplayCueNotifySourceObject::UGameplayCueNotifySourceObject()
{
	static constexpr auto SourceObjectCount = 10;
	SourceObjects.Reserve(SourceObjectCount);
	for (auto Count = 0; Count < SourceObjectCount; ++Count)
	{
		SourceObjects.Emplace(
			CreateDefaultSubobject<URemoteCallableSubObject>(*FString::Printf(TEXT("SourceObject_%d"), Count)));
	}
}
