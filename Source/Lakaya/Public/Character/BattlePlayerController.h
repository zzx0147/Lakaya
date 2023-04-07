// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovablePlayerController.h"
#include "BattlePlayerController.generated.h"

UCLASS()
class LAKAYA_API ABattlePlayerController : public AMovablePlayerController
{
	GENERATED_BODY()

public:
	ABattlePlayerController();

	virtual void OnCharacterBeginPlay(ACharacter* ArgCharacter);

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UGamePlayKillLogWidget> KillLogClass;

	UPROPERTY(VisibleAnywhere)
	UGamePlayKillLogWidget* KillLogWidget;
};
