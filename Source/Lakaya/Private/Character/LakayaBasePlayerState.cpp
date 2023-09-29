// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LakayaBasePlayerState.h"

#include "AbilitySystemComponent.h"
#include "Character/LakayaBaseCharacter.h"
#include "Character/Ability/Attribute/LakayaAttributeSet.h"
#include "Character/Ability/Component/LakayaAbilitySystemComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "UI/DirectionalDamageIndicator.h"
#include "UI/GamePlayHealthWidget.h"
#include "UI/GamePlayPortraitWidget.h"
#include "UI/OccupationMinimapWidget.h"
#include "UI/SkillWidget.h"

void ALakayaBasePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALakayaBasePlayerState, Health);
	DOREPLIFETIME(ALakayaBasePlayerState, Team);
	DOREPLIFETIME(ALakayaBasePlayerState, RespawnTime);
	DOREPLIFETIME(ALakayaBasePlayerState, CharacterName);
	DOREPLIFETIME(ALakayaBasePlayerState, TotalScore);
	DOREPLIFETIME(ALakayaBasePlayerState, SuccessCaptureCount);
	DOREPLIFETIME(ALakayaBasePlayerState, DeathCount);
	DOREPLIFETIME(ALakayaBasePlayerState, KillCount);
	DOREPLIFETIME(ALakayaBasePlayerState, KillStreak);
}

ALakayaBasePlayerState::ALakayaBasePlayerState()
{
	OccupationTickCount = 0;
	CharacterName = TEXT("Rena");
	bRecentAliveState = true;
	OnPawnSet.AddUniqueDynamic(this, &ALakayaBasePlayerState::OnPawnSetCallback);
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FClassFinder<UGamePlayHealthWidget> HealthFinder(
		TEXT("/Game/Blueprints/UMG/WBP_GamePlayHealthWidget"));

	static ConstructorHelpers::FClassFinder<UDirectionalDamageIndicator> DirectionDamageFinder(
		TEXT("/Game/Blueprints/UMG/WBP_DirectionalDamageIndicator"));

	static ConstructorHelpers::FClassFinder<UGamePlayPortraitWidget> PortraitFinder(
		TEXT("/Game/Blueprints/UMG/WBP_Portrait"));

	HealthWidgetClass = HealthFinder.Class;
	DirectionDamageIndicatorClass = DirectionDamageFinder.Class;
	PortraitWidgetClass = PortraitFinder.Class;

	AbilitySystem = CreateDefaultSubobject<ULakayaAbilitySystemComponent>(TEXT("AbilitySystem"));

	LakayaAttributeSet = CreateDefaultSubobject<ULakayaAttributeSet>(TEXT("LakayaAttributeSet"));
}

float ALakayaBasePlayerState::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                                         AController* EventInstigator, AActor* DamageCauser)
{
	if (!ShouldTakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser)) return 0.f;
	const auto Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (Damage == 0.f) return 0.f;

	Health -= Damage;
	const auto IsDead = Health <= 0.f;
	Health = FMath::Clamp(Health, 0, GetMaxHealth());
	OnHealthChanged.Broadcast(Health);

	if (Damage > 0.f) NoticePlayerHit(*DamageCauser->GetName(), DamageCauser->GetActorLocation(), Damage);
	if (IsDead) OnPlayerKilled.Broadcast(GetOwningController(), EventInstigator, DamageCauser);

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
		Other->TotalScore = TotalScore;
		Other->DeathCount = DeathCount;
		Other->KillCount = KillCount;
	}
}

void ALakayaBasePlayerState::OnRep_Owner()
{
	Super::OnRep_Owner();
	OnOwnerChanged.Broadcast(Owner);

	if (const auto LocalController = GetPlayerController(); LocalController && LocalController->IsLocalController())
	{
		PortraitWidget = CreateWidget<UGamePlayPortraitWidget>(LocalController, PortraitWidgetClass);
		if (PortraitWidget.IsValid())
		{
			PortraitWidget->AddToViewport(1);
			PortraitWidget->ChangePortrait(GetCharacterName());
			OnCharacterNameChanged.AddWeakLambda(
				PortraitWidget.Get(), [Widget = PortraitWidget](auto, const FName& Name)
				{
					Widget->ChangePortrait(Name);
				});
		}

		HealthWidget = CreateWidget<UGamePlayHealthWidget>(LocalController, HealthWidgetClass);
		if (HealthWidget.IsValid())
		{
			HealthWidget->AddToViewport();

			OnHealthChanged.AddUObject(HealthWidget.Get(), &UGamePlayHealthWidget::SetCurrentHealth);
			OnMaxHealthChanged.AddUObject(HealthWidget.Get(), &UGamePlayHealthWidget::SetMaximumHealth);

			HealthWidget->SetMaximumHealth(GetMaxHealth());
			HealthWidget->SetCurrentHealth(Health);
		}

		SkillWidget = CreateWidget<USkillWidget>(LocalController, SkillWidgetClass);
		if (SkillWidget)
		{
			SkillWidget->AddToViewport();
			OnCharacterNameChanged.AddWeakLambda(
				SkillWidget.Get(), [Widget = SkillWidget](auto, const FName& Name)
				{
					Widget->SetCharacter(Name);
				});
		}
	}
}

void ALakayaBasePlayerState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// UAbilitySystemComponent* ASC = GetAbilitySystemComponent();

	// if (GetAbilitySystemComponent())
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
	// 		                                 *FString::Printf(TEXT("::%f"), GetAbilitySystemComponent()->GetSet<ULakayaAttributeSet>()->GetHealth() ));
	// }
	//

	// TODO : 미니맵 방식은 RenderTarget -> 정적 이미지 으로 바뀌었습니다.
	// if (!HasInitalizedPawn && GetPlayerController() && GetPlayerController()->GetPawn())
	// {
	// HasInitalizedPawn = true;
	// InitalizeWithPawn();
	// }
}

void ALakayaBasePlayerState::SetTeam(const ETeam& DesireTeam)
{
	if (Team == DesireTeam) return;
	Team = DesireTeam;
	if (const auto Character = GetPawn<ALakayaBaseCharacter>()) Character->SetTeam(Team);
	OnTeamChanged.Broadcast(Team);
}

void ALakayaBasePlayerState::SetRespawnTimer(const float& ReservedRespawnTime, const FRespawnTimerDelegate& Callback)
{
	RespawnTime = ReservedRespawnTime;
	const auto CurrentTime = GetServerTime();
	UpdateAliveStateWithRespawnTime(CurrentTime);

	if (ReservedRespawnTime < CurrentTime) return;
	static FTimerDelegate TimerDelegate;
	TimerDelegate.BindUObject(this, &ALakayaBasePlayerState::RespawnTimerCallback, Callback);
	GetWorld()->GetTimerManager().SetTimer(RespawnTimer, TimerDelegate, ReservedRespawnTime - CurrentTime, false);
}

void ALakayaBasePlayerState::MakeAlive()
{
	RespawnTime = 0.f;
	SetAliveState(true);
}

const uint16& ALakayaBasePlayerState::AddTotalScoreCount(const uint16& NewScore)
{
	TotalScore += NewScore;
	OnRep_TotalScore();
	return TotalScore;
}

void ALakayaBasePlayerState::IncreaseSuccessCaptureCount()
{
	OnSuccessCaptureCountChanged.Broadcast(++SuccessCaptureCount);
}

void ALakayaBasePlayerState::IncreaseCurrentCaptureCount()
{
	OnCurrentCaptureCountChanged.Broadcast(++CurrentCaptureCount);
	CheckCurrentCaptureCount();
}

void ALakayaBasePlayerState::DecreaseCurrentCaptureCount()
{
	OnCurrentCaptureCountChanged.Broadcast(--CurrentCaptureCount);
	CheckCurrentCaptureCount();
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

void ALakayaBasePlayerState::CheckCurrentCaptureCount()
{
	auto& TimerManager = GetWorldTimerManager();
	if (CurrentCaptureCount == 0)
	{
		if (TimerManager.IsTimerActive(CurrentCaptureTimer))
			TimerManager.ClearTimer(CurrentCaptureTimer);
	}

	if (CurrentCaptureCount == 1)
	{
		if (TimerManager.IsTimerActive(CurrentCaptureTimer))
			TimerManager.ClearTimer(CurrentCaptureTimer);

		FTimerDelegate TimerDelegate;
		TimerDelegate.BindWeakLambda(this, [this]
		{
			if (this == nullptr) return;
			if (const auto World = GetWorld())
			{
				if (const auto GameState = World->GetGameState())
				{
					if (GameState->HasMatchEnded())
					{
						World->GetTimerManager().ClearTimer(CurrentCaptureTimer);
						return;
					}
				}
			}

			AddTotalScoreCount(CurrentCaptureCount * 50);
			OccupationTickCount += CurrentCaptureCount;
		});

		TimerManager.SetTimer(CurrentCaptureTimer, TimerDelegate, 1.0f, true);
	}
}

void ALakayaBasePlayerState::SetUniqueStencilMask(const ERendererStencilMask& StencilMask)
{
	UniqueRenderMask = StencilMask;
	if (const auto Character = GetPawn<ALakayaBaseCharacter>()) Character->SetStencilMask(UniqueRenderMask);
}

void ALakayaBasePlayerState::SetAlly(const bool& Ally)
{
	bIsAlly = Ally;
	if (const auto Character = GetPawn<ALakayaBaseCharacter>()) Character->SetAlly(bIsAlly);
}

FPlayerStats ALakayaBasePlayerState::GetPlayerStats()
{
	FPlayerStats MyStats;
	MyStats.Kill = GetKillCount();
	MyStats.Death = GetDeathCount();
	MyStats.PlayerName = GetPlayerName();
	MyStats.PlayerID = GetUniqueId().ToString();
	MyStats.OccupationCount = GetSuccessCaptureCount();
	MyStats.OccupationTickCount = OccupationTickCount;
	return MyStats;
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
	if (!IsAlive() || !HasAuthority()) return false;

	// EventInstigator가 nullptr인 경우 글로벌 데미지이거나 어떤 정의할 수 없는 데미지이지만 일단 받아야하는 데미지라고 판단합니다.
	if (!EventInstigator) return true;

	// 데미지가 피해인 경우 다른 팀인 경우에만 받고, 데미지가 힐인 경우 같은 팀인 경우에만 받습니다.
	const auto Other = EventInstigator->GetPlayerState<ALakayaBasePlayerState>();
	return (DamageAmount > 0.f && !IsSameTeam(Other)) || (DamageAmount < 0.f && IsSameTeam(Other));
}

void ALakayaBasePlayerState::InitializeStatus()
{
	if (const auto Character = GetPawn<ALakayaBaseCharacter>())
	{
		FGameplayEffectSpecHandle SpecHandle = AbilitySystem->MakeOutgoingSpec(
			StatusInitializeEffect, 0, AbilitySystem->MakeEffectContext());
		SpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(TEXT("Stat.MaxHealth")),
		                                               Character->GetCharacterMaxHealth());
		SpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(TEXT("Stat.MaxAmmo")),
		                                               Character->GetCharacterMaxAmmo());
		SpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(TEXT("Stat.AttackPoint")),
		                                               Character->GetCharacterAttackPoint());
		SpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(TEXT("Stat.MaxSkillStack")),
		                                               Character->GetCharacterMaxSkillStack());
		AbilitySystem->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void ALakayaBasePlayerState::OnPawnSetCallback(APlayerState* Player, APawn* NewPawn, APawn* OldPawn)
{
	if (const auto OldCharacter = Cast<ALakayaBaseCharacter>(OldPawn))
	{
		OldCharacter->SetTeam(ETeam::None);
		OnAliveStateChanged.RemoveAll(OldCharacter);
	}

	if (const auto OldAbilityInterface = Cast<IRegisterAbilityInterface>(OldPawn);
		HasAuthority() && OldAbilityInterface)
	{
		OldAbilityInterface->ClearAbilities();
	}

	if (HasAuthority()) AbilitySystem->SetAvatarActor(NewPawn);

	if (const auto NewAbilityInterface = Cast<IRegisterAbilityInterface>(NewPawn);
		HasAuthority() && NewAbilityInterface)
	{
		NewAbilityInterface->GiveAbilities(AbilitySystem);
	}

	if (const auto Character = Cast<ALakayaBaseCharacter>(NewPawn))
	{
		if (Team != ETeam::None) Character->SetTeam(Team);
		if (HealthWidget.IsValid()) HealthWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		OnAliveStateChanged.AddUObject(Character, &ALakayaBaseCharacter::SetAliveState);
		Character->SetStencilMask(UniqueRenderMask);
		Character->SetAlly(bIsAlly);
		
		if(SkillWidget)
		{
			SkillWidget->SetCharacter(Character->GetCharacterName());
		}
	}
	else
	{
		if (HealthWidget.IsValid()) HealthWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	BroadcastMaxHealthChanged();

	if (HealthWidget.IsValid())
	{
		AbilitySystem->GetGameplayAttributeValueChangeDelegate(LakayaAttributeSet->GetHealthAttribute()).AddUObject(
			HealthWidget.Get(), &UGamePlayHealthWidget::SetCurrentHealthAttribute);
		AbilitySystem->GetGameplayAttributeValueChangeDelegate(LakayaAttributeSet->GetMaxHealthAttribute()).AddUObject(
			HealthWidget.Get(), &UGamePlayHealthWidget::SetMaximumHealthAttribute);
		// LakayaAttributeSet->OnHealthChanged.AddUObject(HealthWidget.Get(),&UGamePlayHealthWidget::SetCurrentHealth);
		// LakayaAttributeSet->OnMaxHealthChanged.AddUObject(HealthWidget.Get(),&UGamePlayHealthWidget::SetMaximumHealth);
	}

	if (HasAuthority())
	{
		InitializeStatus();
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
	static FTimerDelegate Delegate;
	Delegate.BindUObject(this, &ALakayaBasePlayerState::SetAliveState, true);
	GetWorldTimerManager().SetTimer(RespawnTimer, Delegate, RespawnTime - CurrentTime, false);
}

void ALakayaBasePlayerState::OnRep_CharacterName()
{
	OnCharacterNameChanged.Broadcast(this, CharacterName);
}

void ALakayaBasePlayerState::OnRep_TotalScore()
{
	OnTotalScoreChanged.Broadcast(TotalScore);
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
	OnDeathCountChanged.Broadcast(DeathCount);
}

void ALakayaBasePlayerState::OnRep_KillCount()
{
	OnKillCountChanged.Broadcast(KillCount);
}

void ALakayaBasePlayerState::OnRep_KillStreak()
{
	OnKillStreakChanged.Broadcast(KillStreak);
}

void ALakayaBasePlayerState::UpdateAliveStateWithRespawnTime(const float& CurrentTime)
{
	SetAliveState(RespawnTime >= 0.f && RespawnTime < CurrentTime);
}

void ALakayaBasePlayerState::SetAliveState(bool AliveState)
{
	if (bRecentAliveState == AliveState) return;
	bRecentAliveState = AliveState;
	OnAliveStateChanged.Broadcast(AliveState);
}

void ALakayaBasePlayerState::RespawnTimerCallback(FRespawnTimerDelegate Callback)
{
	SetAliveState(true);
	Callback.Execute(GetOwningController());
}

// void ALakayaBasePlayerState::InitalizeWithPawn()
// {
// 	DirectionDamageIndicatorWidget = CreateWidget<UDirectionalDamageIndicator>(
// 		LocalController, DirectionDamageIndicatorClass);
// 	if (DirectionDamageIndicatorWidget) DirectionDamageIndicatorWidget->AddToViewport();
//
// 	OccupationMinimapWidget = CreateWidget<UOccupationMinimapWidget>(LocalController, OccupationMinimapWidgetClass);
// 	if (OccupationMinimapWidget.IsValid())
// 	{
// 		OccupationMinimapWidget->AddToViewport();
// 	}
//
// 	FVector PlayerLocation = LocalController->GetPawn()->GetActorLocation();
// 	FVector CaptureLocation = PlayerLocation + FVector(0.0f, 0.0f, 500.0f);
// 	FRotator CaptureRotation = FRotator(-90.0f, 0.0f, 0.0f);
// 	
// 	USceneCaptureComponent2D* MiniMapCapture = NewObject<USceneCaptureComponent2D>(this);
// 	MiniMapCapture->SetupAttachment(LocalController->GetPawn()->GetRootComponent());
// 	
// 	MiniMapCapture->SetWorldLocation(CaptureLocation);
// 	MiniMapCapture->SetWorldRotation(CaptureRotation);
// 	MiniMapCapture->ProjectionType = ECameraProjectionMode::Orthographic;
// 	MiniMapCapture->OrthoWidth = 1000;
//
// 	MiniMapCapture->CaptureScene();
// 	MiniMapCapture->RegisterComponent();
// 	
// 	UTextureRenderTarget2D* MiniMapRenderTarget = NewObject<UTextureRenderTarget2D>(this);
// 	MiniMapRenderTarget->InitAutoFormat(512, 512);
// 	MiniMapRenderTarget->UpdateResource();
//
// 	MiniMapCapture->TextureTarget = MiniMapRenderTarget;
// 	
// 	UMaterial* BaseMaterial = LoadObject<UMaterial>(nullptr, TEXT("/Game/Characters/RenderTarget/M_Minimap"));
// 	
// 	UMaterialInstanceDynamic* DynamicMaterialInstance = UMaterialInstanceDynamic::Create(BaseMaterial, this);
// 	
// 	DynamicMaterialInstance->SetTextureParameterValue(FName("MiniMapTexture"), MiniMapRenderTarget);
// 	
// 	OccupationMinimapWidget->MinimapImage->SetBrushFromMaterial(DynamicMaterialInstance);
// }

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
	if (const auto PlayerController = GetPlayerController(); PlayerController && PlayerController->IsLocalController())
	{
		if (DirectionDamageIndicatorWidget)
			DirectionDamageIndicatorWidget->IndicateStart(CauserName.ToString(), CauserLocation, 3.0f);

		if (const auto Character = GetPawn<ALakayaBaseCharacter>()) Character->PlayHitScreen();
	}
}

void ALakayaBasePlayerState::SetOwner(AActor* NewOwner)
{
	Super::SetOwner(NewOwner);
	OnOwnerChanged.Broadcast(Owner);
}

UAbilitySystemComponent* ALakayaBasePlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}
