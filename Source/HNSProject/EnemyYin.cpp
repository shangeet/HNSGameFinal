// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyYin.h"
#include "Enemy.h"
#include "GameFramework/Character.h"
#include "AIController.h"
#include "Main.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h" 
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "MainPlayerController.h"
#include "TimerManager.h"

AEnemyYin::AEnemyYin() {
	Damage = 10.f;
	bCanAttack = true;
	AttackMinTime = 0.5f;
	AttackMaxTime = 1.f;
}

void AEnemyYin::Attack() {

	UE_LOG(LogTemp, Warning, TEXT("Child attack called!"));

	if (Alive()) {
		UE_LOG(LogTemp, Warning, TEXT("Yin is alive!"));
	}

	if (bHasValidTarget) {
		UE_LOG(LogTemp, Warning, TEXT("Yin has valid target!"));
	}

	if (Alive() && bHasValidTarget) {

		if (AIController) {
			AIController->StopMovement();
			UE_LOG(LogTemp, Warning, TEXT("Setting state to attacking."));
			SetEnemyMovementStatus(EEnemyState::EMS_Attacking);
		}

		if (!bAttacking) {
			UE_LOG(LogTemp, Warning, TEXT("Set attacking to true!"));
			bAttacking = true;
		}

		if (GetCanAttack()) {
			UE_LOG(LogTemp, Warning, TEXT("Can attack is true!"));
			//play anim montage
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

			if (AnimInstance) {
				UE_LOG(LogTemp, Warning, TEXT("Playing montage"));
				AnimInstance->Montage_Play(CombatMontage, 1.35f);

				int32 AttackType = FMath::RandHelper(2);
				
				switch (AttackType) {

					case 0: 
					{
						AnimInstance->Montage_JumpToSection(FName("Attack"), CombatMontage);
					}
					case 1: 
					{
						AnimInstance->Montage_JumpToSection(FName("AttackB"), CombatMontage);
					}
					case 2:
					{
						AnimInstance->Montage_JumpToSection(FName("AttackC"), CombatMontage);
					}
				}
			}

			if (SwingSound) {
				UGameplayStatics::PlaySound2D(this, SwingSound);
			}
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Can Attack is FALSE"));
		}
	}
}

float AEnemyYin::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) {

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && CombatMontage) {

		AnimInstance->Montage_Play(CombatMontage, 1.f);
		AnimInstance->Montage_JumpToSection(FName("HitReact"), CombatMontage);

	}

	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AEnemyYin::AttackEnd() {

	UE_LOG(LogTemp, Warning, TEXT("ATTACK END CALLE!"));

	bAttacking = false;
	bInterpToMain = false;

	if (bOverlappingCombatSphere) {
		UE_LOG(LogTemp, Warning, TEXT("Still overlapping combat sphere!"));
		float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemyYin::Attack, AttackTime);
	}
}

void AEnemyYin::SetCanAttack(bool CanAttack) {
	if (CanAttack) {
		UE_LOG(LogTemp, Warning, TEXT("Setting CanAttack to true"));
		if (bOverlappingCombatSphere) {
			UE_LOG(LogTemp, Warning, TEXT("Still overlapping combat sphere!"));
			float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
			GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemyYin::Attack, AttackTime);
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Setting CanAttack to false"));
	}
	
	bCanAttack = CanAttack;
}

bool AEnemyYin::GetCanAttack() {
	return bCanAttack;
}