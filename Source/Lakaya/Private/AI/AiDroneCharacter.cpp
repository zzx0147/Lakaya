// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AiDroneCharacter.h"

// Sets default values
AAiDroneCharacter::AAiDroneCharacter()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}


// Called when the game starts or when spawned
void AAiDroneCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AAiDroneCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAiDroneCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

