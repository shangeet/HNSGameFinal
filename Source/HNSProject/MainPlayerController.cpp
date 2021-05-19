// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"
#include "Blueprint/UserWidget.h"

void AMainPlayerController::BeginPlay() {

	Super::BeginPlay();

	if (HUDOverlayAsset) {
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayAsset);
	}

	if (HUDOverlay) {
		HUDOverlay->AddToViewport();
		HUDOverlay->SetVisibility(ESlateVisibility::Visible);
	}

	if (WEnemyHealthBar) {
		EnemyHealthBar = CreateWidget<UUserWidget>(this, WEnemyHealthBar);

		if (EnemyHealthBar) {
			EnemyHealthBar->AddToViewport();
			EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
		}

		FVector2D Vec = FVector2D(0.f, 5.f);
		EnemyHealthBar->SetAlignmentInViewport(Vec);
	}

	if (WPauseMenu) {
		PauseMenu = CreateWidget<UUserWidget>(this, WPauseMenu);

		if (PauseMenu) {
			PauseMenu->AddToViewport();
			PauseMenu->SetVisibility(ESlateVisibility::Hidden);
		}
	}

}

void AMainPlayerController::Tick(float DeltaTime) {
	
	Super::Tick(DeltaTime);

	if (EnemyHealthBar) {

		FVector2D PositionInViewport;
		ProjectWorldLocationToScreen(EnemyTargetLocation, PositionInViewport);
		PositionInViewport.Y -= 100.f;
		PositionInViewport.X -= 50.f;
		FVector2D SizeInViewport = FVector2D(300.f, 25.f);

		EnemyHealthBar->SetPositionInViewport(PositionInViewport);
		EnemyHealthBar->SetDesiredSizeInViewport(SizeInViewport);
		
	}

}

void AMainPlayerController::DisplayEnemyHealthBar() {

		if (EnemyHealthBar) {
			bEnemyHealthBarVisible = true;
			EnemyHealthBar->SetVisibility(ESlateVisibility::Visible);
		}

}

void AMainPlayerController::RemoveEnemyHealthBar() {

	if (EnemyHealthBar) {
		bEnemyHealthBarVisible = false;
		EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
	}
}


void AMainPlayerController::DisplayPauseMenu_Implementation() {
	if (PauseMenu) {
		bPauseMenuVisible = true;
		PauseMenu->SetVisibility(ESlateVisibility::Visible);
	}
	FInputModeGameAndUI InputModeGameAndUIOnly;
	SetInputMode(InputModeGameAndUIOnly);
	bShowMouseCursor = true;
}

void AMainPlayerController::RemovePauseMenu_Implementation() {
	if (PauseMenu) {
		bPauseMenuVisible = false;
		PauseMenu->SetVisibility(ESlateVisibility::Hidden);
	}
	FInputModeGameOnly InputModeGameOnly;
	SetInputMode(InputModeGameOnly);
	bShowMouseCursor = false;
}

void AMainPlayerController::TogglePauseMenu() {
	if (bPauseMenuVisible) {
		RemovePauseMenu();
	} else {
		DisplayPauseMenu();
	}
}

void AMainPlayerController::GameModeOnly() {
	FInputModeGameOnly InputModeGameOnly;
	SetInputMode(InputModeGameOnly);
}