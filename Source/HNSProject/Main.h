// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Main.generated.h"


UENUM(BlueprintType)
enum class EMovementStatus : uint8 {

	EMS_Normal UMETA(DisplayName="Normal"),
	EMS_Dead UMETA(DisplayName="Dead"),
	EMS_MAX UMETA(DisplayName="DefaultMAX")
};


UCLASS()
class HNSPROJECT_API AMain : public ACharacter
{
	GENERATED_BODY()

public:
	// Constructor
	AMain();

	UPROPERTY(EditDefaultsOnly, Category="SavedData")
	TSubclassOf<class AWeaponSaver> WeaponSaver;

	/*
	Player Controller
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller")
	class AMainPlayerController* MainPlayerController;


	/*
	Positioning the camera behind the player
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess="true"))
	class USpringArmComponent* CameraBoom;

	/*
	Follow the camera
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/*Turn rates*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	/**
		Character attributes/Player Stats in-game
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats")
	float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats")
	float Stamina;

	float InterpSpeed;

	bool bInterpToEnemy;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class AEnemy* CombatTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	FVector CombatTargetLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bHasCombatTarget;

	FORCEINLINE void SetHasCombatTarget(bool hasTarget) { bHasCombatTarget = hasTarget;  }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	EMovementStatus CurrentStatus;


	/**
		Character Equip Info
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Items")
	class AWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items")
	class AItem* ActiveOverlappingItem;

	bool bEDown;

	int32 BasicAttackCombo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bAttacking;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* CombatMontage;

	/**
		Character Sounds info
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	class USoundCue* SwordSwingASound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	class USoundCue* SwordSwingBSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	class USoundCue* OnHitSound;

	/**
		Particles and Animation Info
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UParticleSystem* HitParticles;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool CanMove(float Value);
	//Moves forward AND backward
	void MoveForward(float Value);
	//Moves left AND right
	void MoveRight(float Value);
	//Turn Yaw Rotation
	void Turn(float Value);
	//Look up Pitch Rotation
	void LookUp(float Value);
	//Turn the character at a given rate (float -> 0 - 100)
	void TurnAtRate(float Rate);
	//Look up/down at a given rate
	void LookUpAtRate(float Rate);

	//Pick up item/attack logic
	void EDown();
	void EUp();

	//pause menu logic
	bool bEscDown;
	void EscDown();
	void EscUp();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	void DecrementHealth(float Amount);
	void IncrementHealth(float Amount);
	void DecrementStamina(float Amount);
	void IncrementStamina(float Amount);
	void SetEquippedWeapon(AWeapon* WeaponToSet);

	void Die();
	virtual void Jump() override;
	void Attack();
	void SetInterpToEnemy(bool Interp);
	FRotator GetLookAtRotationYaw(FVector Target);

	void UpdateCombatTarget();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	TSubclassOf<AEnemy> EnemyFilter;

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom;  }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE AWeapon* GetEquippedWeapon() { return EquippedWeapon; }
	FORCEINLINE void SetActiveOverlappingItem(AItem* ItemToSet) { ActiveOverlappingItem = ItemToSet; }
	FORCEINLINE void SetCombatTarget(AEnemy* Target) { CombatTarget = Target;  }
	FORCEINLINE void SetEMovementStatus(EMovementStatus Status) { CurrentStatus = Status;  }
	FORCEINLINE EMovementStatus GetEMovementStatus() { return CurrentStatus;  }
	
	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	void SwitchLevel(FString LevelName);
	void SwitchSameLevel(FString LevelName);

	UFUNCTION(BlueprintCallable)
	void SaveGame();

	UFUNCTION(BlueprintCallable)
	void LoadGame(bool SetPosition);

	void LoadState();
	void SaveState();

	UFUNCTION(BlueprintCallable)
	void ResetState();
};
