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

	// for (const auto& Tag : Tags)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("- %s"), *Tag.ToString());
	// }
	
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
	GetItem();

	ACollectorPlayerState* CollectorPlayerState = ACollectorPlayerState::GetCollectorPlayerState(Caller);
	if (CollectorPlayerState)
	{
		// CollectorPlayerState를 사용하여 을 수행합니다
		CollectorPlayerState->GainPoint(1);
		UE_LOG(LogTemp, Warning, TEXT("Player %s has gained 1 point"), *CollectorPlayerState->GetPlayerName());
		UE_LOG(LogTemp, Warning, TEXT("Player Totla Point : %d"), CollectorPlayerState->GetPoint());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CollectorPlayer is Null."));
	}
	
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
		GameMode->VectorArray.Remove(ItemNumber);
		GameMode->RandomSpawn();
	}
	
	Destroy();
}

// void AIndividualItem::OnCharacterOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                         // UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
// {
// }