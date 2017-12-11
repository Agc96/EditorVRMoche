/**
 * Funciones de deserialización de los niveles creados con el Editor de Niveles.
 * @author Anthony Gutiérrez
 */

#include "EditorVRFunctions.h"

/** Función que abre un nivel del Editor de Niveles, o verifica si el nivel si es correcto. */
bool UEditorVRFunctions::OpenExtraLevel(UObject* WorldContextObject, const FString& FileName, bool Deserialize)
{
#if PLATFORM_ANDROID
	//Generar el path completo
	FString FilePath = GetExtraLevelDirectory() / FileName + TEXT(".") + EditorFileExtension;
#else
	//En el Editor de Niveles, esta opción no debe poder escogerse.
	if (FilePath.Equals(FString(TEXT("Seleccione un nivel...")))) return false;
	//Generar el path completo
	FString FilePath = GetExtraLevelDirectory() / FileName;	//Incluye ya la extensión
	UE_LOG(LogTemp, Log, TEXT("Archivo a abrirse: %s"), *FilePath);
#endif
	
	//Abrir el archivo binario y verificar el tamaño del archivo
	TArray<uint8> FileBufferArray;
	if (!FFileHelper::LoadFileToArray(FileBufferArray, *FilePath)) return DisplayErrorMessage(TEXT("No se pudo cargar el archivo."), true);
	if (FileBufferArray.Num() <= 8) DisplayErrorMessage(TEXT("El archivo no tiene el formato adecuado para el Editor de Niveles.\
 Asegurese de que el archivo sea un archivo generado por este Editor."), true);
	
	//Abrir el lector de archivos binarios
	FMemoryReader FileReader = FMemoryReader(FileBufferArray, true);
	FileReader.Seek(0);
	//Verificar si el file signature es correcto
	if (!DeserializeFileSignature(FileReader)) DisplayErrorMessage(TEXT("El archivo no tiene el formato adecuado para el Editor de Niveles.\
 Asegurese de que el archivo sea un archivo generado por este Editor."), true);
	
	//Deserializar el archivo si el flag está activado, y luego liberar la memoria.
	bool WasFileOpened = (Deserialize) ? DeserializeEditableLevel(WorldContextObject, FileReader) : true;
	FileReader.FlushCache();
	FileBufferArray.Empty();
	FileReader.Close();
	return Status;
}

/** Función de deserialización del file signature. */
bool UEditorVRFunctions::DeserializeFileSignature(FMemoryReader& FileReader)
{
	uint8 F, i, l, e, Sig, na, tu, re;
	FileReader << F << i << l << e << Sig << na << tu << re;
	return (F == 86 && i == 82 && l == 77 && e == 111 && Sig == 99 && na == 104 && tu == 101 && re == 0);
}

/** Función de deserialización para un nivel del Editor de Niveles. Toma un archivo binario y lo lee para
 ubicar los objetos editables creados con anterioridad, además de ubicar al personaje en la escena. */
bool UEditorVRFunctions::DeserializeEditableLevel(UObject* WorldContextObject, FMemoryReader& FileReader)
{
	//Ajustar el objeto de posición de inicio del personaje
	UE_LOG(LogTemp, Log, TEXT("PlayerStart:"));
	if (!DeserializePlayerLocation(WorldContextObject, FString(TEXT("PlayerStart_C")))) return false;
	
	//Ajustar el objeto de posición final del personaje
	UE_LOG(LogTemp, Log, TEXT("PlayerEnd:"));
	if (!DeserializePlayerLocation(WorldContextObject, FString(TEXT("PlayerEnd_C")))) return false;
	
	//Ajustar la textura del piso de la sala principal
	UE_LOG(LogTemp, Log, TEXT("Floor:"));
	if (!DeserializeFloorAndRoof(WorldContextObject, FString(TEXT("Floor_C")))) return false;
	
	//Ajustar la textura del techo de la sala principal
	UE_LOG(LogTemp, Log, TEXT("Roof:"));
	if (!DeserializeFloorAndRoof(WorldContextObject, FString(TEXT("Roof_C")))) return false;
	
	//Ajustar la textura de las paredes de la sala principal
	UE_LOG(LogTemp, Log, TEXT("Walls:"));
	/***********************************************************************************************
											FALTA TERMINAR
	 ***********************************************************************************************/
	
	if (!DeserializeEditableObjectArray(FileReader, WorldContextObject)) return false;
	
	UE_LOG(LogTemp, Display, TEXT("El archivo ha sido leido correctamente."));
	return true;
}

/** Función de deserialización para un objeto de ubicación del personaje. */
bool UEditorVRFunctions::DeserializePlayerLocation(FMemoryReader& FileReader, const TCHAR* ClassName)
{
	//Obtener una referencia a la clase solicitada
	FString ClassPath = GetEditableClassPath(FString(ClassName));
	TSubclassOf<AActor> PlayerLocationClass = StaticLoadClass(AActor::StaticClass(), NULL, ClassPath, NULL, LOAD_None, NULL);
	
	//Buscar todos los actores de dicha clase que estén dentro del mapa activo y verificar que la lista no sea vacía.
	TArray<AActor*> PlayerLocationArray;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, PlayerLocationClass, PlayerLocationArray);
	if (PlayerLocationArray.Num() <= 0) return DisplayErrorMessage(TEXT("- No se encontro ningun objeto en el mapa del Editor."), false);
	
	//Obtener la posición del objeto de ubicación
	FVector Location;
	FileReader << Location;
	UE_LOG(LogTemp, Log, TEXT("- Posicion: %.2f %.2f %.2f"), Location.X, Location.Y, Location.Z);

	//Reemplazar la posición del primer objeto de la lista
	PlayerLocationArray[0]->SetActorLocation(Location);
	UE_LOG(LogTemp, Log, TEXT("- Se ajusto el objeto de ubicacion correctamente."));
	return true;
}

/** Función de deserialización para el piso y el techo del mapa. */
bool UEditorVRFunctions::DeserializeFloorAndRoof(FMemoryReader& FileReader, const TCHAR* ClassName)
{
	//Obtener una referencia a la clase solicitada
	FString ClassPath = GetEditableClassPath(FString(ClassName));
	TSubclassOf<AActor> Class = StaticLoadClass(AActor::StaticClass(), NULL, ClassPath, NULL, LOAD_None, NULL);
	
	//Buscar todos los actores de dicha clase que estén dentro del mapa activo y verificar que la lista no sea vacía.
	TArray<AActor*> Array;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, Class, Array);
	if (Array.Num() <= 0) return DisplayErrorMessage(TEXT("- No se encontro ningun objeto en el mapa del Editor."), false);
	
	/***********************************************************************************************
											FALTA TERMINAR
	 ***********************************************************************************************/
	return true;
}

/** Función de deserialización para el arreglo de objetos editables ubicados en el nivel a guardarse. */
bool UEditorVRFunctions::DeserializeEditableObjectArray(FMemoryReader& FileReader, UObject* WorldContextObject)
{
	//Obtener las referencias necesarias para la creación de objetos.
	if (!GEngine) return DisplayErrorMessage(TEXT("No se pudo obtener la referencia al mundo desde GEngine."), false);
	UWorld* CurrentWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	if (!CurrentWorld) return DisplayErrorMessage(TEXT("No se pudo obtener la referencia al mundo desde GetWorldFromContextObject."), false);
	
	//Leer el número de objetos editables guardados
	int32 EditableObjectCount;
	FileReader << EditableObjectCount;
	
	//Leer los datos de los objetos editables y ubicarlos en el mapa
	for (int32 i = 0; i < EditableObjectCount; i++)
	{
		UE_LOG(LogTemp, Log, TEXT("Objeto %d de %d:"), i + 1, EditableObjectCount);
		DeserializeEditableObject(FileReader, CurrentWorld);
	}
	
	return true;
}

/** Función de deserialización para un objeto editable. */
bool UEditorVRFunctions::DeserializeEditableObject(FMemoryReader& FileReader, UWorld* CurrentWorld)
{
	//Obtener el nombre de la clase y su dirección completa
	FString ClassName;
	FileReader << ClassName;
	FString ClassPath = GetEditableClassPath(ClassName);
	UE_LOG(LogTemp, Log, TEXT("- Clase: %s (%s)"), *ClassName, *ClassPath);
		
	//Obtener la posición del objeto editable
	FVector Location;
	FileReader << Location;
	UE_LOG(LogTemp, Log, TEXT("- Posicion: %.2f %.2f %.2f"), Location.X, Location.Y, Location.Z);

	//Escribir la rotación del objeto
	FRotator Rotation;
	FileReader << Rotation;
	UE_LOG(LogTemp, Log, TEXT("- Rotacion: %.2f %.2f %.2f"), Rotation.Pitch, Rotation.Roll, Rotation.Yaw);

	//Escribir la escala del objeto
	float Scale;
	FileReader << Scale;
	UE_LOG(LogTemp, Log, TEXT("- Escala: %.2f"), Scale);

	//Obtener las referencias necesarias para poder crear el objeto editable
	UClass* EditableObjectClass = StaticLoadClass(AActor::StaticClass(), NULL, *ClassPath, NULL, LOAD_None, NULL);
	if (!EditableObjectClass) return DisplayErrorMessage(TEXT("- No se pudo obtener la referencia al objeto editable."), false);
	
	//Generar el objeto editable
	AActor* EditableObject = CurrentWorld->SpawnActor(EditableObjectClass);
	if (!EditableObject) return DisplayErrorMessage(TEXT("- No se pudo crear el objeto editable."), false);
	
	//Colocar el objeto editable en la posición, rotación y escala correspondientes
	EditableObject->SetActorLocation(Location);
	EditableObject->SetActorRotation(Rotation);
	EditableObject->SetActorScale3D(FVector(Scale));
	UE_LOG(LogTemp, Log, TEXT("- Objeto editable creado y posicionado correctamente."));
	return true;
}
