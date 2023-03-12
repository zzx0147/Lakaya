// Fill out your copyright notice in the Description page of Project Settings.


#include "RiffleFireClient.h"

void URiffleFireClient::OnFireStartNotify()
{
	Super::OnFireStartNotify();
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("FireStart"));
}

void URiffleFireClient::OnFireStopNotify()
{
	Super::OnFireStopNotify();
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("FireStop"));
}

void URiffleFireClient::OnSwitchSelectorNotify()
{
	Super::OnSwitchSelectorNotify();
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("Switch"));
}
