// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuCallingPlayerController.h"
#include "BattlePlayerController.generated.h"

UCLASS()
class LAKAYA_API ABattlePlayerController : public AMenuCallingPlayerController
{
	GENERATED_BODY()

public:
	ABattlePlayerController();

	virtual void OnCharacterBeginPlay(ACharacter* Character);

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UGamePlayKillLogWidget> KillLogClass;

	UPROPERTY(VisibleAnywhere)
	UGamePlayKillLogWidget* KillLogWidget;
};
