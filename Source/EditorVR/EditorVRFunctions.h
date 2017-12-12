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
#include "Runtime/Engine/Classes/Engine/World.h" //Para la función "SpawnActor"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h" //Para los componentes de malla estática
#include "Runtime/Engine/Classes/Materials/MaterialInterface.h" //Para la interfaz del material
#include "Runtime/Engine/Classes/Materials/Material.h" //Para la referencia al material
#include "Runtime/Online/HTTP/Public/Http.h" //Para las funciones relacionadas con el servicio Web
#include "Runtime/Online/HTTP/Public/HttpModule.h" //Para la clase "FHttpModule"
#include "Runtime/Online/HTTP/Public/HttpManager.h" //Para la clase "FHttpManager"
#include "Serialization/BufferArchive.h" //Para la escritura de archivos binarios
#include "Serialization/MemoryReader.h" //Para la lectura de archivos binarios
#include "EditorVRFunctions.generated.h"

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
	
	/** Función de guardado de un nivel ya existente del Editor de Niveles. */
	UFUNCTION(BlueprintCallable, Category = "Editor VR Moche", meta = (WorldContext = "WorldContextObject"))
		static bool SaveExistingLevel(UObject* WorldContextObject, const FString& FilePath);
	/** Función de guardado de un nivel nuevo del Editor de Niveles. */
	UFUNCTION(BlueprintCallable, Category = "Editor VR Moche", meta = (WorldContext = "WorldContextObject"))
		static bool SaveNewLevel(UObject* WorldContextObject, const FString& FilePath);
	
	/** Función que abre un nivel del Editor de Niveles, o verifica si el nivel si es correcto. */
	UFUNCTION(BlueprintCallable, Category = "Editor VR Moche", meta = (WorldContext = "WorldContextObject"))
		static bool OpenExtraLevel(UObject* WorldContextObject, const FString& FileName, bool Deserialize);
	UFUNCTION(BlueprintCallable, Category = "Editor VR Moche")
		static TMap<FString, FString> GetDecorationMaterials();
	
	/** Función para obtener la lista de archivos binarios del Editor de Niveles */
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
	
	/** Variable con los nombres de objetos y los nombres de materiales para la deserialización. */
	static TMap<FString, FString> DecorationMaterials;
	
private:

	//General
	static EAppReturnType::Type DisplayMessage(EAppMsgType::Type Type, const TCHAR* Message, const TCHAR* Title);
	static FString GetEditableClassPath(const FString& ClassName);
	
	/** Función de serialización para un nivel del Editor de Niveles. */
	static bool SerializeEditableLevel(FBufferArchive& FileBuffer, UObject* WorldContextObject, const FString& FilePath);
	/** Función de serialización del file signature. */
	static void SerializeFileSignature(FBufferArchive& FileBuffer);
	/** Función de serialización para un objeto de ubicación del personaje. */
	static void SerializePlayerLocation(FBufferArchive& FileBuffer, UObject* WorldContextObject, const TCHAR* ClassName);
	/** Función de serialización para el piso y el techo del mapa. */
	static void SerializeFloorAndRoof(FBufferArchive& FileBuffer, UObject* WorldContextObject, const TCHAR* ClassName);
	/** Función que obtiene el nombre un material de un objeto de decoración. */
	static void GetSerializedMaterial(AActor* DecorationActor, FString& MaterialName);
	/** Función de serialización para las paredes del mapa. */
	static void SerializeWalls(FBufferArchive& FileBuffer, UObject* WorldContextObject);
	/** Función de serialización para el arreglo de objetos editables ubicados en el nivel a guardarse. */
	static void SerializeEditableObjectArray(FBufferArchive& FileBuffer, UObject* WorldContextObject);
	/** Función de serialización para un objeto editable. */
	static void SerializeEditableObject(FBufferArchive& FileBuffer, AActor* EditableObject);
	
	/** Función de deserialización para un nivel del Editor de Niveles. */
	static bool DeserializeEditableLevel(FMemoryReader& FileReader, UObject* WorldContextObject);
	/** Función de deserialización del file signature. */
	static bool DeserializeFileSignature(FMemoryReader& FileReader);
	/** Función de deserialización para un objeto de ubicación del personaje. */
	static bool DeserializePlayerLocation(FMemoryReader& FileReader, UObject* WorldContextObject, const TCHAR* ClassName);
	/** Función de deserialización para el piso y el techo del mapa. */
	static void DeserializeFloorAndRoof(FMemoryReader& FileReader, UObject* WorldContextObject, const TCHAR* ClassName);
	/** Función de deserialización para las paredes del mapa. */
	static void DeserializeWalls(FMemoryReader& FileReader, UObject* WorldContextObject);
	/** Función de deserialización para el arreglo de objetos editables ubicados en el nivel a guardarse. */
	static bool DeserializeEditableObjectArray(FMemoryReader& FileReader, UObject* WorldContextObject);
	/** Función de deserialización para un objeto editable. */
	static bool DeserializeEditableObject(FMemoryReader& FileReader, UWorld* CurrentWorld);
	
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
