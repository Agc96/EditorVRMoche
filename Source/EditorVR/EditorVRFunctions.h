// Hecho por el grupo de Desarrollo de Programas 2 "VR Moche".
// Copyright (C) 2017 Pontificia Universidad Católica del Perú.
// Todos los derechos reservados.

#pragma once

#include <iostream>
#include <fstream>
#include <cstring>
#include "CoreMinimal.h"
#include "CoreUObject.h" //Para la función "StaticLoadClass"
#include "Engine.h" //Para la función "GetWorldFromContextObject"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/GameplayStatics.h" //Para la función "GetAllActorsofClass"
#include "Misc/MessageDialog.h" //Para la función "DisplayMessage"
#include "EditorVRFunctions.generated.h"

// Esta clase incluye las funciones de serialización del Editor de Niveles.
UCLASS()
class EDITORVR_API UEditorVRFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "EditorVRMoche", meta = (WorldContext = "WorldContextObject"))
		static bool SerializeLevel(UObject* WorldContextObject, FString FileName);
	UFUNCTION(BlueprintCallable, Category = "EditorVRMoche", meta = (WorldContext = "WorldContextObject"))
		static bool DeserializeLevel(UObject* WorldContextObject, FString FileName);

private:
	static void SerializeEditableObject(std::ofstream& File, AActor* EditableObject);
	static AActor* DeserializeEditableObject(UObject* WorldContextObject, std::ifstream& File);
	static const char* GetClassPathName(char* ClassNameString);
	static void DisplayMessage(const char* Message, const char* Title);
};
