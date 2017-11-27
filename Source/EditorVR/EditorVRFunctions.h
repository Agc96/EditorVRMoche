/**
 * Librer�a de funciones para el Editor de Niveles del Juego de Realidad Virtual del proyecto de
 * Desarrollo de Programas 2 "VR Moche" de la Pontificia Universidad Cat�lica del Per�.
 * @author Anthony Guti�rrez
 * @author Sebasti�n Solari
 * @author Susana Ordaya
 */

#pragma once

#include "CoreMinimal.h"
#include "CoreUObject.h" //Para la funci�n "StaticLoadClass"
#include "Engine/Engine.h" //Para la funci�n "GetWorldFromContextObject"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/GameplayStatics.h" //Para la funci�n "GetAllActorsofClass"
#include "Misc/FileHelper.h" //Para las funciones "SaveArrayToFile" y "LoadFileToArray"
#include "Misc/MessageDialog.h" //Para la funci�n "DisplayMessage"
#include "Misc/Paths.h" //Para las funciones relacionadas con detecci�n de archivos y directorios
#include "Runtime/Online/HTTP/Public/Http.h" //Para las funciones relacionadas con el servicio Web
#include "Serialization/BufferArchive.h" //Para la escritura de archivos binarios
#include "Serialization/MemoryReader.h" //Para la lectura de archivos binarios
#include "EditorVRFunctions.generated.h"

#define PlayerLocationClassPath TEXT("Blueprint'/Game/Blueprints/PlayerLocation.PlayerLocation_C'")
#define EditableObjectClassPath TEXT("Blueprint'/Game/Blueprints/EditableObject.EditableObject_C'")
#define EditorFileExtension FString(TEXT("bin"))
#define ApiBaseUrl FString(TEXT("http://200.16.7.166:8080/api/"))

UCLASS()
class EDITORVR_API UEditorVRFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	//General
	UFUNCTION(BlueprintPure, Category = "Editor VR Moche")
		static FString GetExtraLevelDirectory();
	//Guardar nivel
	UFUNCTION(BlueprintCallable, Category = "Editor VR Moche", meta = (WorldContext = "WorldContextObject"))
		static bool SaveExistingLevel(UObject* WorldContextObject, const FString& FilePath);
	UFUNCTION(BlueprintCallable, Category = "Editor VR Moche", meta = (WorldContext = "WorldContextObject"))
		static bool SaveNewLevel(UObject* WorldContextObject, const FString& FilePath);
	//Abrir nivel
	UFUNCTION(BlueprintCallable, Category = "Editor VR Moche", meta = (WorldContext = "WorldContextObject"))
		static bool DeserializeLevel(UObject* WorldContextObject, const FString& FilePath);
	//Leer archivos y directorios
	UFUNCTION(BlueprintCallable, Category = "Editor VR Moche")
		static TArray<FString> GetExtraLevelsList(const FString& Directory);
	//Exportar nivel al servidor Web
	UFUNCTION(BlueprintCallable, Category = "Editor VR Moche", meta = (WorldContext = "WorldContextObject"))
		static bool ExportExistingLevel(UObject* WorldContextObject, const FString& FilePath);
	UFUNCTION(BlueprintCallable, Category = "Editor VR Moche", meta = (WorldContext = "WorldContextObject"))
		static bool ExportNewLevel(UObject* WorldContextObject, const FString& FilePath);
	
private:
	//General
	static FString GetEditableObjectClassPath(const FString& ClassName);
	static EAppReturnType::Type DisplayMessage(EAppMsgType::Type Type, const TCHAR* Message, const TCHAR* Title);
	//Guardar nivel
	static bool SerializeLevel(FBufferArchive& FileBuffer, UObject* WorldContextObject, const FString& FilePath);
	static void SerializeFileSignature(FBufferArchive& FileBuffer);
	static void SerializePlayerPosition(FBufferArchive& FileBuffer, AActor* PlayerLocation);
	static void SerializeEditableObject(FBufferArchive& FileBuffer, AActor* EditableObject);
	//Abrir nivel
	static bool DeserializeFileSignature(FMemoryReader& OpenFileReader);
	static bool DeserializePlayerLocation(FMemoryReader& OpenFileReader, TArray<AActor*> PlayerLocationArray);
	static bool DeserializeEditableObject(FMemoryReader& OpenFileReader, UObject* WorldContextObject);
	//Leer archivos y directorios
	static bool VerifyOrCreateDirectory(IPlatformFile& PlatformFile, const FString& Directory);
	static bool CheckIfCreateOrReplaceFile(const FString& FilePath);
	//Exportar nivel al servidor Web
    static TSharedRef<IHttpRequest> PostRequest(FString Subroute, const TArray<uint8>& DatosASubir, FString NombreArchivo);
    static TSharedRef<IHttpRequest> RequestWithRoute(FString Subroute);
    static void ResponseMessage(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
    static bool ResponseIsValid(FHttpResponsePtr Response, bool bWasSuccessful);
};
