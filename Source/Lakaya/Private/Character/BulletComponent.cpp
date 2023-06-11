// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BulletComponent.h"

#include "Net/UnrealNetwork.h"
#include "UI/GamePlayBulletWidget.h"

UBulletComponent::UBulletComponent()
{
	bWantsInitializeComponent = true;
	MaxBullets = 30;

	static ConstructorHelpers::FClassFinder<UGamePlayBulletWidget> WidgetFinder(
		TEXT("/Game/Blueprints/UMG/WBP_GamePlayBulletWidget"));

	BulletWidgetClass = WidgetFinder.Class;
}

void UBulletComponent::InitializeComponent()
{
	Super::InitializeComponent();
	if (const auto Pawn = GetOwner<APawn>())
	{
		if (Pawn->HasAuthority()) Reload();
		SetupBulletWidget(Pawn->GetController());
		Pawn->ReceiveControllerChangedDelegate.AddUniqueDynamic(this, &UBulletComponent::OnOwnerControllerChanged);
	}
}

void UBulletComponent::OnAliveStateChanged(const bool& AliveState)
{
	Super::OnAliveStateChanged(AliveState);
	if (AliveState && GetOwner()->HasAuthority()) Reload();
}

void UBulletComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	Super::OnComponentDestroyed(bDestroyingHierarchy);
	RemoveBulletWidget();
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
	SetupBulletWidget(NewController);
}

void UBulletComponent::SetupBulletWidget(AController* Controller)
{
	if (const auto LocalController = Cast<APlayerController>(Controller);
		LocalController && LocalController->IsLocalController())
	{
		// 생성되지 않았다면 생성합니다.
		if (!BulletWidget.IsValid())
			BulletWidget = CreateWidget<UGamePlayBulletWidget>(LocalController, BulletWidgetClass);

		// 정상적으로 생성되지 않는다면 리턴합니다.
		if (!BulletWidget.IsValid()) return;

		// 위젯 바인딩
		BulletWidget->AddToViewport(-1);
		BulletWidget->SetMaxBullet(GetMaxBullets());
		BulletWidget->SetRemainBullet(Bullets);

		OnMaxBulletsChanged.AddUObject(BulletWidget.Get(), &UGamePlayBulletWidget::SetMaxBullet);
		OnBulletsChanged.AddUObject(BulletWidget.Get(), &UGamePlayBulletWidget::SetRemainBullet);
	}
}

void UBulletComponent::RemoveBulletWidget()
{
	if (!BulletWidget.IsValid()) return;
	OnMaxBulletsChanged.RemoveAll(BulletWidget.Get());
	OnBulletsChanged.RemoveAll(BulletWidget.Get());
	BulletWidget->RemoveFromParent();
}
