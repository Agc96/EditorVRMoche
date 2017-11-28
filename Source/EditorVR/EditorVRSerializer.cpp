/**
 * Funciones para la serialización de los niveles creados con el Editor de Niveles.
 * @author Anthony Gutiérrez
 */

#include "EditorVRFunctions.h"

 // Función de guardado de un nivel ya existente del Editor de Niveles.
bool UEditorVRFunctions::SaveExistingLevel(UObject* WorldContextObject, const FString& FilePath)
{
	FBufferArchive FileBuffer;
	return SerializeLevel(FileBuffer, WorldContextObject, FilePath);
}

// Función de guardado de un nivel nuevo del Editor de Niveles.
bool UEditorVRFunctions::SaveNewLevel(UObject* WorldContextObject, const FString& FilePath)
{
	if (!CheckIfCreateOrReplaceFile(FilePath)) return false;

	FBufferArchive FileBuffer;
	return SerializeLevel(FileBuffer, WorldContextObject, FilePath);
}

// Función de serialización para un nivel del Editor de Niveles. Guarda en un archivo binario los objetos editables creados con el
// Editor de Niveles, además de la ubicación del personaje en la escena.
bool UEditorVRFunctions::SerializeLevel(FBufferArchive& FileBuffer, UObject* WorldContextObject, const FString& FilePath)
{
	//Escribir el file signature
	SerializeFileSignature(FileBuffer);

	//Obtener los objetos de ubicación del personaje
	TSubclassOf<AActor> PlayerLocationClass = StaticLoadClass(AActor::StaticClass(), NULL,
		PlayerLocationClassPath, NULL, LOAD_None, NULL);
	TArray<AActor*> PlayerLocationArray;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, PlayerLocationClass, PlayerLocationArray);

	//Guardar el número de objetos de ubicación del personaje
	int32 PlayerLocationCount = PlayerLocationArray.Num();
	FileBuffer << PlayerLocationCount;

	//Guardar los datos de los objetos de ubicación del personaje
	for (int32 i = 0; i < PlayerLocationCount; i++)
	{
		UE_LOG(LogTemp, Log, TEXT("Posicion %d de %d:"), i + 1, PlayerLocationCount);
		SerializePlayerPosition(FileBuffer, PlayerLocationArray[i]);
	}

	//Obtener los objetos editables del mapa
	TSubclassOf<AActor> EditableObjectClass = StaticLoadClass(AActor::StaticClass(), NULL,
		EditableObjectClassPath, NULL, LOAD_None, NULL);
	TArray<AActor*> EditableObjectArray;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, EditableObjectClass, EditableObjectArray);

	//Guardar el número de objetos editables
	int32 EditableObjectCount = EditableObjectArray.Num();
	FileBuffer << EditableObjectCount;

	//Guardar los datos de los objetos editables que están dentro del mapa
	for (int32 i = 0; i < EditableObjectCount; i++) {
		UE_LOG(LogTemp, Log, TEXT("Objeto %d de %d:"), i + 1, EditableObjectCount);
		SerializeEditableObject(FileBuffer, EditableObjectArray[i]);
	}

	//Guardar los datos en el archivo binario indicado.
	if (FileBuffer.Num() <= 0) return false;
	if (FFileHelper::SaveArrayToFile(FileBuffer, *FilePath))
	{
		DisplayMessage(EAppMsgType::Ok, TEXT("El archivo se guardo con exito."), TEXT("Editor VR Moche"));
		return true;
	}
	DisplayMessage(EAppMsgType::Ok, TEXT("No se pudo guardar el archivo."), TEXT("Error"));
	FileBuffer.FlushCache();
	FileBuffer.Empty();
	return false;
}

// Función de serialización del file signature.
void UEditorVRFunctions::SerializeFileSignature(FBufferArchive& FileBuffer)
{
	uint8 F = 86, i = 82, l = 77, e = 111, Sig = 99, na = 104, tu = 101, re = 0;
	FileBuffer << F << i << l << e << Sig << na << tu << re;
}

// Función de serialización para un objeto de ubicación del personaje.
void UEditorVRFunctions::SerializePlayerPosition(FBufferArchive& FileBuffer, AActor* PlayerLocation)
{
	//Escribir el nombre de la clase
	FString ClassName = PlayerLocation->GetClass()->GetName();
	UE_LOG(LogTemp, Log, TEXT("- Nombre: %s"), *ClassName);
	FileBuffer << ClassName;

	//Escribir la posición del objeto
	FVector Location = PlayerLocation->GetActorLocation();
	UE_LOG(LogTemp, Log, TEXT("- Posicion: %.2f %.2f %.2f"), Location.X, Location.Y, Location.Z);
	FileBuffer << Location;
}

// Función de serialización para un objeto editable.
void UEditorVRFunctions::SerializeEditableObject(FBufferArchive& FileBuffer, AActor* EditableObject)
{
	//Escribir el nombre de la clase
	FString ClassName = EditableObject->GetClass()->GetName();
	UE_LOG(LogTemp, Log, TEXT("- Nombre: %s"), *ClassName);
	FileBuffer << ClassName;

	//Escribir la posición del objeto
	FVector Location = EditableObject->GetActorLocation();
	UE_LOG(LogTemp, Log, TEXT("- Posicion: %.2f %.2f %.2f"), Location.X, Location.Y, Location.Z);
	FileBuffer << Location;

	//Escribir la rotación del objeto
	FRotator Rotation = EditableObject->GetActorRotation();
	UE_LOG(LogTemp, Log, TEXT("- Rotacion: %.2f %.2f %.2f"), Rotation.Pitch, Rotation.Roll, Rotation.Yaw);
	FileBuffer << Rotation;

	//Escribir la escala del objeto
	FVector Scale = EditableObject->GetActorScale();
	UE_LOG(LogTemp, Log, TEXT("- Escala: %.2f %.2f %.2f"), Scale.X, Scale.Y, Scale.Z);
	FileBuffer << Scale.X;
}
