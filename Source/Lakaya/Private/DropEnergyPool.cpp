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

void ADropEnergyPool::Initialize(int32 PoolSize)
{
	for (int32 i = 0 ; i < PoolSize; i++)
	{
		AIndividualDropEnergy* DropEnergy = GetWorld()->SpawnActor<AIndividualDropEnergy>(DropEnergyClass);
		InactiveDropEnergys.Add(DropEnergy);
		UE_LOG(LogTemp, Warning, TEXT("Add Succeess."));
		UE_LOG(LogTemp, Warning, TEXT("InactiveDropEnergys Num : %d"), InactiveDropEnergys.Num());
	}
}

AIndividualDropEnergy* ADropEnergyPool::GetDropEnergy()
{
	if (InactiveDropEnergys.Num() > 0)
	{
		AIndividualDropEnergy* DropEnergy = InactiveDropEnergys.Pop();
		DropEnergy->SetActorHiddenInGame(false);
		DropEnergy->SetActorEnableCollision(true);
		UE_LOG(LogTemp, Warning, TEXT("Pop Succeess."));
		return DropEnergy;
	}
	else
	{
		// TODO : 오브젝트풀링 사이즈가 부족하다면 늘려야 함.
		// UE_LOG(LogTemp, Warning, TEXT("Pop failed."));
		// return nullptr;
		AIndividualDropEnergy* DropEnergy = GetWorld()->SpawnActor<AIndividualDropEnergy>(DropEnergyClass);
		if (DropEnergy == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("DropEnergyPool_DropEnergy is null."));
			return nullptr;
		}
		InactiveDropEnergys.Add(DropEnergy);
		UE_LOG(LogTemp, Warning, TEXT("DropEnergyPool_Add Succeess"));
		AIndividualDropEnergy* PopDropEnergy = InactiveDropEnergys.Pop();
		UE_LOG(LogTemp, Warning, TEXT("DropEnergyPool_Pop Succeess"));
		PopDropEnergy->SetActorHiddenInGame(false);
		PopDropEnergy->SetActorEnableCollision(true);
		return PopDropEnergy;
	}
}

void ADropEnergyPool::ReturnDropEnergy(AIndividualDropEnergy* DropEnergy)
{
	DropEnergy->Deactivate();
	InactiveDropEnergys.Add(DropEnergy);
}