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
		// UE_LOG(LogTemp, Warning, TEXT("머리털 다 빠지겠다"));
		UE_LOG(LogTemp, Warning, TEXT("InactiveDropEnergys Num : %d"), InactiveDropEnergys.Num());
	}
}

AIndividualDropEnergy* ADropEnergyPool::GetDropEnergy()
{
	if (InactiveDropEnergys.Num() > 0)
	{
		AIndividualDropEnergy* DropEnergy = InactiveDropEnergys.Pop();
		DropEnergy->Activate();
		UE_LOG(LogTemp, Warning, TEXT("Pop Succeess."));
		return DropEnergy;
	}
	else
	{
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
		PopDropEnergy->Activate();
		return PopDropEnergy;
	}
}

void ADropEnergyPool::ReturnDropEnergy(AIndividualDropEnergy* DropEnergy)
{
	DropEnergy->Deactivate();
	InactiveDropEnergys.Add(DropEnergy);
}