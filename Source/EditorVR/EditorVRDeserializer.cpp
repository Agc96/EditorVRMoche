/**
 * Funciones de deserialización de los niveles creados con el Editor de Niveles.
 * @author Anthony Gutiérrez
 */

#include "EditorVRFunctions.h"

TMap<FString, FString> UEditorVRFunctions::DecorationMaterials = TMap<FString, FString>();

/** Función que abre un nivel del Editor de Niveles, o verifica si el nivel si es correcto. */
bool UEditorVRFunctions::OpenExtraLevel(UObject* WorldContextObject, const FString& FileName, bool Deserialize)
{
#if PLATFORM_ANDROID
	//Generar el path completo
	FString FilePath = GetExtraLevelDirectory() / FileName + TEXT(".") + EditorFileExtension;
#else
	//En el Editor de Niveles, esta opción no debe poder escogerse.
	if (FileName.Equals(FString(TEXT("Seleccione un nivel...")))) return false;
	//Generar el path completo
	FString FilePath = GetExtraLevelDirectory() / FileName;	//Incluye ya la extensión
	UE_LOG(LogTemp, Log, TEXT("Archivo a abrirse: %s"), *FilePath);
#endif
	
	//Abrir el archivo binario y verificar el tamaño del archivo
	TArray<uint8> FileBufferArray;
	if (!FFileHelper::LoadFileToArray(FileBufferArray, *FilePath))
	{
		DisplayMessage(EAppMsgType::Ok, TEXT("No se pudo cargar el archivo."), TEXT("Error"));
		return false;
	}
	if (FileBufferArray.Num() <= 8)
	{
		DisplayMessage(EAppMsgType::Ok, TEXT("El archivo no tiene el formato adecuado para el Editor de Niveles.\
 Asegurese de que el archivo sea un archivo generado por este Editor."), TEXT("Error"));
		return false;
	}
	
	//Abrir el lector de archivos binarios
	FMemoryReader FileReader = FMemoryReader(FileBufferArray, true);
	FileReader.Seek(0);
	//Verificar si el file signature es correcto
	if (!DeserializeFileSignature(FileReader))
	{
		DisplayMessage(EAppMsgType::Ok, TEXT("El archivo no tiene el formato adecuado para el Editor de Niveles.\
 Asegurese de que el archivo sea un archivo generado por este Editor."), TEXT("Error"));
		return false;
	}
	
	//Deserializar el archivo si el flag está activado, y luego liberar la memoria.
	bool WasFileOpened = (Deserialize) ? DeserializeEditableLevel(FileReader, WorldContextObject) : true;
	FileReader.FlushCache();
	FileBufferArray.Empty();
	FileReader.Close();
	return WasFileOpened;
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
bool UEditorVRFunctions::DeserializeEditableLevel(FMemoryReader& FileReader, UObject* WorldContextObject)
{
	//Ajustar los objetos de posición inicial y final del personaje
	DeserializePlayerLocation(FileReader, WorldContextObject, TEXT("PlayerStart_C"));
	DeserializePlayerLocation(FileReader, WorldContextObject, TEXT("PlayerEnd_C"));
	
	//Ajustar las texturas del piso, techo y paredes de la sala principal
	DeserializeFloorAndRoof(FileReader, WorldContextObject, TEXT("Floor_C"));
	DeserializeFloorAndRoof(FileReader, WorldContextObject, TEXT("Roof_C"));
	DeserializeWalls(FileReader, WorldContextObject);
	
	//Crear y ajustar la posición, rotación y escala de cada objeto editable del mapa
	DeserializeEditableObjectArray(FileReader, WorldContextObject);
	
	UE_LOG(LogTemp, Display, TEXT("El archivo ha sido leido correctamente."));
	return true;
}

/** Función de deserialización para un objeto de ubicación del personaje. */
bool UEditorVRFunctions::DeserializePlayerLocation(FMemoryReader& FileReader, UObject* WorldContextObject, const TCHAR* ClassName)
{	
	//Obtener la posición del objeto de ubicación
	FVector Location;
	FileReader << Location;
	UE_LOG(LogTemp, Log, TEXT("- Posicion: %.2f %.2f %.2f"), Location.X, Location.Y, Location.Z);
	
	//Obtener una referencia a la clase solicitada
	FString ClassPath = GetEditableClassPath(FString(ClassName));
	TSubclassOf<AActor> PlayerLocationClass = StaticLoadClass(AActor::StaticClass(), NULL, *ClassPath, NULL, LOAD_None, NULL);
	
	//Buscar todos los actores de dicha clase que estén dentro del mapa activo y verificar que la lista no sea vacía.
	TArray<AActor*> PlayerLocationArray;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, PlayerLocationClass, PlayerLocationArray);
	if (PlayerLocationArray.Num() <= 0)
	{
		UE_LOG(LogTemp, Log, TEXT("- No se encontro ningun %s en el mapa del Editor."), ClassName);
		return false;
	}
	
	//Reemplazar la posición del primer objeto de la lista
	PlayerLocationArray[0]->SetActorLocation(Location);
	UE_LOG(LogTemp, Log, TEXT("- Se ajusto el objeto de ubicacion %s correctamente."), ClassName);
	return true;
}

/** Función de deserialización para el piso y el techo del mapa. */
void UEditorVRFunctions::DeserializeFloorAndRoof(FMemoryReader& FileReader, UObject* WorldContextObject, const TCHAR* ClassName)
{	
	//Obtener el nombre del material
	FString MaterialName;
	FileReader << MaterialName;
	UE_LOG(LogTemp, Log, TEXT("- Nombre del material: %s"), *MaterialName);
	
	//Colocarlo en el mapa de nombres de objeto - materiales.
	DecorationMaterials.Add(FString(ClassName), MaterialName);
}

/** Función de deserialización para las paredes del mapa. */
void UEditorVRFunctions::DeserializeWalls(FMemoryReader& FileReader, UObject* WorldContextObject)
{
	//Obtener la cantidad de objetos de ubicación del personaje guardados
	int32 WallCount = 0;
	FileReader << WallCount;
	UE_LOG(LogTemp, Log, TEXT("Objeto de decoracion Wall_C: Count = %d"), WallCount);
	
	//Obtener los nombres de las paredes y los nombres de los materiales para cada pared.
	for (int32 i = 0; i < WallCount; i++)
	{
		FString WallName, MaterialName;
		FileReader << WallName << MaterialName;
		UE_LOG(LogTemp, Log, TEXT("- Nombre = %s, Material = %s"), *WallName, *MaterialName);
		
		//Colocar en el mapa de nombres de objeto - materiales.
		DecorationMaterials.Add(WallName, MaterialName);
	}
}

/** Función de deserialización para el arreglo de objetos editables ubicados en el nivel a guardarse. */
bool UEditorVRFunctions::DeserializeEditableObjectArray(FMemoryReader& FileReader, UObject* WorldContextObject)
{
	//Obtener las referencias necesarias para la creación de objetos.
	if (!GEngine)
	{
		UE_LOG(LogTemp, Log, TEXT("No se pudo obtener la referencia al mundo desde GEngine."));
		return false;
	}
	UWorld* CurrentWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	if (!CurrentWorld)
	{
		UE_LOG(LogTemp, Log, TEXT("No se pudo obtener la referencia al mundo desde GetWorldFromContextObject."));
		return false;
	}
	
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
	FVector Scale;
	FileReader << Scale;
	UE_LOG(LogTemp, Log, TEXT("- Escala: %.2f %.2f %.2f"), Scale.X, Scale.Y, Scale.Z);

	//Obtener las referencias necesarias para poder crear el objeto editable
	UClass* EditableObjectClass = StaticLoadClass(AActor::StaticClass(), NULL, *ClassPath, NULL, LOAD_None, NULL);
	if (!EditableObjectClass)
	{
		UE_LOG(LogTemp, Log, TEXT("- No se pudo obtener la referencia al objeto editable."));
		return false;
	}
	
	//Generar el objeto editable
	AActor* EditableObject = CurrentWorld->SpawnActor(EditableObjectClass);
	if (!EditableObject)
	{
		UE_LOG(LogTemp, Log, TEXT("- No se pudo crear el objeto editable."));
		return false;
	}

	//Colocar el objeto editable en la posición, rotación y escala correspondientes
	EditableObject->SetActorLocation(Location);
	EditableObject->SetActorRotation(Rotation);
	EditableObject->SetActorScale3D(Scale);
	UE_LOG(LogTemp, Log, TEXT("- Objeto editable creado y posicionado correctamente."));
	return true;
}

TMap<FString, FString> UEditorVRFunctions::GetDecorationMaterials()
{
	return DecorationMaterials;
}