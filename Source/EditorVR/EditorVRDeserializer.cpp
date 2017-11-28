/**
 * Funciones de deserializaci�n de los niveles creados con el Editor de Niveles.
 * @author Anthony Guti�rrez
 */

#include "EditorVRFunctions.h"

// Funci�n de deserializaci�n para un nivel del Editor de Niveles. Toma un archivo binario y lo lee para
// ubicar los objetos editables creados con anterioridad, adem�s de ubicar al personaje en la escena.
bool UEditorVRFunctions::DeserializeLevel(UObject* WorldContextObject, const FString& FilePath)
{
	//Abrir el archivo binario
	TArray<uint8> OpenFileBuffer;
	if (!FFileHelper::LoadFileToArray(OpenFileBuffer, *FilePath))
	{
		DisplayMessage(EAppMsgType::Ok, TEXT("No se pudo cargar el archivo"), TEXT("Error"));
		return false;
	}
	if (OpenFileBuffer.Num() <= 8)
	{
		DisplayMessage(EAppMsgType::Ok, 
			TEXT("El archivo no se puede abrir, asegurese de que es un archivo generado por este Editor."),
			TEXT("Editor VR Moche"));
		return false;
	}
	FMemoryReader OpenFileReader = FMemoryReader(OpenFileBuffer, true);
	OpenFileReader.Seek(0);

	//Verificar si el file signature es correcto
	if (!DeserializeFileSignature(OpenFileReader))
	{
		DisplayMessage(EAppMsgType::Ok,
			TEXT("El archivo no se puede abrir, asegurese de que es un archivo generado por este Editor."),
			TEXT("Editor VR Moche"));
		return false;
	}

	//Leer la cantidad de objetos de ubicaci�n del personaje
	int32 PlayerLocationCount;
	OpenFileReader << PlayerLocationCount;

	//Obtener la lista de objetos de ubicaci�n del personaje
	TSubclassOf<AActor> PlayerLocationClass = StaticLoadClass(AActor::StaticClass(), NULL,
		PlayerLocationClassPath, NULL, LOAD_None, NULL);
	TArray<AActor*> PlayerLocationArray;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, PlayerLocationClass, PlayerLocationArray);

	//Leer los datos de los objetos de ubicaci�n del personaje y reemplazarlas en el mapa
	for (int32 i = 0; i < PlayerLocationCount; i++)
	{
		UE_LOG(LogTemp, Log, TEXT("Posicion %d de %d:"), i + 1, PlayerLocationCount);
		DeserializePlayerLocation(OpenFileReader, PlayerLocationArray);
	}

	//Leer el n�mero de objetos editables guardados
	int32 EditableObjectCount;
	OpenFileReader << EditableObjectCount;

	//Leer los datos de los objetos editables y ubicarlos en el mapa
	for (int32 i = 0; i < EditableObjectCount; i++)
	{
		UE_LOG(LogTemp, Log, TEXT("Objeto %d de %d:"), i + 1, EditableObjectCount);
		DeserializeEditableObject(OpenFileReader, WorldContextObject);
	}

	//Cerrar el archivo
	DisplayMessage(EAppMsgType::Ok, TEXT("El archivo ha sido leido correctamente."), TEXT("Editor VR Moche"));
	//UE_LOG(LogTemp, Display, TEXT("El archivo ha sido leido correctamente."));
	OpenFileReader.FlushCache();
	OpenFileBuffer.Empty();
	OpenFileReader.Close();
	return true;
}

// Funci�n de deserializaci�n del file signature.
bool UEditorVRFunctions::DeserializeFileSignature(FMemoryReader& OpenFileReader)
{
	uint8 F, i, l, e, Sig, na, tu, re;
	OpenFileReader << F << i << l << e << Sig << na << tu << re;
	return (F == 86 && i == 82 && l == 77 && e == 111 && Sig == 99 && na == 104 && tu == 101 && re == 0);
}

// Funci�n de deserializaci�n para un objeto de ubicaci�n del personaje.
bool UEditorVRFunctions::DeserializePlayerLocation(FMemoryReader& OpenFileReader, TArray<AActor*> PlayerLocationArray)
{
	//Obtener el nombre de la clase y su direcci�n completa
	FString ClassName;
	OpenFileReader << ClassName;
	UE_LOG(LogTemp, Log, TEXT("- Clase: %s"), *ClassName);

	//Obtener la posici�n del objeto de ubicaci�n
	FVector Location;
	OpenFileReader << Location;
	UE_LOG(LogTemp, Log, TEXT("- Posicion: %.2f %.2f %.2f"), Location.X, Location.Y, Location.Z);

	//Ubicar el objeto en la lista y reemplazar su posici�n
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

// Funci�n de deserializaci�n para un objeto editable.
bool UEditorVRFunctions::DeserializeEditableObject(FMemoryReader& OpenFileReader, UObject* WorldContextObject)
{
	//Obtener el nombre de la clase y su direcci�n completa
	FString ClassName;
	OpenFileReader << ClassName;
	FString ClassPath = GetEditableObjectClassPath(ClassName);
	UE_LOG(LogTemp, Log, TEXT("- Clase: %s (%s)"), *ClassName, *ClassPath);

	//Obtener la posici�n del objeto editable
	FVector Location;
	OpenFileReader << Location;
	UE_LOG(LogTemp, Log, TEXT("- Posicion: %.2f %.2f %.2f"), Location.X, Location.Y, Location.Z);

	//Escribir la rotaci�n del objeto
	FRotator Rotation;
	OpenFileReader << Rotation;
	UE_LOG(LogTemp, Log, TEXT("- Rotacion: %.2f %.2f %.2f"), Rotation.Pitch, Rotation.Roll, Rotation.Yaw);

	//Escribir la escala del objeto
	float Scale;
	OpenFileReader << Scale;
	UE_LOG(LogTemp, Log, TEXT("- Escala: %.2f"), Scale);

	//Obtener las referencias necesarias para poder crear el objeto editable
	UClass* EditableObjectClass = StaticLoadClass(AActor::StaticClass(), NULL, *ClassPath, NULL, LOAD_None, NULL);
	UWorld* CurrentWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	if (!EditableObjectClass || !CurrentWorld) {
		UE_LOG(LogTemp, Error, TEXT("- No se pudieron obtener las referencias para generar el objeto."));
		return false;
	}

	//Generar el objeto y colocarlo en el mapa
	AActor* EditableObject = CurrentWorld->SpawnActor(EditableObjectClass);
	if (!EditableObject) {
		UE_LOG(LogTemp, Error, TEXT("- No se pudo crear el objeto editable %s."), *ClassName);
		return false;
	}
	EditableObject->SetActorLocation(Location);
	EditableObject->SetActorRotation(Rotation);
	EditableObject->SetActorScale3D(FVector(Scale));
	UE_LOG(LogTemp, Log, TEXT("- Objeto editable creado y posicionado correctamente."));
	return true;
}
