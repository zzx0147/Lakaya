// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AiDronePawn.h"

// Sets default values
AAiDronePawn::AAiDronePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	FloatingMovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingMovementComponent"));
}


// Called when the game starts or when spawned
void AAiDronePawn::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AAiDronePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAiDronePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

