// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/GunComponent.h"

#include "Weapon/GunAssetData.h"
#include "Weapon/WeaponUpgradeData.h"
#include "Weapon/RiffleFireCore.h"
#include "Engine/DataTable.h"
#include "Net/UnrealNetwork.h"
#include "UI/GamePlayBulletWidget.h"



void UGunComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UGunComponent, MagazineCapacity);
	DOREPLIFETIME(UGunComponent, RemainBullets);
}

UGunComponent::UGunComponent()
{
	static const ConstructorHelpers::FObjectFinder<UDataTable> DataFinder(
		TEXT("DataTable'/Game/Dev/Yongwoo/DataTables/DT_GunAssetDataTable'"));

	if (DataFinder.Succeeded()) WeaponAssetDataTable = DataFinder.Object;
}

void UGunComponent::Reload()
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("Reloading"));
	RemainBullets = MagazineCapacity;

	if (BulletWidget != nullptr)
		BulletWidget->OnChangeRemainBullets(RemainBullets);
}

void UGunComponent::Reload(const uint16& Bullets)
{
	RemainBullets += Bullets;
	if (RemainBullets > MagazineCapacity) RemainBullets = MagazineCapacity;

	if (BulletWidget != nullptr)
		BulletWidget->OnChangeRemainBullets(RemainBullets);
}

bool UGunComponent::CostBullets(const uint16& Bullets)
{
	if (Bullets > RemainBullets) return false;
	RemainBullets -= Bullets;

	if (BulletWidget != nullptr)
		BulletWidget->OnChangeRemainBullets(RemainBullets);

	return true;
}

void UGunComponent::UpgradeWeapon()
{
	Super::UpgradeWeapon();
	FWeaponUpgradeData* Data = WeaponUpgradeDataTable->FindRow<FWeaponUpgradeData>(FName(FString::FromInt(UpgradeLevel)), TEXT("GunComponentUpgradeWeapon"));

	for (auto temp : Data->UpgradeDataArray)
	{
		switch (temp.UpgradeType)
		{
		case UpgradeTypeEnum::Bullet:
		{
			MagazineCapacity += (int8)FCString::Atoi(*temp.UpgradeData);
			if (BulletWidget != nullptr)
				BulletWidget->OnChangeMagazineCapacity(MagazineCapacity);
			break;
		}
		case UpgradeTypeEnum::Damage:
		{
			auto RiffleFireCoreRef = Cast<URiffleFireCore>(FireSubObject);
			float AdditionalDamage = AdditionalDamage = FCString::Atof(*temp.UpgradeData);
			if (RiffleFireCoreRef != nullptr)
				RiffleFireCoreRef->SetBaseDamage(RiffleFireCoreRef->GetBaseDamage() + AdditionalDamage);
			break;
		}
		case UpgradeTypeEnum::Range:
		{
			UE_LOG(LogTemp, Warning, TEXT("RangeUpgrade was not Developed"));
			break;
		}
		case UpgradeTypeEnum::SkillDamage:
		{
			UE_LOG(LogTemp, Warning, TEXT("RangeUpgrade was not Developed"));
			break;
		}
		case UpgradeTypeEnum::SkillRange:
		{
			UE_LOG(LogTemp, Warning, TEXT("RangeUpgrade was not Developed"));
			break;
		}
		}
	}

}

void UGunComponent::SetupData()
{
	Super::SetupData();
	auto Data = WeaponAssetDataTable->FindRow<FGunAssetData>(RequestedRowName, TEXT("GunComponent"));
	if (!Data) return;

	RemainBullets = MagazineCapacity = OriginMagazineCapacity = Data->Magazine;
}

void UGunComponent::SetupUI()
{
	Super::SetupUI();
	UClass* BulletWidgetClass = LoadClass<UGamePlayBulletWidget>(nullptr, TEXT("/Game/Blueprints/UMG/WBP_GamePlayBulletWidget.WBP_GamePlayBulletWidget_C"));
	if (BulletWidgetClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("BulletWidget Does Not Loaded Please Check Path"));
		return;
	}

	ACharacter* MyCharacter = Cast<ACharacter>(this->GetOwner());
	if (MyCharacter != nullptr)
	{
		APlayerController* MyController = Cast<APlayerController>(MyCharacter->GetController());
		if (MyController != nullptr)
		{
			BulletWidget = CreateWidget<UGamePlayBulletWidget>(MyController, BulletWidgetClass);
			if (BulletWidget != nullptr)
			{
				BulletWidget->AddToViewport();
				BulletWidget->OnChangeMagazineCapacity(MagazineCapacity);
				BulletWidget->OnChangeRemainBullets(RemainBullets);
			}
		}
	}

}

void UGunComponent::OnRep_MagazineCapacity()
{
	if(BulletWidget != nullptr)
		BulletWidget->OnChangeMagazineCapacity(MagazineCapacity);
}

void UGunComponent::OnRep_RemainBullets()
{
	if (BulletWidget != nullptr)
		BulletWidget->OnChangeRemainBullets(RemainBullets);
}
