#include "IndividualItem.h"

#include "CollectorPlayerState.h"
#include "IndividualGameMode.h"
#include "MenuCallingPlayerController.h"

AIndividualItem::AIndividualItem()
{
	PrimaryActorTick.bCanEverTick = false;
	Tags.Add("Interactable");
	
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));
	Box = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BOX"));

	RootComponent = Trigger;
	Box->SetupAttachment(RootComponent);

	Trigger->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_BOX(TEXT("/Game/Dev/KDJ/Box_D50973B1.Box_D50973B1"));
	if (SM_BOX.Succeeded())
		Box->SetStaticMesh(SM_BOX.Object);
	
	Trigger->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));

	// Collision Profile
	// Trigger->SetCollisionProfileName(TEXT("IndividualItem"));
	// Box->SetCollisionProfileName(TEXT("NoCollision"));

	bReplicates = true;
}

void AIndividualItem::BeginPlay()
{
	Super::BeginPlay();
}

void AIndividualItem::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 등록
	// Trigger->OnComponentBeginOverlap.AddDynamic(this, &AIndividualItem::OnCharacterOverlap);
}

void AIndividualItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AIndividualItem::InteractionStart(const float& Time, APawn* Caller)
{
	GetItem();

	if (Caller && Caller->GetController())
	{
		ACollectorPlayerState* CollectorPlayerState = Cast<ACollectorPlayerState>(Caller->GetController()->PlayerState);
		if (CollectorPlayerState)
		{
			// Use CollectorPlayerState to do something
			CollectorPlayerState->GainPoint(1);
			UE_LOG(LogTemp, Warning, TEXT("Player %s has gained 1 point"), *CollectorPlayerState->GetPlayerName());
			UE_LOG(LogTemp, Warning, TEXT("Player Total Point: %d"), CollectorPlayerState->GetPoint());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("CollectorPlayerState is null."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Caller or Controller."));
	}

	UE_LOG(LogTemp, Warning, TEXT("InteractionStart_Implementation Function "));
}

void AIndividualItem::InteractionStop(const float& Time, APawn* Caller)
{
	UE_LOG(LogTemp, Warning, TEXT("InteractionStop_Implementation Function "));
}

int32 AIndividualItem::GetItemNumber()
{
	return ItemNumber;
}

void AIndividualItem::GetItem()
{
	UE_LOG(LogTemp, Warning, TEXT("GetItem !"));
	AIndividualGameMode* GameMode = Cast<AIndividualGameMode>(GetWorld()->GetAuthGameMode());
	
	if (!GameMode)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameMode is Null."));
		return;
	}
	else
	{
		GameMode->SpawnItem();
		GameMode->VectorArray.Remove(ItemNumber);
	}
	
	Destroy();
}

// void AIndividualItem::OnCharacterOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                         // UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
// {
// }