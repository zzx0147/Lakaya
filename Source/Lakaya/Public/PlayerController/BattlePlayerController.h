// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovablePlayerController.h"
#include "InputActionValue.h"
#include "BattlePlayerController.generated.h"

UCLASS()
class LAKAYA_API ABattlePlayerController : public AMovablePlayerController
{
	GENERATED_BODY()

public:
	ABattlePlayerController();

	virtual void BeginPlay() override;
	virtual void SetupEnhancedInputComponent(UEnhancedInputComponent* const& EnhancedInputComponent) override;
	virtual void SetupMappingContext(UEnhancedInputLocalPlayerSubsystem* const& InputSubsystem) override;
	virtual void OnPossessedPawnChangedCallback(APawn* OldPawn, APawn* NewPawn) override;
	
	virtual void OnCharacterBeginPlay(ACharacter* ArgCharacter);
	virtual void OnWeaponChanged(class UWeaponComponent* const& WeaponComponent);

protected:
	template <class T>
	T* CreateViewportWidget(const TSubclassOf<UUserWidget>& UserWidgetClass);

private:
	void FireStart(const FInputActionValue& Value);
	void FireStop(const FInputActionValue& Value);
	void AbilityStart(const FInputActionValue& Value);
	void AbilityStop(const FInputActionValue& Value);
	void ReloadStart(const FInputActionValue& Value);
	void ReloadStop(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, Category="Input|Weapon|Context")
	UInputMappingContext* WeaponControlContext;

	UPROPERTY(EditAnywhere, Category="Input|Weapon|Context")
	int8 WeaponContextPriority;

	UPROPERTY(EditAnywhere, Category="Input|Weapon|Actions")
	UInputAction* FireStartAction;

	UPROPERTY(EditAnywhere, Category="Input|Weapon|Actions")
	UInputAction* FireStopAction;

	UPROPERTY(EditAnywhere, Category="Input|Weapon|Actions")
	UInputAction* AbilityStartAction;

	UPROPERTY(EditAnywhere, Category="Input|Weapon|Actions")
	UInputAction* AbilityStopAction;

	UPROPERTY(EditAnywhere, Category="Input|Weapon|Actions")
	UInputAction* ReloadStartAction;

	UPROPERTY(EditAnywhere, Category="Input|Weapon|Actions")
	UInputAction* ReloadStopAction;

	UPROPERTY(EditAnywhere, Category=Widget)
	TSubclassOf<class UGamePlayKillLogWidget> KillLogClass;

	UPROPERTY(EditAnywhere, Category=Widget)
	TSubclassOf<class UGamePlayHealthWidget> HealthWidgetClass;

	UPROPERTY(EditAnywhere, Category=Widget)
	TSubclassOf<class UGamePlayBulletWidget> BulletWidgetClass;

	UPROPERTY(EditAnywhere, Category=Widget)
	TSubclassOf<class UGamePlayConsecutiveKillsWidget> ConsecutiveKillsWidgetClass;

	UPROPERTY(VisibleAnywhere)
	UGamePlayKillLogWidget* KillLogWidget;

	UPROPERTY(VisibleAnywhere)
	UGamePlayHealthWidget* HealthWidget;

	UPROPERTY(VisibleAnywhere)
	UGamePlayBulletWidget* BulletWidget;

	UPROPERTY(VisibleAnywhere)
	UGamePlayConsecutiveKillsWidget* ConsecutiveKillsWidget;

	TWeakObjectPtr<class AArmedCharacter> ArmedCharacter;
};

template <class T>
T* ABattlePlayerController::CreateViewportWidget(const TSubclassOf<UUserWidget>& UserWidgetClass)
{
	auto Widget = CreateWidget<T>(this, UserWidgetClass);
	Widget->AddToViewport();
	Widget->SetVisibility(ESlateVisibility::Collapsed);
	return Widget;
}
