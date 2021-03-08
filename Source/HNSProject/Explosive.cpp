// Fill out your copyright notice in the Description page of Project Settings.


#include "Explosive.h"
#include "Main.h"
#include "particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/World.h"

AExplosive::AExplosive() {
	Damage = 15.f;
}

void AExplosive::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	UE_LOG(LogTemp, Warning, TEXT("Overalp EXP beg"));

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

			Main->DecrementHealth(Damage);
			Destroy();
		}
	}

	

}

void AExplosive::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	UE_LOG(LogTemp, Warning, TEXT("Overalp EXP beg"));

}