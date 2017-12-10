/**
 * Funciones de deserialización de los niveles creados con el Editor de Niveles.
 * @author Anthony Gutiérrez
 */

#include "EditorVRFunctions.h"

/** Función que abre un nivel del Editor de Niveles, o verifica si el nivel si es correcto. */
bool UEditorVRFunctions::OpenLevel(UObject* WorldContextObject, FString FilePath, bool JustVerify)
{
	if (FilePath.Equals(FString(TEXT("Seleccione un nivel...")))) return false;
	if (JustVerify) FilePath = GetExtraLevelDirectory() / FilePath;	//Incluye ya la extensión
	TArray<uint8> FileBufferArray;
	
	//Abrir el archivo binario
	if (!FFileHelper::LoadFileToArray(FileBufferArray, *FilePath))
	{
		DisplayMessage(EAppMsgType::Ok, TEXT("No se pudo cargar el archivo."), TEXT("Error"));
		return false;
	}
	//Si el archivo tiene menos de 8 bytes, es seguro que el archivo no es válido.
	if (FileBufferArray.Num() <= 8)
	{
		DisplayMessage(EAppMsgType::Ok, TEXT("El archivo no tiene el formato adecuado para el Editor de Niveles.\
 Asegurese de que el archivo sea un archivo generado por este Editor."), TEXT("Error"));
		return false;
	}
	
	FMemoryReader FileReader = FMemoryReader(FileBufferArray, true);
	FileReader.Seek(0);
	//Verificar si el file signature es correcto
	if (!DeserializeFileSignature(FileReader))
	{
		DisplayMessage(EAppMsgType::Ok, TEXT("El archivo no tiene el formato adecuado para el Editor de Niveles.\
 Asegurese de que el archivo sea un archivo generado por este Editor."), TEXT("Error"));
		return false;
	}
	
	if (JustVerify) return true;
	return DeserializeEditableLevel(WorldContextObject, FilePath, FileBufferArray, FileReader);
}


/** Función de deserialización para un nivel del Editor de Niveles. Toma un archivo binario y lo lee para
 ubicar los objetos editables creados con anterioridad, además de ubicar al personaje en la escena. */
bool UEditorVRFunctions::DeserializeEditableLevel(UObject* WorldContextObject, const FString& FilePath,
	TArray<uint8>& FileBufferArray, FMemoryReader& FileReader)
{	
	//Leer la cantidad de objetos de ubicación del personaje
	int32 PlayerLocationCount;
	FileReader << PlayerLocationCount;

	//Obtener la lista de objetos de ubicación del personaje
	TSubclassOf<AActor> PlayerLocationClass = StaticLoadClass(AActor::StaticClass(), NULL,
		PlayerLocationClassPath, NULL, LOAD_None, NULL);
	TArray<AActor*> PlayerLocationArray;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, PlayerLocationClass, PlayerLocationArray);

	//Leer los datos de los objetos de ubicación del personaje y reemplazarlas en el mapa
	for (int32 i = 0; i < PlayerLocationCount; i++)
	{
		UE_LOG(LogTemp, Log, TEXT("Posicion %d de %d:"), i + 1, PlayerLocationCount);
		DeserializePlayerLocation(FileReader, PlayerLocationArray);
	}

	//Leer el número de objetos editables guardados
	int32 EditableObjectCount;
	FileReader << EditableObjectCount;

	//Leer los datos de los objetos editables y ubicarlos en el mapa
	for (int32 i = 0; i < EditableObjectCount; i++)
	{
		UE_LOG(LogTemp, Log, TEXT("Objeto %d de %d:"), i + 1, EditableObjectCount);
		DeserializeEditableObject(FileReader, WorldContextObject);
	}

	//Cerrar el archivo
	UE_LOG(LogTemp, Display, TEXT("El archivo ha sido leido correctamente."));
	FileReader.FlushCache();
	FileBufferArray.Empty();
	FileReader.Close();
	return true;
}

/** Función de deserialización del file signature. */
bool UEditorVRFunctions::DeserializeFileSignature(FMemoryReader& OpenFileReader)
{
	uint8 F, i, l, e, Sig, na, tu, re;
	OpenFileReader << F << i << l << e << Sig << na << tu << re;
	return (F == 86 && i == 82 && l == 77 && e == 111 && Sig == 99 && na == 104 && tu == 101 && re == 0);
}

// Función de deserialización para un objeto de ubicación del personaje.
bool UEditorVRFunctions::DeserializePlayerLocation(FMemoryReader& OpenFileReader, TArray<AActor*> PlayerLocationArray)
{
	//Obtener el nombre de la clase y su dirección completa
	FString ClassName;
	OpenFileReader << ClassName;
	UE_LOG(LogTemp, Log, TEXT("- Clase: %s"), *ClassName);

	//Obtener la posición del objeto de ubicación
	FVector Location;
	OpenFileReader << Location;
	UE_LOG(LogTemp, Log, TEXT("- Posicion: %.2f %.2f %.2f"), Location.X, Location.Y, Location.Z);

	//Ubicar el objeto en la lista y reemplazar su posición
	for (int32 i = 0; i < PlayerLocationArray.Num(); i++)
	{
		if (ClassName.Compare(PlayerLocationArray[i]->GetClass()->GetName()) == 0)
		{
			PlayerLocationArray[i]->SetActorLocation(Location);
			UE_LOG(LogTemp, Log, TEXT("- Se ajusto el objeto de ubicacion correctamente."));
			return true;
		}
	}
	UE_LOG(LogTemp, Error, TEXT("- No se pudo encontrar al objeto de ubicacion en el mapa."));
	return false;
}

// Función de deserialización para un objeto editable.
bool UEditorVRFunctions::DeserializeEditableObject(FMemoryReader& OpenFileReader, UObject* WorldContextObject)
{
	//Obtener el nombre de la clase y su dirección completa
	FString ClassName;
	OpenFileReader << ClassName;
	FString ClassPath = GetEditableObjectClassPath(ClassName);
	UE_LOG(LogTemp, Log, TEXT("- Clase: %s (%s)"), *ClassName, *ClassPath);

	//Obtener la posición del objeto editable
	FVector Location;
	OpenFileReader << Location;
	UE_LOG(LogTemp, Log, TEXT("- Posicion: %.2f %.2f %.2f"), Location.X, Location.Y, Location.Z);

	//Escribir la rotación del objeto
	FRotator Rotation;
	OpenFileReader << Rotation;
	UE_LOG(LogTemp, Log, TEXT("- Rotacion: %.2f %.2f %.2f"), Rotation.Pitch, Rotation.Roll, Rotation.Yaw);

	//Escribir la escala del objeto
	float Scale;
	OpenFileReader << Scale;
	UE_LOG(LogTemp, Log, TEXT("- Escala: %.2f"), Scale);

	//Obtener las referencias necesarias para poder crear el objeto editable
	UClass* EditableObjectClass = StaticLoadClass(AActor::StaticClass(), NULL, *ClassPath, NULL, LOAD_None, NULL);
	UWorld* CurrentWorld = (GEngine) ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull) : NULL;
	if (!EditableObjectClass || !CurrentWorld)
	{
		UE_LOG(LogTemp, Error, TEXT("- No se pudieron obtener las referencias para generar el objeto."));
		return false;
	}

	//Generar el objeto y colocarlo en el mapa
	AActor* EditableObject = CurrentWorld->SpawnActor(EditableObjectClass);
	if (!EditableObject)
	{
		UE_LOG(LogTemp, Error, TEXT("- No se pudo crear el objeto editable %s."), *ClassName);
		return false;
	}
	EditableObject->SetActorLocation(Location);
	EditableObject->SetActorRotation(Rotation);
	EditableObject->SetActorScale3D(FVector(Scale));
	UE_LOG(LogTemp, Log, TEXT("- Objeto editable creado y posicionado correctamente."));
	return true;
}
