#include "Occupation/OccupationObject.h"

#include "Character/InteractableCharacter.h"
#include "Character/LakayaBasePlayerState.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameMode/OccupationGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PlayerController/MovablePlayerController.h"

AOccupationObject::AOccupationObject()
{
	PrimaryActorTick.bCanEverTick = false;
	Tags.Add("Interactable");

	Trigger = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Trigger"));
	Cylinder = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cylinder"));

	RootComponent = Trigger;
	Cylinder->SetupAttachment(RootComponent);

	TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger Sphere"));
	TriggerSphere->InitSphereRadius(200.0f);
	TriggerSphere->SetCollisionProfileName("Custom"); // Set to custom collision profile
	TriggerSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // Enable overlap for Pawn channel
	// TriggerSphere->SetCollisionResponseToChannel(ECC_Object, ECR_Ignore); // Disable collision for Object channel
	TriggerSphere->SetupAttachment(RootComponent);

	Trigger->SetCapsuleSize(50.0f, 100.0f, true);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_Cylinder(
		TEXT("/Game/Dev/KML/antena/White/Antenna.Antenna"));
	if (SM_Cylinder.Succeeded())
		Cylinder->SetStaticMesh(SM_Cylinder.Object);

	Trigger->SetRelativeLocation(FVector::ZeroVector);

	bReplicates = true;
}

void AOccupationObject::BeginPlay()
{
	Super::BeginPlay();
}

void AOccupationObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOccupationObject, ObjectOwner);
}

void AOccupationObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AOccupationObject::OnInteractionStart(const float& Time, APawn* Caller)
{
	auto* OccupationPlayerState = Cast<ALakayaBasePlayerState>(Caller->GetController()->PlayerState);
	if (OccupationPlayerState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OccupationObject_OccupationPlayerState is null."));
		return;
	}

	// 아무도 상호작용을 하지 않고 있는 경우
	if (InteractingPawn == nullptr)
	{
		InteractingPawn = Caller;
		FirstCallerTime = Time;
	}
	else // 누군가가 상호작용을 이미 하고 있다고 판단될 때 (서버시간을 체크)
	{
		APawn* SecondCaller = Caller;
		float SecondCallTime = Time;

		// 더 빠르게 상호작용을 시작한 캐릭터를 결정합니다.
		if (SecondCallTime < FirstCallerTime)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s is faster!"), *SecondCaller->GetName());
			if (InteractingPawn != nullptr && InteractingPawn->GetController() == Caller->Controller) return;
			InteractingPawn = SecondCaller;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%s is faster!"), *InteractingPawn->GetName());
			if (SecondCaller != nullptr && SecondCaller->GetController() == Caller->Controller)
			{
				UE_LOG(LogTemp, Warning, TEXT("이미 누군가가 상호작용을 하고 있습니다."));
				return;
			}
		}
	}
	
	// 소우자 팀에서 상호작용 할 경우 막아두기
	FString PlayerStateString = UEnum::GetValueAsString(OccupationPlayerState->GetTeam());
	if (PlayerStateString.Equals("EPlayerTeam::A", ESearchCase::IgnoreCase))
	{
		if (ObjectOwner == EPlayerTeam::A)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("이미 본인 소유의 점령 오브젝트입니다."));
			return;
		}
	}
	else if (PlayerStateString.Equals("EPlayerTeam::B", ESearchCase::IgnoreCase))
	{
		if (ObjectOwner == EPlayerTeam::B)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("이미 점령한 오브젝트 입니다."));
			return;
		}
	}

	// TODO : 점프 도중 상호작용 시, 물리를 무시하고 공중에서 상호작용합니다.
	// 상호작용중에는 움직임을 막아줍니다.
	if (auto CastedCaller = Cast<AInteractableCharacter>(Caller))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("Object Interaction Start!"));
		CastedCaller->GetCharacterMovement()->DisableMovement();
		InteractingPawn = Caller;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OninteractionStart_CastedCaller is null."));
		return;
	}
	
	// 시작 시간을 가져옵니다.
	InteractingStartTime = UGameplayStatics::GetRealTimeSeconds(this);

	// 시작 한 후 4초가 지나게 되면 성공.
	GetWorldTimerManager().SetTimer(InteractionTimerHandle, this, &AOccupationObject::AutomaticInteractionStop, MaxInteractionDuration, false);
}

void AOccupationObject::OnInteractionStop(const float& Time, APawn* Caller)
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("Object Interaction Stop!"));

	InteractingPawn = nullptr;
	FirstCallerTime = 0;
	
	// 상호작용이 끝나게 되면 다시 움직일 수 있도록 해줍니다.
	if (auto CastedCaller = Cast<AInteractableCharacter>(Caller))
	{
		CastedCaller->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CastedCaller is null."));
		return;
	}

	// InteractionStop 기능에서 타이머가 이미 만료되었는지를 확인해줍니다.
	// 그렇지 않으 경우 타이머를 취소.
	if (GetWorldTimerManager().IsTimerActive(InteractionTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(InteractionTimerHandle);
	}

	// 끝났을 때 시점의 시간을 가져옵니다.
	InteractingStopTime = UGameplayStatics::GetRealTimeSeconds(this);

	// 상호작용한 시간을 가져옵니다.
	float InteractionDuration = InteractingStopTime - InteractingStartTime;
	
	UE_LOG(LogTemp, Warning, TEXT("InteractingStopTime : %f seconds"), InteractingStopTime);
	UE_LOG(LogTemp, Warning, TEXT("InteractingStartTime : %f seconds"), InteractingStartTime);
	UE_LOG(LogTemp, Warning, TEXT("Interaction Duration : %f seconds"), InteractionDuration);

	if (InteractionDuration > MaxInteractionDuration)
	{
		// 상호작용에 성공했을 경우.
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("Interaction Success"));

		auto* OccupationPlayerState = Cast<ALakayaBasePlayerState>(Caller->GetController()->PlayerState);
		if (OccupationPlayerState == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("OccupationObject_OccupationPlayerState is null."));
			return;
		}

		AOccupationGameMode* OccupationGameMode = Cast<AOccupationGameMode>(GetWorld()->GetAuthGameMode());
		if (OccupationGameMode == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("OccupationObject_OccupationGameMode is null."));
			return;
		}

		FString PlayerStateString = UEnum::GetValueAsString(OccupationPlayerState->GetTeam());
		if (PlayerStateString.Equals("EPlayerTeamState::None", ESearchCase::IgnoreCase))
		{
			UE_LOG(LogTemp, Warning, TEXT("Suspected player captured successfully."));
			return;
		}
		else if (PlayerStateString.Equals("EPlayerTeamState::A", ESearchCase::IgnoreCase))
		{
			UE_LOG(LogTemp, Warning, TEXT("Team A player captured successfully."));

			if (ObjectOwner == EPlayerTeam::B)
				OccupationGameMode->SubOccupyObject(EPlayerTeam::B);

			ObjectOwner = EPlayerTeam::A;
			OccupationGameMode->AddOccupyObject(EPlayerTeam::A);
			SetTeamObject(ObjectOwner);
			return;
		}
		else if (PlayerStateString.Equals("EPlayerTeamState::B", ESearchCase::IgnoreCase))
		{
			UE_LOG(LogTemp, Warning, TEXT("Team B player captured successfully."));

			if (ObjectOwner == EPlayerTeam::A)
				OccupationGameMode->SubOccupyObject(EPlayerTeam::A);
			
			ObjectOwner = EPlayerTeam::B;
			OccupationGameMode->SubOccupyObject(EPlayerTeam::B);
			SetTeamObject(ObjectOwner);
			return;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Error ! Error ! Error !"));
			return;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Interaction Failed."));
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("Interaction Failed."));
		return;
	}
}

void AOccupationObject::OnCharacterDead(APawn* Caller)
{
	IInteractable::OnCharacterDead(Caller);
}

void AOccupationObject::AutomaticInteractionStop()
{
	UE_LOG(LogTemp, Warning, TEXT("Cylinder AutomaticInteractionStop !"));

	if(InteractingPawn == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AutomaticInteractionStop_InteractingPawn is null."));
		return;
	}
	
	OnInteractionStop(UGameplayStatics::GetRealTimeSeconds(this), InteractingPawn);
	InteractingPawn = nullptr;
}

void AOccupationObject::OnRep_BroadCastTeamObject()
{
	SetTeamObject(ObjectOwner);
}

void AOccupationObject::SetTeamObject(const EPlayerTeam& Team)
{
	switch (Team)
	{
	case EPlayerTeam::A:
		Cylinder->SetMaterial(
			0, LoadObject<UMaterialInterface>(
				nullptr, TEXT("/Game/Characters/LakayaCharacter/Dummy/Materials/RedTeam.RedTeam")));
		break;
	case EPlayerTeam::B:
		Cylinder->SetMaterial(
			0, LoadObject<UMaterialInterface>(
				nullptr, TEXT("/Game/Characters/LakayaCharacter/Dummy/Materials/BlueTeam.BlueTeam")));
		break;
	case EPlayerTeam::None:
		break;
	}
}