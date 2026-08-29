// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ALSControllerInterface.h"
#include "GameFramework/PlayerController.h"
#include "ALSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ALSV4_API AALSPlayerController : public APlayerController, public IALSControllerInterface
{
	GENERATED_BODY()

public:
	virtual void GetDebugInfo(TObjectPtr<ACharacter>& OutDebugFocusCharacter, bool& bOutDebugView, bool& bOutShouHUD,
	                          bool& bOutShouTraces, bool& bOutShouDebugShapes, bool& bOutShouLayerColors,
	                          bool& bOutShowSlomo, bool& bOutShowCharacterInfo) override;
	virtual bool GetShowTraces() override;
protected:
	TObjectPtr<ACharacter> DebugFocusCharacter;
	TArray<TObjectPtr<ACharacter>> AvailableDebugCharacters;
	bool bDebugView = false;
	bool bShowHUD = false;
	bool bShowTraces = false;
	bool bShowDebugShapes = false;
	bool bShowLayerColors = false;
	bool bShowCharacterInfo = false;
	bool bShowSlomo = false;
};
