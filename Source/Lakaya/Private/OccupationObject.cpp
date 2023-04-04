// Fill out your copyright notice in the Description page of Project Settings.


#include "OccupationObject.h"

#include "Character/CollectorPlayerState.h"
#include "Character/InteractableCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameMode/OccupationGameState.h"
#include "Kismet/GameplayStatics.h"

AOccupationObject::AOccupationObject()
{
	PrimaryActorTick.bCanEverTick = false;
	Tags.Add("Interactable");

	Trigger = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Trigger"));
	Cylinder = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cylinder"));

	RootComponent = Trigger;
	Cylinder->SetupAttachment(RootComponent);

	Trigger->SetCapsuleSize(50.0f, 100.0f, true);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_Cylinder (TEXT("/Game/Dev/KDJ/Antena/SM_Antenna.SM_Antenna"));
	if (SM_Cylinder.Succeeded())
		Cylinder->SetStaticMesh(SM_Cylinder.Object);
	
	Trigger->SetRelativeLocation(FVector::ZeroVector);

	bReplicates = true;
}

void AOccupationObject::BeginPlay()
{
	Super::BeginPlay();
}

void AOccupationObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AOccupationObject::OnServerInteractionBegin(const float& Time, APawn* Caller)
{
	if (auto CastedCaller = Cast<AInteractableCharacter>(Caller))
		CastedCaller->InitiateInteractionStart(Time, this, 3.f);
	else UE_LOG(LogActor, Error, TEXT("OnServerInteractionBegin::Caller was not AInteractableCharacter!"));
}

void AOccupationObject::OnInteractionStart(APawn* Caller)
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("Object Interaction Start!"));

	if (InteractingPawn != nullptr)
	{
		UE_LOG(LogActor, Error, TEXT("Object is Using."));
		return;
	}
	
	InteractingPawn = Caller;

	InteractingStartTime = UGameplayStatics::GetRealTimeSeconds(this);

	// 시작 한 후 4초가 지나면 자동으로 성공.
	GetWorldTimerManager().SetTimer(InteractionTimerHandle, this, &AOccupationObject::AutomaticInteractionStop, MaxInteractionDuration, false);
}

void AOccupationObject::OnLocalInteractionStopBegin(APawn* Caller)
{
}

void AOccupationObject::OnServerInteractionStopBegin(const float& Time, APawn* Caller)
{
	if (auto CastedCaller = Cast<AInteractableCharacter>(Caller))
		CastedCaller->InteractionStopNotify(Time, this);
	else UE_LOG(LogActor, Error, TEXT("OnServerInteractionStopBegin::Caller was not AInteractableCharacter!"));
}

void AOccupationObject::OnInteractionStop(APawn* Caller)
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("Object Interaction Stop!"));

	if (InteractingPawn == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("InteractionStop_InteractingPawn is null."));
		return;
	}

	InteractingPawn = nullptr;
	
	if (!Caller && !Caller->GetController())
	{
		UE_LOG(LogTemp, Warning, TEXT("Caller is null."));
		return;
	}
	
	//InteractionStop 기능에서 타이머가 이미 만료되었는지 확인. 그렇지 않은 경우 타이머를 취소.
	if (GetWorldTimerManager().IsTimerActive(InteractionTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(InteractionTimerHandle);
	}
	
	InteractingStopTime = UGameplayStatics::GetRealTimeSeconds(this);
	
	float InteractionDuration = InteractingStopTime - InteractingStartTime;
	
	UE_LOG(LogTemp, Warning, TEXT("InteractingStopTime : %f seconds"), InteractingStopTime);
	UE_LOG(LogTemp, Warning, TEXT("InteractingStartTime : %f seconds"), InteractingStartTime);
	UE_LOG(LogTemp, Warning, TEXT("Interaction Duration : %f seconds"), InteractionDuration);

	
	if (InteractionDuration > MaxInteractionDuration)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("Interaction success."));
		
		ACollectorPlayerState* CollectorPlayerState = Cast<ACollectorPlayerState>(Caller->GetController()->PlayerState);
		if (CollectorPlayerState == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("OccupationObject_CollectorPlayerstate is null."));
			return;
		}

		AOccupationGameState* OccupationGameState = Cast<AOccupationGameState>(GetWorld()->GetGameState());
		if (OccupationGameState == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("OccupationObject_OccupationGameState is null."));
			return;
		}

		// TODO : 상호작용 성공 시 그 팀의 점수 증가
		FString PlayerStateString = UEnum::GetValueAsString(CollectorPlayerState->GetPlayerTeamState());
		if (PlayerStateString.Equals("EPlayerTeamState::None", ESearchCase::IgnoreCase))
		{
			UE_LOG(LogTemp, Warning, TEXT("Suspected player captured successfully."));
		}
		else if (PlayerStateString.Equals("EPlayerTeamState::A", ESearchCase::IgnoreCase))
		{
			UE_LOG(LogTemp, Warning, TEXT("Team A player captured successfully."));
		}
		else if (PlayerStateString.Equals("EPlayerTeamState::B", ESearchCase::IgnoreCase))
		{
			UE_LOG(LogTemp, Warning, TEXT("Team B player captured successfully."));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Error ! Error ! Error !"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Interaction Failed."));
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("Interaction Failed."));
		return;
	}
}

void AOccupationObject::AutomaticInteractionStop()
{
	UE_LOG(LogTemp, Warning, TEXT("Cylinder AutomaticInteractionStop !"));

	if(InteractingPawn == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AutomaticInteractionStop_InteractingPawn is null."));
		return;
	}
	
	OnInteractionStop(InteractingPawn);
	InteractingPawn = nullptr;
}