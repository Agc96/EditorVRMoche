/**
 * Funciones para la lectura del directorio de los niveles creados con el Editor de Niveles.
 * @author Susana Ordaya
 * @author Anthony Gutiérrez
 */

#include "EditorVRFunctions.h"

// Función para obtener la lista de archivos binarios del Editor de Niveles (extensión ".bin") 
TArray<FString> UEditorVRFunctions::GetExtraLevelsList(const FString& Directory)
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	TArray<FString> EmptyArray;

	//Verificar si el directorio donde se guardan los archivos binarios existe
	if (!VerifyOrCreateDirectory(PlatformFile, Directory))
	{
		UE_LOG(LogTemp, Error, TEXT("No se pudo abrir el directorio de los niveles extra."));
		return EmptyArray;
	}

	//Obtener recursivamente la lista de archivos de la carpeta ubicada en "Directory"
	FLocalTimestampDirectoryVisitor Visitor(PlatformFile, EmptyArray, EmptyArray, false);
	PlatformFile.IterateDirectory(*Directory, Visitor);
	TArray<FString> ExtraLevelsFileList;

	//Obtener la lista de archivos
	for (auto& FileTime : Visitor.FileTimes)
	{
		FString FilePath = FileTime.Key;
		FString FileName = FPaths::GetCleanFilename(FilePath);
		UE_LOG(LogTemp, Log, TEXT("Archivo: %s (path: %s)"), *FileName, *FilePath);

		//Añadir nombre de archivo a la lista
		if (FPaths::GetExtension(FileName, false).Equals(EditorFileExtension, ESearchCase::IgnoreCase))
		{
			ExtraLevelsFileList.Add(FileName);
			UE_LOG(LogTemp, Log, TEXT("- Añadido al arreglo."));
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Arreglo completado con %d nombres de archivo."), ExtraLevelsFileList.Num());
	return ExtraLevelsFileList;
}

// Función que chequea si existe la carpeta especificada. En el caso de que no lo encuentre, crea dicha carpeta.
bool UEditorVRFunctions::VerifyOrCreateDirectory(IPlatformFile& PlatformFile, const FString& Directory)
{
	//Si el directorio existe, no hay problema, no hacer nada.
	if (PlatformFile.DirectoryExists(*Directory))
	{
		UE_LOG(LogTemp, Log, TEXT("El directorio %s ya existe por lo que no hay problema."), *Directory);
		return true;
	}

	//Si el directorio no existe, crearlo y verificar si fue creado correctamente.
	PlatformFile.CreateDirectory(*Directory);
	UE_LOG(LogTemp, Log, TEXT("Se ha intentado crear el directorio %s."), *Directory);
	return PlatformFile.DirectoryExists(*Directory);
}

// Función que chequea si el archivo existe y no se desea reemplazar, y falso en caso contrario.
bool UEditorVRFunctions::CheckIfCreateOrReplaceFile(const FString& FilePath)
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	//Si el archivo no existía antes no habrá problema en guardar.
	if (!PlatformFile.FileExists(*FilePath)) return true;

	//Si el archivo existe, dependerá del usuario decidir si sobreescribirlo o no.
	FString Message = FString(TEXT("¿Desea sobreescribir el archivo ")) + FilePath + FString(TEXT("?"));
	EAppReturnType::Type ReplaceFile = DisplayMessage(EAppMsgType::YesNo, *Message, TEXT("Editor VR Moche"));

	if (ReplaceFile == EAppReturnType::Yes) return true;
	return false;
}
