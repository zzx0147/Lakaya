// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/LakayaBaseGameSession.h"

#include "GameMode/LakayaDefaultPlayGameMode.h"

void ALakayaBaseGameSession::RegisterServer()
{
	Super::RegisterServer();

	if (ALakayaDefaultPlayGameMode* DefaultPlayGameMode = Cast<ALakayaDefaultPlayGameMode>(GetWorld()->GetAuthGameMode()))
	{
		if(UAgonesComponent* AgonesSDK = DefaultPlayGameMode->GetAgonesSDK())
		{
			// AgonesSDK->SetPlayerCapacity();
			// AgonesSDK->SetLabel();
		}
	}
}

void ALakayaBaseGameSession::NotifyLogout(const APlayerController* PC)
{
	Super::NotifyLogout(PC);

	if (ALakayaDefaultPlayGameMode* DefaultPlayGameMode = Cast<ALakayaDefaultPlayGameMode>(GetWorld()->GetAuthGameMode()))
	{
		if(UAgonesComponent* AgonesSDK = DefaultPlayGameMode->GetAgonesSDK())
		{
			//우선 튜토리얼대로 썼는데, PlayerId를 UniqueNetID로 변경해야 할 수도 있음
			AgonesSDK->PlayerDisconnect(TEXT("netspeak-player"), {}, {});
		}
	}
}

void ALakayaBaseGameSession::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (ALakayaDefaultPlayGameMode* DefaultPlayGameMode = Cast<ALakayaDefaultPlayGameMode>(GetWorld()->GetAuthGameMode()))
	{
		if(UAgonesComponent* AgonesSDK = DefaultPlayGameMode->GetAgonesSDK())
		{
			//우선 튜토리얼대로 썼는데, PlayerId를 UniqueNetID로 변경해야 할 수도 있음
			AgonesSDK->PlayerConnect(TEXT("netspeak-player"), {}, {});
		}
	}
}
