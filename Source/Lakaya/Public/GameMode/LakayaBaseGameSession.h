// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "LakayaBaseGameSession.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API ALakayaBaseGameSession : public AGameSession
{
	GENERATED_BODY()

public:
	virtual void RegisterServer() override;
	virtual void NotifyLogout(const APlayerController* PC) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
};
