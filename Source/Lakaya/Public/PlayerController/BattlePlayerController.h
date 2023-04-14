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

	virtual void SetupEnhancedInputComponent(UEnhancedInputComponent* const& EnhancedInputComponent) override;
	virtual void SetupMappingContext(UEnhancedInputLocalPlayerSubsystem* const& InputSubsystem) override;
	virtual void OnPossessedPawnChangedCallback(APawn* ArgOldPawn, APawn* NewPawn) override;

private:
	void PrimaryStart(const FInputActionValue& Value);
	void PrimaryStop(const FInputActionValue& Value);
	void SecondStart(const FInputActionValue& Value);
	void SecondStop(const FInputActionValue& Value);
	void FireStart(const FInputActionValue& Value);
	void FireStop(const FInputActionValue& Value);
	void AbilityStart(const FInputActionValue& Value);
	void AbilityStop(const FInputActionValue& Value);
	void ReloadStart(const FInputActionValue& Value);
	void ReloadStop(const FInputActionValue& Value);

protected:
	UPROPERTY(EditAnywhere, Category=Widget)
	TSubclassOf<class UGamePlayHealthWidget> HealthWidgetClass;

	UPROPERTY(EditAnywhere, Category=Widget)
	TSubclassOf<class UGamePlayConsecutiveKillsWidget> ConsecutiveKillsWidgetClass;

	UPROPERTY(EditAnywhere, Category=Widget)
	TSubclassOf<class UDirectionalDamageIndicator> DamageIndicatorClass;

private:
	UPROPERTY(EditAnywhere, Category="Input|Weapon|Context")
	UInputMappingContext* WeaponControlContext;

	UPROPERTY(EditAnywhere, Category="Input|Weapon|Context")
	int8 WeaponContextPriority;

	UPROPERTY(EditAnywhere, Category="Input|Weapon|Actions")
	UInputAction* PrimaryStartAction;

	UPROPERTY(EditAnywhere, Category="Input|Weapon|Actions")
	UInputAction* PrimaryStopAction;

	UPROPERTY(EditAnywhere, Category="Input|Weapon|Actions")
	UInputAction* SecondStartAction;

	UPROPERTY(EditAnywhere, Category="Input|Weapon|Actions")
	UInputAction* SecondStopAction;

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

	// 캐릭터 전용 위젯 컴포넌트의 클래스 정보를 불러올 데이터 테이블입니다.
	UPROPERTY(EditAnywhere)
	class UDataTable* CharacterWidgetComponentTable;

	// 캐릭터 전용 위젯들을 관리하는 컴포넌트입니다.
	UPROPERTY(VisibleAnywherem, Transient)
	class UCharacterWidgetComponent* CharacterWidgetComponent;

	UPROPERTY(VisibleAnywhere, Transient)
	UGamePlayHealthWidget* HealthWidget;

	UPROPERTY(VisibleAnywhere, Transient)
	UGamePlayConsecutiveKillsWidget* ConsecutiveKillsWidget;

	UPROPERTY(VisibleAnywhere, Transient)
	UDirectionalDamageIndicator* DamageIndicatorWidget;

	TWeakObjectPtr<class AArmedCharacter> ArmedCharacter;
};
