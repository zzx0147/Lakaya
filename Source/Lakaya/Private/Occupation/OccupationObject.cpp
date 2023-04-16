#include "Occupation/OccupationObject.h"
#include "EnhancedInputSubsystems.h"
#include "Character/InteractableCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameMode/OccupationGameState.h"
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
}

void AOccupationObject::OnInteractionStop(const float& Time, APawn* Caller)
{
	IInteractable::OnInteractionStop(Time, Caller);
}

void AOccupationObject::OnCharacterDead(APawn* Caller)
{
	IInteractable::OnCharacterDead(Caller);
}

// void AOccupationObject::OnServerInteractionBegin(const float& Time, APawn* Caller)
// {
// 	ACollectorPlayerState* CollectorPlayerState = Cast<ACollectorPlayerState>(Caller->GetController()->PlayerState);
// 	if (CollectorPlayerState == nullptr)
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("OccupationObject_CollectorPlayerstate is null."));
// 		return;
// 	}
// 	
// 	// 소유자 팀에서 상호작용 할경우 막아두기.
// 	FString PlayerStateString = UEnum::GetValueAsString(CollectorPlayerState->GetPlayerTeamState());
// 	if (PlayerStateString.Equals("EPlayerTeamState::A", ESearchCase::IgnoreCase))
// 	{
// 		if (ObjectOwner == EObjectOwner::A)
// 		{
// 			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("이미 점령한 오브젝트 입니다."));
// 			return;
// 		}
// 	}
// 	else if (PlayerStateString.Equals("EPlayerTeamState::B", ESearchCase::IgnoreCase))
// 	{
// 		if (ObjectOwner == EObjectOwner::B)
// 		{
// 			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("이미 점령한 오브젝트 입니다."));
// 			return;
// 		}
// 	}
// 	
// 	if (auto CastedCaller = Cast<AInteractableCharacter>(Caller))
// 	{
// 		CastedCaller->InitiateInteractionStart(Time, this, 3.f);
// 		CastedCaller->GetCharacterMovement()->DisableMovement();
// 	}
// 	else UE_LOG(LogActor, Error, TEXT("OnServerInteractionBegin::Caller was not AInteractableCharacter!"));
// }
//
// void AOccupationObject::OnInteractionStart(APawn* Caller)
// {
// 	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("Object Interaction Start!"));
//
// 	if (InteractingPawn != nullptr)
// 	{
// 		UE_LOG(LogActor, Error, TEXT("Object is Using."));
// 		return;
// 	}
// 	
// 	InteractingPawn = Caller;
// 	
// 	InteractingStartTime = UGameplayStatics::GetRealTimeSeconds(this);
//
// 	// 시작 한 후 4초가 지나면 자동으로 성공.
// 	GetWorldTimerManager().SetTimer(InteractionTimerHandle, this, &AOccupationObject::AutomaticInteractionStop, MaxInteractionDuration, false);
// }
//
// void AOccupationObject::OnLocalInteractionStopBegin(APawn* Caller)
// {
// }
//
// void AOccupationObject::OnServerInteractionStopBegin(const float& Time, APawn* Caller)
// {
// 	if (auto CastedCaller = Cast<AInteractableCharacter>(Caller))
// 	{
// 		CastedCaller->InteractionStopNotify(Time, this);
// 		CastedCaller->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
// 	}
// 	else UE_LOG(LogActor, Error, TEXT("OnServerInteractionStopBegin::Caller was not AInteractableCharacter!"));
// }
//
// void AOccupationObject::OnInteractionStop(APawn* Caller)
// {
// 	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("Object Interaction Stop!"));
//
// 	auto CastedCaller = Cast<AInteractableCharacter>(Caller);
// 	CastedCaller->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
// 	
// 	if (InteractingPawn == nullptr)
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("InteractionStop_InteractingPawn is null."));
// 		return;
// 	}
//
// 	InteractingPawn = nullptr;
// 	
// 	if (!Caller && !Caller->GetController())
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("Caller is null."));
// 		return;
// 	}
// 	
// 	//InteractionStop 기능에서 타이머가 이미 만료되었는지 확인. 그렇지 않은 경우 타이머를 취소.
// 	if (GetWorldTimerManager().IsTimerActive(InteractionTimerHandle))
// 	{
// 		GetWorldTimerManager().ClearTimer(InteractionTimerHandle);
// 	}
// 	
// 	InteractingStopTime = UGameplayStatics::GetRealTimeSeconds(this);
// 	
// 	float InteractionDuration = InteractingStopTime - InteractingStartTime;
// 	
// 	UE_LOG(LogTemp, Warning, TEXT("InteractingStopTime : %f seconds"), InteractingStopTime);
// 	UE_LOG(LogTemp, Warning, TEXT("InteractingStartTime : %f seconds"), InteractingStartTime);
// 	UE_LOG(LogTemp, Warning, TEXT("Interaction Duration : %f seconds"), InteractionDuration);
//
// 	
// 	if (InteractionDuration > MaxInteractionDuration)
// 	{
// 		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("Interaction success."));
// 		
// 		ACollectorPlayerState* CollectorPlayerState = Cast<ACollectorPlayerState>(Caller->GetController()->PlayerState);
// 		if (CollectorPlayerState == nullptr)
// 		{
// 			UE_LOG(LogTemp, Warning, TEXT("OccupationObject_CollectorPlayerstate is null."));
// 			return;
// 		}
//
// 		AOccupationGameState* OccupationGameState = Cast<AOccupationGameState>(GetWorld()->GetGameState());
// 		if (OccupationGameState == nullptr)
// 		{
// 			UE_LOG(LogTemp, Warning, TEXT("OccupationObject_OccupationGameState is null."));
// 			return;
// 		}
// 		
// 		FString PlayerStateString = UEnum::GetValueAsString(CollectorPlayerState->GetPlayerTeamState());
// 		if (PlayerStateString.Equals("EPlayerTeamState::None", ESearchCase::IgnoreCase))
// 		{
// 			UE_LOG(LogTemp, Warning, TEXT("Suspected player captured successfully."));
// 			return;
// 		}
// 		else if (PlayerStateString.Equals("EPlayerTeamState::A", ESearchCase::IgnoreCase))
// 		{
// 			UE_LOG(LogTemp, Warning, TEXT("Team A player captured successfully."));
// 			
// 			if (ObjectOwner == EObjectOwner::B)
// 				OccupationGameState->SubBTeamObjectNum();
// 			
// 			ObjectOwner = EObjectOwner::A;
// 			OccupationGameState->AddATeamObjectNum();
// 			SetTeamObject(ObjectOwner);
// 			return;
// 		}
// 		else if (PlayerStateString.Equals("EPlayerTeamState::B", ESearchCase::IgnoreCase))
// 		{
// 			UE_LOG(LogTemp, Warning, TEXT("Team B player captured successfully."));
//
// 			if (ObjectOwner == EObjectOwner::A)
// 				OccupationGameState->SubATeamObjectNum();
// 			
// 			ObjectOwner = EObjectOwner::B;
// 			OccupationGameState->AddBTeamObjectNum();
// 			SetTeamObject(ObjectOwner);
// 			return;
// 		}
// 		else
// 		{
// 			UE_LOG(LogTemp, Warning, TEXT("Error ! Error ! Error !"));
// 			return;
// 		}
// 	}
// 	else
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("Interaction Failed."));
// 		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("Interaction Failed."));
// 		return;
// 	}
// }

// void AOccupationObject::AutomaticInteractionStop()
// {
// 	UE_LOG(LogTemp, Warning, TEXT("Cylinder AutomaticInteractionStop !"));
//
// 	if(InteractingPawn == nullptr)
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("AutomaticInteractionStop_InteractingPawn is null."));
// 		return;
// 	}
// 	
// 	OnInteractionStop(InteractingPawn);
// 	InteractingPawn = nullptr;
// }

void AOccupationObject::OnRep_BroadCastTeamObject()
{
	SetTeamObject(ObjectOwner);
}

void AOccupationObject::SetTeamObject(const EPlayerTeamState& Team)
{
	switch (Team)
	{
	case EPlayerTeamState::A:
		Cylinder->SetMaterial(
			0, LoadObject<UMaterialInterface>(
				nullptr, TEXT("/Game/Characters/LakayaCharacter/Dummy/Materials/RedTeam.RedTeam")));
		break;
	case EPlayerTeamState::B:
		Cylinder->SetMaterial(
			0, LoadObject<UMaterialInterface>(
				nullptr, TEXT("/Game/Characters/LakayaCharacter/Dummy/Materials/BlueTeam.BlueTeam")));
		break;
	case EPlayerTeamState::None:
		break;
	}
}
