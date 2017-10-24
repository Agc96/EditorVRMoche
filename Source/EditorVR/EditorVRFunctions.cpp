// Hecho por el grupo de Desarrollo de Programas 2 "VR Moche".
// Copyright (C) 2017 Pontificia Universidad Cat�lica del Per�.
// Todos los derechos reservados.

#include "EditorVRFunctions.h"

// Funci�n de serializaci�n para un nivel del Editor de Niveles. Guarda en un archivo binario los objetos
// editables creados con el Editor de Niveles, adem�s de la ubicaci�n del personaje en la escena.
bool UEditorVRFunctions::SerializeLevel(UObject* WorldContextObject, FString FileName)
{
	std::ofstream File(*FileName, std::ios::out | std::ios::binary);
	if (!File) {
		DisplayMessage("No se pudo generar el archivo.", "Error");
		return false;
	}

	//Escribir el "file signature"
	File.write("VRMoche", 8 * sizeof(char));

	//Escribir la posici�n del personaje
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

	//Escribir el n�mero de objetos editables
	int EditableObjectCount = EditableObjectArray.Num();
	if (EditableObjectCount < 0)
		UE_LOG(LogTemp, Warning, TEXT("EditableObjects tiene cantidad negativa, por alguna raz�n..."));
	File.write(reinterpret_cast<const char*>(&EditableObjectCount), sizeof(EditableObjectCount));

	//Escribir los datos de los objetos editables
	for (int32 i = 0; i < EditableObjectCount; i++) {
		UE_LOG(LogTemp, Log, TEXT("Objeto #%d"), i + 1);
		SerializeEditableObject(File, EditableObjectArray[i]);
	}

	//Cerrar el archivo
	File.close();
	DisplayMessage("Se gener� el archivo correctamente.", "Editor VR Moche");
	return true;
}

// Funci�n de serializaci�n para un objeto editable.
void UEditorVRFunctions::SerializeEditableObject(std::ofstream &File, AActor* EditableObject)
{
	//Escribir el nombre de la clase
	FString ClassNameString = EditableObject->GetClass()->GetName();
	const char* ClassNameChar = TCHAR_TO_ANSI(*ClassNameString);
	int ClassNameFullLength = strlen(ClassNameChar) + 1;
	UE_LOG(LogTemp, Log, TEXT("- Nombre: %s (full length: %d)"), *ClassNameString, ClassNameFullLength);
	File.write(reinterpret_cast<const char*>(&ClassNameFullLength), sizeof(ClassNameFullLength));
	File.write(ClassNameChar, sizeof(char) * ClassNameFullLength);

	//Escribir la posici�n del objeto
	FVector Location = EditableObject->GetActorLocation();
	UE_LOG(LogTemp, Log, TEXT("- Posicion: %.2f %.2f %.2f"), Location.X, Location.Y, Location.Z);
	File.write(reinterpret_cast<const char*>(&Location.X), sizeof(Location.X));
	File.write(reinterpret_cast<const char*>(&Location.Y), sizeof(Location.Y));
	File.write(reinterpret_cast<const char*>(&Location.Z), sizeof(Location.Z));

	//Escribir la rotaci�n del objeto
	FRotator Rotation = EditableObject->GetActorRotation();
	UE_LOG(LogTemp, Log, TEXT("- Rotacion: %.2f %.2f %.2f"), Rotation.Pitch, Rotation.Roll, Rotation.Yaw);
	File.write(reinterpret_cast<const char*>(&Rotation.Pitch), sizeof(Rotation.Pitch));
	File.write(reinterpret_cast<const char*>(&Rotation.Roll), sizeof(Rotation.Roll));
	File.write(reinterpret_cast<const char*>(&Rotation.Yaw), sizeof(Rotation.Yaw));

	//Escribir la escala del objeto
	FVector Scale = EditableObject->GetActorScale();
	UE_LOG(LogTemp, Log, TEXT("- Escala: %.2f %.2f %.2f"), Scale.X, Scale.Y, Scale.Z)
	File.write(reinterpret_cast<const char*>(&Scale.X), sizeof(Scale.X));
}

// Funci�n de deserializaci�n para un nivel del Editor de Niveles. Toma un archivo binario y lo lee para
// ubicar los objetos editables creados con anterioridad, adem�s de ubicar al personaje en la escena.
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
		DisplayMessage("El archivo seleccionado no es del tipo serializable. Aseg�rese de que haya sido \
un archivo generado por este editor.", "Error");
		File.close();
		return false;
	}

	//Abrir el nivel de edici�n
	UGameplayStatics::OpenLevel(WorldContextObject, FName(TEXT("EditLevel")));

	//Leer la posici�n del personaje
	FVector PlayerStart(0);
	File.read(reinterpret_cast<char*>(&PlayerStart.X), sizeof(PlayerStart.X));
	File.read(reinterpret_cast<char*>(&PlayerStart.Y), sizeof(PlayerStart.Y));
	File.read(reinterpret_cast<char*>(&PlayerStart.Z), sizeof(PlayerStart.Z));
	UE_LOG(LogTemp, Log, TEXT("Personaje: %.2f %.2f %.2f"), PlayerStart.X, PlayerStart.Y, PlayerStart.Z);

	//Colocar los objetos editables
	int EditableObjectCount;
	File.read(reinterpret_cast<char*>(&EditableObjectCount), sizeof(EditableObjectCount));
	UE_LOG(LogTemp, Log, TEXT("# objetos: %d"), EditableObjectCount);

	for (int i = 0; i < EditableObjectCount; i++) {
		UE_LOG(LogTemp, Log, TEXT("Objeto #%d:"), EditableObjectCount);
		DeserializeEditableObject(WorldContextObject, File);
	}

	//Cerrar el archivo
	File.close();
	DisplayMessage("Se abri� el archivo correctamente.", "Editor VR Moche");
	return true;
}

// Funci�n de deserializaci�n para un objeto editable.
AActor* UEditorVRFunctions::DeserializeEditableObject(UObject* WorldContextObject, std::ifstream &File)
{
	//Leer el nombre de la clase
	int ClassNameFullLength = 0;
	File.read(reinterpret_cast<char*>(&ClassNameFullLength), sizeof(ClassNameFullLength));
	char* ClassNameString = new char[ClassNameFullLength];
	File.read(ClassNameString, sizeof(char) * ClassNameFullLength);
	UE_LOG(LogTemp, Log, TEXT("- Clase: %s (longitud: %d)"), ANSI_TO_TCHAR(ClassNameString), ClassNameFullLength);
	const char* ClassPathName = GetClassPathName(ClassNameString);
	delete[] ClassNameString; //MUY IMPORTANTE SI NO QUEREMOS SATURAR LA MEMORIA
	UE_LOG(LogTemp, Log, TEXT("- ClassPath: %s"), ANSI_TO_TCHAR(ClassPathName));

	//Leer la posici�n del objeto
	FVector Location(0);
	File.read(reinterpret_cast<char*>(&Location.X), sizeof(Location.X));
	File.read(reinterpret_cast<char*>(&Location.Y), sizeof(Location.Y));
	File.read(reinterpret_cast<char*>(&Location.Z), sizeof(Location.Z));
	UE_LOG(LogTemp, Log, TEXT("- Posicion: %.2f %.2f %.2f"), Location.X, Location.Y, Location.Z);

	//Leer la rotaci�n del objeto
	FRotator Rotation(0);
	File.read(reinterpret_cast<char*>(&Rotation.Pitch), sizeof(Rotation.Pitch));
	File.read(reinterpret_cast<char*>(&Rotation.Roll), sizeof(Rotation.Roll));
	File.read(reinterpret_cast<char*>(&Rotation.Yaw), sizeof(Rotation.Yaw));
	UE_LOG(LogTemp, Log, TEXT("- Rotacion: %.2f %.2f %.2f"), Rotation.Pitch, Rotation.Roll, Rotation.Yaw);

	//Leer la escala del objeto
	float Scale = 0;
	File.read(reinterpret_cast<char*>(&Scale), sizeof(Scale));
	UE_LOG(LogTemp, Log, TEXT("- Escala: %.2f"), Scale);

	//Obtener las referencias necesarias para poder crear el objeto editable
	UClass* EditableObjectClass = StaticLoadClass(AActor::StaticClass(), NULL, ANSI_TO_TCHAR(ClassPathName),
		NULL, LOAD_None, NULL);
	if (!EditableObjectClass) {
		UE_LOG(LogTemp, Warning, TEXT("No se pudo obtener la referencia a EditableObjectClass."));
		return;
	}
	UWorld* CurrentWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	if (!CurrentWorld) {
		UE_LOG(LogTemp, Warning, TEXT("No se pudo obtener la referencia a CurrentWorld."));
		return;
	}

	//Crear el objeto editable
	AActor* EditableObject = CurrentWorld->SpawnActor(EditableObjectClass);
	if (!EditableObject) {
		UE_LOG(LogTemp, Warning, TEXT("No se pudo crear el objeto editable %s."), ClassPathName);
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("Objeto editable creado!"));
	EditableObject->SetActorLocation(Location);
	EditableObject->SetActorRotation(Rotation);
	EditableObject->SetActorScale3D(FVector(Scale));
	UE_LOG(LogTemp, Log, TEXT("Objeto editable seteado!"));

	return EditableObject;
}

// Funci�n auxiliar que mapea la direcci�n completa de la clase guardada en el archivo serializable, de
// manera que se pueda usar en el SpawnActor(). Esta parte del c�digo va a variar entre el editor y el
// juego, ya que depende de las ubicaciones de las clases dentro del proyecto fuente. (Content/)
const char* UEditorVRFunctions::GetClassPathName(char* ClassNameString)
{
	if (strcmp(ClassNameString, "EditableCube_C") == 0)
		return "Blueprint'/Game/Classes/EditableCube.EditableCube_C'";
	if (strcmp(ClassNameString, "EditableSphere_C") == 0)
		return "Blueprint'/Game/Classes/EditableSphere.EditableSphere_C'";
	//Si por casualidad no se encuentra la clase exacta, retornar EditableObject por defecto.
	return "Blueprint'/Game/Classes/EditableObject.EditableObject_C'";
}

// Muestra mensajes en las funciones especiales del Editor de Niveles.
void UEditorVRFunctions::DisplayMessage(const char* Message, const char* Title)
{
	FString TitleString(UTF8_TO_TCHAR(Title)), MessageString(UTF8_TO_TCHAR(Message));
	FText TitleText = FText::FromString(TitleString), MessageText = FText::FromString(MessageString);
	FMessageDialog::Open(EAppMsgType::Ok, MessageText, &TitleText);
}
