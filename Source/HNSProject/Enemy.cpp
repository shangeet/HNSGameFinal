// Fill out your copyright notice in the Description page of Project Settings.


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
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BehaviorTreeComponent.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// FObjectInitializer const& object_initializer = FObjectInitializer::Get()
	PrimaryActorTick.bCanEverTick = true;

	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());
	AgroSphere->InitSphereRadius(600.f);

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(75.f);

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(GetMesh(), FName("EnemySocket"));

	bOverlappingCombatSphere = false;

	Health = 75.f;
	Damage = 10.f;
	MaxHealth = 100.f;

	AttackMaxTime = 3.5f;
	AttackMinTime = 0.5f;

	EEnemyStatus = EEnemyState::EMS_Idle;

	DeathDelay = 1.f;

	bHasValidTarget = false;

	bInterpToMain = false;
	InterpSpeed = 15.f;

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> obj(TEXT("BehaviorTree'/Game/AI/NPC_BT.NPC_BT'"));
		if (obj.Succeeded()) {
			Btree = obj.Object;
		}
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	UE_LOG(LogTemp, Warning, TEXT("Beginplay()"));
	AIController = Cast<AAIController>(GetController());
	/*if (blackboard) {
		blackboard->InitializeBlackboard(*btree->BlackboardAsset);
	}
	AIController.RunBehaviorTree(btree);*/
	//behavior_tree_component->StartTree(*btree);
	
	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapBegin);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapEnd);
	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapEnd);
	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapEnd);

	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CombatTarget && bInterpToMain) {
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);
		SetActorRotation(InterpRotation);
	}

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);



}

void AEnemy::AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	UE_LOG(LogTemp, Warning, TEXT("Event sensed..."));
	if (OtherActor) {
		AMain* Main = Cast<AMain>(OtherActor);
		UE_LOG(LogTemp, Warning, TEXT("Casting to main"));
		if (Main) {
			UE_LOG(LogTemp, Warning, TEXT("MoveToTarget called"));
			MoveToTarget(Main);
		}
	}
}

void AEnemy::AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	UE_LOG(LogTemp, Warning, TEXT("AGRO Overlap End"));
	if (OtherActor) {
		AMain* Main = Cast<AMain>(OtherActor);
		
		if (Main) {

			bHasValidTarget = false;
			if (Main->CombatTarget == this) {
				Main->SetCombatTarget(nullptr);
			}

			Main->SetHasCombatTarget(false);

			if (Main->MainPlayerController) {
				Main->UpdateCombatTarget();
			}

			SetEnemyMovementStatus(EEnemyState::EMS_Idle);
			if (AIController) {
				AIController->StopMovement();
			}
		}
	}
}

void AEnemy::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	UE_LOG(LogTemp, Warning, TEXT("Combat Overlap Begin"));

	if (OtherActor) {
		AMain* Main = Cast<AMain>(OtherActor);
		
		if (Main) {

			bHasValidTarget = true;
			bOverlappingCombatSphere = true;
			Main->SetCombatTarget(this);
			Main->SetHasCombatTarget(true);

			Main->UpdateCombatTarget();

			CombatTarget = Main;
			bInterpToMain = true;
			UE_LOG(LogTemp, Warning, TEXT("Calling Attack"));
			float AttackTime = FMath::FRandRange(0.2f, 0.5f);
			GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
			//Attack();
		}
	}
}

void AEnemy::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	UE_LOG(LogTemp, Warning, TEXT("Combat Overlap End"));
	if (OtherActor) {

		AMain* Main = Cast<AMain>(OtherActor);

		if (Main) {


			bOverlappingCombatSphere = false;
			MoveToTarget(Main);
			CombatTarget = nullptr;

			if (Main->CombatTarget == this) {

				Main->SetCombatTarget(nullptr);
				Main->bHasCombatTarget = false;
				Main->UpdateCombatTarget();
			}

			if (Main->MainPlayerController) {
				USkeletalMeshComponent* MainMesh = Cast<USkeletalMeshComponent>(OtherComp);
				if (MainMesh) Main->MainPlayerController->RemoveEnemyHealthBar();

			}

			GetWorldTimerManager().ClearTimer(AttackTimer);
		}
	}
}

void AEnemy::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	UE_LOG(LogTemp, Warning, TEXT("Claw HIT MAIN!!!!"));
	if (OtherActor) {

		AMain* Main = Cast<AMain>(OtherActor);
		if (Main) {

			if (Main->HitParticles) {
				const USkeletalMeshSocket* TipSocket = GetMesh()->GetSocketByName("TipSocket");
				if (TipSocket) {
					UE_LOG(LogTemp, Warning, TEXT("PARTICLE SPAWNED!!!!"));
					FVector TipParticleSpawnLocation = TipSocket->GetSocketLocation(GetMesh());

					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Main->HitParticles, TipParticleSpawnLocation, FRotator(0.f), true);
				}

			}

			if (Main->OnHitSound) {
				UGameplayStatics::PlaySound2D(this, Main->OnHitSound);
			}

			if (DamageTypeClass) {
				UGameplayStatics::ApplyDamage(Main, Damage, AIController, this, DamageTypeClass);
			}
		}
	}
}

void AEnemy::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {

}

void AEnemy::ActivateCollision() {
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}



void AEnemy::DeactivateCollision() {
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::AttackEnd() {

	UE_LOG(LogTemp, Warning, TEXT("ATTACK END CALLE!"));

	bAttacking = false;
	bInterpToMain = false;

	if (bOverlappingCombatSphere) {
		UE_LOG(LogTemp, Warning, TEXT("Still overlapping combat sphere!"));
		float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
	}
}

void AEnemy::Attack() {
	
	if (Alive() && bHasValidTarget) {
		if (AIController) {
			AIController->StopMovement();
			UE_LOG(LogTemp, Warning, TEXT("Setting state to attacking."));
			SetEnemyMovementStatus(EEnemyState::EMS_Attacking);
		}
		if (!bAttacking) {
			bAttacking = true;
		}

		//play anim montage
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		if (AnimInstance) {
			UE_LOG(LogTemp, Warning, TEXT("Playing montage"));
			AnimInstance->Montage_Play(CombatMontage, 1.35f);
			AnimInstance->Montage_JumpToSection(FName("Attack"), CombatMontage);
		}

		if (SwingSound) {
			UGameplayStatics::PlaySound2D(this, SwingSound);
		}

	}

}

void AEnemy::MoveToTarget(class AMain* Target) {

	if (Alive()) {
		SetEnemyMovementStatus(EEnemyState::EMS_MoveToTarget);

		if (AIController) {
			UE_LOG(LogTemp, Warning, TEXT("MoveToTarget()"));
			FAIMoveRequest MoveRequest;
			MoveRequest.SetGoalActor(Target);
			MoveRequest.SetAcceptanceRadius(1.f);

			FNavPathSharedPtr NavPathPtr;
		
			AIController->MoveTo(MoveRequest, &NavPathPtr);
	
		}
	}

}

FRotator AEnemy::GetLookAtRotationYaw(FVector Target) {
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);
	return LookAtRotationYaw;
} 

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) {

	if (Health - DamageAmount <= 0.f) {
		Health -= DamageAmount;
		Die(DamageCauser);
	}
	Health -= DamageAmount;
	return DamageAmount;
}

void AEnemy::Die(AActor* Causer) {

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	SetEnemyMovementStatus(EEnemyState::EMS_Dead);

	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AgroSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (AnimInstance && CombatMontage) {

		AnimInstance->Montage_Play(CombatMontage, 2.f);
		AnimInstance->Montage_JumpToSection(FName("Death"), CombatMontage);
		
	}

	AMain* Main = Cast<AMain>(Causer);

	if (Main) {
		Main->UpdateCombatTarget();
	}
}

void AEnemy::DeathEnd() {

	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
	GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemy::Disappear, DeathDelay);
}

bool AEnemy::Alive() {
	return GetEnemyMovementStatus() != EEnemyState::EMS_Dead;
}

void AEnemy::Disappear() {
	Destroy();
}