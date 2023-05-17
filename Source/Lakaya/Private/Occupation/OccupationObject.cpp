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

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_Cylinder(
		TEXT("/Game/Dev/MeeLim/antena/White/Antenna.Antenna"));
	
	if (SM_Cylinder.Succeeded())
		Mesh->SetStaticMesh(SM_Cylinder.Object);

	bReplicates = true;
}

void AOccupationObject::BeginPlay()
{
	Super::BeginPlay();
	FOnOccupationStateSignature.AddUObject(this,&AOccupationObject::SetTeam);
}

void AOccupationObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOccupationObject, ObjectTeam);
}

void AOccupationObject::OnInteractionStart(const float& Time, APawn* Caller)
{
	//TODO: Caller에 접근하기 전에 혹시 Caller가 널은 아닌지 체크해보는 것이 좋습니다.
	auto* OccupationPlayerState = Cast<ALakayaBasePlayerState>(Caller->GetController()->PlayerState);
	//TODO: 다음과 같이 더 간단하게 선언할 수 있습니다.
	// auto* OccupationPlayerState = Caller->GetPlayerState<ALakayaBasePlayerState>();
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
			//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("OccupationObject_이미 점령한 오브젝트 입니다."));
			return;
		}
	}
	else if (PlayerStateString.Equals("EPlayerTeam::B", ESearchCase::IgnoreCase))
	{
		if (ObjectTeam == EPlayerTeam::B)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("OccupationObject_이미 점령한 오브젝트 입니다."));
			return;
		}
	}
	//TODO: 위의 코드는 점령한 팀과 점령을 시도하는 플레이어의 팀이 같다면 점령이 진행되지 않도록 합니다. 따라서 그냥 다음과 같이 간단하게 코드를 정리할 수 있습니다.
	// if (OccupationPlayerState->IsSameTeam(ObjectTeam))
	// {
	// 	//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("OccupationObject_이미 점령한 오브젝트 입니다."));
	// 	return;
	// }
	// 그런데 이렇게만 하면 점령을 시도한 플레이어가 점령을 시도했지만 점령이 취소되지 않았으므로 영원히 인터렉션이 끝나지 않게 됩니다. 따라서 캐릭터의 인터렉션 상태를 초기화시켜야 합니다.
	// 인터렉션 초기화는 AInteractableCharacter::FinishInteraction 함수를 통해 이뤄집니다. UML을 참고해주세요.
	
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
	//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("Object Interaction Stop!"));

	//TODO: 인터렉션 중단을 요청한 Caller가 기존에 인터렉션중이던 InteractingPawn이 맞는지 검사하는 로직이 필요합니다. 그렇지 않고 위의 코드처검 그냥 초기화되면 다른 플레이어어에 의해 인터렉션이 취소당하는 어이없는 일이 발생할 수 있습니다.
	InteractingPawn = nullptr;
	FirstCallerTime = 0;
	
	// 상호작용이 끝나게 되면, 상호작용 관련 부분을 초기화 해줍니다.
	OnInteractionFinish(Caller);
	//TODO: 이 함수는 여기에서 호출되도록 기획하지 않았습니다. UML을 참고해주세요. OnInteractionFinish함수는 캐릭터가 호출해주는 함수입니다.
	
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
		//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("Object Interaction Start!"));
		CastedCaller->GetCharacterMovement()->DisableMovement();
		// InteractingPawn = Caller;
		//TODO: CharacterMovementComponent는 ACharacter에 있으므로 AInteractableCharacter로 형변환할 필요는 없습니다. 또 그냥 함수의 매개변수 형식이 ACharacter면 캐스팅할 필요도 없어집니다.
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
		//TODO: CharacterMovementComponent는 ACharacter에 있으므로 AInteractableCharacter로 형변환할 필요는 없습니다. 또 그냥 함수의 매개변수 형식이 ACharacter면 캐스팅할 필요도 없어집니다.
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
	
	//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Black, FString::Printf(TEXT("Interaction Successed !")));

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
		OnRep_BroadCastTeamObject();
		return;
	}
	else if (PlayerStateString.Equals("EPlayerTeam::B", ESearchCase::IgnoreCase))
	{
		UE_LOG(LogTemp, Warning, TEXT("Team B player captured successfully."));
	
		if (ObjectTeam == EPlayerTeam::A)
			OccupationGameMode->SubOccupyObject(EPlayerTeam::A);
			
		SetTeamObject(EPlayerTeam::B);
		OccupationGameMode->AddOccupyObject(EPlayerTeam::B);
		OnRep_BroadCastTeamObject();
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
	// InteractingActor = nullptr;
}

void AOccupationObject::OnRep_BroadCastTeamObject()
{
	SetTeamObject(ObjectTeam);
	FOnOccupationStateSignature.Broadcast(ObjectTeam);
}

void AOccupationObject::SetTeamObject(const EPlayerTeam& Team)
{
	ObjectTeam = Team;
	// OnRep_BroadCastTeamObject();
}