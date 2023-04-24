// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/DamageableCharacter.h"

#include "NiagaraComponent.h"
#include "PlayerController/BattlePlayerController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "UI/GamePlayHealthWidget.h"
#include "Character/CollectorPlayerState.h"

ADamageableCharacter::ADamageableCharacter()
{
	// static const ConstructorHelpers::FClassFinder<AActor> BP_ReResurrection(
	// 	TEXT("/Game/Blueprints/ETC/BP_Resurrection.BP_Resurrection_C"));
	
	MaximumHealth = 100.f;
}

void ADamageableCharacter::BeginPlay()
{
	Super::BeginPlay();
	Health = MaximumHealth;
	if (IsRunningDedicatedServer()) return;

	OnTakeAnyDamage.AddDynamic(this, &ADamageableCharacter::OnTakeAnyDamageCallback);
	if (const auto BattleController = GetWorld()->GetFirstPlayerController<ABattlePlayerController>())
		BattleController->OnCharacterBeginPlay(this);
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

	if (EventInstigator == nullptr) return 0.0f;

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
	OnHitEffectPlay(CollectorPlayerState->GetOwningController()->GetCharacter());
	
	Health -= Damage;
	if (Health > MaximumHealth) Health = MaximumHealth;
	OnHealthChanged.Broadcast(this, Health);
	
	if (Health <= 0.f) KillCharacter(EventInstigator, DamageCauser);
	return Damage;
}

void ADamageableCharacter::OnHitEffectPlay_Implementation(AActor* DamagedActor)
{
	static bool bPlayedNiagaraEffect = false;
	// TODO : 나이아가라 이펙트 경로 지정
	UNiagaraSystem* NiagaraEffect =
	Cast<UNiagaraSystem>(StaticLoadObject(UNiagaraSystem::StaticClass(), nullptr,
		TEXT("/Game/Effects/M_VFX/VFX_Hit_Impact_3")));

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
	Health = MaximumHealth;
	OnHealthChanged.Broadcast(this, Health);
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
	OnMaximumHealthChanged.Broadcast(this, MaximumHealth);
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red,
	                                 FString::Printf(TEXT("MaximumHealth Changed : %f"), MaximumHealth));
}

void ADamageableCharacter::OnRep_Health()
{
	OnHealthChanged.Broadcast(this, Health);
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, FString::Printf(TEXT("Health Changed : %f"), Health));
}

void ADamageableCharacter::OnTakeAnyDamageCallback(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                                                   AController* InstigatedBy, AActor* DamageCauser)
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, FString::Printf(TEXT("Damaged : %f"), Damage));
	
	IndicateRPC(DamageCauser->GetFName(), DamageCauser->GetActorLocation(), 3.0f);
}

void ADamageableCharacter::IndicateRPC_Implementation(FName CauserName, FVector DamageCursorPosition, float time)
{
	if (GetController()->IsLocalPlayerController())
	{
		AMenuCallingPlayerController* MenuCallingPlayercontroller = Cast<AMenuCallingPlayerController>(GetController());
		if (MenuCallingPlayercontroller != nullptr)
		{
			MenuCallingPlayercontroller->IndicateStart(CauserName, DamageCursorPosition, 3.0f);

			// ScreenEffect : 피격 당할 시 화면에 표기 되는 이펙트
			FSoftObjectPath NiagaraPath;
			NiagaraPath = (TEXT("/Game/Effects/M_VFX/VFX_Screeneffect.VFX_Screeneffect"));
			UNiagaraSystem* NiagaraEffect = Cast<UNiagaraSystem>(NiagaraPath.TryLoad());
		
			if (NiagaraEffect)
			{
				NiagaraComponent =
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraEffect,
				FVector(0.0f, 0.0f, 0.0f));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to load Niagara system!"));
			}

		}
	}
}

void ADamageableCharacter::RespawnNotify_Implementation()
{
	GetMesh()->SetVisibility(true, true);
	
	OnRespawnCharacterNotify.Broadcast(this);
	
	// UNiagaraSystem* NiagaraResurrection = Cast<UNiagaraSystem>(StaticLoadObject(UNiagaraSystem::StaticClass()
	// 	, nullptr, TEXT("/Game/Effects/M_VFX/VFX_Resurrection")));
	// UNiagaraComponent* NiagaraResurrectionComponent =
	// 	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
	// 	GetWorld(),
	// 	NiagaraResurrection,
	// 	GetController()->GetCharacter()->GetActorLocation() + FVector(0.0f, 0.0f, -80.0f),
	// 	FRotator::ZeroRotator);
	
}