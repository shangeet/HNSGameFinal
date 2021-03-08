// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"

// Sets default values
AFloatingPlatform::AFloatingPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	InterpolationSpeed = 20.0f;
	InterpolationTime = 1.f;
}

// Called when the game starts or when spawned
void AFloatingPlatform::BeginPlay()
{
	Super::BeginPlay();
	StartPoint = GetActorLocation();
	EndPoint += StartPoint;

	bInterping = false;

	GetWorldTimerManager().SetTimer(InterpTimer, this, &AFloatingPlatform::ToggleInterping, InterpolationTime);
	InterpolationDistance = (EndPoint - StartPoint).Size();
}

// Called every frame
void AFloatingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bInterping) {
		FVector CurrentLocation = GetActorLocation();
		FVector Interpol = FMath::VInterpTo(CurrentLocation, EndPoint, DeltaTime, InterpolationSpeed);
		SetActorLocation(Interpol);

		//Time to pause and swap?
		float DistanceTravelled = (GetActorLocation() - StartPoint).Size();
		if (InterpolationDistance - DistanceTravelled <= 100.f) {
			ToggleInterping();
			GetWorldTimerManager().SetTimer(InterpTimer, this, &AFloatingPlatform::ToggleInterping, InterpolationTime);
			SwapVectors(StartPoint, EndPoint);
		}
	}
}

void AFloatingPlatform::ToggleInterping() {
	bInterping = !bInterping;
}

void AFloatingPlatform::SwapVectors(FVector& A, FVector& B) {
	FVector temp = A;
	A = B;
	B = temp;
}
