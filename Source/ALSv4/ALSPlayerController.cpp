// Fill out your copyright notice in the Description page of Project Settings.


#include "ALSPlayerController.h"

void AALSPlayerController::GetDebugInfo(TObjectPtr<ACharacter>& OutDebugFocusCharacter, bool& bOutDebugView,
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
