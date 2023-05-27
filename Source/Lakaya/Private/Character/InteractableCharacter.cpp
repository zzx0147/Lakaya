#include "Character/InteractableCharacter.h"
#include "Character/LakayaBasePlayerState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interactable/Interactable.h"
#include "Net/UnrealNetwork.h"
#include "Occupation/OccupationObject.h"

AInteractableCharacter::AInteractableCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	InteractionInfo.InteractionState = EInteractionState::None;
}

void AInteractableCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AInteractableCharacter, InteractionInfo);
}

void AInteractableCharacter::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if (OtherActor->ActorHasTag(TEXT("Interactable")))
	{
		InteractableActor = OtherActor;
		OnInteractableActorChanged.Broadcast(InteractableActor.Get());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NotifyActorBeginOverlap_InteractableActor is null."));
	}
}

void AInteractableCharacter::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	if (OtherActor == InteractableActor)
	{
		InteractableActor = nullptr;
		OnInteractableActorChanged.Broadcast(InteractableActor.Get());
	}
}

bool AInteractableCharacter::ShouldInteract() const
{
	if (!InteractableActor.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("상호작용 가능한 오브젝트 존재하지 않습니다."));
		return false;
	}

	const auto OccupationObject = Cast<AOccupationObject>(InteractableActor);
	if (!OccupationObject)
	{
		UE_LOG(LogTemp, Warning, TEXT("ShouldInteractStart_OccupationObject is null."));
		return false;
	}

	const auto InteractablePlayerState = Cast<ALakayaBasePlayerState>(GetPlayerState());
	if (!InteractablePlayerState)
	{
		UE_LOG(LogTemp, Warning, TEXT("InteractablePlayerState is null."));
		return false;
	}

	if (InteractablePlayerState->GetTeam() == OccupationObject->GetObjectTeam())
	{
		UE_LOG(LogTemp, Warning, TEXT("이미 점령한 포집기 입니다."));
		return false;
	}

	if (OccupationObject->GetInteractingPawn() != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("이미 누군가가 점령을 시도하고 있습니다."));
		return false;
	}

	if (InteractionInfo.InteractionState != EInteractionState::None)
	{
		UE_LOG(LogTemp, Warning, TEXT("InteractionState is not none."));
		return false;
	}

	return true;
}

void AInteractableCharacter::StartInteraction()
{
	UE_LOG(LogTemp, Warning, TEXT("StartInteraction."));

	if (!ShouldInteract())
	{
		UE_LOG(LogTemp, Warning, TEXT("StartInteraction ShouldInteract()"));
		return;
	}

	// bInteractionRequested = true;

	RequestInteractionStart(GetServerTime(), InteractableActor.Get());
}

void AInteractableCharacter::StopInteraction(EInteractionState NewState)
{
	UE_LOG(LogTemp, Warning, TEXT("StopInteraction."));

	if (InteractionInfo.InteractingActor == nullptr) return;

	// if (bInteractionRequested == false)
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("bInteractionRequested is False."));
	// 	return;
	// }
	//
	// bInteractionRequested = false;

	RequestInteractionStop(GetServerTime(), InteractableActor.Get(), NewState);
}

bool AInteractableCharacter::RequestInteractionStart_Validate(const float& Time, AActor* Actor)
{
	if (!ShouldInteract()) return false;

	if (Actor && Actor->ActorHasTag("Interactable") && Time < GetServerTime() + 0.05f)
	{
		return true;
	}

	return false;
}

void AInteractableCharacter::RequestInteractionStart_Implementation(const float& Time, AActor* Actor)
{
	InteractionInfo.InteractingActor = Actor;
	OnInteractingActorChanged.Broadcast(InteractionInfo.InteractingActor.Get());
	InteractionInfo.InteractionState = EInteractionState::OnGoing;
	OnInteractionStateChanged.Broadcast(InteractionInfo);
	GetCharacterMovement()->SetMovementMode(MOVE_None);

	Cast<AInteractable>(Actor)->OnInteractionStart(Time, this);
}

bool AInteractableCharacter::RequestInteractionStop_Validate(const float& Time, AActor* Actor,
                                                             EInteractionState NewState)
{
	if (Actor && Actor->ActorHasTag("Interactable") && Time < GetServerTime() + 0.05f)
	{
		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("RequestInteractionStop_Validate failed."));
	return false;
}

void AInteractableCharacter::RequestInteractionStop_Implementation(const float& Time, AActor* Actor,
                                                                   EInteractionState NewState)
{
	if (!InteractionInfo.InteractingActor.IsValid() ||
		InteractionInfo.InteractionState != EInteractionState::OnGoing)
		return;

	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	Cast<AInteractable>(Actor)->OnInteractionStop(GetServerTime(), this, NewState);
}

bool AInteractableCharacter::FinishInteraction_Validate(EInteractionState NewState, float Time)
{
	return true;
}

void AInteractableCharacter::FinishInteraction_Implementation(EInteractionState NewState, float Time)
{
	InteractionInfo.InteractionState = NewState;
	OnInteractionStateChanged.Broadcast(InteractionInfo);

	if (InteractionInfo.InteractionState == EInteractionState::None)
	{
		InteractionInfo.InteractingActor = nullptr;
		OnInteractingActorChanged.Broadcast(InteractionInfo.InteractingActor.Get());
		return;
	}

	Cast<AInteractable>(InteractionInfo.InteractingActor)->OnInteractionFinish(this);

	InteractionInfo.InteractingActor = nullptr;
	OnInteractingActorChanged.Broadcast(InteractionInfo.InteractingActor.Get());

	// 성공했다면 1초뒤에 None상태로 돌아옵니다.
	if (InteractionInfo.InteractionState == EInteractionState::Success)
	{
		Cast<ALakayaBasePlayerState>(GetPlayerState())->IncreaseSuccessCaptureCount();
		Cast<ALakayaBasePlayerState>(GetPlayerState())->SetTotalScoreCount(500);
		
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([this]()
		{
			InteractionInfo.InteractionState = EInteractionState::None;
			OnInteractionStateChanged.Broadcast(InteractionInfo);
		});
		GetWorldTimerManager().SetTimer(InteractionClearTimer, TimerDelegate, 0.5f, false);
	}
}

void AInteractableCharacter::OnRep_InteractingActor() const
{
	OnInteractingActorChanged.Broadcast(InteractionInfo.InteractingActor.Get());
}