   // Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"
#include "GameFramework/Pawn.h"
#include "Enemy.h"
#include "GameFramework/CharacterMovementComponent.h"

void UEnemyAnimInstance::NativeInitializeAnimation() {

	if (Pawn == nullptr) {
		Pawn = TryGetPawnOwner();

	}

}

void UEnemyAnimInstance::UpdateAnimationProperties() {

	if (Pawn == NULL) {
		
		Pawn = TryGetPawnOwner();


	}		
	
	if (Pawn) {
		FVector Speed = Pawn->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
		MovementSpeed = LateralSpeed.Size();

		if (Enemy == nullptr) {
			Enemy = Cast<AEnemy>(Pawn);
		}
	}

}