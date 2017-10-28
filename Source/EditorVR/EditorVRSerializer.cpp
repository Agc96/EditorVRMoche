// EditorVRSerializer.cpp
// Copyright (C) 2017 Desarrollo de Programas 2 "VR Moche" - Pontificia Universidad Católica del Perú.
// Todos los derechos reservados.

#include "EditorVRSerializer.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////  SERIALIZACIÓN  ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

// Función de serialización para un nivel del Editor de Niveles. Guarda en un archivo binario los objetos
// editables creados con el Editor de Niveles, además de la ubicación del personaje en la escena.
bool UEditorVRSerializer::SerializeLevel(UObject* WorldContextObject, const FString& SaveFilePath)
{
	FBufferArchive SaveFileBuffer;

	//Obtener los objetos de ubicación del personaje
	TSubclassOf<AActor> EditablePlayerLocationClass = StaticLoadClass(AActor::StaticClass(), NULL,
		TEXT("Blueprint'/Game/Classes/EditablePlayerStart.EditablePlayerStart_C'"), NULL, LOAD_None, NULL);
	TArray<AActor*> EditablePlayerLocationArray;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, EditablePlayerLocationClass, EditablePlayerLocationArray);

	//Guardar el número de objetos de ubicación del personaje
	int32 EditablePlayerLocationCount = EditablePlayerLocationArray.Num();
	SaveFileBuffer << EditablePlayerLocationCount;

	//Guardar los datos de los objetos de ubicación del personaje
	for (int32 i = 0; i < EditablePlayerLocationCount; i++) {
		UE_LOG(LogTemp, Log, TEXT("Posicion %d de %d:"), i + 1, EditablePlayerLocationCount);
		SerializeEditablePlayerPosition(SaveFileBuffer, EditablePlayerLocationArray[i]);
	}

	//Obtener los objetos editables del mapa
	TSubclassOf<AActor> EditableObjectClass = StaticLoadClass(AActor::StaticClass(), NULL,
		TEXT("Blueprint'/Game/Classes/EditableObject.EditableObject_C'"), NULL, LOAD_None, NULL);
	TArray<AActor*> EditableObjectArray;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, EditableObjectClass, EditableObjectArray);

	//Guardar el número de objetos editables
	int32 EditableObjectCount = EditableObjectArray.Num();
	SaveFileBuffer << EditableObjectCount;

	//Guardar los datos de los objetos editables que están dentro del mapa
	for (int32 i = 0; i < EditableObjectCount; i++) {
		UE_LOG(LogTemp, Log, TEXT("Objeto %d de %d:"), i + 1, EditableObjectCount);
		SerializeEditableObject(SaveFileBuffer, EditableObjectArray[i]);
	}

	//Guardar los datos en el archivo binario indicado.
	if (SaveFileBuffer.Num() <= 0) return false;
	if (FFileHelper::SaveArrayToFile(SaveFileBuffer, *SaveFilePath)) {
		UE_LOG(LogTemp, Display, TEXT("El archivo se guardo con exito."));
		return true;
	}
	UE_LOG(LogTemp, Error, TEXT("No se pudo guardar el archivo."));
	SaveFileBuffer.FlushCache();
	SaveFileBuffer.Empty();
	return false;
}

// Función de serialización para un objeto de ubicación del personaje.
void UEditorVRSerializer::SerializeEditablePlayerPosition(FBufferArchive& SaveFileBuffer, AActor* EditablePlayerLocation)
{
	//Escribir el nombre de la clase
	FString ClassName = EditablePlayerLocation->GetClass()->GetName();
	UE_LOG(LogTemp, Log, TEXT("- Nombre: %s"), *ClassName);
	SaveFileBuffer << ClassName;

	//Escribir la posición del objeto
	FVector Location = EditablePlayerLocation->GetActorLocation();
	UE_LOG(LogTemp, Log, TEXT("- Posicion: %.2f %.2f %.2f"), Location.X, Location.Y, Location.Z);
	SaveFileBuffer << Location;
}

// Función de serialización para un objeto editable.
void UEditorVRSerializer::SerializeEditableObject(FBufferArchive& SaveFileBuffer, AActor* EditableObject)
{
	//Escribir el nombre de la clase
	FString ClassName = EditableObject->GetClass()->GetName();
	UE_LOG(LogTemp, Log, TEXT("- Nombre: %s"), *ClassName);
	SaveFileBuffer << ClassName;

	//Escribir la posición del objeto
	FVector Location = EditableObject->GetActorLocation();
	UE_LOG(LogTemp, Log, TEXT("- Posicion: %.2f %.2f %.2f"), Location.X, Location.Y, Location.Z);
	SaveFileBuffer << Location;

	//Escribir la rotación del objeto
	FRotator Rotation = EditableObject->GetActorRotation();
	UE_LOG(LogTemp, Log, TEXT("- Rotacion: %.2f %.2f %.2f"), Rotation.Pitch, Rotation.Roll, Rotation.Yaw);
	SaveFileBuffer << Rotation;

	//Escribir la escala del objeto
	FVector Scale = EditableObject->GetActorScale();
	UE_LOG(LogTemp, Log, TEXT("- Escala: %.2f %.2f %.2f"), Scale.X, Scale.Y, Scale.Z);
	SaveFileBuffer << Scale.X;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////// DESERIALIZACIÓN ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

// Función de deserialización para un nivel del Editor de Niveles. Toma un archivo binario y lo lee para
// ubicar los objetos editables creados con anterioridad, además de ubicar al personaje en la escena.
bool UEditorVRSerializer::DeserializeLevel(UObject* WorldContextObject, const FString& OpenFilePath)
{
	//Abrir el archivo binario
	TArray<uint8> OpenFileBuffer;
	if (!FFileHelper::LoadFileToArray(OpenFileBuffer, *OpenFilePath)) {
		UE_LOG(LogTemp, Error, TEXT("No se pudo cargar el archivo."));
		return false;
	}
	if (OpenFileBuffer.Num() <= 0) return false;
	FMemoryReader OpenFileReader = FMemoryReader(OpenFileBuffer, true);
	OpenFileReader.Seek(0);

	//Leer la cantidad de objetos de ubicación del personaje
	int32 EditablePlayerLocationCount;
	OpenFileReader << EditablePlayerLocationCount;

	//Obtener la lista de objetos de ubicación del personaje
	TSubclassOf<AActor> EditablePlayerLocationClass = StaticLoadClass(AActor::StaticClass(), NULL,
		TEXT("Blueprint'/Game/Classes/EditablePlayerStart.EditablePlayerStart_C'"), NULL, LOAD_None, NULL);
	TArray<AActor*> EditablePlayerLocationArray;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, EditablePlayerLocationClass, EditablePlayerLocationArray);

	//Leer los datos de los objetos de ubicación del personaje y reemplazarlas en el mapa
	for (int32 i = 0; i < EditablePlayerLocationCount; i++) {
		UE_LOG(LogTemp, Log, TEXT("Posicion %d de %d:"), i + 1, EditablePlayerLocationCount);
		DeserializeEditablePlayerLocation(OpenFileReader, EditablePlayerLocationArray);
	}

	//Leer el número de objetos editables guardados
	int32 EditableObjectCount;
	OpenFileReader << EditableObjectCount;

	//Leer los datos de los objetos editables y ubicarlos en el mapa
	for (int32 i = 0; i < EditableObjectCount; i++) {
		UE_LOG(LogTemp, Log, TEXT("Objeto %d de %d:"), i + 1, EditableObjectCount);
		DeserializeEditableObject(OpenFileReader, WorldContextObject);
	}

	//Cerrar el archivo
	UE_LOG(LogTemp, Display, TEXT("El archivo ha sido leido correctamente."));
	OpenFileReader.FlushCache();
	OpenFileBuffer.Empty();
	OpenFileReader.Close();
	return true;
}

// Función de deserialización para un objeto de ubicación del personaje.
bool UEditorVRSerializer::DeserializeEditablePlayerLocation(FMemoryReader& OpenFileReader,
	TArray<AActor*> EditablePlayerLocationArray)
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
	for (int32 i = 0; i < EditablePlayerLocationArray.Num(); i++) {
		if (ClassName.Compare(EditablePlayerLocationArray[i]->GetClass()->GetName()) == 0) {
			EditablePlayerLocationArray[i]->SetActorLocation(Location);
			UE_LOG(LogTemp, Log, TEXT("- Se ajusto el objeto de ubicacion correctamente."));
			return true;
		}
	}
	UE_LOG(LogTemp, Error, TEXT("- No se pudo encontrar al objeto de ubicacion en el mapa."));
	return false;
}

// Función de deserialización para un objeto editable.
AActor* UEditorVRSerializer::DeserializeEditableObject(FMemoryReader& OpenFileReader, UObject* WorldContextObject)
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
	UWorld* CurrentWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	if (!EditableObjectClass || !CurrentWorld) {
		UE_LOG(LogTemp, Error, TEXT("- No se pudieron obtener las referencias para generar el objeto."));
		return NULL;
	}

	//Generar el objeto y colocarlo en el mapa
	AActor* EditableObject = CurrentWorld->SpawnActor(EditableObjectClass);
	if (!EditableObject) {
		UE_LOG(LogTemp, Error, TEXT("- No se pudo crear el objeto editable %s."), *ClassName);
		return NULL;
	}
	EditableObject->SetActorLocation(Location);
	EditableObject->SetActorRotation(Rotation);
	EditableObject->SetActorScale3D(FVector(Scale));
	UE_LOG(LogTemp, Log, TEXT("- Objeto editable creado y posicionado correctamente."));
	return EditableObject;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// FUNCIONES  AUXILIARES /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

// Función auxiliar que mapea la dirección completa de la clase guardada en el archivo serializable, de
// manera que se pueda usar en el SpawnActor(). Esta parte del código va a variar entre el editor y el
// juego, ya que depende de las ubicaciones de las clases dentro del proyecto fuente. (Content/)
FString UEditorVRSerializer::GetEditableObjectClassPath(const FString& ClassName)
{
	if (ClassName.Compare(FString(TEXT("EditableCube_C"))) == 0)
		return FString(TEXT("Blueprint'/Game/Classes/EditableCube.EditableCube_C'"));
	if (ClassName.Compare(FString(TEXT("EditableSphere_C"))) == 0)
		return FString(TEXT("Blueprint'/Game/Classes/EditableSphere.EditableSphere_C'"));
	//Si por casualidad no se encuentra la clase exacta, retornar una cadena vacía.
	return FString();
}
