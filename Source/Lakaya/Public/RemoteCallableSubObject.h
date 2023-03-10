// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RemoteCallableSubObject.generated.h"

/**
 * @brief RPC를 호출하는 기능이 구현된 UObject 클래스입니다. 이 클래스를 상속하면 RPC를 사용할 수 있습니다.
 */
UCLASS()
class LAKAYA_API URemoteCallableSubObject : public UObject
{
	GENERATED_BODY()
	
public:
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual bool CallRemoteFunction(UFunction* Function, void* Parms, FOutParmRec* OutParms, FFrame* Stack) override;
	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
};
