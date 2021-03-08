// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"
#include "Main.h"
#include "particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/World.h"

APickup::APickup() {

	HealthAmountToGain = 0.f;
	StaminaAmountToGain = 0.f;
}

void APickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	UE_LOG(LogTemp, Warning, TEXT("Overalp PIK beg"));


	if (OtherActor) {
		AMain* Main = Cast<AMain>(OtherActor);

		if (Main) {

			if (OverlapParticles) {
				UWorld* World = GetWorld();
				UGameplayStatics::SpawnEmitterAtLocation(World, OverlapParticles, GetActorLocation(), FRotator(0.f), true);
			}

			if (OverlapSound) {
				UGameplayStatics::PlaySound2D(this, OverlapSound);
			}

			Main->IncrementHealth(HealthAmountToGain);
			Main->IncrementStamina(StaminaAmountToGain);	
			Destroy();
		}

	}



}

void APickup::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	UE_LOG(LogTemp, Warning, TEXT("Overalp PIK beg"));

}