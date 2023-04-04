#include "Individual/IndividualStaticEnergy.h"
#include "Character/CollectorPlayerState.h"
#include "GameMode/IndividualGameMode.h"
#include "Character/InteractableCharacter.h"
#include "Net/UnrealNetwork.h"

AIndividualStaticEnergy::AIndividualStaticEnergy()
{
	PrimaryActorTick.bCanEverTick = false;
	Tags.Add("Interactable");
	
	Trigger = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Trigger"));
	Cylinder = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cylinder"));

	RootComponent = Trigger;
	Cylinder->SetupAttachment(RootComponent);

	Trigger->SetCapsuleSize(50.0f, 50.0f, true);
	Trigger->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_Antenna(TEXT("/Game/Dev/KDJ/Antena/SM_Antenna.SM_Antenna"));
	if (SM_Antenna.Succeeded())
		Cylinder->SetStaticMesh(SM_Antenna.Object);
	
	Trigger->SetRelativeLocation(FVector::ZeroVector);

	bIsAvailable = true;
	
	bReplicates = true;
}

void AIndividualStaticEnergy::BeginPlay()
{
	Super::BeginPlay();
}

void AIndividualStaticEnergy::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AIndividualStaticEnergy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AIndividualStaticEnergy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AIndividualStaticEnergy::OnLocalInteractionBegin(APawn* Caller)
{
	if (auto CastedCaller = Cast<AInteractableCharacter>(Caller))
	{
		CastedCaller->NoticeInstantInteractionLocal();
	}
	else UE_LOG(LogActor, Error, TEXT("OnLocalInteractionBegin::Caller was not AInteractableCharacter!"));
}

void AIndividualStaticEnergy::OnServerInteractionBegin(const float& Time, APawn* Caller)
{
	if (auto CastedCaller = Cast<AInteractableCharacter>(Caller))
	{
		CastedCaller->InitiateInteractionStart(Time, this);
	}
}

// TODO : 애니메이션 적용
void AIndividualStaticEnergy::OnInteractionStart(APawn* Caller)
{
	// 비활성중이면 상호작용x
	if (!bIsAvailable)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("StaticEnergy Not Available."));
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("StaticEnergy Get !"));
	
	if (Caller && Caller->GetController())
	{
		ACollectorPlayerState* CollectorPlayerState = Cast<ACollectorPlayerState>(Caller->GetController()->PlayerState);
		if (CollectorPlayerState)
		{
			AInteractableCharacter* Character = Cast<AInteractableCharacter>(Caller);
			if (Character == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("OnInteractionStart_Character is null."));
				return;
			}

			// TODO : 애니메이션 적용
			// 상호작용 애니메이션 시작하는 지점.
			
			
			CollectorPlayerState->GainEnergy(1);
			UE_LOG(LogTemp, Warning, TEXT("Player Total Energy: %d"), CollectorPlayerState->GetEnergy());
			bIsAvailable = false;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("CollectorPlayerState is null."));
			return;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Caller or Controller."));
	}

	// TODO : N초 후 StaticEnergy bIsAvailable = true로 변경
	GetWorldTimerManager().SetTimer(AvailableTimerHandle, this, &AIndividualStaticEnergy::MakeAvailable, 3.0f, false);
}

void AIndividualStaticEnergy::MakeAvailable()
{
	bIsAvailable = true;
}