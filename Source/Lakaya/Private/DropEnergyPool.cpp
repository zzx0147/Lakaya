#include "DropEnergyPool.h"

ADropEnergyPool::ADropEnergyPool()
{
	PrimaryActorTick.bCanEverTick = false;

	DropEnergyClass = AIndividualDropEnergy::StaticClass();
	
	bReplicates = true;
	bAlwaysRelevant = true;
}

void ADropEnergyPool::BeginPlay()
{
	Super::BeginPlay();
}

void ADropEnergyPool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADropEnergyPool::Initialize(uint8 PoolSize)
{
	for (uint8 i = 0 ; i < PoolSize; i++)
	{
		AIndividualDropEnergy* DropEnergy = GetWorld()->SpawnActor<AIndividualDropEnergy>(DropEnergyClass);
		// DropEnergy->Deactivate();
		InactiveDropEnergys.Add(DropEnergy);
		UE_LOG(LogTemp, Warning, TEXT("Add Succeess."));
		UE_LOG(LogTemp, Warning, TEXT("InactiveDropEnergys Num : %d"), InactiveDropEnergys.Num());
	}
}

AIndividualDropEnergy* ADropEnergyPool::GetDropEnergy()
{
	if (InactiveDropEnergys.Num() > 0)
	{
		AIndividualDropEnergy* ItemDrop = InactiveDropEnergys.Pop();
		ItemDrop->SetActorHiddenInGame(false);
		ItemDrop->SetActorEnableCollision(true);
		return ItemDrop;
		UE_LOG(LogTemp, Warning, TEXT("Pop Succeess."));
	}
	else
	{
		// TODO : 오브젝트풀링 사이즈가 부족하다면 늘려야 함.
		return nullptr;
		UE_LOG(LogTemp, Warning, TEXT("Pop failed."));
	}
}

void ADropEnergyPool::ReturnDropEnergy(AIndividualDropEnergy* DropEnergy)
{
	DropEnergy->Deactivate();
	InactiveDropEnergys.Add(DropEnergy);
}