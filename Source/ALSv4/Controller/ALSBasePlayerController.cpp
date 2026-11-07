// Fill out your copyright notice in the Description page of Project Settings.


#include "ALSBasePlayerController.h"

#include "ALSv4/Camera/ALSBasePlayerCameraManager.h"

void AALSBasePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (AALSBasePlayerCameraManager* ALSPlayerCameraManager = Cast<AALSBasePlayerCameraManager>(PlayerCameraManager))
	{
		ALSPlayerCameraManager->OnPossess(InPawn);
	}
}

void AALSBasePlayerController::GetDebugInfo(TObjectPtr<ACharacter>& OutDebugFocusCharacter, bool& bOutDebugView,
                                        bool& bOutShouHUD, bool& bOutShouTraces, bool& bOutShouDebugShapes,
                                        bool& bOutShouLayerColors, bool& bOutShowSlomo,
                                        bool& bOutShowCharacterInfo)
{
	OutDebugFocusCharacter = DebugFocusCharacter;
	bOutDebugView = bDebugView;
	bOutShouHUD = bShowHUD;
	bOutShouTraces = bShowTraces;
	bOutShouDebugShapes = bShowDebugShapes;
	bOutShouLayerColors = bShowLayerColors;
	bOutShowSlomo = bShowSlomo;
	bOutShowCharacterInfo = bShowCharacterInfo;
}

bool AALSBasePlayerController::GetShowTraces()
{
	return bShowTraces;
}
