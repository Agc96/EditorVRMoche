/**
 * Librería de funciones para el Editor de Niveles del Juego de Realidad Virtual del proyecto de
 * Desarrollo de Programas 2 "VR Moche" de la Pontificia Universidad Católica del Perú.
 * @author Anthony Gutiérrez
 * @author Sebastián Solari
 * @author Susana Ordaya
 */

#pragma once

#include "CoreMinimal.h"
#include "CoreUObject.h" //Para la función "StaticLoadClass"
#include "Engine/Engine.h" //Para la función "GetWorldFromContextObject"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/GameplayStatics.h" //Para la función "GetAllActorsofClass"
#include "Misc/FileHelper.h" //Para las funciones "SaveArrayToFile" y "LoadFileToArray"
#include "Misc/MessageDialog.h" //Para la función "DisplayMessage"
#include "Misc/Paths.h" //Para las funciones relacionadas con detección de archivos y directorios
#include "Runtime/Online/HTTP/Public/Http.h" //Para las funciones relacionadas con el servicio Web
#include "Runtime/Online/HTTP/Public/HttpModule.h" //Para la clase "FHttpModule"
#include "Runtime/Online/HTTP/Public/HttpManager.h" //Para la clase "FHttpManager"
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

	/** Función que retorna el path del directorio donde se guardan los niveles extra del juego. */
	UFUNCTION(BlueprintPure, Category = "Editor VR Moche")
		static FString GetExtraLevelDirectory();
	
	//Guardar nivel
	UFUNCTION(BlueprintCallable, Category = "Editor VR Moche", meta = (WorldContext = "WorldContextObject"))
		static bool SaveExistingLevel(UObject* WorldContextObject, const FString& FilePath);
	UFUNCTION(BlueprintCallable, Category = "Editor VR Moche", meta = (WorldContext = "WorldContextObject"))
		static bool SaveNewLevel(UObject* WorldContextObject, const FString& FilePath);
	
	/** Función que abre un nivel del Editor de Niveles, o verifica si el nivel si es correcto. */
	UFUNCTION(BlueprintCallable, Category = "Editor VR Moche", meta = (WorldContext = "WorldContextObject"))
		static bool OpenLevel(UObject* WorldContextObject, FString FilePath, bool JustVerify);
	
	//Leer archivos y directorios
	UFUNCTION(BlueprintCallable, Category = "Editor VR Moche")
		static TArray<FString> GetExtraLevelsList(const FString& Directory);
	
	/** Función que guarda y exporta al servidor Web un nivel ya existente desde el Editor de Niveles. */
	UFUNCTION(BlueprintCallable, Category = "Editor VR Moche", meta = (WorldContext = "WorldContextObject"))
		static bool ExportExistingLevel(UObject* WorldContextObject, const FString& FilePath);
	
	/** Función que guarda y exporta al servidor Web un nivel nuevo creado con el Editor de Niveles. */
	UFUNCTION(BlueprintCallable, Category = "Editor VR Moche", meta = (WorldContext = "WorldContextObject"))
		static bool ExportNewLevel(UObject* WorldContextObject, const FString& FilePath);

protected:

	/** Variable que controla el estado del nivel que se quiere exportar. */
	static TArray<bool> ExportedLevelStatus;
	
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
	/** Función de deserialización para un nivel del Editor de Niveles. */
	static bool DeserializeEditableLevel(UObject* WorldContextObject, const FString& FilePath,
		TArray<uint8>& FileBufferArray, FMemoryReader& FileReader);
	static bool DeserializeFileSignature(FMemoryReader& OpenFileReader);
	static bool DeserializePlayerLocation(FMemoryReader& OpenFileReader, TArray<AActor*> PlayerLocationArray);
	static bool DeserializeEditableObject(FMemoryReader& OpenFileReader, UObject* WorldContextObject);
	//Leer archivos y directorios
	static bool VerifyOrCreateDirectory(IPlatformFile& PlatformFile, const FString& Directory);
	static bool CheckIfCreateOrReplaceFile(const FString& FilePath);
	//Exportar nivel al servidor Web
    static TSharedRef<IHttpRequest> PostRequest(FString Subroute, const TArray<uint8>& DatosASubir, FString NombreArchivo);
	static bool SendRequest(TSharedRef<IHttpRequest>& Request);
    static TSharedRef<IHttpRequest> RequestWithRoute(FString Subroute);
    static void ExportMessage(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
    static bool ResponseIsValid(FHttpResponsePtr Response, bool bWasSuccessful);
};
