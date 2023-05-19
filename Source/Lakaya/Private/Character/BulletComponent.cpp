// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BulletComponent.h"

#include "Net/UnrealNetwork.h"
#include "UI/GamePlayBulletWidget.h"

UBulletComponent::UBulletComponent()
{
	MaxBullets = 30;

	static ConstructorHelpers::FClassFinder<UGamePlayBulletWidget> WidgetFinder(
		TEXT("/Game/Blueprints/UMG/WBP_GamePlayBulletWidget"));

	if (WidgetFinder.Succeeded()) BulletWidgetClass = WidgetFinder.Class;
}

void UBulletComponent::OnAliveStateChanged(const bool& AliveState)
{
	Super::OnAliveStateChanged(AliveState);
	if (AliveState && GetOwner()->HasAuthority()) Reload();
}

void UBulletComponent::OnRegister()
{
	Super::OnRegister();
	if (const auto Pawn = GetOwner<APawn>())
	{
		if (Pawn->HasAuthority()) Reload();

		if (const auto LocalController = Pawn->GetController<APlayerController>();
			LocalController && LocalController->IsLocalController())
			SetupBulletWidget(LocalController);

		Pawn->ReceiveControllerChangedDelegate.AddUniqueDynamic(this, &UBulletComponent::OnOwnerControllerChanged);
	}
}

void UBulletComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UBulletComponent, Bullets, COND_OwnerOnly);
}

bool UBulletComponent::CostBullet(const uint16& Value)
{
	if (Bullets < Value) return false;
	Bullets -= Value;
	OnBulletsChanged.Broadcast(Bullets);
	return true;
}

void UBulletComponent::Reload()
{
	Bullets = MaxBullets;
	OnBulletsChanged.Broadcast(Bullets);
}

void UBulletComponent::OnRep_Bullets()
{
	OnBulletsChanged.Broadcast(Bullets);
}

void UBulletComponent::OnRep_MaxBullets()
{
	OnMaxBulletsChanged.Broadcast(MaxBullets);
}

void UBulletComponent::OnOwnerControllerChanged(APawn* Pawn, AController* OldController, AController* NewController)
{
	// 이전에 총알 위젯이 바인딩되어있었다면 제거합니다.
	RemoveBulletWidget();

	// 새로운 컨트롤러가 로컬 플레이어 컨트롤러라면 위젯을 바인딩합니다.
	if (const auto LocalController = Cast<APlayerController>(NewController);
		LocalController && LocalController->IsLocalController())
		SetupBulletWidget(LocalController);
}

void UBulletComponent::SetupBulletWidget(APlayerController* LocalController)
{
	// 생성되지 않았다면 생성합니다.
	if (!BulletWidget.IsValid()) BulletWidget = CreateWidget<UGamePlayBulletWidget>(LocalController, BulletWidgetClass);

	// 정상적으로 생성되지 않는다면 리턴합니다.
	if (!BulletWidget.IsValid()) return;

	// 위젯 바인딩
	BulletWidget->AddToViewport();
	BulletWidget->SetMaxBullet(GetMaxBullets());
	BulletWidget->SetRemainBullet(Bullets);

	OnMaxBulletsChanged.AddUObject(BulletWidget.Get(), &UGamePlayBulletWidget::SetMaxBullet);
	OnBulletsChanged.AddUObject(BulletWidget.Get(), &UGamePlayBulletWidget::SetRemainBullet);
}

void UBulletComponent::RemoveBulletWidget()
{
	if (!BulletWidget.IsValid()) return;
	OnMaxBulletsChanged.RemoveAll(BulletWidget.Get());
	OnBulletsChanged.RemoveAll(BulletWidget.Get());
	BulletWidget->RemoveFromParent();
}
