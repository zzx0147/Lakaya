// Fill out your copyright notice in the Description page of Project Settings.


#include "RemoteCallableSubObject.h"

bool URemoteCallableSubObject::CallRemoteFunction(UFunction* Function, void* Parms, FOutParmRec* OutParms,
                                                  FFrame* Stack)
{
	if (auto Actor = FindActor())
		if (auto Driver = Actor->GetNetDriver())
		{
			Driver->ProcessRemoteFunction(Actor, Function, Parms, OutParms, Stack, this);
			return true;
		}

	return false;
}

int32 URemoteCallableSubObject::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
	AActor* Actor = FindActor();
	return Actor ? Actor->GetFunctionCallspace(Function, Stack) : FunctionCallspace::Local;
}

AActor* URemoteCallableSubObject::FindActor()
{
	AActor* Actor = nullptr;
	if (GetOuter()->IsA(UActorComponent::StaticClass())) Actor = Cast<UActorComponent>(GetOuter())->GetOwner();
	else Actor = Cast<AActor>(GetOuter());
	return Actor;
}
