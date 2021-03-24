// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponSaver.generated.h"

UCLASS()
class HNSPROJECT_API AWeaponSaver : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponSaver();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	UPROPERTY(EditDefaultsOnly, Category="Savedata")
	TMap<FString, TSubclassOf<class AWeapon>> WeaponMap;

};
