// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "HNSSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FCharacterStats {

	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category="SaveGameData")
	float Health;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float Stamina;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float MaxStamina;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FVector Location;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FRotator Rotation;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FString CurrentMap;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FString WeaponName;
};

/**
 * 
 */
UCLASS()
class HNSPROJECT_API UHNSSaveGame : public USaveGame
{
	GENERATED_BODY()

public: 

	UHNSSaveGame();

	UPROPERTY(VisibleAnywhere, Category="Basic")
	FString PlayerName;

	UPROPERTY(VisibleAnywhere, Category = "Basic")
	uint32 UserIndex;

	UPROPERTY(VisibleAnywhere, Category = "Basic")
	FCharacterStats CharacterStats;

};
