// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LakayaBasePlayerState.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Character/LakayaBaseCharacter.h"
#include "Components/Image.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "UI/DirectionalDamageIndicator.h"
#include "UI/GamePlayHealthWidget.h"

void ALakayaBasePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALakayaBasePlayerState, Health);
	DOREPLIFETIME(ALakayaBasePlayerState, Team);
	DOREPLIFETIME(ALakayaBasePlayerState, RespawnTime);
	DOREPLIFETIME(ALakayaBasePlayerState, CharacterName);
	// DOREPLIFETIME(ALakayaBasePlayerState, ScoreCount);
	DOREPLIFETIME(ALakayaBasePlayerState, DeathCount);
	DOREPLIFETIME(ALakayaBasePlayerState, KillCount);
	DOREPLIFETIME(ALakayaBasePlayerState, KillStreak);
}

ALakayaBasePlayerState::ALakayaBasePlayerState()
{
	CharacterName = TEXT("Rena");
	bRecentAliveState = true;

	static ConstructorHelpers::FClassFinder<UGamePlayHealthWidget> HealthFinder(
		TEXT("/Game/Blueprints/UMG/WBP_GamePlayHealthWidget"));
	static ConstructorHelpers::FClassFinder<UDirectionalDamageIndicator> DirectionDamageFinder(
		TEXT("/Game/Blueprints/UMG/WBP_DirectionalDamageIndicator"));

	if (HealthFinder.Succeeded()) HealthWidgetClass = HealthFinder.Class;
	if (DirectionDamageFinder.Succeeded()) DirectionDamageIndicatorClass = DirectionDamageFinder.Class;
}

void ALakayaBasePlayerState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	OnPawnSet.AddUniqueDynamic(this, &ALakayaBasePlayerState::OnPawnSetCallback);
}

float ALakayaBasePlayerState::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                                         AController* EventInstigator, AActor* DamageCauser)
{
	if (!ShouldTakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser)) return 0.f;
	const auto Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (Damage == 0.f) return 0.f;
	Health -= Damage;

	// 힐이고, 최대체력을 초과한 경우 최대체력으로 맞춰줍니다.
	if (Damage < 0.f)
		if (const auto MaxHealth = GetMaxHealth(); Health > MaxHealth)
			Health = MaxHealth;

	OnHealthChanged.Broadcast(Health);
	NoticePlayerHit(*DamageCauser->GetName(), DamageCauser->GetActorLocation(), Damage);
	if (Health <= 0.f) OnPlayerKilled.Broadcast(GetOwningController(), EventInstigator, DamageCauser);

	return Damage;
}

void ALakayaBasePlayerState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();
	OnPlayerNameChanged.Broadcast(GetPlayerName());
}

void ALakayaBasePlayerState::BeginPlay()
{
	Super::BeginPlay();
	if (const auto LocalController = Cast<APlayerController>(GetOwningController());
		LocalController && LocalController->IsLocalController())
	{
		HealthWidget = CreateWidget<UGamePlayHealthWidget>(LocalController, HealthWidgetClass);
		if (HealthWidget.IsValid())
		{
			HealthWidget->AddToViewport();
			HealthWidget->SetVisibility(ESlateVisibility::Hidden);
			
			OnHealthChanged.AddUObject(HealthWidget.Get(), &UGamePlayHealthWidget::SetCurrentHealth);
			OnMaxHealthChanged.AddUObject(HealthWidget.Get(), &UGamePlayHealthWidget::SetMaximumHealth);

			HealthWidget->SetMaximumHealth(GetMaxHealth());
			HealthWidget->SetCurrentHealth(Health);
		}

		DirectionDamageIndicatorWidget = CreateWidget<UDirectionalDamageIndicator>(
			LocalController, DirectionDamageIndicatorClass);
		if (DirectionDamageIndicatorWidget == nullptr)
		{
			UE_LOG(LogTemp, Warning,
			       TEXT("LakayaBasePlayerState_PreInitializeComponents DirectionDamageIndicatorWidget is null."));
			return;
		}

		DirectionDamageIndicatorWidget->AddToViewport();
	}
}

void ALakayaBasePlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);
	if (const auto Other = Cast<ALakayaBasePlayerState>(PlayerState))
	{
		Other->Health = Health;
		Other->Team = Team;
		Other->RespawnTime = RespawnTime;
		Other->CharacterName = CharacterName;
		// Other->ScoreCount = ScoreCount;
		Other->DeathCount = DeathCount;
		Other->KillCount = KillCount;
	}
}

void ALakayaBasePlayerState::OnRep_Owner()
{
	Super::OnRep_Owner();
	OnOwnerChanged.Broadcast(Owner);
}

bool ALakayaBasePlayerState::IsSameTeam(const ALakayaBasePlayerState* Other) const
{
	return Other && IsSameTeam(Other->Team);
}

bool ALakayaBasePlayerState::IsSameTeam(const EPlayerTeam& Other) const
{
	return JudgeSameTeam(Team, Other);
}

void ALakayaBasePlayerState::SetTeam(const EPlayerTeam& DesireTeam)
{
	if (Team == DesireTeam) return;
	Team = DesireTeam;
	if (const auto Character = GetPawn<ALakayaBaseCharacter>()) Character->SetTeam(Team);
	OnTeamChanged.Broadcast(Team);
}

void ALakayaBasePlayerState::MakeAlive()
{
	RespawnTime = 0.f;
	SetAliveState(true);
}

const uint16& ALakayaBasePlayerState::SetScoreCount(const uint16& NewScore)
{
	ScoreCount = NewScore;
	OnRep_ScoreCount();
	return ScoreCount;
}

// void ALakayaBasePlayerState::IncreaseScoreCount()
// {
// 	// TODO :
// 	OnScoreCountChanged.Broadcast(ScoreCount);
// }

void ALakayaBasePlayerState::IncreaseSuccessCaptureCount()
{
	OnSuccessCaptureCountChanged.Broadcast(++SuccessCaptureCount);
}

void ALakayaBasePlayerState::IncreaseCurrentCaptureCount()
{
	OnCurrentCaptureCountChanged.Broadcast(++CurrentCaptureCount);
}

void ALakayaBasePlayerState::DecreaseCurrentCaptureCount()
{
	OnCurrentCaptureCountChanged.Broadcast(--CurrentCaptureCount);
}

void ALakayaBasePlayerState::IncreaseDeathCount()
{
	OnDeathCountChanged.Broadcast(++DeathCount);
}

void ALakayaBasePlayerState::IncreaseKillCount()
{
	OnKillCountChanged.Broadcast(++KillCount);
}

void ALakayaBasePlayerState::IncreaseKillStreak()
{
	OnKillStreakChanged.Broadcast(++KillStreak);
}

void ALakayaBasePlayerState::ResetKillStreak()
{
	// 변경될 필요가 없다면 리턴합니다.
	if (KillStreak == 0) return;
	KillStreak = 0;
	OnKillStreakChanged.Broadcast(KillStreak);
}

float ALakayaBasePlayerState::GetServerTime() const
{
	return GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
}

void ALakayaBasePlayerState::BroadcastMaxHealthChanged() const
{
	OnMaxHealthChanged.Broadcast(GetMaxHealth());
}

bool ALakayaBasePlayerState::ShouldTakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                                              AController* EventInstigator, AActor* DamageCauser)
{
	// 플레이어가 이미 사망한 상태인 경우 데미지를 받지 않습니다.
	if (!IsAlive()) return false;

	// EventInstigator가 nullptr인 경우 글로벌 데미지이거나 어떤 정의할 수 없는 데미지이지만 일단 받아야하는 데미지라고 판단합니다.
	if (!EventInstigator) return true;

	// 데미지가 피해인 경우 다른 팀인 경우에만 받고, 데미지가 힐인 경우 같은 팀인 경우에만 받습니다.
	const auto Other = EventInstigator->GetPlayerState<ALakayaBasePlayerState>();
	return (DamageAmount > 0.f && !IsSameTeam(Other)) || (DamageAmount < 0.f && IsSameTeam(Other));
}

void ALakayaBasePlayerState::OnPawnSetCallback(APlayerState* Player, APawn* NewPawn, APawn* OldPawn)
{
	if (const auto OldCharacter = Cast<ALakayaBaseCharacter>(OldPawn))
	{
		OldCharacter->SetTeam(EPlayerTeam::None);
		OnAliveStateChanged.RemoveAll(OldCharacter);
	}

	if (const auto Character = Cast<ALakayaBaseCharacter>(NewPawn))
	{
		if (Team != EPlayerTeam::None) Character->SetTeam(Team);
		if (HealthWidget.IsValid())
		{
			HealthWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			
			if (GetCharacterName().ToString() == "Rena")
				HealthWidget->UserInfoCharImageRena->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			if (GetCharacterName().ToString() == "Wazi")
				HealthWidget->UserInfoCharImageWazi->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		
		OnAliveStateChanged.AddUObject(Character, &ALakayaBaseCharacter::SetAliveState);
	}
	else
	{
		if (HealthWidget.IsValid()) HealthWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	BroadcastMaxHealthChanged();

	if (HasAuthority())
	{
		// 캐릭터가 변경된 경우 그 캐릭터에 맞는 체력으로 재설정합니다.
		Health = GetMaxHealth();
		OnHealthChanged.Broadcast(Health);
	}
}

float ALakayaBasePlayerState::GetMaxHealth() const
{
	if (const auto Character = GetPawn<ALakayaBaseCharacter>()) return Character->GetCharacterMaxHealth();
	return 0.f;
}

bool ALakayaBasePlayerState::ShouldChangeCharacterName(const FName& Name)
{
	//TODO: 캐릭터 이름 변경 가능 조건을 작성합니다.
	return true;
}

void ALakayaBasePlayerState::OnRep_Health()
{
	OnHealthChanged.Broadcast(Health);
}

void ALakayaBasePlayerState::OnRep_Team()
{
	if (const auto Character = GetPawn<ALakayaBaseCharacter>()) Character->SetTeam(Team);
	OnTeamChanged.Broadcast(Team);
}

void ALakayaBasePlayerState::OnRep_RespawnTime()
{
	const auto CurrentTime = GetServerTime();
	UpdateAliveStateWithRespawnTime(CurrentTime);

	// 부활시간에 OnAliveStateChanged 이벤트가 호출될 수 있도록 타이머를 설정합니다.
	GetWorldTimerManager().SetTimer(RespawnTimer, [this] { SetAliveState(true); },
	                                RespawnTime - CurrentTime, false);
}

void ALakayaBasePlayerState::OnRep_CharacterName()
{
	OnCharacterNameChanged.Broadcast(this, CharacterName);
}

void ALakayaBasePlayerState::OnRep_ScoreCount()
{
	OnScoreCountChanged.Broadcast(ScoreCount);
}

void ALakayaBasePlayerState::OnRep_CurrentCaptureCount()
{
	OnCurrentCaptureCountChanged.Broadcast(CurrentCaptureCount);
}

void ALakayaBasePlayerState::OnRep_SuccessCaptureCount()
{
	OnSuccessCaptureCountChanged.Broadcast(SuccessCaptureCount);
}

void ALakayaBasePlayerState::OnRep_DeathCount()
{
	OnKillCountChanged.Broadcast(KillCount);
}

void ALakayaBasePlayerState::OnRep_KillCount()
{
	OnDeathCountChanged.Broadcast(DeathCount);
}

void ALakayaBasePlayerState::OnRep_KillStreak()
{
	OnKillStreakChanged.Broadcast(KillStreak);
}

void ALakayaBasePlayerState::UpdateAliveStateWithRespawnTime(const float& CurrentTime)
{
	SetAliveState(RespawnTime >= 0.f && RespawnTime < CurrentTime);
}

void ALakayaBasePlayerState::SetAliveState(const bool& AliveState)
{
	if (bRecentAliveState == AliveState) return;
	bRecentAliveState = AliveState;
	OnAliveStateChanged.Broadcast(AliveState);
}

void ALakayaBasePlayerState::RequestCharacterChange_Implementation(const FName& Name)
{
	if (!ShouldChangeCharacterName(Name)) return;
	CharacterName = Name;
	OnCharacterNameChanged.Broadcast(this, CharacterName);
}

bool ALakayaBasePlayerState::RequestCharacterChange_Validate(const FName& Name)
{
	//TODO: 캐릭터 이름이 유효한 키값인지 검사합니다.
	return true;
}

void ALakayaBasePlayerState::NoticePlayerHit_Implementation(const FName& CauserName, const FVector& CauserLocation,
                                                            const float& Damage)
{
	const auto PlayerController = GetPlayerController();
	if(!PlayerController) return;
	
	if (PlayerController->IsLocalPlayerController())
	{
		//TODO: 위젯 nullptr 체크 필요, 매개변수 하드코딩 수정 필요
		DirectionDamageIndicatorWidget->IndicateStart(CauserName.ToString(), CauserLocation, 3.0f);

		Cast<ALakayaBaseCharacter>(PlayerController->GetCharacter())->PlayHitScreen();
	}
}

void ALakayaBasePlayerState::SetOwner(AActor* NewOwner)
{
	Super::SetOwner(NewOwner);
	OnOwnerChanged.Broadcast(Owner);
}