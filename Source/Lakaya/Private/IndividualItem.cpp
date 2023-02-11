#include "IndividualItem.h"
#include "IndividualGameMode.h"

AIndividualItem::AIndividualItem()
{
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));
	Box = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BOX"));

	RootComponent = Trigger;
	Box->SetupAttachment(RootComponent);

	Trigger->SetBoxExtent(FVector(10.5f, 10.5f, 10.5f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_BOX(TEXT("/Game/KDJ/Box_D50973B1.Box_D50973B1"));
	if (SM_BOX.Succeeded())
		Box->SetStaticMesh(SM_BOX.Object);
	
	// Trigger->SetRelativeLocation(FVector(400.0f, 0.0f, 0.0f));

	// Collision Profile
	Trigger->SetCollisionProfileName(TEXT("IndividualItem"));
	Box->SetCollisionProfileName(TEXT("NoCollision"));
}

void AIndividualItem::BeginPlay()
{
	Super::BeginPlay();

}

void AIndividualItem::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 등록
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AIndividualItem::OnCharacterOverlap);
}

void AIndividualItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AIndividualItem::OnCharacterOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("OnCharacterOverlap"));
	
	Box->SetHiddenInGame(true, true);
	SetActorEnableCollision(false);
}