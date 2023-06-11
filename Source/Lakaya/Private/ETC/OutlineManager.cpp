// Fill out your copyright notice in the Description page of Project Settings.


#include "ETC/OutlineManager.h"

#include "Character/LakayaBaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/PostProcessComponent.h"

// Sets default values
AOutlineManager::AOutlineManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	SetRootComponent(BoxComponent);
	ClairvoyancePostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(
		TEXT("ClairvoyancePostProcessComponent"));
	ClairvoyancePostProcessComponent->bEnabled = true;
	EnemyRenderingParameterName = FName(TEXT("bIsRenderEnemy"));
	EnemyNumberStartParameterName = FName(TEXT("EnemyNumberStart"));
}

void AOutlineManager::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ClairvoyanceDynamic = UMaterialInstanceDynamic::Create(ClairvoyanceMaterial, this);
	if (!ClairvoyanceDynamic.IsValid()) return;
	ClairvoyancePostProcessComponent->AddOrUpdateBlendable(ClairvoyanceDynamic.Get());
	ClairvoyanceDynamic->SetScalarParameterValue(EnemyRenderingParameterName, 0.f);
}

void AOutlineManager::SetClairvoyance(const bool& bIsClairvoyance)
{
	if (ClairvoyanceDynamic.IsValid())
		ClairvoyanceDynamic->SetScalarParameterValue(EnemyRenderingParameterName, bIsClairvoyance ? 1.f : 0.f);
}

void AOutlineManager::SetTeam(const EPlayerTeam& NewTeam)
{
	ClientTeam = NewTeam;
	ClairvoyanceDynamic->SetScalarParameterValue(EnemyNumberStartParameterName,
	                                             NewTeam == EPlayerTeam::Individual ? 2.f : 4.f);
}

void AOutlineManager::RegisterClairvoyance(const ALakayaBaseCharacter* Character)
{
	// 개인전에선 로컬 플레이어 컨트롤러인 경우에만, 다른 모드에서는 로컬 클라이언트와 같은 팀인 경우에만 투시를 허용합니다.
	if ((ClientTeam == EPlayerTeam::Individual
		     ? !Character->IsPlayerControlled() || !Character->IsLocallyControlled()
		     : !Character->IsSameTeam(ClientTeam))
		|| ActivatedClairvoyanceSet.Contains(Character)) return;
	
	ActivatedClairvoyanceSet.Emplace(Character);
	if (!ActivatedClairvoyanceSet.IsEmpty()) SetClairvoyance(true);
}

void AOutlineManager::UnRegisterClairvoyance(const ALakayaBaseCharacter* Character)
{
	if (!ActivatedClairvoyanceSet.Contains(Character)) return;
	ActivatedClairvoyanceSet.Remove(Character);
	if (ActivatedClairvoyanceSet.IsEmpty()) SetClairvoyance(false);
}
