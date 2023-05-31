#include "Occupation/OccupationObject.h"

#include "Character/InteractableCharacter.h"
#include "Character/LakayaBasePlayerState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameMode/OccupationGameMode.h"
#include "Net/UnrealNetwork.h"

AOccupationObject::AOccupationObject(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static const ConstructorHelpers::FObjectFinder<UStaticMesh> SM_Cylinder(
		TEXT("/Game/Dev/MeeLim/antena/White/Antenna.Antenna"));
	
	if (SM_Cylinder.Succeeded())
		Mesh->SetStaticMesh(SM_Cylinder.Object);
	
	bReplicates = true;
}

void AOccupationObject::BeginPlay()
{
	Super::BeginPlay();
	
	OnOccupationStateSignature.AddUObject(this, &AOccupationObject::SetTeam);
}

void AOccupationObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOccupationObject, ObjectTeam);
}

void AOccupationObject::OnInteractionStart(const float& Time, APawn* Caller)
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("Object Interaction Start!"));
	
	if (Caller == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnInteractionStart_Caller is null."));
		return;
	}
	
	const auto OccupationPlayerState = Caller->GetPlayerState<ALakayaBasePlayerState>();
	if (OccupationPlayerState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OccupationObject_OccupationPlayerState is null."));
		return;
	}

	// 소유자 팀에서 상호작용 할 경우 막아두기
	if (OccupationPlayerState->IsSameTeam(ObjectTeam))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("OccupationObject_이미 점령한 오브젝트 입니다."));
		Cast<AInteractableCharacter>(Caller)->StopInteraction(EInteractionState::None);
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
			Cast<AInteractableCharacter>(Caller)->StopInteraction(EInteractionState::None);
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

	// 시작 한 후 3초가 지나게 되면 자동으로 성공.
	GetWorldTimerManager().SetTimer(InteractionTimerHandle, [this, Caller]
	{
		Cast<AInteractableCharacter>(Caller)->StopInteraction(EInteractionState::Success);
	}, MaxInteractionDuration, false);
}

void AOccupationObject::OnInteractionStop(const float& Time, APawn* Caller, EInteractionState NewState)
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("Object Interaction Stop!"));
	
	if (Caller == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnInteractionStop_Caller is null."));
		return;
	}

	// 인터렉션 중단을 요청한 Caller가 가지고 있는 InteractingPawn과 일치한지 검사합니다.
	if (Cast<AInteractableCharacter>(GetInteractingPawn()) != InteractingPawn) return;
	
	InteractingPawn = nullptr;
	FirstCallerTime = 0;
	
	// InteractionStop 기능에서 타이머가 이미 만료되었는지를 확인해줍니다.
	// 그렇지 않은 경우 타이머를 취소.
	if (GetWorldTimerManager().IsTimerActive(InteractionTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(InteractionTimerHandle);
	}

	Cast<AInteractableCharacter>(Caller)->FinishInteraction(NewState, Time);
}

void AOccupationObject::OnCharacterDead(APawn* Caller)
{
	AInteractable::OnCharacterDead(Caller);
	// TODO : 구현해야 함.
}

void AOccupationObject::OnInteractionFinish(APawn* Caller)
{
	const auto OccupationGameMode = Cast<AOccupationGameMode>(GetWorld()->GetAuthGameMode());
	if (OccupationGameMode == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("InteractionSuccess_OccupationGameMode is null."));
		return;
	}

	const auto CallerState = Cast<ALakayaBasePlayerState>(Caller->GetPlayerState());

	// 만약 성공한 플레이어 팀이 A라면
	if (CallerState->IsSameTeam(EPlayerTeam::A))
	{
		// 기존에 이미 다른 팀에서 점령을 했던 상활이라면, 점령했던 플레이어의 현재 점령한 오브젝트 갯수를 줄여줍니다.
		// 다른 팀의 ObjectCount 또한 줄여줍니다.
		// 같은 팀일 경우는 InteractableCharacter에서 막아줬기에 예외처리를 따로 하지 않아도 됩니다.
		if (OwnerPlayer != nullptr)
		{
			Cast<ALakayaBasePlayerState>(OwnerPlayer->GetPlayerState())->DecreaseCurrentCaptureCount();
			OccupationGameMode->SubOccupyObject(EPlayerTeam::B);
		}
		
		SetTeamObject(EPlayerTeam::A);
		OnOccupationStateSignature.Broadcast(ObjectTeam);
		OccupationGameMode->AddOccupyObject(EPlayerTeam::A);
		OwnerPlayer = Caller;
		Cast<ALakayaBasePlayerState>(OwnerPlayer->GetPlayerState())->IncreaseCurrentCaptureCount();

		
	}
	else // 만약 성공한 플레이어 팀이 B팀 이라면
	{
		// 기존에 이미 다른 팀에서 점령을 했던 상화이라면, 점령했떤 플레이어의 현재 점령한 오브젝트 갯수를 줄여줍니다.
		// 다른 팀의 ObjectCount 또한 줄여줍니다.
		// 같음 팀일 경우는 InteractableCharacter에서 막아줬기에 예외처리를 따로 하지 않아도 됩니다.
		if (OwnerPlayer != nullptr)
		{
			Cast<ALakayaBasePlayerState>(OwnerPlayer->GetPlayerState())->DecreaseCurrentCaptureCount();
			OccupationGameMode->SubOccupyObject(EPlayerTeam::A);
		}
		
		SetTeamObject(EPlayerTeam::B);
		OnOccupationStateSignature.Broadcast(ObjectTeam);
		OccupationGameMode->AddOccupyObject(EPlayerTeam::B);
		OwnerPlayer = Caller;
		Cast<ALakayaBasePlayerState>(OwnerPlayer->GetPlayerState())->IncreaseCurrentCaptureCount();
	}
}

void AOccupationObject::OnRep_BroadCastTeamObject()
{
	SetTeamObject(ObjectTeam);
	OnOccupationStateSignature.Broadcast(ObjectTeam);
}

void AOccupationObject::SetTeamObject(const EPlayerTeam& Team)
{
	ObjectTeam = Team;
}