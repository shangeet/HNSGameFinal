// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "EnemyYin.generated.h"

/**
 *
 */
UCLASS()
class HNSPROJECT_API AEnemyYin : public AEnemy
{
	GENERATED_BODY()

public:

	AEnemyYin();


	// Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		bool bCanAttack;


	// Functions

	virtual void Attack() override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
		void SetCanAttack(bool CanAttack);

	UFUNCTION(BlueprintCallable)
		bool GetCanAttack();

	virtual void AttackEnd() override;
};