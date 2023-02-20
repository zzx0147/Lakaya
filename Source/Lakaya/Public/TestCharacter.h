#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Character.h"
#include "TestCharacter.generated.h"

UCLASS()
class LAKAYA_API ATestCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATestCharacter();
	
	class UHUDWidget* GetHUDWidget() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UHUDWidget> HUDWidgetClass;
	class UHUDWidget* HUDWidget;

protected:
	virtual void BeginPlay() override;

	void SetSpringArm();
public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PostInitializeComponents() override;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* Camera;

private:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);

};