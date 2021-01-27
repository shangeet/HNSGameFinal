// Fill out your copyright notice in the Description page of Project Settings.


#include "Floater.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AFloater::AFloater()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InitialLocation = FVector(0.0f);
	PlacedLocation = FVector(0.0f);
	WorldOrigin = FVector(0.0f, 0.0f, 0.0f);
	InitialDirection = FVector(0.0f, 0.0f, 0.0f);
	bShouldFloat = false;
	bInitializeFloaterLocation = false;
	InitialForce = FVector(200000.f, 0.0f, 0.0f);
	InitialTorque = FVector(200000.f, 0.0f, 0.0f);
}

// Called when the game starts or when spawned
void AFloater::BeginPlay()
{
	Super::BeginPlay();

	float InitialX = FMath::FRand();
	float InitialY = FMath::FRand();
	float InitialZ = FMath::FRand();
	
	InitialLocation.X = InitialX;
	InitialLocation.Y = InitialY;
	InitialLocation.Z = InitialZ; 

	InitialLocation *= 20.f; 

	PlacedLocation = GetActorLocation();

	if (bInitializeFloaterLocation) {
		SetActorLocation(InitialLocation);
	}

	
}

// Called every frame
void AFloater::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (bShouldFloat) {
		FHitResult HitResult;
		
		AddActorLocalOffset(FVector(1.0f, 1.0f, 1.0f), false, &HitResult);
		FVector HitLocation = HitResult.Location;
	}

	FRotator Rotation = FRotator(0.0f, 1.0f, 0.0f);
	AddActorLocalRotation(Rotation);

}

