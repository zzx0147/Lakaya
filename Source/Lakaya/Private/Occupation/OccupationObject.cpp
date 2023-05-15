#include "Occupation/OccupationObject.h"
#include "Character/InteractableCharacter.h"
#include "Character/LakayaBasePlayerState.h"
#include "Components/CapsuleComponent.h"
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

	Trigger->InitSphereRadius(500.0f);
	Trigger->SetupAttachment(RootComponent);
	Trigger->SetRelativeLocation(FVector::ZeroVector);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_Cylinder(
		TEXT("/Game/Dev/MeeLim/antena/White/Antenna.Antenna"));
	
	if (SM_Cylinder.Succeeded())
		Mesh->SetStaticMesh(SM_Cylinder.Object);

	bReplicates = true;
}

void AOccupationObject::BeginPlay()
{
	Super::BeginPlay();
}

void AOccupationObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AOccupationObject::OnInteractionStart(const float& Time, APawn* Caller)
{
	auto* OccupationPlayerState = Cast<ALakayaBasePlayerState>(Caller->GetController()->PlayerState);
	if (OccupationPlayerState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OccupationObject_OccupationPlayerState is null."));
		return;
	}

	// 소유자 팀에서 상호작용 할 경우 막아두기
	// TODO : IsSameTeam(), 오브젝트에는 PlayerState 없어요
	FString PlayerStateString = UEnum::GetValueAsString(OccupationPlayerState->GetTeam());
	if (PlayerStateString.Equals("EPlayerTeam::A", ESearchCase::IgnoreCase))
	{
		if (ObjectTeam == EPlayerTeam::A)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("이미 점령한 오브젝트 입니다."));
			return;
		}
	}
	else if (PlayerStateString.Equals("EPlayerTeam::B", ESearchCase::IgnoreCase))
	{
		if (ObjectTeam == EPlayerTeam::B)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("이미 점령한 오브젝트 입니다."));
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

	

	// TODO : 점프 도중 상호작용 시, 물리를 무시하고 공중에서 상호작용합니다.
	// 상호작용중에는 움직임을 막아줍니다.
	CharacterImMovable(Caller);
	
	// 시작 한 후 3초가 지나게 되면 성공.
	// TODO : 
	GetWorldTimerManager().SetTimer(InteractionTimerHandle, [this, Caller]
	{
		InteractionSuccess(Caller);
	}, MaxInteractionDuration , false);
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
		// InteractingPawn = Caller;
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

	// InteractionStop 기능에서 타이머가 이미 만료되었는지를 확인해줍니다.
	// 그렇지 않으 경우 타이머를 취소.
	// if (GetWorldTimerManager().IsTimerActive(InteractionTimerHandle))
	// {
	// 	GetWorldTimerManager().ClearTimer(InteractionTimerHandle);
	// }
}

void AOccupationObject::InteractionSuccess(APawn* Caller)
{
	auto* OccupationGameMode = Cast<AOccupationGameMode>(GetWorld()->GetAuthGameMode());
	if (OccupationGameMode == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("InteractionSuccess_OccupationGameMode is null."));
		return;
	}
	
	if (InteractingPawn == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AutomaticInteractionStop_InteractingPawn is null."));
		return;
	}
	
	InteractingPawn = nullptr;

	auto* OccupationPlayerState = Cast<ALakayaBasePlayerState>(Caller->GetController()->PlayerState);
	if (OccupationPlayerState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OccupationObject_OccupationPlayerState is null."));
		return;
	}

	Cast<AInteractableCharacter>(Caller)->SetInteractionState(EInteractionState::Success);
	
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Black, FString::Printf(TEXT("Interaction Successed !")));

	// 상호작용이 끝나게 되면, 상호작용 관련 부분을 초기화 해줍니다.
	OnInteractionFinish(Caller);
	
	// 상호작용을 성공하고 1초 뒤에 State 상태는 Success에서 None 상태로 변경
	// TODO :
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([this, Caller]()
	{
		Cast<AInteractableCharacter>(Caller)->SetInteractionState(EInteractionState::None);
	});
	GetWorldTimerManager().SetTimer(InteractionStateHandle, TimerDelegate, 1.0f, false);
	
	FString PlayerStateString = UEnum::GetValueAsString(OccupationPlayerState->GetTeam());
	if (PlayerStateString.Equals("EPlayerTeam::None", ESearchCase::IgnoreCase))
	{
		UE_LOG(LogTemp, Warning, TEXT("Suspected player captured successfully."));
		return;
	}
	else if (PlayerStateString.Equals("EPlayerTeam::A", ESearchCase::IgnoreCase))
	{
		UE_LOG(LogTemp, Warning, TEXT("Team A player captured successfully."));
	
		if (ObjectTeam == EPlayerTeam::B)
			 OccupationGameMode->SubOccupyObject(EPlayerTeam::B);
	
		SetTeamObject(EPlayerTeam::A);
		OccupationGameMode->AddOccupyObject(EPlayerTeam::A);
		return;
	}
	else if (PlayerStateString.Equals("EPlayerTeam::B", ESearchCase::IgnoreCase))
	{
		UE_LOG(LogTemp, Warning, TEXT("Team B player captured successfully."));
	
		if (ObjectTeam == EPlayerTeam::A)
			OccupationGameMode->SubOccupyObject(EPlayerTeam::A);
			
		SetTeamObject(EPlayerTeam::B);
		OccupationGameMode->AddOccupyObject(EPlayerTeam::B);
		return;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Error ! Error ! Error !"));
		return;
	}
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

void AOccupationObject::OnRep_BroadCastTeamObject()
{
	SetTeamObject(ObjectTeam);
}

void AOccupationObject::SetTeamObject(const EPlayerTeam& Team)
{
	ObjectTeam = Team;
	// UE_LOG(LogTemp, Warning, TEXT("SetTeamObject"));
	// switch (Team)
	// {
	// case EPlayerTeam::A:
	// 	Cylinder->SetMaterial(
	// 		0, LoadObject<UMaterialInterface>(
	// 			nullptr, TEXT("/Game/Characters/LakayaCharacter/Dummy/Materials/RedTeam.RedTeam")));
	// 	break;
	// case EPlayerTeam::B:
	// 	Cylinder->SetMaterial(
	// 		0, LoadObject<UMaterialInterface>(
	// 			nullptr, TEXT("/Game/Characters/LakayaCharacter/Dummy/Materials/BlueTeam.BlueTeam")));
	// 	break;
	// case EPlayerTeam::None:
	// 	break;
	// }
}