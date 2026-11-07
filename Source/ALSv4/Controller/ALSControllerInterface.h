// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ALSControllerInterface.generated.h"

UINTERFACE()
class UALSControllerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ALSV4_API IALSControllerInterface
{
	GENERATED_BODY()

public:
	virtual void GetDebugInfo(OUT TObjectPtr<ACharacter>& OutDebugFocusCharacter, OUT bool& bOutDebugView,
	                          OUT bool& bOutShouHUD, OUT bool& bOutShouTraces, OUT bool& bOutShouDebugShapes,
	                          OUT bool& bOutShouLayerColors, OUT bool& bOutShowSlomo, OUT bool& bOutShowCharacterInfo);
	virtual bool GetShowTraces();
};
