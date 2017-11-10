// Hecho por el grupo de Desarrollo de Programas 2 "VR Moche".
// Copyright (C) 2017 Pontificia Universidad Cat�lica del Per�. Todos los derechos reservados.

#pragma once

#include <iostream>
#include <fstream>
#include <cstring>
#include "CoreMinimal.h"
#include "CoreUObject.h" //Para la funci�n "StaticLoadClass"
#include "Engine.h" //Para la funci�n "GetWorldFromContextObject"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/GameplayStatics.h" //Para la funci�n "GetAllActorsofClass"
#include "Misc/MessageDialog.h" //Para la funci�n "DisplayMessage"
#include "EditorVRFunctions.generated.h"

// Esta clase incluye las funciones de serializaci�n del Editor de Niveles.
UCLASS()
class EDITORVR_API UEditorVRFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "EditorVRMoche", meta = (WorldContext = "WorldContextObject"))
		static bool SerializeLevel(UObject* WorldContextObject, FString FileName);
	UFUNCTION(BlueprintCallable, Category = "EditorVRMoche", meta = (WorldContext = "WorldContextObject"))
		static bool DeserializeLevel(UObject* WorldContextObject, FString FileName);
	UFUNCTION(BlueprintCallable, Category = "EditorVRMoche", meta = (WorldContext = "WorldContextObject"))
		static TArray<FString> GetAllFilesInDirectory(const FString directory, const bool fullPath, const FString onlyFilesStartingWith, const FString onlyFilesWithExtension);


private:
	static void SerializeEditablePlayerStart(std::ofstream &File, UObject* WorldContextObject);
	static void SerializeEditableObject(std::ofstream &File, AActor* EditableObject);
	static void DeserializeEditablePlayerStart(std::ifstream &File, UObject* WorldContextObject);
	static void DeserializeEditableObject(std::ifstream &File, UObject* WorldContextObject);
	static const char* GetClassPathName(char* ClassNameString);
	static void DisplayMessage(const char* Message, const char* Title);
	
};
