#include "Occupation/OccupationObject.h"
#include "Character/InteractableCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

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

	// DOREPLIFETIME(AOccupationObject, ObjectOwner);
}

void AOccupationObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AOccupationObject::OnInteractionStart(const float& Time, APawn* Caller)
{
#pragma region TODO : ObjectOwner
	// AOccupationPlayerState* OccupationPlayerState = Cast<AOccupationPlayerState>(Caller->GetController()->PlayerState);
	// if (OccupationPlayerState == nullptr)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("OccupationObject_OccupationPlayerState is null."));
	// 	return;
	// }
#pragma endregion
	
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

#pragma region TODO : ObjectOwner
	// 소유자 팀에서 상호작용 할 경우 막아두기
	// FString PlayerStateString = UEnum::GetValueAsString(OccupationPlayerState->GetPlayerTeamState());
	// if (PlayerStateString.Equals("EPlayerTeam::A", ESearchCase::IgnoreCase))
	// {
	// 	if (ObjectOwner == EPlayerTeam::A)
	// 	{
	// 		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("이미 본인 소유의 점령 오브젝트입니다."));
	// 		return;
	// 	}
	// }
	// else if (PlayerStateString.Equals("EPlayerTeam::B", ESearchCase::IgnoreCase))
	// {
	// 	if (ObjectOwner == EPlayerTeam::B)
	// 	{
	// 		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("이미 점령한 오브젝트 입니다."));
	// 		return;
	// 	}
	// }
#pragma endregion 
	
	// TODO : 점프 도중 상호작용 시, 물리를 무시하고 공중에서 상호작용합니다.
	// 상호작용중에는 움직임을 막아줍니다.
	CharacterImMovable(Caller);
	
	// 시작 한 후 4초가 지나게 되면 성공.
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([this, Caller]()
	{
		InteractionSuccess(Caller);
	});
	GetWorldTimerManager().SetTimer(InteractionTimerHandle, TimerDelegate, MaxInteractionDuration, false);
}

void AOccupationObject::OnInteractionStop(const float& Time, APawn* Caller)
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("Object Interaction Stop!"));

	InteractingPawn = nullptr;
	FirstCallerTime = 0;
	
	// 상호작용이 끝나게 되면, 상호작용 관련 부분을 초기화 해줍니다.
	OnInteractionFinish(Caller);
	
	// InteractionStop 기능에서 타이머가 이미 만료되었는지를 확인해줍니다.
	// 그렇지 않은 경우 타이머를 취소.
	if (GetWorldTimerManager().IsTimerActive(InteractionTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(InteractionTimerHandle);
	}
}

void AOccupationObject::OnCharacterDead(APawn* Caller)
{
	AInteractable::OnCharacterDead(Caller);
}

void AOccupationObject::CharacterImMovable(APawn* Caller)
{
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
}

void AOccupationObject::CharacterMovable(APawn* Caller)
{
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
}

void AOccupationObject::InteractionSuccess(APawn* Caller)
{
	if (InteractingPawn == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AutomaticInteractionStop_InteractingPawn is null."));
		return;
	}
	
	InteractingPawn = nullptr;

#pragma region TODO : ObjectOwner
	// AOccupationPlayerState* OccupationPlayerState = Cast<AOccupationPlayerState>(Caller->GetController()->PlayerState);
	// if (OccupationPlayerState == nullptr)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("OccupationObject_OccupationPlayerState is null."));
	// 	return;
	// }
#pragma endregion

	Cast<AInteractableCharacter>(Caller)->SetInteractionState(EInteractionState::Success);
	
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Black, FString::Printf(TEXT("Interaction Successed !")));

	// 상호작용이 끝나게 되면, 상호작용 관련 부분을 초기화 해줍니다.
	OnInteractionFinish(Caller);
	
	// 상호작용을 성공하고 1초 뒤에 State 상태는 Success에서 None 상태로 변경
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([this, Caller]()
	{
		Cast<AInteractableCharacter>(Caller)->SetInteractionState(EInteractionState::None);
	});

	GetWorldTimerManager().SetTimer(InteractionStateHandle, TimerDelegate, 1.0f, false);
	
#pragma region TODO : ObjectOwner
	// FString PlayerStateString = UEnum::GetValueAsString(OccupationPlayerState->GetPlayerTeamState());
	// if (PlayerStateString.Equals("EPlayerTeamState::None", ESearchCase::IgnoreCase))
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("Suspected player captured successfully."));
	// 	return;
	// }
	// else if (PlayerStateString.Equals("EPlayerTeamState::A", ESearchCase::IgnoreCase))
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("Team A player captured successfully."));
	//
	// 	if (ObjectOwner == EPlayerTeam::B)
	// 		OccupationGameMode->SubOccupyObject(EPlayerTeam::B);
	//
	// 	ObjectOwner = EPlayerTeam::A;
	// 	OccupationGameMode->AddOccupyObject(EPlayerTeam::A);
	// 	SetTeamObject(ObjectOwner);
	// 	return;
	// }
	// else if (PlayerStateString.Equals("EPlayerTeamState::B", ESearchCase::IgnoreCase))
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("Team B player captured successfully."));
	//
	// 	if (ObjectOwner == EPlayerTeam::A)
	// 		OccupationGameMode->SubOccupyObject(EPlayerTeam::A);
	// 		
	// 	ObjectOwner = EPlayerTeam::B;
	// 	OccupationGameMode->SubOccupyObject(EPlayerTeam::B);
	// 	SetTeamObject(ObjectOwner);
	// 	return;
	// }
	// else
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("Error ! Error ! Error !"));
	// 	return;
	// }
#pragma endregion
}

void AOccupationObject::OnInteractionFinish(APawn* Caller)
{
	// 상호작용이 끝나게 되면 다시 움직일 수 있도록 해줍니다.
	CharacterMovable(Caller);

	auto* InteractableCharacter = Cast<AInteractableCharacter>(Caller);
	if (InteractableCharacter == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("InteractionSuccess_InteractableCharacter is null."));
		return;
	}
	InteractableCharacter->InitializeInteraction();
}

#pragma region TODO : ObjectOwner
// void AOccupationObject::OnRep_BroadCastTeamObject()
// {
// 	SetTeamObject(ObjectOwner);
// }

// void AOccupationObject::SetTeamObject(const EPlayerTeam& Team)
// {
// 	switch (Team)
// 	{
// 	case EPlayerTeam::A:
// 		Cylinder->SetMaterial(
// 			0, LoadObject<UMaterialInterface>(
// 				nullptr, TEXT("/Game/Characters/LakayaCharacter/Dummy/Materials/RedTeam.RedTeam")));
// 		break;
// 	case EPlayerTeam::B:
// 		Cylinder->SetMaterial(
// 			0, LoadObject<UMaterialInterface>(
// 				nullptr, TEXT("/Game/Characters/LakayaCharacter/Dummy/Materials/BlueTeam.BlueTeam")));
// 		break;
// 	case EPlayerTeam::None:
// 		break;
// 	}
// }
#pragma endregion