/**
 * Funciones auxiliares comunes dentro de la librer�a de funciones del Editor de Niveles.
 * @author Anthony Guti�rrez
 */

#include "EditorVRFunctions.h"

 // Funci�n que retorna el path del directorio donde se guardan los niveles extra del juego, tanto en el Editor de Niveles
 // como en el Juego de Realidad Virtual.
FString UEditorVRFunctions::GetExtraLevelDirectory()
{
	FString RelativePath = FString(FPlatformProcess::UserDir()) / TEXT("VRMoche");
	UE_LOG(LogTemp, Log, TEXT("RelativePath: %s"), *RelativePath);

	FString AbsolutePath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*RelativePath);
	UE_LOG(LogTemp, Log, TEXT("AbsolutePath: %s"), *AbsolutePath);
	return AbsolutePath;
}

// Funci�n auxiliar que muestra una ventana de di�logo con un mensaje, t�tulo y botones dependiendo del tipo especificado.
// Retorna el valor correspondiente a la respuesta del dialog.
EAppReturnType::Type UEditorVRFunctions::DisplayMessage(EAppMsgType::Type Type, const TCHAR* Message, const TCHAR* Title)
{
	FString MessageString = FString(Message), TitleString = FString(Title);
	FText MessageText = FText::FromString(Message), TitleText = FText::FromString(Title);
	return FMessageDialog::Open(Type, MessageText, &TitleText);
}

// Funci�n auxiliar que mapea la direcci�n completa de la clase guardada en el archivo serializable, de
// manera que se pueda usar en el SpawnActor(). Esta parte del c�digo va a variar entre el editor y el
// juego, ya que depende de las ubicaciones de las clases dentro del proyecto fuente. (Content/)
FString UEditorVRFunctions::GetEditableObjectClassPath(const FString& ClassName)
{
	if (ClassName.Equals(FString(TEXT("EditableCube_C")), ESearchCase::IgnoreCase))
		return FString(TEXT("Blueprint'/Game/Blueprints/EditableCube.EditableCube_C'"));

	if (ClassName.Equals(FString(TEXT("EditableSphere_C")), ESearchCase::IgnoreCase))
		return FString(TEXT("Blueprint'/Game/Blueprints/EditableSphere.EditableSphere_C'"));

	//Si por casualidad no se encuentra la clase exacta, retornar una cadena vac�a.
	return FString();
}
