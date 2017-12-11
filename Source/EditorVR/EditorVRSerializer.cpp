/**
 * Funciones para la serializaci�n de los niveles creados con el Editor de Niveles.
 * @author Anthony Guti�rrez
 */

#include "EditorVRFunctions.h"

/** Funci�n de guardado de un nivel ya existente del Editor de Niveles. */
bool UEditorVRFunctions::SaveExistingLevel(UObject* WorldContextObject, const FString& FilePath)
{
	FBufferArchive FileBuffer;
	bool WasSerialized = SerializeLevel(FileBuffer, WorldContextObject, FilePath);
	FileBuffer.FlushCache();
	FileBuffer.Empty();
	return WasSerialized;
}

/** Funci�n de guardado de un nivel nuevo del Editor de Niveles. */
bool UEditorVRFunctions::SaveNewLevel(UObject* WorldContextObject, const FString& FilePath)
{
	//Verificar si se sobreescribir� un archivo.
	if (!CheckIfCreateOrReplaceFile(FilePath)) return false;
	
	FBufferArchive FileBuffer;
	bool WasSerialized = SerializeLevel(FileBuffer, WorldContextObject, FilePath);
	FileBuffer.FlushCache();
	FileBuffer.Empty();
	return WasSerialized;
}

/** Funci�n de serializaci�n para un nivel del Editor de Niveles. Guarda en un archivo binario los objetos editables creados con el
 Editor de Niveles, adem�s de objetos auxiliares de ubicaci�n y de decoraci�n. */
bool UEditorVRFunctions::SerializeEditableLevel(FBufferArchive& FileBuffer, UObject* WorldContextObject, const FString& FilePath)
{
	//Escribir el file signature
	SerializeFileSignature(FileBuffer);
	
	//Escribir el objeto de posici�n de inicio del personaje
	UE_LOG(LogTemp, Log, TEXT("PlayerStart:"));
	if (!SerializePlayerLocation(WorldContextObject, TEXT("PlayerStart_C"))) return false;
	
	//Escribir el objeto de posici�n final del personaje
	UE_LOG(LogTemp, Log, TEXT("PlayerEnd:"));
	if (!SerializePlayerLocation(WorldContextObject, TEXT("PlayerEnd_C"))) return false;
	
	//Escribir la textura del piso de la sala principal
	UE_LOG(LogTemp, Log, TEXT("Floor:"));
	if (!SerializeFloorAndRoof(WorldContextObject, FString(TEXT("Floor_C")))) return false;
	
	//Escribir la textura del techo de la sala principal
	UE_LOG(LogTemp, Log, TEXT("Roof:"));
	if (!SerializeFloorAndRoof(WorldContextObject, FString(TEXT("Roof_C")))) return false;
	
	//Escribir la textura de las paredes de la sala principal
	UE_LOG(LogTemp, Log, TEXT("Walls:"));
	/***********************************************************************************************
											FALTA TERMINAR
	 ***********************************************************************************************/
	
	//Escribir la posici�n, rotaci�n y escala de cada objeto editable del mapa
	if (!SerializeEditableObjectArray(FileBuffer, WorldContextObject)) return false;
	
	//Guardar los datos en el archivo binario indicado.
	if (FileBuffer.Num() <= 0) return false;
	if (!FFileHelper::SaveArrayToFile(FileBuffer, *FilePath)) return DisplayErrorMessage(TEXT("No se pudo guardar el archivo."), true);
	
	DisplayMessage(EAppMsgType::Ok, TEXT("El archivo se guardo con exito."), TEXT("Editor VR Moche"));
	return true;
}

/** Funci�n de serializaci�n del file signature. */
void UEditorVRFunctions::SerializeFileSignature(FBufferArchive& FileBuffer)
{
	uint8 F = 86, i = 82, l = 77, e = 111, Sig = 99, na = 104, tu = 101, re = 0;
	FileBuffer << F << i << l << e << Sig << na << tu << re;
}

/** Funci�n de serializaci�n para un objeto de ubicaci�n del personaje.
 *  @return Verdadero si se guard� correctamente, falso si no se pudo ubicar al objeto. */
bool UEditorVRFunctions::SerializePlayerLocation(FBufferArchive& FileBuffer, const TCHAR* ClassName)
{
	//Obtener una referencia a la clase solicitada
	FString ClassPath = GetEditableClassPath(FString(ClassName));
	TSubclassOf<AActor> PlayerLocationClass = StaticLoadClass(AActor::StaticClass(), NULL, ClassPath, NULL, LOAD_None, NULL);
	
	//Buscar todos los actores de dicha clase que est�n dentro del mapa activo y verificar que la lista no sea vac�a.
	TArray<AActor*> PlayerLocationArray;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, PlayerLocationClass, PlayerLocationArray);
	if (PlayerLocationArray.Num() <= 0) return DisplayErrorMessage(TEXT("- No se encontro ningun objeto en el mapa del Editor."), false);
	
	//Escribir el nombre de la clase
	FString ClassName = PlayerLocation[0]->GetClass()->GetName();
	UE_LOG(LogTemp, Log, TEXT("- Nombre: %s"), *ClassName);
	FileBuffer << ClassName;
	
	//Escribir la posici�n del objeto
	FVector Location = PlayerLocation[0]->GetActorLocation();
	UE_LOG(LogTemp, Log, TEXT("- Posicion: %.2f %.2f %.2f"), Location.X, Location.Y, Location.Z);
	FileBuffer << Location;
	
	return true;
}

/** Funci�n de serializaci�n para el piso y el techo del mapa. */
bool UEditorVRFunctions::SerializeFloorAndRoof(FBufferArchive& FileBuffer, const TCHAR* ClassName)
{
	//Obtener una referencia a la clase solicitada
	FString ClassPath = GetEditableClassPath(FString(ClassName));
	TSubclassOf<AActor> Class = StaticLoadClass(AActor::StaticClass(), NULL, ClassPath, NULL, LOAD_None, NULL);
	
	//Buscar todos los actores de dicha clase que est�n dentro del mapa activo y verificar que la lista no sea vac�a.
	TArray<AActor*> Array;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, Class, Array);
	if (Array.Num() <= 0) return DisplayErrorMessage(TEXT("- No se encontro ningun objeto en el mapa del Editor."), false);
	
	/***********************************************************************************************
											FALTA TERMINAR
	 ***********************************************************************************************/
	 
	return true;
}

/** Funci�n de serializaci�n para el arreglo de objetos editables ubicados en el nivel a guardarse. */
bool UEditorVRFunctions::SerializeEditableObjectArray(FBufferArchive& FileBuffer, UObject* WorldContextObject)
{
	//Obtener una referencia a la clase solicitada
	FString ClassPath = GetEditableClassPath(FString(TEXT("EditableObject_C")));
	TSubclassOf<AActor> PlayerLocationClass = StaticLoadClass(AActor::StaticClass(), NULL, ClassPath, NULL, LOAD_None, NULL);
	
	//Buscar todos los actores de dicha clase que est�n dentro del mapa activo y verificar que la lista no sea vac�a.
	TArray<AActor*> PlayerLocationArray;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, PlayerLocationClass, PlayerLocationArray);
	if (PlayerLocationArray.Num() <= 0) return DisplayErrorMessage(TEXT("No se encontro ningun objeto editable en el mapa del Editor."), false);
	
	//Guardar el n�mero de objetos editables
	int32 EditableObjectCount = EditableObjectArray.Num();
	FileBuffer << EditableObjectCount;

	//Guardar los datos de los objetos editables que est�n dentro del mapa
	for (int32 i = 0; i < EditableObjectCount; i++)
	{
		UE_LOG(LogTemp, Log, TEXT("Objeto editable %d de %d:"), i + 1, EditableObjectCount);
		SerializeEditableObject(FileBuffer, EditableObjectArray[i]);
	}
	
	return true;
}

/** Funci�n de serializaci�n para un objeto editable. */
void UEditorVRFunctions::SerializeEditableObject(FBufferArchive& FileBuffer, AActor* EditableObject)
{
	//Escribir el nombre de la clase
	FString ClassName = EditableObject->GetClass()->GetName();
	UE_LOG(LogTemp, Log, TEXT("- Nombre: %s"), *ClassName);
	FileBuffer << ClassName;
	
	//Escribir la posici�n del objeto
	FVector Location = EditableObject->GetActorLocation();
	UE_LOG(LogTemp, Log, TEXT("- Posicion: %.2f %.2f %.2f"), Location.X, Location.Y, Location.Z);
	FileBuffer << Location;
	
	//Escribir la rotaci�n del objeto
	FRotator Rotation = EditableObject->GetActorRotation();
	UE_LOG(LogTemp, Log, TEXT("- Rotacion: %.2f %.2f %.2f"), Rotation.Pitch, Rotation.Roll, Rotation.Yaw);
	FileBuffer << Rotation;
	
	//Escribir la escala del objeto
	FVector Scale = EditableObject->GetActorScale();
	UE_LOG(LogTemp, Log, TEXT("- Escala: %.2f %.2f %.2f"), Scale.X, Scale.Y, Scale.Z);
	FileBuffer << Scale.X;
}
