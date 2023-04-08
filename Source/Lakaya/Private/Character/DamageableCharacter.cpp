// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/DamageableCharacter.h"

#include "Character/BattlePlayerController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameMode/IndividualGameMode.h"
#include "UI/GamePlayHealthWidget.h"
#include "Character/CollectorPlayerState.h"

ADamageableCharacter::ADamageableCharacter()
{
	MaximumHealth = 100.f;
}

void ADamageableCharacter::BeginPlay()
{
	Super::BeginPlay();
	Health = MaximumHealth;
	if (IsRunningDedicatedServer()) return;

	OnTakeAnyDamage.AddDynamic(this, &ADamageableCharacter::OnTakeAnyDamageCallback);
	if (auto BattleController = GetWorld()->GetFirstPlayerController<ABattlePlayerController>())
		BattleController->OnCharacterBeginPlay(this);

	if (GetController() == nullptr) return;

	if (GetController()->IsLocalController() && HasAuthority())
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Server DCharacter Begin Play"));
	}

	if (GetController()->IsLocalController())
	{
		// 스코어보드 위젯
		UClass* GamePlayHealthWidgetClass = LoadClass<UGamePlayHealthWidget>(nullptr, TEXT("/Game/Blueprints/UMG/WBP_GamePlayHealthWidget.WBP_GamePlayHealthWidget_C"));
		if (GamePlayHealthWidgetClass == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("GamePlayHealthWidgetClass is null."));
			return;
		}

		APlayerController* MyPlayerController = Cast<APlayerController>(GetController());

		if (MyPlayerController == nullptr) return;

		GamePlayHealthWidget = CreateWidget<UGamePlayHealthWidget>(MyPlayerController, GamePlayHealthWidgetClass);
		if (GamePlayHealthWidget == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("GamePlayHealthWidget is null."));
			return;
		}
		GamePlayHealthWidget->AddToViewport();
		GamePlayHealthWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

float ADamageableCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                                       AController* EventInstigator, AActor* DamageCauser)
{
	auto MyPlayerState = GetPlayerState();
	if (MyPlayerState == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("No Player State"));
		return 0.0f;
	}
	auto CollectorPlayerState = Cast<ACollectorPlayerState>(MyPlayerState);
	if (CollectorPlayerState == nullptr) 
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("No CollectorPlayer State"));
		return 0.0f;
	}

	auto enemyPlayerState = EventInstigator->GetPlayerState<ACollectorPlayerState>();
	if (enemyPlayerState == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("No EnemyPlayer State"));
		return 0.0f;
	}
	if (enemyPlayerState->GetPlayerTeamState() == CollectorPlayerState->GetPlayerTeamState())
	{
		return 0.0f;
	}

	auto Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// TODO : NetMultiCast 된 함수이며 피격받는 캐릭터를 받아 피격 이펙트를 재생합니다.
	OnHitEffectPlay(CollectorPlayerState->GetPlayerController()->GetCharacter());
	
	Health -= Damage;
	if (Health > MaximumHealth)
	{
		Health = MaximumHealth;
	}
	OnRep_Health();
	if (Health <= 0.f) KillCharacter(EventInstigator, DamageCauser);
	return Damage;
}

void ADamageableCharacter::OnHitEffectPlay_Implementation(AActor* DamagedActor)
{
	static bool bPlayedNiagaraEffect = false;
	// TODO : 나이아가라 이펙트 경로 지정
	UNiagaraSystem* NiagaraEffect =
	Cast<UNiagaraSystem>(StaticLoadObject(UNiagaraSystem::StaticClass(), nullptr,
		TEXT("/Game/Effects/VFX_Character/VFX/Materials/Impacts/GunImpact/VFX/VFX_Hit_Impact_3")));

	FVector HitLoaction = DamagedActor->GetActorLocation() + FVector(0,0,60);

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraEffect, HitLoaction);
	bPlayedNiagaraEffect = true;
}

void ADamageableCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADamageableCharacter, MaximumHealth);
	DOREPLIFETIME(ADamageableCharacter, Health);
}

void ADamageableCharacter::Respawn()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	SetActorEnableCollision(true);
	RespawnNotify();
}

void ADamageableCharacter::KillCharacter(AController* EventInstigator, AActor* DamageCauser)
{
	GetCharacterMovement()->DisableMovement();
	//TODO: 트레이스 충돌은 꺼지지만, 여전히 다른 캐릭터의 움직임을 제한하고 있습니다..
	SetActorEnableCollision(false);
	KillCharacterNotify(EventInstigator, DamageCauser);
}

void ADamageableCharacter::KillCharacterNotify_Implementation(AController* EventInstigator, AActor* DamageCauser)
{
	GetMesh()->SetVisibility(false, true);
	OnKillCharacterNotify.Broadcast(GetController(), this, EventInstigator, DamageCauser);
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Dead"));
}

void ADamageableCharacter::OnRep_MaximumHealth()
{
	OnMaximumHealthReplicated.Broadcast(this, MaximumHealth);
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red,
	                                 FString::Printf(TEXT("MaximumHealth Changed : %f"), MaximumHealth));
}

void ADamageableCharacter::OnRep_Health()
{
	OnHealthReplicated.Broadcast(this, Health);
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, FString::Printf(TEXT("Health Changed : %f"), Health));
}

void ADamageableCharacter::OnTakeAnyDamageCallback(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                                                   AController* InstigatedBy, AActor* DamageCauser)
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, FString::Printf(TEXT("Damaged : %f"), Damage));
}

void ADamageableCharacter::RespawnNotify_Implementation()
{
	GetMesh()->SetVisibility(true, true);
	OnRespawnCharacterNotify.Broadcast(this);
}
