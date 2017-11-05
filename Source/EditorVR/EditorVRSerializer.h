// EditorVRSerializer.h
// Copyright (C) 2017 Desarrollo de Programas 2 "VR Moche" - Pontificia Universidad Cat�lica del Per�.
// Todos los derechos reservados.

#pragma once

#include "CoreMinimal.h"
#include "CoreUObject.h" //Para la funci�n "StaticLoadClass"
#include "Engine/Engine.h" //Para la funci�n "GetWorldFromContextObject"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/GameplayStatics.h" //Para la funci�n "GetAllActorsofClass"
#include "Misc/FileHelper.h" //Para la funci�n "SaveArrayToFile"
#include "Misc/MessageDialog.h" //Para la funci�n "DisplayMessage"
#include "Serialization/BufferArchive.h"
#include "Serialization/MemoryReader.h"
#include "EditorVRSerializer.generated.h"

// Esta clase incluye las funciones de serializaci�n del Editor de Niveles.
UCLASS()
class EDITORVR_API UEditorVRSerializer : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:	
	UFUNCTION(BlueprintCallable, Category = "Editor VR Moche", meta = (WorldContext = "WorldContextObject"))
		static bool SerializeLevel(UObject* WorldContextObject, const FString& SaveFilePath);
	UFUNCTION(BlueprintCallable, Category = "Editor VR Moche", meta = (WorldContext = "WorldContextObject"))
		static bool DeserializeLevel(UObject* WorldContextObject, const FString& OpenFilePath);

private:
	static void SerializeEditablePlayerPosition(FBufferArchive& SaveFileBuffer, AActor* EditablePlayerLocation);
	static void SerializeEditableObject(FBufferArchive& SaveFileBuffer, AActor* EditableObject);
	static bool DeserializeEditablePlayerLocation(FMemoryReader& OpenFileReader, TArray<AActor*> EditablePlaayerLocationArray);
	static AActor* DeserializeEditableObject(FMemoryReader& OpenFileReader, UObject* WorldContextObject);
	static FString GetEditableObjectClassPath(const FString& ClassName);
};
