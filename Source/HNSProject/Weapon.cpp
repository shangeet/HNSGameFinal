// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Main.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/BoxComponent.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h" 
#include "Sound/SoundCue.h"

AWeapon::AWeapon() {

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(GetRootComponent());
	WeaponState = EWeaponState::EWS_Pickup;

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Combat Collision"));
	CombatCollision->SetupAttachment(GetRootComponent());

	Damage = 25.f;
}

void AWeapon::BeginPlay() {
	Super::BeginPlay();

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapEnd);

	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void AWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	UE_LOG(LogTemp, Warning, TEXT("Overalp weapon"));
	if (WeaponState == EWeaponState::EWS_Pickup && OtherActor) {
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main) {
			UE_LOG(LogTemp, Warning, TEXT("Main is Valid, attaching overlapping item!!!!"));
			Main->SetActiveOverlappingItem(this);
		}
	}
}

void AWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	if (OtherActor) {
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main) {
			UE_LOG(LogTemp, Warning, TEXT("Main is Valid!!"));
			Main->SetActiveOverlappingItem(nullptr);
		}
	}
}

void AWeapon::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	
	UE_LOG(LogTemp, Warning, TEXT("Sword HIT ENEMY!!!!"));
	if (OtherActor) {

		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy) {

			if (Enemy->HitParticles) {
				const USkeletalMeshSocket* WeaponSocket = SkeletalMesh->GetSocketByName("WeaponSocket");
				if (WeaponSocket) {
					UE_LOG(LogTemp, Warning, TEXT("PARTICLE SPAWNED!!!!"));
					FVector WeaponParticleSpawnLocation = WeaponSocket->GetSocketLocation(SkeletalMesh);
					
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Enemy->HitParticles, WeaponParticleSpawnLocation, FRotator(0.f), true);
				}
				
			}

			if (Enemy->OnHitSound) {
				UGameplayStatics::PlaySound2D(this, Enemy->OnHitSound);
			}

			if (DamageTypeClass) {
				UGameplayStatics::ApplyDamage(Enemy, Damage, WeaponInstigator, this, DamageTypeClass);
			}
		}
	}
}

void AWeapon::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {

}


void AWeapon::Equip(AMain* Char) {

	if (Char) {

		SetInstiagtor(Char->GetController());
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

		SkeletalMesh->SetSimulatePhysics(false);

		const USkeletalMeshSocket* RightHandSocket = Char->GetMesh()->GetSocketByName("RightHandSocket");

		if (RightHandSocket) {
			RightHandSocket->AttachActor(this, Char->GetMesh());
			Char->SetEquippedWeapon(this);
			Char->SetActiveOverlappingItem(nullptr);
			SetWeaponState(EWeaponState::EWS_Equipped);
		}

	}
}

void AWeapon::ActivateCollision() {
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}



void AWeapon::DeactivateCollision() {
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}