// Fill out your copyright notice in the Description page of Project Settings.


#include "OccupationObject.h"

#include "Character/CollectorPlayerState.h"
#include "Character/InteractableCharacter.h"
#include "Components/CapsuleComponent.h"
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

	// TODO : 애니메이션 적용
	// 긴 상호작용 애니메이션 시작하는 지점.
	
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
		// TODO :
		
		// ACollectorPlayerState* CollectorPlayerState = Cast<ACollectorPlayerState>(Caller->GetController()->PlayerState);
		// if (CollectorPlayerState)
		// {
			// uint8 CurrentEnergy = CollectorPlayerState->GetEnergy();
			// CollectorPlayerState->GainPoint(CurrentEnergy);
			// CollectorPlayerState->ResetEnergy();

			
			
			// UE_LOG(LogTemp, Warning, TEXT("Player Total Point : %d"), CollectorPlayerState->GetPoint());
			// UE_LOG(LogTemp, Warning, TEXT("Player Current Energy Num : %d"), CollectorPlayerState->GetEnergy());
		// }
		// else
		// {
			// UE_LOG(LogTemp, Warning, TEXT("CollectorPlayerState is Null."));
			// return;
		// }
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Interaction Failed."));
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("Interaction Failed."));
		return;
	}

	// InteractingStartTime = 0.0f;
	
	// TODO : 애니메이션 적용
	// 긴 상호작용 애니메이션 끝나는 지점.
}

void AOccupationObject::AutomaticInteractionStop()
{
	UE_LOG(LogTemp, Warning, TEXT("Cylinder AutomaticInteractionStop !"));

	// InteractionStop(MaxInteractionDuration, nullptr);
	if(InteractingPawn == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AutomaticInteractionStop_InteractingPawn is null."));
		return;
	}
	
	OnInteractionStop(InteractingPawn);
	InteractingPawn = nullptr;
}