// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharAnimInstance.generated.h"
/**
 * 
 */
UCLASS()
class LAKAYA_API UCharAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire")
	bool bIsGPSFire;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reload")
	bool bIsReload;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crouch")
	bool bIsCrouch;

	UFUNCTION(BlueprintCallable, Category = "Fire")
	void SetIsGPSFire(bool GPSFireValue);
	
	UFUNCTION(BlueprintCallable, Category = "Reload")
	void SetIsReload(bool ReloadValue);

	UFUNCTION(BlueprintCallable, Category = "Crouch")
	void SetIsCrouching(bool CrouchValue);
	
};
