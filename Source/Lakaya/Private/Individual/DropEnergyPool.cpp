#include "Individual/DropEnergyPool.h"

ADropEnergyPool::ADropEnergyPool()
{
	// TODO : 테스트를 위해 true. 나중에 false로 수정해야 함
	PrimaryActorTick.bCanEverTick = true;

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

	// TODO : 테스트를 위한 코드들. 나중에 전부 주석처리 및 삭제해야 함
	// if (!InitPool) return;
	//
	// ElapsedTime += DeltaTime;
	//
	// if (FMath::IsNearlyEqual(ElapsedTime, 1.0f, 0.1f))
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("Tick_InactiveDropEnergys Num : %d"), InactiveDropEnergys.Num());
	// 	
	// 	ElapsedTime = 0.0f;
	// }
}

void ADropEnergyPool::Initialize(int32 PoolSize)
{
	for (int32 i = 0 ; i < PoolSize; i++)
	{
		AIndividualDropEnergy* DropEnergy = GetWorld()->SpawnActor<AIndividualDropEnergy>(DropEnergyClass);
		InactiveDropEnergys.Add(DropEnergy);
		UE_LOG(LogTemp, Warning, TEXT("Initialize_InactiveDropEnergys Num : %d"), InactiveDropEnergys.Num());
	}

	InitPool = true;
}

AIndividualDropEnergy* ADropEnergyPool::GetDropEnergy()
{
	if (InactiveDropEnergys.Num() > 0)
	{
		AIndividualDropEnergy* DropEnergy = InactiveDropEnergys.Pop();
		DropEnergy->Activate();
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
		AIndividualDropEnergy* PopDropEnergy = InactiveDropEnergys.Pop();
		PopDropEnergy->Activate();
		return PopDropEnergy;
	}
}

void ADropEnergyPool::ReturnDropEnergy(AIndividualDropEnergy* DropEnergy)
{
	if (DropEnergy == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ADropEnergyPool_ReturnDropEnergy is null."));
		return;
	}
	
	DropEnergy->Deactivate();
	InactiveDropEnergys.Add(DropEnergy);
}