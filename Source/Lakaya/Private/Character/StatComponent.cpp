// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/StatComponent.h"

#include "Character/StatSetupData.h"
#include "Engine/DataTable.h"
#include "Net/UnrealNetwork.h"

UStatComponent::UStatComponent()
{
	static const ConstructorHelpers::FObjectFinder<UDataTable> TableFinder(
		TEXT("/Script/Engine.DataTable'/Game/Dev/Yongwoo/DataTables/DT_StatSetupDataTable.DT_StatSetupDataTable'"));

	if (TableFinder.Succeeded()) StatSetupDataTable = TableFinder.Object;
}

void UStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UStatComponent, WalkSpeed);
	DOREPLIFETIME(UStatComponent, AdditiveRunSpeed);
	DOREPLIFETIME_CONDITION(UStatComponent, MaximumHealth, COND_OwnerOnly);
}


void UStatComponent::SetupStat(const FName& RowName)
{
	const auto Data = StatSetupDataTable->FindRow<FStatSetupData>(RowName,TEXT("SetupStat"));
	if (!Data) return;
	SetupStatServer(Data);
}

void UStatComponent::RegisterAdditionalWalkSpeed(const float& Value)
{
	WalkSpeed += Value;
	OnWalkSpeedChanged.Broadcast(WalkSpeed);
	OnRunSpeedChanged.Broadcast(GetRunSpeed());
}

void UStatComponent::RegisterAdditionalWalkSpeed(const float& Value, const float& Time)
{
	RegisterAdditionalWalkSpeed(Value);
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, [this,Value] { RegisterAdditionalWalkSpeed(-Value); }, Time, false);
}

void UStatComponent::RegisterAdditionalRunSpeed(const float& Value)
{
	AdditiveRunSpeed += Value;
	OnRunSpeedChanged.Broadcast(GetRunSpeed());
}

void UStatComponent::RegisterAdditionalRunSpeed(const float& Value, const float& Time)
{
	RegisterAdditionalRunSpeed(Value);
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, [this,Value] { RegisterAdditionalRunSpeed(-Value); }, Time, false);
}

void UStatComponent::SetupStatServer(const FStatSetupData* Data)
{
	WalkSpeed = Data->BaseWalkSpeed;
	OnWalkSpeedChanged.Broadcast(WalkSpeed);

	AdditiveRunSpeed = Data->BaseRunAdditive;
	OnRunSpeedChanged.Broadcast(GetRunSpeed());

	MaximumHealth = Data->BaseMaximumHealth;
	OnMaximumHealthChanged.Broadcast(MaximumHealth);
}

void UStatComponent::OnRep_WalkSpeed()
{
	OnWalkSpeedChanged.Broadcast(WalkSpeed);
	OnRunSpeedChanged.Broadcast(GetRunSpeed());
}

void UStatComponent::OnRep_RunSpeed()
{
	OnRunSpeedChanged.Broadcast(GetRunSpeed());
}

void UStatComponent::OnRep_MaximumHealth()
{
	OnMaximumHealthChanged.Broadcast(MaximumHealth);
}
