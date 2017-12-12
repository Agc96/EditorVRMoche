/**
 * Funciones para la serializaci�n de los niveles creados con el Editor de Niveles.
 * @author Anthony Guti�rrez
 */

#include "EditorVRFunctions.h"

/** Funci�n de guardado de un nivel ya existente del Editor de Niveles. */
bool UEditorVRFunctions::SaveExistingLevel(UObject* WorldContextObject, const FString& FilePath)
{
	FBufferArchive FileBuffer;
	bool WasSerialized = SerializeEditableLevel(FileBuffer, WorldContextObject, FilePath);
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
	bool WasSerialized = SerializeEditableLevel(FileBuffer, WorldContextObject, FilePath);
	FileBuffer.FlushCache();
	FileBuffer.Empty();
	return WasSerialized;
}

/** Funci�n de serializaci�n para un nivel del Editor de Niveles. Guarda en un archivo binario los objetos editables creados con el
 *  Editor de Niveles, adem�s de objetos auxiliares de ubicaci�n y de decoraci�n. */
bool UEditorVRFunctions::SerializeEditableLevel(FBufferArchive& FileBuffer, UObject* WorldContextObject, const FString& FilePath)
{
	//Escribir el file signature
	SerializeFileSignature(FileBuffer);
	
	//Escribir los objetos de posici�n inicial y final del personaje
	SerializePlayerLocation(FileBuffer, WorldContextObject, TEXT("PlayerStart_C"));
	SerializePlayerLocation(FileBuffer, WorldContextObject, TEXT("PlayerEnd_C"));

	//Escribir las texturas del piso, techo y paredes de la sala principal
	SerializeFloorAndRoof(FileBuffer, WorldContextObject, TEXT("Floor_C"));
	SerializeFloorAndRoof(FileBuffer, WorldContextObject, TEXT("Roof_C"));
	SerializeWalls(FileBuffer, WorldContextObject);

	//Escribir la posici�n, rotaci�n y escala de cada objeto editable del mapa
	SerializeEditableObjectArray(FileBuffer, WorldContextObject);
	
	//Guardar los datos en el archivo binario indicado.
	if (FileBuffer.Num() <= 0)
	{
		DisplayMessage(EAppMsgType::Ok, TEXT("Hubo un error desconocido en el buffer del archivo serializable."), TEXT("Error"));
		return false;
	}
	if (!FFileHelper::SaveArrayToFile(FileBuffer, *FilePath))
	{
		DisplayMessage(EAppMsgType::Ok, TEXT("No se pudo guardar el archivo."), TEXT("Error"));
		return false;
	}
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
void UEditorVRFunctions::SerializePlayerLocation(FBufferArchive& FileBuffer, UObject* WorldContextObject, const TCHAR* ClassName)
{
	//Obtener una referencia a la clase solicitada
	FString ClassPath = GetEditableClassPath(FString(ClassName));
	TSubclassOf<AActor> PlayerLocationClass = StaticLoadClass(AActor::StaticClass(), NULL, *ClassPath, NULL, LOAD_None, NULL);
	
	//Buscar todos los actores de dicha clase que est�n dentro del mapa activo y verificar que la lista no sea vac�a.
	TArray<AActor*> PlayerLocationArray;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, PlayerLocationClass, PlayerLocationArray);

	//Obtener el n�mero de elementos del arreglo
	int32 PlayerLocationCount = PlayerLocationArray.Num();
	FileBuffer << PlayerLocationCount;

	for (int32 i = 0; i < PlayerLocationCount; i++)
	{
		//Escribir la posici�n del objeto
		FVector Location = PlayerLocationArray[i]->GetActorLocation();
		UE_LOG(LogTemp, Log, TEXT("Posicion de %s: %.2f %.2f %.2f"), ClassName, Location.X, Location.Y, Location.Z);
		FileBuffer << Location;
	}
}

/** Funci�n de serializaci�n para el piso y el techo del mapa. */
void UEditorVRFunctions::SerializeFloorAndRoof(FBufferArchive& FileBuffer, UObject* WorldContextObject, const TCHAR* ClassName)
{
	//Obtener una referencia a la clase solicitada
	FString ClassPath = GetEditableClassPath(FString(ClassName));
	TSubclassOf<AActor> Class = StaticLoadClass(AActor::StaticClass(), NULL, *ClassPath, NULL, LOAD_None, NULL);
	
	//Buscar todos los actores de dicha clase que est�n dentro del mapa activo y verificar que la lista no sea vac�a.
	TArray<AActor*> Array;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, Class, Array);

	int32 ArrayCount = Array.Num();
	UE_LOG(LogTemp, Log, TEXT("%s: %d elementos"), ClassName, ArrayCount);
	FileBuffer << ArrayCount;

	for (int i = 0; i < ArrayCount; i++)
	{
		FString MaterialName;

		//Obtener el componente de malla est�tica
		TArray<UStaticMeshComponent*> Components;
		Array[i]->GetComponents<UStaticMeshComponent>(Components);
		if (Components.Num() > 0 && Components[0])
		{
			//Obtener el material de la malla est�tica
			UMaterialInterface* MaterialInterface = Components[0]->GetMaterial(0);
			UMaterial* Material = (MaterialInterface) ? MaterialInterface->GetMaterial() : NULL;

			//Obtener el nombre del material
			if (MaterialInterface && Material) MaterialName = Material->GetName();
		}

		UE_LOG(LogTemp, Log, TEXT("- MaterialName = %s"), *MaterialName);
		FileBuffer << MaterialName;
	}
}

/** Funci�n de serializaci�n para las paredes del mapa. */
void UEditorVRFunctions::SerializeWalls(FBufferArchive& FileBuffer, UObject* WorldContextObject)
{
	//Obtener una referencia a la clase solicitada
	FString WallClassPath = GetEditableClassPath(FString(TEXT("Wall_C")));
	TSubclassOf<AActor> WallClass = StaticLoadClass(AActor::StaticClass(), NULL, *WallClassPath, NULL, LOAD_None, NULL);

	//Buscar todos los actores de dicha clase que est�n dentro del mapa activo y verificar que la lista no sea vac�a.
	TArray<AActor*> WallArray;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, WallClass, WallArray);

	int32 WallArrayCount = WallArray.Num();
	UE_LOG(LogTemp, Log, TEXT("Wall_C: %d elementos"), WallArrayCount);
	FileBuffer << WallArrayCount;

	for (int32 i = 0; i < WallArrayCount; i++)
	{
		FString WallName = WallArray[i]->GetName(), WallMaterialName;

		//Obtener el componente de malla est�tica
		TArray<UStaticMeshComponent*> Components;
		WallArray[i]->GetComponents<UStaticMeshComponent>(Components);
		if (Components.Num() > 0 && Components[0])
		{
			//Obtener el material de la malla est�tica
			UMaterialInterface* MaterialInterface = Components[0]->GetMaterial(0);
			UMaterial* Material = (MaterialInterface) ? MaterialInterface->GetMaterial() : NULL;

			//Obtener el nombre del material
			if (MaterialInterface && Material) WallMaterialName = Material->GetName();
		}

		UE_LOG(LogTemp, Log, TEXT("- WallName = %s, MaterialName = %s"), *WallName, *WallMaterialName);
		FileBuffer << WallName << WallMaterialName;
	}
}

/** Funci�n de serializaci�n para el arreglo de objetos editables ubicados en el nivel a guardarse. */
void UEditorVRFunctions::SerializeEditableObjectArray(FBufferArchive& FileBuffer, UObject* WorldContextObject)
{
	//Obtener una referencia a la clase solicitada
	FString ClassPath = GetEditableClassPath(FString(TEXT("EditableObject_C")));
	TSubclassOf<AActor> EditableObjectClass = StaticLoadClass(AActor::StaticClass(), NULL, *ClassPath, NULL, LOAD_None, NULL);
	
	//Buscar todos los actores de dicha clase que est�n dentro del mapa activo y verificar que la lista no sea vac�a.
	TArray<AActor*> EditableObjectArray;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, EditableObjectClass, EditableObjectArray);
	
	//Guardar el n�mero de objetos editables
	int32 EditableObjectCount = EditableObjectArray.Num();
	UE_LOG(LogTemp, Log, TEXT("Numero de objetos editables: %d"), EditableObjectCount);
	FileBuffer << EditableObjectCount;

	//Guardar los datos de los objetos editables que est�n dentro del mapa
	for (int32 i = 0; i < EditableObjectCount; i++)
	{
		UE_LOG(LogTemp, Log, TEXT("Objeto editable %d de %d:"), i + 1, EditableObjectCount);
		SerializeEditableObject(FileBuffer, EditableObjectArray[i]);
	}
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
