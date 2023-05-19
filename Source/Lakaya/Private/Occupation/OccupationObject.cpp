#include "Occupation/OccupationObject.h"

#include "Character/InteractableCharacter.h"
#include "Character/LakayaBasePlayerState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameMode/OccupationGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AOccupationObject::AOccupationObject()
{
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger Sphere"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	RootComponent = Trigger;
	Mesh->SetupAttachment(RootComponent);

	Trigger->InitSphereRadius(600.0f);
	Trigger->SetupAttachment(RootComponent);
	Trigger->SetRelativeLocation(FVector::ZeroVector);

	static const ConstructorHelpers::FObjectFinder<UStaticMesh> SM_Cylinder(
		TEXT("/Game/Dev/MeeLim/antena/White/Antenna.Antenna"));

	if (SM_Cylinder.Succeeded())
		Mesh->SetStaticMesh(SM_Cylinder.Object);

	bReplicates = true;
}

void AOccupationObject::BeginPlay()
{
	Super::BeginPlay();
	FOnOccupationStateSignature.AddUObject(this, &AOccupationObject::SetTeam);
}

void AOccupationObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOccupationObject, ObjectTeam);
}

void AOccupationObject::OnInteractionStart(const float& Time, APawn* Caller)
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("Object Interaction Start!"));
	
	const auto OccupationPlayerState = Cast<ALakayaBasePlayerState>(Caller->GetController()->PlayerState);
	if (OccupationPlayerState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OccupationObject_OccupationPlayerState is null."));
		return;
	}

	// 소유자 팀에서 상호작용 할 경우 막아두기
	// TODO : IsSameTeam(), 오브젝트에는 PlayerState 없어요
	const FString PlayerStateString = UEnum::GetValueAsString(OccupationPlayerState->GetTeam());
	if (PlayerStateString.Equals("EPlayerTeam::A", ESearchCase::IgnoreCase))
	{
		if (ObjectTeam == EPlayerTeam::A)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("OccupationObject_이미 점령한 오브젝트 입니다."));
			return;
		}
	}
	else if (PlayerStateString.Equals("EPlayerTeam::B", ESearchCase::IgnoreCase))
	{
		if (ObjectTeam == EPlayerTeam::B)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("OccupationObject_이미 점령한 오브젝트 입니다."));
			return;
		}
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
			// TODO :
			InteractingPawn = SecondCaller;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%s is faster!"), *InteractingPawn->GetName());
			if (SecondCaller != nullptr && SecondCaller->GetController() == Caller->Controller)
			{
				UE_LOG(LogTemp, Warning, TEXT("이미 누군가가 상호작용을 하고 있습니다."));
				// TODO : 
				return;
			}
		}
	}

	// 시작 한 후 3초가 지나게 되면 자동으로 성공.
	GetWorldTimerManager().SetTimer(InteractionTimerHandle, [this, Caller, Time]
	{
		// Caller->FinishInteraction
		Cast<AInteractableCharacter>(Caller)->FinishInteraction(EInteractionState::Success, Time);
	}, MaxInteractionDuration, false);
}

void AOccupationObject::OnInteractionStop(const float& Time, APawn* Caller)
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("Object Interaction Stop!"));

	if (Caller == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnInteractionStop_Caller is null."));
		return;
	}

	InteractingPawn = nullptr;
	FirstCallerTime = 0;

	// InteractionStop 기능에서 타이머가 이미 만료되었는지를 확인해줍니다.
	// 그렇지 않은 경우 타이머를 취소.
	if (GetWorldTimerManager().IsTimerActive(InteractionTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(InteractionTimerHandle);
	}

	Cast<AInteractableCharacter>(Caller)->FinishInteraction(EInteractionState::None, Time);
}

void AOccupationObject::OnCharacterDead(APawn* Caller)
{
	AInteractable::OnCharacterDead(Caller);
	// TODO : 구현해야 함.
}

// 상호작용에 성공했을 때, 실행되는 함수
void AOccupationObject::OnInteractionFinish(const APawn* Caller)
{
	AOccupationGameMode* OccupationGameMode = Cast<AOccupationGameMode>(GetWorld()->GetAuthGameMode<AGameMode>());
	if (OccupationGameMode == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("InteractionSuccess_OccupationGameMode is null."));
		return;
	}

	const auto CallerState = Cast<ALakayaBasePlayerState>(Caller->GetPlayerState());

	// 만약 성공한 플레이어 팀이 A라면
	if (CallerState->IsSameTeam(EPlayerTeam::A))
	{
		if (ObjectTeam == EPlayerTeam::B)
			OccupationGameMode->SubOccupyObject(EPlayerTeam::B);

		SetTeamObject(EPlayerTeam::A);
		OccupationGameMode->AddOccupyObject(EPlayerTeam::A);
		OnRep_BroadCastTeamObject();
	}
	else // 만약 성공한 플레이어 팀이 B팀 이라면
	{
		if (ObjectTeam == EPlayerTeam::A)
			OccupationGameMode->SubOccupyObject(EPlayerTeam::A);

		SetTeamObject(EPlayerTeam::B);
		OccupationGameMode->AddOccupyObject(EPlayerTeam::B);
		OnRep_BroadCastTeamObject();
	}
}

void AOccupationObject::OnRep_BroadCastTeamObject()
{
	SetTeamObject(ObjectTeam);
	FOnOccupationStateSignature.Broadcast(ObjectTeam);
}

void AOccupationObject::SetTeamObject(const EPlayerTeam& Team)
{
	ObjectTeam = Team;
}