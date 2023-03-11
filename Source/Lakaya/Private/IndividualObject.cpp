#include "IndividualObject.h"

#include "CollectorPlayerState.h"
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

void AIndividualObject::InteractionStart(const float& Time, APawn* Caller)
{
	UE_LOG(LogTemp, Warning, TEXT("Cylinder InteractionStart !"));

	InteractingStartTime = UGameplayStatics::GetRealTimeSeconds(this);	
	
}

void AIndividualObject::InteractionStop(const float& Time, APawn* Caller)
{
	UE_LOG(LogTemp, Warning, TEXT("Cylinder InteractionStop !"));

	InteractingStopTime = UGameplayStatics::GetRealTimeSeconds(this);

	float InteractionDuration = InteractingStopTime - InteractingStartTime;
	UE_LOG(LogTemp, Warning, TEXT("Interaction Duration : %f seconds"), InteractionDuration);
	
	// if (Caller && Caller->GetController())
	// {
	// 	ACollectorPlayerState* CollectorPlayerState = Cast<ACollectorPlayerState>(Caller->GetController()->PlayerState);
	// 	if (CollectorPlayerState)
	// 	{
	// 		CollectorPlayerState->GetEnergy();
	// 		UE_LOG(LogTemp, Warning, TEXT("Player is interacting with an object."));
	// 		UE_LOG(LogTemp, Warning, TEXT("Player Total Energy : %d"), CollectorPlayerState->GetEnergy());
	// 	}
	// 	else
	// 	{
	// 		UE_LOG(LogTemp, Warning, TEXT("CollectorPlayerState is null."));
	// 	}
	// }
	// else
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("Invalid Caller or Controller."));
	// }
}