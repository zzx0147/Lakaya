// Fill out your copyright notice in the Description page of Project Settings.

#include "Occupation/CaptureArena.h"

#include "Character/ArmedCharacter.h"
#include "Character/LakayaBasePlayerState.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"

ACaptureArena::ACaptureArena()
{
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));

	RootComponent = Trigger;

	Trigger->SetBoxExtent(FVector(200.f, 200.f, 200.f));
	
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ACaptureArena::OnOverlapBegin);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &ACaptureArena::OnOverlapEnd);

	OccupyingPlayerList.Emplace(EPlayerTeam::Anti);
	OccupyingPlayerList.Emplace(EPlayerTeam::Pro);
}

void ACaptureArena::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACaptureArena, CurrentCaptureArenaState);
}

void ACaptureArena::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 동일한 액터를 확인하여 self-overlaps를 무시합니다.
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		// 충돌한 액터가 캐릭터인지 확인합니다.
		const AArmedCharacter* OverlappedArmedCharacter = Cast<AArmedCharacter>(OtherActor);

		if (OverlappedArmedCharacter)
		{
			if(auto OccupyingPlayerState = Cast<ALakayaBasePlayerState>(OverlappedArmedCharacter->GetPlayerState()))
			{
				// 겹친 액터가 캐릭터입니다.
				// GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Character OverlapBegin."));
				AddToOccupyPlayerList(OccupyingPlayerState->GetTeam(), OccupyingPlayerState);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Character Cast Failed."));	
			}

		}
		else
		{
			// 겹친 액터는 캐릭터가 아닙니다.
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Trigger Overlapped by non-ArmedCharacter"));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Same Trigger"));
	}
}

void ACaptureArena::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// 동일한 액터를 확인하여 self-overlaps를 무시합니다.
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		// 충돌이 끝난 액터가 캐릭터인지 확인합니다.
		const AArmedCharacter* OverlappedArmedCharacter = Cast<AArmedCharacter>(OtherActor);

		if (OverlappedArmedCharacter)
		{
			if(auto OccupyingPlayerState = Cast<ALakayaBasePlayerState>(OverlappedArmedCharacter->GetPlayerState()))
			{
				// 충돌이 끝난 액터가 캐릭터입니다.
				// GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Character OverlapEnd."));
				RemoveFromOccupyPlayerList(OccupyingPlayerState->GetTeam(), OccupyingPlayerState);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Cast Failed."));	
			}

		}
		else
		{
			// 겹친 액터는 캐릭터가 아닙니다.
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Trigger Overlapped by non-ArmedCharacter"));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Same Trigger"));
	}
}

void ACaptureArena::AddToOccupyPlayerList(EPlayerTeam Team, ALakayaBasePlayerState* Player)
{
	if (OccupyingPlayerList.Contains(Team))
	{
		OccupyingPlayerList[Team].Add(Player);
	}

	CheckCaptureArenaInPlayer();
}

void ACaptureArena::RemoveFromOccupyPlayerList(EPlayerTeam Team, ALakayaBasePlayerState* Player)
{
	if (OccupyingPlayerList.Contains(Team))
	{
		OccupyingPlayerList[Team].Remove(Player);
	}

	CheckCaptureArenaInPlayer();
}

void ACaptureArena::CheckCaptureArenaInPlayer()
{
	UE_LOG(LogTemp, Warning, TEXT("AntiTeam Occupying Player Number : %d"), OccupyingPlayerList[EPlayerTeam::Anti].Num());
	UE_LOG(LogTemp, Warning, TEXT("ProTeam Occupying Player Number : %d"), OccupyingPlayerList[EPlayerTeam::Pro].Num());

	uint8 AntiTeamPlayerCount = OccupyingPlayerList.Contains(EPlayerTeam::Anti) ? OccupyingPlayerList[EPlayerTeam::Anti].Num() : 0;
	uint8 ProTeamPlayerCount = OccupyingPlayerList.Contains(EPlayerTeam::Pro) ? OccupyingPlayerList[EPlayerTeam::Pro].Num() : 0;

	if (AntiTeamPlayerCount > 0 && ProTeamPlayerCount == 0)
	{
		// Anti팀이 점령하고 있는 상태입니다.
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Anti팀이 점령하고 있는 상태입니다."));
	}
	else if (AntiTeamPlayerCount == 0 && ProTeamPlayerCount > 0)
	{
		// Pro팀이 점령하고 있는 상태입니다.
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Pro팀이 점령하고 있는 상태입니다."));
	}
	else if (AntiTeamPlayerCount > 0 && ProTeamPlayerCount > 0)
	{
		// Anti팀과 Pro팀이 대치하고 있는 상태입니다.
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Anti팀과 Pro팀이 대치하고 있는 상태입니다."));
	}
	else if (AntiTeamPlayerCount == 0 && ProTeamPlayerCount == 0)
	{
		// 아무도 점령하지 않은 상태입니다.
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("아무도 점령하지 않은 상태입니다."));
	}
}