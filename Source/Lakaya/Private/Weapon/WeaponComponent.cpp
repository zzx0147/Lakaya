// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/WeaponComponent.h"

#include "Weapon/WeaponAssetData.h"
#include "Weapon/WeaponFire.h"
#include "Engine/DataTable.h"
#include "Net/UnrealNetwork.h"
#include "UI/GamePlayConsecutiveKillsWidget.h"
#include "UI/GamePlayBulletWidget.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "UI/GameTimeWidget.h"

UWeaponComponent::UWeaponComponent()
{
	bReplicateUsingRegisteredSubObjectList = true;
	bIsDataSetupRequested = false;
	UpgradeLevel = 0;

	static const ConstructorHelpers::FObjectFinder<UDataTable> DataFinder(
		TEXT("DataTable'/Game/Dev/Yongwoo/DataTables/DT_WeaponAssetDataTable'"));

	if (DataFinder.Succeeded()) WeaponAssetDataTable = DataFinder.Object;

	//static const ConstructorHelpers::FObjectFinder<UDataTable> TestUpgradeDataFinder(
	//	TEXT("/Game/DataTable/DT_TestUpgrade"));

	//if (TestUpgradeDataFinder.Succeeded()) WeaponUpgradeDataTable = TestUpgradeDataFinder.Object;
}

void UWeaponComponent::ReadyForReplication()
{
	Super::ReadyForReplication();
	if (bIsDataSetupRequested)
	{
		SetupData();
		SetupUI();//리슨 서버 위젯 생성
	}
}

void UWeaponComponent::RequestSetupData(const FName& RowName)
{
	RequestedRowName = RowName;
	bIsDataSetupRequested = true;
	if (IsReadyForReplication())
	{
		SetupData();
		SetupUI();//리슨 서버 위젯 생성
	}
}

void UWeaponComponent::UpgradeWeapon()
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("Upgraded"));
	UpgradeLevel++;
	if (ConsecutiveKillsWidget != nullptr)
	{
		ConsecutiveKillsWidget->SetConsecutiveKills(UpgradeLevel);
	}

	//TODO: 무기가 업그레이드 될 때 어떤 행동을 할 지 정의합니다.
}

void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	ACharacter* MyCharacter = Cast<ACharacter>(this->GetOwner());
	if (MyCharacter != nullptr)
	{
		APlayerController* MyController = Cast<APlayerController>(MyCharacter->GetController());
		if (MyController != nullptr)
		{
			SetupUI();//클라이언트 위젯 생성
		}
	}
}

void UWeaponComponent::SetupData()
{
	auto Data = WeaponAssetDataTable->FindRow<FWeaponAssetData>(RequestedRowName, TEXT("WeaponComponent"));
	if (!Data) return;

	FireSubObject = CreateSingleSubObject<UWeaponFire>(Data->FireClass.LoadSynchronous(), Data->FireRowName);
	AbilitySubObject = CreateSingleSubObject<UWeaponAbility>(Data->AbilityClass.LoadSynchronous(),
		Data->AbilityRowName);
	ReloadSubObject = CreateSingleSubObject<UWeaponReload>(Data->ReloadClass.LoadSynchronous(), Data->ReloadRowName);

	WeaponUpgradeDataTable = Data->UpgradeTable.LoadSynchronous();

	if (!IsReplicatedSubObjectRegistered(FireSubObject))
		UE_LOG(LogTemp, Warning, TEXT("FireSubObject is NOT replicated"));
	if (!IsReplicatedSubObjectRegistered(AbilitySubObject))
		UE_LOG(LogTemp, Warning, TEXT("AbilitySubObject is NOT replicated"));
	if (!IsReplicatedSubObjectRegistered(ReloadSubObject))
		UE_LOG(LogTemp, Warning, TEXT("ReloadSubObject is NOT replicated"));
	if (WeaponUpgradeDataTable == nullptr)
		UE_LOG(LogTemp, Fatal, TEXT("UpgradeDataTable is not Found Please Check WeaponAssetDataTable"));
}

void UWeaponComponent::SetupUI()
{
	UClass* ConsecutiveKiilsWidgetClass = LoadClass<UGamePlayConsecutiveKillsWidget>(nullptr, TEXT("/Game/Blueprints/UMG/WBP_GamePlayConsecutiveKillsWidget.WBP_GamePlayConsecutiveKillsWidget_C"));

	if (ConsecutiveKiilsWidgetClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ConsecutiveKillsWidget Does not Loaded Please Check Path"));
		return;
	}

	ACharacter* MyCharacter = Cast<ACharacter>(this->GetOwner());
	if (MyCharacter != nullptr)
	{
		APlayerController* MyController = Cast<APlayerController>(MyCharacter->GetController());
		if (MyController != nullptr)
		{
			ConsecutiveKillsWidget = CreateWidget<UGamePlayConsecutiveKillsWidget>(MyController, ConsecutiveKiilsWidgetClass);

			if (ConsecutiveKillsWidget != nullptr)
			{
				ConsecutiveKillsWidget->AddToViewport();
				ConsecutiveKillsWidget->SetConsecutiveKills(0);
			}
		}
	}
}

void UWeaponComponent::OnRep_UpgradeLevel()
{
	if (ConsecutiveKillsWidget != nullptr)
	{
		ConsecutiveKillsWidget->SetConsecutiveKills(UpgradeLevel);
	}
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, FString::Printf(TEXT("OnRepUpgradeLevel")));
}

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UWeaponComponent, FireSubObject);
	DOREPLIFETIME(UWeaponComponent, AbilitySubObject);
	DOREPLIFETIME(UWeaponComponent, ReloadSubObject);
	DOREPLIFETIME(UWeaponComponent, UpgradeLevel);
}