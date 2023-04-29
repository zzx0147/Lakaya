// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/InteractableCharacter.h"
#include "Occupation/PlayerTeam.h"
#include "OccupationCharacter.generated.h"

UCLASS()
class LAKAYA_API AOccupationCharacter : public AInteractableCharacter
{
	GENERATED_BODY()

public:
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                         AActor* DamageCauser) override;

protected:
	virtual void KillCharacter(AController* EventInstigator, AActor* DamageCauser) override;
	
public:
	// 대상 액터가 같은 팀인지 판별합니다.
	bool IsSameTeam(AActor* const& Other) const;

	/**
	 * @brief 캐릭터의 팀을 설정합니다.
	 * @param Team 어떤 팀으로 설정할지 나타냅니다.
	 */
	virtual void SetTeam(const EPlayerTeam& Team);
};
