// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RiffleFireServer.h"
#include "RiffleFireClient.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API URiffleFireClient : public URiffleFireServer
{
	GENERATED_BODY()

protected:
	virtual void OnFireStartNotify() override;
	virtual void OnFireStopNotify() override;
	virtual void OnSwitchSelectorNotify() override;

private:
};
