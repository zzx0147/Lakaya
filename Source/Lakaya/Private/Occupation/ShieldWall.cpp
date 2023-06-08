// Fill out your copyright notice in the Description page of Project Settings.


#include "Occupation/ShieldWall.h"

// Sets default values
AShieldWall::AShieldWall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AShieldWall::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShieldWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

