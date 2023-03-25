#include "Individual/IndividualObject.h"
#include "Character/CollectorPlayerState.h"
#include "Character/DamageableCharacter.h"
#include "Character/InteractableCharacter.h"
#include "Kismet/GameplayStatics.h"

AIndividualObject::AIndividualObject()
{
	PrimaryActorTick.bCanEverTick = false;
	Tags.Add("Interactable");

	Trigger = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Trigger"));
	Cylinder = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cylinder"));

	RootComponent = Trigger;
	Cylinder->SetupAttachment(RootComponent);

	Trigger->SetCapsuleSize(50.0f, 100.0f, true);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_Cylinder (TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (SM_Cylinder.Succeeded())
		Cylinder->SetStaticMesh(SM_Cylinder.Object);
	
	Trigger->SetRelativeLocation(FVector::ZeroVector);

	bIsAvailable = true;
	
	bReplicates = true;
}

void AIndividualObject::BeginPlay()
{
	Super::BeginPlay();
}

void AIndividualObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AIndividualObject::OnLocalInteractionBegin(APawn* Caller)
{
}

void AIndividualObject::OnServerInteractionBegin(const float& Time, APawn* Caller)
{
	if (auto CastedCaller = Cast<AInteractableCharacter>(Caller))
		CastedCaller->InitiateInteractionStart(Time, this, 5.f);
	else UE_LOG(LogActor, Error, TEXT("OnServerInteractionBegin::Caller was not AInteractableCharacter!"));
}

void AIndividualObject::OnInteractionStart(APawn* Caller)
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("Object Interaction Start!"));

	if (!bIsAvailable)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("Object Not Available."));
		return;
	}

	InteractingStartTime = UGameplayStatics::GetRealTimeSeconds(this);

	// 시작 한 후 4초가 지나면 자동으로 성공.
	GetWorldTimerManager().SetTimer(InteractionTimerHandle, this, &AIndividualObject::AutomaticInteractionStop, MaxInteractionDuration, false);
}

void AIndividualObject::OnLocalInteractionStopBegin(APawn* Caller)
{
}

void AIndividualObject::OnServerInteractionStopBegin(const float& Time, APawn* Caller)
{
	if (auto CastedCaller = Cast<AInteractableCharacter>(Caller))
		CastedCaller->InteractionStopNotify(Time, this);
	else UE_LOG(LogActor, Error, TEXT("OnServerInteractionStopBegin::Caller was not AInteractableCharacter!"));
}

void AIndividualObject::OnInteractionStop(APawn* Caller)
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("Object Interaction Stop!"));

	if (!bIsAvailable)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("Object Not Available."));
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Cylinder InteractionStop !"));

	InteractingStopTime = UGameplayStatics::GetRealTimeSeconds(this);

	//InteractionStop 기능에서 타이머가 이미 만료되었는지 확인. 그렇지 않은 경우 타이머를 취소.
	if (GetWorldTimerManager().IsTimerActive(InteractionTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(InteractionTimerHandle);
	}
	
	InteractingStopTime = UGameplayStatics::GetRealTimeSeconds(this);
	float InteractionDuration = InteractingStopTime - InteractingStartTime;
	
	UE_LOG(LogTemp, Warning, TEXT("Interaction Duration : %f seconds"), InteractionDuration);

	if (InteractionDuration > 4.0f)
	{
		if (Caller && Caller->GetController())
		{
			ACollectorPlayerState* CollectorPlayerState = Cast<ACollectorPlayerState>(Caller->GetController()->PlayerState);
			if (CollectorPlayerState)
			{
				uint8 CurrentEnergy = CollectorPlayerState->GetEnergy();
				CollectorPlayerState->GainPoint(CurrentEnergy);
				CollectorPlayerState->ResetEnergy();
				UE_LOG(LogTemp, Warning, TEXT("Player Total Point : %d"), CollectorPlayerState->GetPoint());
				UE_LOG(LogTemp, Warning, TEXT("Player Current Energy Num : %d"), CollectorPlayerState->GetEnergy());
				bIsAvailable = false;
				GetWorldTimerManager().SetTimer(AvailableTimerHandle, this, &AIndividualObject::MakeAvailable, 30.0f, false);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("CollectorPlayerState is Null."));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid Caller or Controller."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Interaction Failed."));
	}
}

void AIndividualObject::AutomaticInteractionStop()
{
	UE_LOG(LogTemp, Warning, TEXT("Cylinder AutomaticInteractionStop !"));

	InteractionStop(MaxInteractionDuration, nullptr);
}

void AIndividualObject::MakeAvailable()
{
	bIsAvailable = true;
}