// Hecho por el grupo de Desarrollo de Programas 2 "VR Moche".
// Copyright (C) 2017 Pontificia Universidad Católica del Perú.
// Todos los derechos reservados.

#include "EditorVRFunctions.h"

// Función de serialización para todo el nivel del Editor de Niveles.
// Guarda en un archivo binario los objetos editables creados con el Editor de Niveles, además de la ubicación 
// del personaje en la escena.
bool UEditorVRFunctions::SerializeLevel(UObject* WorldContextObject, FString FileName)
{
	std::ofstream File(*FileName, std::ios::out | std::ios::binary);
	if (!File) {
		DisplayMessage("No se pudo generar el archivo.", "Error");
		return false;
	}

	//Escribir el "file signature"
	File.write("VRMoche", 8 * sizeof(char));

	//Escribir la posición del personaje
	FVector PlayerStart(0, 0, 120);
	//ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(WorldContextObject, 0);
	File.write(reinterpret_cast<const char*>(&PlayerStart.X), sizeof(PlayerStart.X));
	File.write(reinterpret_cast<const char*>(&PlayerStart.Y), sizeof(PlayerStart.Y));
	File.write(reinterpret_cast<const char*>(&PlayerStart.Z), sizeof(PlayerStart.Z));

	//Obtener los objetos editables
	TSubclassOf<AActor> EditableObjectClass = StaticLoadClass(AActor::StaticClass(), NULL,
		TEXT("Blueprint'/Game/Classes/EditableObject.EditableObject_C'"), NULL, LOAD_None, NULL);
	TArray<AActor*> EditableObjectArray;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, EditableObjectClass, EditableObjectArray);

	//Escribir el número de objetos editables
	int EditableObjectCount = EditableObjectArray.Num();
	if (EditableObjectCount < 0)
		UE_LOG(LogTemp, Warning, TEXT("EditableObjects tiene cantidad negativa, por alguna razón..."));
	File.write(reinterpret_cast<const char*>(&EditableObjectCount), sizeof(EditableObjectCount));

	//Escribir los datos de los objetos editables
	for (int32 i = 0; i < EditableObjectCount; i++) {
		UE_LOG(LogTemp, Log, TEXT("Objeto #%d"), i + 1);
		SerializeEditableObject(File, EditableObjectArray[i]);
	}

	//Cerrar el archivo
	File.close();
	DisplayMessage("Se generó el archivo correctamente.", "Editor VR Moche");
	return true;
}

// Función de serialización para un objeto editable.
void UEditorVRFunctions::SerializeEditableObject(std::ofstream& File, AActor* EditableObject)
{
	//Escribir el nombre de la clase
	FString ClassNameString = EditableObject->GetClass()->GetPathName();
	const char* ClassNameConstChar = TCHAR_TO_ANSI(*ClassNameString);
	UE_LOG(LogTemp, Log, TEXT("- Nombre: %s"), *ClassNameString);
	File.write(ClassNameConstChar, sizeof(char) * (strlen(ClassNameConstChar) + 1));

	//Escribir la posición del objeto
	FVector vector = EditableObject->GetActorLocation();
	UE_LOG(LogTemp, Log, TEXT("- Posicion: %.2f %.2f %.2f"), vector.X, vector.Y, vector.Z);
	File.write(reinterpret_cast<const char*>(&vector.X), sizeof(vector.X));
	File.write(reinterpret_cast<const char*>(&vector.Y), sizeof(vector.Y));
	File.write(reinterpret_cast<const char*>(&vector.Z), sizeof(vector.Z));

	//Escribir la rotación del objeto
	FRotator rotator = EditableObject->GetActorRotation();
	UE_LOG(LogTemp, Log, TEXT("- Rotacion: %.2f %.2f %.2f"), rotator.Pitch, rotator.Roll, rotator.Yaw);
	File.write(reinterpret_cast<const char*>(&rotator.Pitch), sizeof(rotator.Pitch));
	File.write(reinterpret_cast<const char*>(&rotator.Roll), sizeof(rotator.Roll));
	File.write(reinterpret_cast<const char*>(&rotator.Yaw), sizeof(rotator.Yaw));

	//Escribir la escala del objeto
	FVector scale = EditableObject->GetActorScale();
	UE_LOG(LogTemp, Log, TEXT("- Escala: %.2f %.2f %.2f"), scale.X, scale.Y, scale.Z)
	File.write(reinterpret_cast<const char*>(&scale.X), sizeof(scale.X));
}

// Función de deserialización.
// Toma un archivo binario y lo lee para ubicar los objetos editables creados con anterioridad, además de ubicar
// al personaje en la escena.
bool UEditorVRFunctions::DeserializeLevel(UObject* WorldContextObject, FString FileName)
{
	std::ifstream File(*FileName, std::ios::in | std::ios::binary);
	if (!File) {
		DisplayMessage("No se pudo abrir el archivo.", "Error");
		return false;
	}
	
	//Verificar si el archivo es del tipo serializable que hemos creado (file signature)
	char FileSignature[9] = { '\0' };
	File.read(FileSignature, 8 * sizeof(char));
	if (strcmp(FileSignature, "VRMoche") != 0)
	{
		DisplayMessage("El archivo seleccionado no es del tipo serializable. Asegúrese de que haya sido \
un archivo generado por este editor.", "Error");
		File.close();
		return false;
	}

	//Abrir el nivel de edición
	UGameplayStatics::OpenLevel(WorldContextObject, FName("EditLevel"));

	//Leer la posición del personaje
	float PlayerX, PlayerY, PlayerZ;
	File.read(reinterpret_cast<char*>(&PlayerX), sizeof(PlayerX));
	File.read(reinterpret_cast<char*>(&PlayerY), sizeof(PlayerY));
	File.read(reinterpret_cast<char*>(&PlayerZ), sizeof(PlayerZ));
	UE_LOG(LogTemp, Log, TEXT("Vector personaje: %.2f %.2f %.2f"), PlayerX, PlayerY, PlayerZ);

	//Leer la cantidad de objetos editables a colocar
	int EditableObjectCount;
	File.read(reinterpret_cast<char*>(&EditableObjectCount), sizeof(EditableObjectCount));
	UE_LOG(LogTemp, Log, TEXT("# objetos: %d"), EditableObjectCount);

	for (int i = 0; i < EditableObjectCount; i++) {
		UE_LOG(LogTemp, Log, TEXT("Objeto #%d:"), EditableObjectCount);
	}

	//Cerrar el archivo
	File.close();
	DisplayMessage("Se abrió el archivo correctamente.", "Editor VR Moche");
	return true;
}

void UEditorVRFunctions::DisplayMessage(const char* Message, const char* Title)
{
	FString TitleString(UTF8_TO_TCHAR(Title)), MessageString(UTF8_TO_TCHAR(Message));
	FText TitleText = FText::FromString(TitleString), MessageText = FText::FromString(MessageString);
	FMessageDialog::Open(EAppMsgType::Ok, MessageText, &TitleText);
}
