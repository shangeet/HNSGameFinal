// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MyObject.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class HNSPROJECT_API UMyObject : public UObject
{
	GENERATED_BODY()

public:

	UMyObject();

	UPROPERTY(BluePrintReadWrite, Category = "MyVariables")
	float MyFloat;

	UFUNCTION(BluePrintCallable, Category = "MyFunctions")
	void MyFunction();

	
};
