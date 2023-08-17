// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AgonesSetupGameMode.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API AAgonesSetupGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	AAgonesSetupGameMode();

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	virtual void OnConnectedAgones(const FGameServerResponse& Response);

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAgonesComponent* AgonesSDK;

private:
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> TargetLevel;

	FProcHandle ServerProcess;
};
