#include "IndividualItem.h"
#include "IndividualGameMode.h"

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

	for (const auto& Tag : Tags)
	{
		UE_LOG(LogTemp, Warning, TEXT("- %s"), *Tag.ToString());
	}
	
	// const FColor TriggerColor = FColor(255, 0, 0);
	//
	// DrawDebugBox(
	// 	GetWorld(),
	// 	GetActorLocation(),
	// 	GetComponentsBoundingBox().GetExtent(),
	// 	TriggerColor,
	// 	true,
	// 	-1,
	// 	0,
	// 	10);
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

void AIndividualItem::InteractionStart_Implementation(const float& Time, APawn* Caller)
{
	UE_LOG(LogTemp, Warning, TEXT("InteractionStart_Implementation Function "));
}

void AIndividualItem::InteractionStop_Implementation(const float& Time, APawn* Caller)
{
	AIndividualGameMode* GameMode = GetWorld()->GetAuthGameMode<AIndividualGameMode>();
	
	if (GameMode)
	{
		GameMode->VectorArray.Remove(ItemNumber);
		GameMode->RandomSpawn();
	}
	else return;
	
	Destroy();
	
	UE_LOG(LogTemp, Warning, TEXT("InteractionStop_Implementation Function "));
}

int32 AIndividualItem::GetItemNumber()
{
	return ItemNumber;
}

void AIndividualItem::GetItem()
{
	AIndividualGameMode* GameMode = GetWorld()->GetAuthGameMode<AIndividualGameMode>();
	
	if (GameMode)
	{
		GameMode->VectorArray.Remove(ItemNumber);
		GameMode->RandomSpawn();
	}
	else
	{
		return;
	}
	
	Destroy();
}

// void AIndividualItem::OnCharacterOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                         // UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
// {
// }