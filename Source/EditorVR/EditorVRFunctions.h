// Hecho por el grupo de Desarrollo de Programas 2 "VR Moche".
// Copyright (C) 2017 Pontificia Universidad Cat�lica del Per�.
// Todos los derechos reservados.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EditorVRFunctions.generated.h"

// Esta clase incluye las funciones de serializaci�n del Editor de Niveles.
UCLASS()
class EDITORVR_API UEditorVRFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "EditorVRMoche")
		static void SerializeLevel();
	UFUNCTION(BlueprintCallable, Category = "EditorVRMoche")
		static void DeserializeLevel();
};
