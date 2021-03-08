// Fill out your copyright notice in the Description page of Project Settings.


#include "MainAnimInstance.h"
#include "GameFramework/Pawn.h"
#include "Main.h"
#include "GameFramework/CharacterMovementComponent.h"

void UMainAnimInstance::NativeInitializeAnimation() {

	if (Pawn == NULL) {
		Pawn = TryGetPawnOwner();
	}


}

void UMainAnimInstance::UpdateAnimationProperties() {
	if (Pawn == NULL) {
		Pawn = TryGetPawnOwner();
	}

	if (Pawn) {
		FVector Speed = Pawn->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
		MovementSpeed = LateralSpeed.Size();

		bIsInAir = Pawn->GetMovementComponent()->IsFalling();

		if (Main == nullptr) {
			Main = Cast<AMain>(Pawn);
		}
	}
}