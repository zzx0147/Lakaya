// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageableCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "ArmedCharacter.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API AArmedCharacter : public ADamageableCharacter
{
	GENERATED_BODY()

public:
	AArmedCharacter();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual ELifetimeCondition
	AllowActorComponentToReplicate(const UActorComponent* ComponentToReplicate) const override;

protected:
	virtual void BeginPlay() override;
	virtual void KillCharacter(AController* EventInstigator, AActor* DamageCauser) override;
	virtual void KillCharacterNotify_Implementation(AController* EventInstigator, AActor* DamageCauser) override;
	virtual void RespawnNotify_Implementation() override;
	virtual void AddInputContext() override;

public:
	/**
	 * @brief 캐릭터의 첫번째 무기를 설정합니다.
	 * @param WeaponClassRowName WeaponAssetDataTable에서의 RowName
	 */
	void SetupPrimaryWeapon(const FName& WeaponClassRowName);

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

	UPROPERTY(EditAnywhere)
	class UDataTable* WeaponClassDataTable;

	UPROPERTY(Replicated, VisibleAnywhere)
	class UWeaponComponent* PrimaryWeapon;
};
