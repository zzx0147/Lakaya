// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LakayaBaseCharacter.h"

#include "AbilitySystemComponent.h"
#include "Character/LakayaAbilitySet.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Character/BulletSpreadComponent.h"
#include "Character/LakayaBasePlayerState.h"
#include "Character/ResourceComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PostProcessComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameMode/OccupationGameState.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

const FName ALakayaBaseCharacter::SpringArmComponentName = FName(TEXT("SpringArm"));
const FName ALakayaBaseCharacter::CameraComponentName = FName(TEXT("Camera"));
const FName ALakayaBaseCharacter::ResourceComponentName = FName(TEXT("ResourceComponent"));
const FName ALakayaBaseCharacter::ClairvoyanceMeshComponentName = FName(TEXT("ClairvoyanceMesh"));
const FName ALakayaBaseCharacter::DamageImmuneMeshComponentName = FName(TEXT("DamageImmuneMesh"));
const FName ALakayaBaseCharacter::ResurrectionNiagaraName = FName(TEXT("ResurrectionNiagara"));
const FName ALakayaBaseCharacter::GrayScalePostProcessComponentName = FName(TEXT("GrayScalePostProcess"));
const FName ALakayaBaseCharacter::BulletSpreadComponentName = FName(TEXT("BulletSpread"));

ALakayaBaseCharacter::ALakayaBaseCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	DissolveTimelineLength = 2.5f;
	MaxHealth = 100.f;
	PrimaryActorTick.bCanEverTick = true;
	PlayerRotationInterpolationAlpha = 0.65f;
	ATeamObjectType = ECC_GameTraceChannel5;
	BTeamObjectType = ECC_GameTraceChannel6;
	bIsAlive = true;
	bEnableLocalOutline = true;
	ResurrectionDamageImmuneTime = 5.f;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	bUseControllerRotationYaw = bUseControllerRotationPitch = bUseControllerRotationRoll = false;
	CharacterName = TEXT("Base");

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(SpringArmComponentName);
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(CameraComponentName);
	Camera->SetupAttachment(SpringArm);

	HitScreenEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
	HitScreenEffect->SetupAttachment(Camera);

	ResourceComponent = CreateDefaultSubobject<UResourceComponent>(ResourceComponentName);
	ResourceComponent->SetIsReplicated(true);

	ClairvoyanceMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(ClairvoyanceMeshComponentName);
	ClairvoyanceMeshComponent->SetupAttachment(GetMesh());
	ClairvoyanceMeshComponent->SetRenderCustomDepth(true);
	ClairvoyanceMeshComponent->SetCustomDepthStencilValue(255);
	ClairvoyanceMeshComponent->SetCustomDepthStencilWriteMask(ERendererStencilMask::ERSM_128);
	ClairvoyanceMeshComponent->SetCollisionEnabled(ECollisionEnabled::ProbeOnly);
	ClairvoyanceMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	ClairvoyanceMeshComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
	ClairvoyanceMeshComponent->SetVisibility(false);

	DamageImmuneMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(DamageImmuneMeshComponentName);
	DamageImmuneMeshComponent->SetupAttachment(GetMesh());
	DamageImmuneMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DamageImmuneMeshComponent->SetVisibility(false);

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ResurrectionFinder(
		TEXT("/Script/Niagara.NiagaraSystem'/Game/Effects/M_VFX/Char_Common/VFX_Resurrection.VFX_Resurrection'"));

	ResurrectionNiagara = CreateDefaultSubobject<UNiagaraComponent>(ResurrectionNiagaraName);
	ResurrectionNiagara->SetupAttachment(GetMesh());
	ResurrectionNiagara->SetAutoActivate(false);
	ResurrectionNiagara->SetAutoDestroy(false);
	ResurrectionNiagara->SetAsset(ResurrectionFinder.Object);

	static ConstructorHelpers::FObjectFinder<UCurveFloat> DissolveCurveFinder(
		TEXT("/Game/Blueprints/Curve/CV_Float_DissolveCurve.CV_Float_DissolveCurve"));

	GrayScalePostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(GrayScalePostProcessComponentName);
	GrayScalePostProcessComponent->SetupAttachment(RootComponent);
	GrayScalePostProcessComponent->bUnbound = true;
	GrayScalePostProcessComponent->Priority = -1.0f;

	BulletSpreadComponent = CreateDefaultSubobject<UBulletSpreadComponent>(BulletSpreadComponentName);
	
	if (DissolveCurveFinder.Succeeded()) DissolveCurve = DissolveCurveFinder.Object;

	static ConstructorHelpers::FObjectFinder<UTexture2D> QuestionIconFinder(
		TEXT("/Game/UI_2/UI_Minimap/Minimap_QuestionMark"));

	if (QuestionIconFinder.Succeeded())
	{
		QuestionIcon = QuestionIconFinder.Object;
	}
}

ELifetimeCondition ALakayaBaseCharacter::AllowActorComponentToReplicate(
	const UActorComponent* ComponentToReplicate) const
{
	if (ComponentToReplicate->IsA(UResourceComponent::StaticClass())) return COND_None;
	return Super::AllowActorComponentToReplicate(ComponentToReplicate);
}

float ALakayaBaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                                       AController* EventInstigator, AActor* DamageCauser)
{
	// 무적이면 넘깁니다.
	if (DamageImmuneEndingTime >= GetServerTime()) return 0.f;
	const auto LocalState = GetPlayerState();

	// 플레이어 스테이트가 없는 경우 원본의 로직을 실행합니다.
	if (!LocalState) return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// 플레이어 스테이트에서 데미지를 처리하고나서, 애니메이션 재생을 위해 캐릭터에서도 데미지를 처리합니다.
	const auto Damage = LocalState->TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}

float ALakayaBaseCharacter::InternalTakeRadialDamage(float Damage, FRadialDamageEvent const& RadialDamageEvent,
                                                     AController* EventInstigator, AActor* DamageCauser)
{
	// 플레이어 스테이트가 존재하는 경우 이미 범위피해 보정이 이뤄졌을 것이므로, 여기서는 무시합니다.
	if (GetPlayerState()) return Damage;
	return Super::InternalTakeRadialDamage(Damage, RadialDamageEvent, EventInstigator, DamageCauser);
}

void ALakayaBaseCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (DissolveTimeline.IsPlaying()) DissolveTimeline.TickTimeline(DeltaSeconds);

	// 서버에서는 계속해서 플레이어의 회전정보를 업데이트합니다.
	if (HasAuthority())
	{
		PlayerRotation.Rotation = GetCamera()->GetComponentRotation().Quaternion();
		PlayerRotation.Time = GetServerTime();
	}

	// 시뮬레이트되는 클라이언트에서는 계속해서 플레이어의 회전을 외삽합니다. 
	else if (GetLocalRole() == ROLE_SimulatedProxy)
	{
		// 이전 프레임에서 사용했던 회전값과 현재 시간을 기준으로 외삽된 Raw회전값을 구면보간하여 현재 프레임에서 사용한 회전값을 지정합니다. 
		LatestUpdateRotation = FQuat::Slerp(LatestUpdateRotation, GetRawExtrapolatedRotator(GetServerTime()),
		                                    PlayerRotationInterpolationAlpha);
		//DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + LatestUpdateRotation.Vector() * 100.f,
		//              FColor::Green, false, 0.3f);
	}
}

UAbilitySystemComponent* ALakayaBaseCharacter::GetAbilitySystemComponent() const
{
	// 어빌리티 핸들 컨테이너에 캐싱된 어빌리티 시스템이 유효한 경우 해당 어빌리티 시스템을 반환합니다.
	if (AbilityHandleContainer.AbilitySystem.IsValid())
	{
		return AbilityHandleContainer.AbilitySystem.Get();
	}

	// 마지막으로 플레이어 스테이트를 타입캐스팅하여 플레이어 스테이트의 어빌리티 시스템 컴포넌트를 사용합니다.
	const auto CastedState = GetPlayerState<IAbilitySystemInterface>();
	return ensure(CastedState) ? CastedState->GetAbilitySystemComponent() : nullptr;
}

void ALakayaBaseCharacter::GiveAbilities(UAbilitySystemComponent* InAbilitySystem)
{
	if (!ensure(InAbilitySystem) || CharacterAbilities.IsNull()) return;
	CharacterAbilities.LoadSynchronous()->GiveAbilities(InAbilitySystem, AbilityHandleContainer);
	UE_LOG(LogTemp, Log, TEXT("%s Give Abilities"), *GetName());
}

void ALakayaBaseCharacter::ClearAbilities()
{
	if (!CharacterAbilities.IsValid()) return;
	AbilityHandleContainer.ClearAbilities();
	UE_LOG(LogTemp, Log, TEXT("%s Clear Abilities"), *GetName());
}

void ALakayaBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	MeshCollisionProfile = GetMesh()->GetCollisionProfileName();
	MeshRelativeLocation = GetMesh()->GetRelativeLocation();
	MeshRelativeRotation = GetMesh()->GetRelativeRotation();
}

FRotator ALakayaBaseCharacter::GetPlayerRotation() const
{
	// 서버이거나 Autonomous인 경우 그냥 카메라 컴포넌트를 사용합니다.
	if (HasAuthority() || GetLocalRole() == ROLE_AutonomousProxy) return GetCamera()->GetComponentRotation();
	return LatestUpdateRotation.Rotator();
}

bool ALakayaBaseCharacter::IsSameTeam(const ETeam& Team) const
{
	return JudgeSameTeam(RecentTeam, Team);
}

void ALakayaBaseCharacter::PlayHitScreen()
{
	HitScreenEffect->Activate(true);
}

void ALakayaBaseCharacter::EnableClairvoyance()
{
	ClairvoyanceMeshComponent->SetVisibility(true);
}

void ALakayaBaseCharacter::DisableClairvoyance()
{
	ClairvoyanceMeshComponent->SetVisibility(false);
}

void ALakayaBaseCharacter::SetStencilMask_Implementation(const ERendererStencilMask& StencilMask)
{
	GetMesh()->SetCustomDepthStencilWriteMask(StencilMask);
	GetMesh()->SetCustomDepthStencilValue(255);
	GetMesh()->SetRenderCustomDepth(true);
}

void ALakayaBaseCharacter::SetAlly(const bool& IsAlly)
{
	// 로컬 캐릭터에 대한 아웃라인이 비활성화되어있다면 아무것도 하지 않습니다.
	if (!bEnableLocalOutline && IsPlayerControlled() && IsLocallyControlled())
	{
		GetMesh()->SetOverlayMaterial(nullptr);
		return;
	}

	if (!CharacterOverlayMaterial.IsValid())
	{
		CharacterOverlayMaterial = UMaterialInstanceDynamic::Create(GetMesh()->GetOverlayMaterial(), this);
		if (!CharacterOverlayMaterial.IsValid()) return;
		GetMesh()->SetOverlayMaterial(CharacterOverlayMaterial.Get());
	}

	CharacterOverlayMaterial->SetVectorParameterValue(TEXT("Color"), IsAlly ? FLinearColor::Blue : FLinearColor::Red);
}

bool ALakayaBaseCharacter::IsEnemyVisibleInCamera(const ETeam& EnemyTeam,
                                                  const TWeakObjectPtr<ALakayaBasePlayerState> EnemyState, const TWeakObjectPtr<UImage> EnemyImage)
{
#pragma region NullCheck
	if (!EnemyState.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("State is not valid."));
		return false;
	}

	const APawn* EnemyPawn = EnemyState->GetPawn();
	if (!EnemyPawn)
	{
		// UE_LOG(LogTemp, Warning, TEXT("EnemyPawn is null."));
		return false;
	}

	// TODO : GetFristPlayerController를 가져오는 것이 맞는지 확인해야 합니다.
	const APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	
	if (!PlayerController || !PlayerController->PlayerCameraManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerController or PlayerCameraManager is null."));
		return false;
	}
#pragma endregion
	
	// 시야에 적이 들어왔는지 확인합니다.
	{
		const FVector CameraLocation = PlayerController->PlayerCameraManager->GetCameraLocation();
		const FVector EnemyLocation = EnemyPawn->GetActorLocation();
		const FVector DirectionToTarget = (EnemyLocation - CameraLocation).GetSafeNormal();
		const FVector LookDirection = PlayerController->GetControlRotation().Vector();

		// 시야각을 설정합하여, 라디언으로 변환합니다.
		const float AngleThreshold = FMath::DegreesToRadians(90.0f);

		// 두 벡터 사이의 각도를 계산합니다.
		const float AngleBetweenVectors = FMath::Acos(FVector::DotProduct(DirectionToTarget.GetSafeNormal(), LookDirection));

		// 계산된 각도와 임계값을 비교하여, 시야 내에 있는지 판단합니다.
		bool bIsVisible = AngleBetweenVectors <= AngleThreshold;
		
		FHitResult HitResult;

		if (bool bIsObstructed = UKismetSystemLibrary::LineTraceSingle(
			this,
			CameraLocation,
			EnemyLocation,
			TraceTypeQuery1,
			false,
			TArray<AActor*>(),
			EDrawDebugTrace::None,
			HitResult,
			true); bIsObstructed && HitResult.GetActor() != EnemyPawn)
		{
			bIsVisible = false;
		}

		// 적을 발견했다면, Spotted를 true로 바꿔줍니다.
		if (bIsVisible)
		{
			Server_OnEnemySpotted(EnemyTeam, EnemyState.Get());
		}
		else if (!bIsVisible)
		{
			Server_OnEnemyLost(EnemyTeam, EnemyState.Get());
			// EnemyImage->SetBrushFromTexture(QuestionIcon);
		}

		Server_SetEnemyVisibility(EnemyState.Get(), bIsVisible);
		
		return bIsVisible;
	}
}

void ALakayaBaseCharacter::Server_OnEnemyLost_Implementation(const ETeam& EnemyTeam, ALakayaBasePlayerState* EnemyState)
{
	if (AOccupationGameState* OccupationGameState = GetWorld()->GetGameState<AOccupationGameState>())
	{
		OccupationGameState->OnEnemyLost(EnemyTeam, EnemyState);
	}
}

void ALakayaBaseCharacter::Server_OnEnemySpotted_Implementation(const ETeam& EnemyTeam,
                                                                ALakayaBasePlayerState* EnemyState)
{
	if (AOccupationGameState* OccupationGameState = GetWorld()->GetGameState<AOccupationGameState>())
	{
		OccupationGameState->OnEnemySpotted(EnemyTeam, EnemyState);
	}
}

void ALakayaBaseCharacter::Server_SetEnemyVisibility_Implementation(
	ALakayaBasePlayerState* EnemyState, bool bIsVisible)
{
	if (bIsVisible && !VisibleEnemies.Contains(EnemyState))
	{
		VisibleEnemies.Emplace(EnemyState);
		UE_LOG(LogTemp, Warning, TEXT("Server_Emplace"));
		Client_SetEnemyVisibility(EnemyState, true);
	}
	else if (!bIsVisible && VisibleEnemies.Contains(EnemyState))
	{
		VisibleEnemies.Remove(EnemyState);
		UE_LOG(LogTemp, Warning, TEXT("Server_Remove"));
		Client_SetEnemyVisibility(EnemyState, false);
	}
}

void ALakayaBaseCharacter::Client_SetEnemyVisibility_Implementation(
	ALakayaBasePlayerState* EnemyState, bool bIsVisible)
{
	if (bIsVisible)
	{
		VisibleEnemies.Emplace(EnemyState);
		UE_LOG(LogTemp, Warning, TEXT("Client_Emplace"));
	}
	else
	{
		VisibleEnemies.Remove(EnemyState);
		UE_LOG(LogTemp, Warning, TEXT("Client_Remove"));
	}
}

void ALakayaBaseCharacter::SetTeam_Implementation(const ETeam& Team)
{
	RecentTeam = Team;
	if (Team == ETeam::Anti) GetCapsuleComponent()->SetCollisionObjectType(ATeamObjectType);
	else if (Team == ETeam::Pro) GetCapsuleComponent()->SetCollisionObjectType(BTeamObjectType);
}

void ALakayaBaseCharacter::SetAliveState_Implementation(bool IsAlive)
{
	bIsAlive = IsAlive;
	// ResourceComponent->OnAliveStateChanged(IsAlive);
	if (IsAlive)
	{
		GetMesh()->SetAllBodiesSimulatePhysics(false);
		GetMesh()->SetCollisionProfileName(MeshCollisionProfile);
		GetMesh()->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		GetMesh()->SetRelativeLocationAndRotation(MeshRelativeLocation, MeshRelativeRotation);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ResurrectionNiagara->Activate(true);
		if (HasAuthority())
		{
			DamageImmuneEndingTime = GetServerTime() + ResurrectionDamageImmuneTime;
			OnRep_DamageImmuneEndingTime();
		}
		RemoveDissolveEffect();
		

	}
	else
	{
		GetMesh()->SetCollisionProfileName(TEXT("RagDoll"));
		GetMesh()->SetAllBodiesSimulatePhysics(true);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		StartDissolveEffect();
	}
	if (HasAuthority()) GetCharacterMovement()->SetMovementMode(IsAlive ? MOVE_Walking : MOVE_None);

	if (const auto PlayerController = Cast<APlayerController>(Controller); PlayerController && PlayerController->IsLocalController())
		ToggleGrayScalePostProcess(IsAlive);
	
}

float ALakayaBaseCharacter::GetServerTime() const
{
	return GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
}

void ALakayaBaseCharacter::OnRep_PlayerRotation()
{
	// 최신 주기를 이전 주기로 바꿔주고, 새로 들어온 PlayerRotation을 최신주기로 설정합니다.
	PrevPlayerRotation = LatestPlayerRotation;
	LatestPlayerRotation = PlayerRotation;
}

void ALakayaBaseCharacter::OnRep_DamageImmuneEndingTime()
{
	DamageImmuneMeshComponent->SetVisibility(true);
	GetWorldTimerManager().SetTimer(DamageImmuneTimer, this, &ALakayaBaseCharacter::DamageImmuneTimerCallback,
	                                DamageImmuneEndingTime - GetServerTime());
}

bool ALakayaBaseCharacter::CanJumpInternal_Implementation() const
{
	return JumpIsAllowedInternal();
}

FQuat ALakayaBaseCharacter::GetRawExtrapolatedRotator(const float& CurrentTime) const
{
	// 이전 주기의 회전 정보와, 최신 주기의 회전 정보를 구면보간하여 반환합니다.
	// 베지어 곡선 같은 것을 사용하지 않기 때문에 보간하여 사용하지 않으면 도약현상이 발생할 수 있습니다. 
	return FQuat::Slerp(PrevPlayerRotation.Rotation, LatestPlayerRotation.Rotation,
	                    UKismetMathLibrary::NormalizeToRange(CurrentTime, PrevPlayerRotation.Time,
	                                                         LatestPlayerRotation.Time));
}

void ALakayaBaseCharacter::DamageImmuneTimerCallback()
{
	DamageImmuneMeshComponent->SetVisibility(false);
}

void ALakayaBaseCharacter::StartDissolveEffect()
{
	if (DissolveCurve)
	{
		FOnTimelineFloat DissolveCurveCallback;
		DissolveCurveCallback.BindUFunction(this,TEXT("DissolveTick"));
		DissolveTimeline.AddInterpFloat(DissolveCurve, DissolveCurveCallback);
		DissolveTimeline.SetTimelineLength(DissolveTimelineLength);

		DissolveTimeline.PlayFromStart();
	}

	if (CharacterOverlayMaterial.IsValid()) CharacterOverlayMaterial->SetScalarParameterValue(TEXT("Opacity"), 0.0f);
}

void ALakayaBaseCharacter::RemoveDissolveEffect()
{
	for (const auto TargetMaterial : DissolveTargetArray)
		TargetMaterial->SetScalarParameterValue(TEXT("Dissolve"), 2.0f);

	if (CharacterOverlayMaterial.IsValid()) CharacterOverlayMaterial->SetScalarParameterValue(TEXT("Opacity"), 0.04f);
}

void ALakayaBaseCharacter::ToggleGrayScalePostProcess(const bool& bIsActivate)
{
	if(!GrayScalePostProcessComponent || !GrayScalePostProcessMaterial)
		return;
	
	GrayScalePostProcessComponent->AddOrUpdateBlendable(GrayScalePostProcessMaterial,bIsActivate ? 0.0f : 1.0f);
}

void ALakayaBaseCharacter::DissolveTick(const float& Value)
{
	for (const auto DissolveTarget : DissolveTargetArray)
	{
		DissolveTarget->SetScalarParameterValue(TEXT("Dissolve"), Value);
	}
}

void ALakayaBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ALakayaBaseCharacter, ResourceComponent, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ALakayaBaseCharacter, PlayerRotation, COND_SkipOwner);
	DOREPLIFETIME(ALakayaBaseCharacter, DamageImmuneEndingTime);
	// DOREPLIFETIME(ALakayaBaseCharacter, bIsSpottedByTeammate);
}
