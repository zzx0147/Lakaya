#include "Occupation/OccupationObject.h"

#include "Character/InteractableCharacter.h"
#include "Character/LakayaBasePlayerState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameMode/OccupationGameMode.h"
#include "Net/UnrealNetwork.h"

AOccupationObject::AOccupationObject()
{
	//TODO: 취향차이지만, 기본적으로 bCanEverTick은 false로 설정되어있어 꼭 이렇게 해주지 않아도 되긴 합니다.
	PrimaryActorTick.bCanEverTick = false;

	//TODO: 트리거와 메시는 부모 클래스의 멤버변수이므로, 부모클래스에서 생성하도록 하는 것이 좋습니다. 그렇게 해야 트리거와 메시가 항상 nullptr이 아님을 보장할 수 있습니다.
	Trigger = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger Sphere"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	RootComponent = Trigger;
	//TODO: 조금 더 안전하게 루트 컴포넌트를 셋업하려면 아래와 같이 구현하는 것이 좋을 것 같습니다.
	// SetRootComponent(Trigger);
	
	Mesh->SetupAttachment(RootComponent);

	Trigger->InitSphereRadius(600.0f);
	//Trigger->SetupAttachment(RootComponent);
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
	
	// FOnOccupationStateSignature.AddUObject(this, &AOccupationObject::SetTeam);
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
	
	auto OccupationPlayerState = Caller->GetPlayerState<ALakayaBasePlayerState>();
	if (OccupationPlayerState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OccupationObject_OccupationPlayerState is null."));
		return;
	}

	// 소유자 팀에서 상호작용 할 경우 막아두기
	if (OccupationPlayerState->IsSameTeam(ObjectTeam))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("OccupationObject_이미 점령한 오브젝트 입니다."));
		// Cast<AInteractableCharacter>(Caller)->FinishInteraction(EInteractionState::None, Time);
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
			// TODO : 기존에 상호작용중이던 폰이 자신의 인터렉션이 취소되었다는 사실을 알지 못합니다. 따라서 영원히 인터렉션이 종료되지 않습니다.
			// FinishInteraction함수를 통해 인터렉션 상태를 초기화시켜야 합니다.
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
	GetWorldTimerManager().SetTimer(InteractionTimerHandle, [this, Caller, Time]
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

	// 상호작용이 중단됐을 때, 성공했는지, 실패했는지 여부를 전달해줍니다.
	// if (NewState == EInteractionState::Success)
	// 	OnInteractionFinish(Caller);
	
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
		if (ObjectTeam == EPlayerTeam::B)
			OccupationGameMode->SubOccupyObject(EPlayerTeam::B);

		SetTeamObject(EPlayerTeam::A);
		OnOccupationStateSignature.Broadcast(ObjectTeam);
		OccupationGameMode->AddOccupyObject(EPlayerTeam::A);
		// OnRep_BroadCastTeamObject();
	}
	else // 만약 성공한 플레이어 팀이 B팀 이라면
	{
		if (ObjectTeam == EPlayerTeam::A)
			OccupationGameMode->SubOccupyObject(EPlayerTeam::A);

		SetTeamObject(EPlayerTeam::B);
		OnOccupationStateSignature.Broadcast(ObjectTeam);
		OccupationGameMode->AddOccupyObject(EPlayerTeam::B);
		// OnRep_BroadCastTeamObject();
	}
}

void AOccupationObject::OnRep_BroadCastTeamObject()
{
	SetTeamObject(ObjectTeam);
	OnOccupationStateSignature.Broadcast(ObjectTeam);
	// FOnOccupationStateSignature.Broadcast(ObjectTeam);
}

void AOccupationObject::SetTeamObject(const EPlayerTeam& Team)
{
	ObjectTeam = Team;
}