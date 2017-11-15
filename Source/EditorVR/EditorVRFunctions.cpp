/**
 * Funciones auxiliares comunes dentro de la librería de funciones del Editor de Niveles.
 * @author Anthony Gutiérrez
 */

#include "EditorVRFunctions.h"

 // Función que retorna el path del directorio donde se guardan los niveles extra del juego, tanto en el Editor de Niveles
 // como en el Juego de Realidad Virtual.
FString UEditorVRFunctions::GetExtraLevelDirectory()
{
	FString RelativePath = FString(FPlatformProcess::UserDir()) / TEXT("VRMoche");
	UE_LOG(LogTemp, Log, TEXT("RelativePath: %s"), *RelativePath);

	FString AbsolutePath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*RelativePath);
	UE_LOG(LogTemp, Log, TEXT("AbsolutePath: %s"), *AbsolutePath);
	return AbsolutePath;
}

// Función auxiliar que muestra una ventana de diálogo con un mensaje, título y botones dependiendo del tipo especificado.
// Retorna el valor correspondiente a la respuesta del dialog.
EAppReturnType::Type UEditorVRFunctions::DisplayMessage(EAppMsgType::Type Type, const TCHAR* Message, const TCHAR* Title)
{
	FString MessageString = FString(Message), TitleString = FString(Title);
	FText MessageText = FText::FromString(Message), TitleText = FText::FromString(Title);
	return FMessageDialog::Open(Type, MessageText, &TitleText);
}

// Función auxiliar que mapea la dirección completa de la clase guardada en el archivo serializable, de
// manera que se pueda usar en el SpawnActor(). Esta parte del código va a variar entre el editor y el
// juego, ya que depende de las ubicaciones de las clases dentro del proyecto fuente. (Content/)
FString UEditorVRFunctions::GetEditableObjectClassPath(const FString& ClassName)
{
	if (ClassName.Equals(FString(TEXT("EditableCube_C")), ESearchCase::IgnoreCase))
		return FString(TEXT("Blueprint'/Game/Blueprints/EditableCube.EditableCube_C'"));

	if (ClassName.Equals(FString(TEXT("EditableSphere_C")), ESearchCase::IgnoreCase))
		return FString(TEXT("Blueprint'/Game/Blueprints/EditableSphere.EditableSphere_C'"));

	//Si por casualidad no se encuentra la clase exacta, retornar una cadena vacía.
	return FString();
}
