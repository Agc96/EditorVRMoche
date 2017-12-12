/**
 * Funciones auxiliares comunes dentro de la librer�a de funciones del Editor de Niveles.
 * @author Anthony Guti�rrez
 */

#include "EditorVRFunctions.h"

/** Funci�n que retorna el path del directorio donde se guardan los niveles extra del juego, tanto en el Editor de Niveles
 como en el Juego de Realidad Virtual. */
FString UEditorVRFunctions::GetExtraLevelDirectory()
{
#if PLATFORM_ANDROID
	extern FString GExternalFilePath;
	FString Path = GExternalFilePath;
#else
	FString RelativePath = FString(FPlatformProcess::UserDir()) / TEXT("EditorVRMoche");
	FString Path = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*RelativePath);
#endif
	UE_LOG(LogTemp, Log, TEXT("Path: %s"), *Path);
	return Path;
}

/** Funci�n auxiliar que muestra una ventana de di�logo con un mensaje, t�tulo y botones dependiendo del tipo especificado.
 *  @return Valor correspondiente a la respuesta del dialog. */
EAppReturnType::Type UEditorVRFunctions::DisplayMessage(EAppMsgType::Type Type, const TCHAR* Message, const TCHAR* Title)
{
	FString MessageString = FString(Message), TitleString = FString(Title);
	FText MessageText = FText::FromString(Message), TitleText = FText::FromString(Title);
	return FMessageDialog::Open(Type, MessageText, &TitleText);
}

/** Funci�n auxiliar que mapea la direcci�n completa de la clase guardada en el archivo serializable, de
 manera que se pueda usar en el SpawnActor(). Esta parte del c�digo va a variar entre el editor y el
 juego, ya que depende de las ubicaciones de las clases dentro del proyecto fuente. (Content/) */
FString UEditorVRFunctions::GetEditableClassPath(const FString& ClassName)
{
	//Objetos de prueba (b�sicos)
	if (ClassName.Equals(FString(TEXT("EditableObject_C")), ESearchCase::IgnoreCase))
		return FString(TEXT("Blueprint'/Game/Blueprints/EditableObject.EditableObject_C'"));
	if (ClassName.Equals(FString(TEXT("EditableCube_C")), ESearchCase::IgnoreCase))
		return FString(TEXT("Blueprint'/Game/Blueprints/EditableCube.EditableCube_C'"));
	if (ClassName.Equals(FString(TEXT("EditableSphere_C")), ESearchCase::IgnoreCase))
		return FString(TEXT("Blueprint'/Game/Blueprints/EditableSphere.EditableSphere_C'"));

	//Objetos de ubicaci�n y decoraci�n
	if (ClassName.Equals(FString(TEXT("PlayerStart_C")), ESearchCase::IgnoreCase))
		return FString(TEXT("Blueprint'/Game/Blueprints/PlayerStart.PlayerStart_C'"));
	if (ClassName.Equals(FString(TEXT("PlayerEnd_C")), ESearchCase::IgnoreCase))
		return FString(TEXT("Blueprint'/Game/Blueprints/PlayerEnd.PlayerEnd_C'"));
	if (ClassName.Equals(FString(TEXT("Floor_C")), ESearchCase::IgnoreCase))
		return FString(TEXT("Blueprint'/Game/Blueprints/Floor.Floor_C'"));
	if (ClassName.Equals(FString(TEXT("Roof_C")), ESearchCase::IgnoreCase))
		return FString(TEXT("Blueprint'/Game/Blueprints/Roof.Roof_C'"));
	if (ClassName.Equals(FString(TEXT("Wall_C")), ESearchCase::IgnoreCase))
		return FString(TEXT("Blueprint'/Game/Blueprints/Wall.Wall_C'"));
	
	//Objetos del proyecto Desaf�o Moche
	if (ClassName.Equals(FString(TEXT("EditableAiapaec_C")), ESearchCase::IgnoreCase))
		return FString(TEXT("Blueprint'/Game/Blueprints/EditableAiapaec.EditableAiapaec_C'"));
	
	//Objetos de luz
	if (ClassName.Equals(FString(TEXT("EditableDirectionalLight_C")), ESearchCase::IgnoreCase))
		return FString(TEXT("Blueprint'/Game/Blueprints/EditableDirectionalLight.EditableDirectionalLight_C'"));
	if (ClassName.Equals(FString(TEXT("EditablePointLight_C")), ESearchCase::IgnoreCase))
		return FString(TEXT("Blueprint'/Game/Blueprints/EditablePointLight.EditablePointLight_C'"));
	if (ClassName.Equals(FString(TEXT("EditableSpotLight_C")), ESearchCase::IgnoreCase))
		return FString(TEXT("Blueprint'/Game/Blueprints/EditableSpotLight.EditableSpotLight_C'"));
	if (ClassName.Equals(FString(TEXT("EditableSkyLight_C")), ESearchCase::IgnoreCase))
		return FString(TEXT("Blueprint'/Game/Blueprints/EditableSkyLight.EditableSkyLight_C'"));
	
	//Objetos de sonido
	if (ClassName.Equals(FString(TEXT("EditableSound1_C")), ESearchCase::IgnoreCase))
		return FString(TEXT("Blueprint'/Game/Blueprints/EditableSound1.EditableSound1_C'"));
	
	//Si por casualidad no se encuentra la clase exacta, retornar una cadena vac�a.
	return FString();
}
