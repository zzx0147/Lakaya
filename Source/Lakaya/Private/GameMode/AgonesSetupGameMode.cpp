// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/AgonesSetupGameMode.h"

#include "AgonesComponent.h"
#include "Classes.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Core/Public/HAL/PlatformProcess.h"
#include "Runtime/Core/Public/Misc/Parse.h"

AAgonesSetupGameMode::AAgonesSetupGameMode() : Super()
{
	AgonesSDK = CreateDefaultSubobject<UAgonesComponent>(TEXT("AgonesSDK"));
	AgonesSDK->bDisableAutoConnect = false;
	AgonesSDK->ConnectedDelegate.AddDynamic(this, &AAgonesSetupGameMode::OnConnectedAgones);
}

void AAgonesSetupGameMode::BeginPlay()
{
	Super::BeginPlay();

	GConfig->SetInt(TEXT("OnlineSubsystemSteam"),TEXT("ConfigWriteTest"), 54321, GEngineIni);
	GConfig->Flush(false, GEngineIni);
}

void AAgonesSetupGameMode::OnConnectedAgones(const FGameServerResponse& Response)
{
	int32 PrevQueryPort;
	int32 PrevGamePort;
	bool IsPortChanged = false;

	int32 MyGamePort = 7777;
	int32 MyQueryPort = 27015;
	for (FPort Port : Response.Status.Ports)
	{
		if (Port.Name.Equals(TEXT("steam"), ESearchCase::IgnoreCase))
		{
			MyQueryPort = Port.Port;
			continue;
		}
	
		if (Port.Name.Equals(TEXT("game"), ESearchCase::IgnoreCase))
		{
			MyGamePort = Port.Port;
		}
	}

	if (FParse::Value(FCommandLine::Get(), TEXT("QueryPort="), PrevQueryPort) == false)
	{
		IsPortChanged = true;
	}

	if (FParse::Value(FCommandLine::Get(), TEXT("Port="), PrevGamePort) == false)
	{
		IsPortChanged = true;
	}
	

	
	if (IsPortChanged)
	{
		FTimerHandle RestartTimer;
		UE_LOG(LogTemp, Warning, TEXT("Port Have to Change %d, %d, Restart Game"), MyGamePort, MyQueryPort);
		
		FString CommandLine = FString::Printf(TEXT("-c \"sh /home/ue4/LakayaServer.sh -Port=%d -QueryPort=%d\""), MyGamePort, MyQueryPort);
		ServerProcess = FPlatformProcess::CreateProc(TEXT("/bin/bash"), *CommandLine, true, false, false, nullptr, 0, nullptr, nullptr);
		FGenericPlatformMisc::RequestExit(false);
		// FString CommandLine = FString::Printf(TEXT("-c \"screen -dmS GameServer sh /home/ue4/LakayaServer.sh -Port=%d -QueryPort=%d\""), MyGamePort, MyQueryPort);
		// FPlatformProcess::CreateProc(TEXT("/bin/bash"), *CommandLine, true, false, false, nullptr, 0, nullptr, nullptr);

		// FString CommandLine = FString::Printf(TEXT("-c \"tmux new-session -d -s GameServer 'sh /home/ue4/LakayaServer.sh -Port=%d -QueryPort=%d'\""), MyGamePort, MyQueryPort);
		// FPlatformProcess::CreateProc(TEXT("/bin/bash"), *CommandLine, true, false, false, nullptr, 0, nullptr, nullptr);
		
		// FString CommandLine = FString::Printf(TEXT("-c \"nohup sh /home/ue4/LakayaServer.sh -Port=%d -QueryPort=%d > /dev/null 2>&1 &\""), MyGamePort, MyQueryPort);
		// FPlatformProcess::CreateProc(TEXT("/bin/bash"), *CommandLine, true, false, false, nullptr, 0, nullptr, nullptr);

		
		// FGenericPlatformMisc::RequestExit(false);
		//GetWorldTimerManager().SetTimer(RestartTimer,this,&AAgonesSetupGameMode::RestartServer,60.0f,false);
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Port Setup, Start Game"));
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, TargetLevel);
}

void AAgonesSetupGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	FPlatformProcess::WaitForProc(ServerProcess);
}
