// Fill out your copyright notice in the Description page of Project Settings.


#include "Main.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Weapon.h"
#include "Kismet/KismetMathLibrary.h"
#include "Enemy.h"
#include "MainPlayerController.h"
#include "HNSSaveGame.h"
#include "WeaponSaver.h"

// Sets default values
AMain::AMain()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Create Camera Boom (pulls towards the player if there's a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 200.f;
	CameraBoom->bUsePawnControlRotation = true; //Rotate arm based on controller
	
	//Set size for collision capsule
	GetCapsuleComponent()->SetCapsuleSize(40.f, 110.f);

	//Create follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	//Attach the camera to the end of the boom. Let boom adjust to match controller
	FollowCamera->bUsePawnControlRotation = false;

	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;

	//Don't rotate when controller rotates
	//Let that just affect the camera
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	//Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; //Char moves in the dir of input
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.f, 0.0f); //move at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 650.f;
	GetCharacterMovement()->AirControl = 0.2f;

	bEDown = false;
	bEscDown = false;

	MaxHealth = 100.f;

	Health = 100.f;

	MaxStamina = 100.f;

	Stamina = 100.f;

	BasicAttackCombo = 0;

	InterpSpeed = 15.f;
	bInterpToEnemy = false;
	bHasCombatTarget = false;

	SetEMovementStatus(EMovementStatus::EMS_Normal);
}

// Called when the game starts or when spawned
void AMain::BeginPlay()
{
	Super::BeginPlay();

	MainPlayerController = Cast<AMainPlayerController>(GetController());
	
}

// Called every frame
void AMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bInterpToEnemy && CombatTarget) {

		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);
		SetActorRotation(InterpRotation);
	}

	if (CombatTarget) {
		CombatTargetLocation = CombatTarget->GetActorLocation();

		if (MainPlayerController) {
			MainPlayerController->EnemyTargetLocation = CombatTargetLocation;
		}
	}

}

// Called to bind functionality to input
void AMain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UE_LOG(LogTemp, Warning, TEXT("Setup player input component!"));
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMain::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &AMain::EDown);
	PlayerInputComponent->BindAction("Equip", IE_Released, this, &AMain::EUp);
	PlayerInputComponent->BindAction("ESC", IE_Pressed, this, &AMain::EscDown);
	PlayerInputComponent->BindAction("ESC", IE_Released, this, &AMain::EscUp);

	//Bind axis
	PlayerInputComponent->BindAxis("MoveForward", this, &AMain::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMain::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAxis("TurnRate", this, &AMain::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpAtRate", this, &AMain::LookUpAtRate);

}

void AMain::MoveForward(float Value) {

	if (Controller != NULL && Value != 0.0f && !bAttacking && (CurrentStatus != EMovementStatus::EMS_Dead)) {
		//Find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		FString TheFloatStr = FString::SanitizeFloat(Value);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMain::MoveRight(float Value) {
	if (Controller != NULL && Value != 0.0f && !bAttacking && (CurrentStatus != EMovementStatus::EMS_Dead)) {
		//Find out which way is Right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void AMain::TurnAtRate(float Rate) {
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMain::LookUpAtRate(float Rate) {
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}


void AMain::EDown() {
	bEDown = true;

	if (CurrentStatus == EMovementStatus::EMS_Dead) return;

	if (ActiveOverlappingItem) {
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
		if (Weapon) {
			Weapon->Equip(this);
			SetActiveOverlappingItem(nullptr);
		}
	}
	else if (EquippedWeapon) {
		Attack();
	}
}

void AMain::EUp() {
	bEDown = false;
}

void AMain::EscDown() {
	bEscDown = true;

	if (MainPlayerController) {
		MainPlayerController->TogglePauseMenu();
	}
}

void AMain::EscUp() {
	bEscDown = false;
}

void AMain::IncrementHealth(float Amount) {

	Health += Amount;
	if (Health > MaxHealth) {
		Health = MaxHealth;
	}
}

void AMain::DecrementHealth(float Amount) {

	if (Health - Amount <= 0.f) {
		Health -= Amount;
		Die();
	}
	else {
		Health -= Amount;
	}
}

void AMain::DecrementStamina(float Amount) {

	if (Stamina - Amount <= 0.f) {
		Stamina = 0.f;
	}
	else {
		Stamina -= Amount;
	}

}

void AMain::IncrementStamina(float Amount) {

	Stamina += Amount;
	if (Stamina > MaxStamina) {
		Stamina = MaxStamina;
	}
}

void AMain::SetEquippedWeapon(AWeapon* WeaponToSet) { 

	if (EquippedWeapon) {
		EquippedWeapon->Destroy();
	}

	EquippedWeapon = WeaponToSet; 

}

void AMain::Attack() {

	if (!bAttacking && (CurrentStatus != EMovementStatus::EMS_Dead)) {
		bAttacking = true;
		SetInterpToEnemy(true);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && CombatMontage) {

			if (BasicAttackCombo % 2 == 0) {
				UE_LOG(LogTemp, Warning, TEXT("Attacking with Attack_1!"));
				AnimInstance->Montage_Play(CombatMontage, 2.5f);
				AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
				BasicAttackCombo += 1;

				//Play sound 
				if (this->SwordSwingASound) {
					UGameplayStatics::PlaySound2D(this, this->SwordSwingASound);
				}
			}
			else if (BasicAttackCombo % 2 == 1) {
				UE_LOG(LogTemp, Warning, TEXT("Attacking with Attack_2!"));
				AnimInstance->Montage_Play(CombatMontage, 2.5f);
				AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);
				BasicAttackCombo -= 1;
				//Play sound 
				if (this->SwordSwingBSound) {
					UGameplayStatics::PlaySound2D(this, this->SwordSwingBSound);
				}
			}
		}
	}
}

void AMain::AttackEnd() {
	bAttacking = false;
	SetInterpToEnemy(false);
}


void AMain::SetInterpToEnemy(bool Interp) {
	bInterpToEnemy = Interp;
}

FRotator AMain::GetLookAtRotationYaw(FVector Target) {
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);
	return LookAtRotationYaw;
}

float AMain::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) {

	if (Health - DamageAmount <= 0.f) {
		Health -= DamageAmount;
		Die();

		if (DamageCauser) {

			AEnemy* Enemy = Cast<AEnemy>(DamageCauser);
			if (Enemy) {
				Enemy->bHasValidTarget = false;
			}
		}
	}
	else {
		Health -= DamageAmount;
	}

	return DamageAmount;

}


void AMain::Die() {
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (CurrentStatus == EMovementStatus::EMS_Dead) return;

	if (AnimInstance && CombatMontage) {

		AnimInstance->Montage_Play(CombatMontage, 4.35f);
		AnimInstance->Montage_JumpToSection(FName("Death"));
	}
	SetEMovementStatus(EMovementStatus::EMS_Dead);
}

void AMain::Jump() {
	if (CurrentStatus != EMovementStatus::EMS_Dead) {
		ACharacter::Jump();
	}
}

void AMain::UpdateCombatTarget() {

	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, EnemyFilter);

	if (OverlappingActors.Num() == 0) { 
		if (MainPlayerController) {
			MainPlayerController->RemoveEnemyHealthBar();
		}
		return;
	} else {
		AEnemy* ClosestEnemy = Cast<AEnemy>(OverlappingActors[0]);
		FVector MainLocation = GetActorLocation();

		if (ClosestEnemy) {
			float MinDistance = (ClosestEnemy->GetActorLocation() - GetActorLocation()).Size();

			for (auto Actor : OverlappingActors) {

				AEnemy* Enemy = Cast<AEnemy>(Actor);
				if (Enemy) {
					float DistanceToActor = (Enemy->GetActorLocation() - GetActorLocation()).Size();
				
					if (DistanceToActor < MinDistance) {
						ClosestEnemy = Enemy;
						MinDistance = DistanceToActor;
					}
				
				}
				
			}
			if (MainPlayerController) {
				MainPlayerController->DisplayEnemyHealthBar();
			}
			SetCombatTarget(ClosestEnemy);
			bHasCombatTarget = true;
		}
	}
}

void AMain::DeathEnd() {
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
}

void AMain::SwitchLevel(FName LevelName) {
	UWorld* World = GetWorld();
	if (World) {
		FString CurrentLevel = World->GetMapName();

		FName CurrentLevelName(*CurrentLevel);
		if (CurrentLevelName != LevelName) {
			UGameplayStatics::OpenLevel(World, LevelName);
		}
	}
}

void AMain::SaveGame() {

	UHNSSaveGame* SaveGameInstance = Cast<UHNSSaveGame>(UGameplayStatics::CreateSaveGameObject(UHNSSaveGame::StaticClass()));

	SaveGameInstance->CharacterStats.Health = Health;
	SaveGameInstance->CharacterStats.MaxHealth = MaxHealth;
	SaveGameInstance->CharacterStats.Stamina = Stamina;
	SaveGameInstance->CharacterStats.MaxStamina = MaxStamina;
	SaveGameInstance->CharacterStats.Location = GetActorLocation();
	SaveGameInstance->CharacterStats.Rotation = GetActorRotation();

	if (EquippedWeapon) {
		SaveGameInstance->CharacterStats.WeaponName = EquippedWeapon->Name;
	}

	//Save the map
	UWorld* World = GetWorld();
	if (World) {
		SaveGameInstance->CharacterStats.CurrentMap = World->GetMapName();
	}

	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->PlayerName, SaveGameInstance->UserIndex);
}

void AMain::LoadGame(bool SetPosition) {

	UHNSSaveGame* LoadGameInstance = Cast<UHNSSaveGame>(UGameplayStatics::CreateSaveGameObject(UHNSSaveGame::StaticClass()));

	LoadGameInstance = Cast<UHNSSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->UserIndex));

	Health = LoadGameInstance->CharacterStats.Health;
	MaxHealth = LoadGameInstance->CharacterStats.MaxHealth;
	Stamina = LoadGameInstance->CharacterStats.Stamina;
	MaxStamina = LoadGameInstance->CharacterStats.MaxStamina;

	if (LoadGameInstance->CharacterStats.WeaponName != "") {
		if (WeaponSaver) {
			AWeaponSaver* Weapons = GetWorld()->SpawnActor<AWeaponSaver>(WeaponSaver);
			if (Weapons) {
				FString WeaponName = LoadGameInstance->CharacterStats.WeaponName;
				if (Weapons->WeaponMap.Contains(WeaponName)) {
					AWeapon* WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(Weapons->WeaponMap[WeaponName]);
					WeaponToEquip->Equip(this);
				}
			}
		}
	}

	if (SetPosition) {
		SetActorLocation(LoadGameInstance->CharacterStats.Location);
		SetActorRotation(LoadGameInstance->CharacterStats.Rotation);
	}

}