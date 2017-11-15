// Copyright (C) 2017 DP2 VR Moche - PUCP. Todos los derechos reservados.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "EditorVRInstance.generated.h"

/**
 * 
 */
UCLASS()
class EDITORVR_API UEditorVRInstance : public UGameInstance
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	FString CurrentFilePath;
	

};
