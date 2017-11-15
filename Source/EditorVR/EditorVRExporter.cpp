/**
 * Funciones para la exportación de los niveles creados con el Editor de Niveles al servidor Web.
 * @author Sebastián Solari
 * @author Anthony Gutiérrez
 */

#include "EditorVRFunctions.h"

// Función que guarda y exporta al servidor Web un nivel ya existente desde el Editor de Niveles.
bool UEditorVRFunctions::ExportExistingLevel(UObject* WorldContextObject, const FString& FilePath)
{
	FBufferArchive FileBuffer;
	if (!SerializeLevel(FileBuffer, WorldContextObject, FilePath)) return false;
	//aquí falta algo
	return true;
}

// Función que guarda y exporta al servidor Web un nivel nuevo creado con el Editor de Niveles.
bool UEditorVRFunctions::ExportNewLevel(UObject* WorldContextObject, const FString& FilePath)
{
	if (!CheckIfCreateOrReplaceFile(FilePath)) return false;

	FBufferArchive FileBuffer;
	if (!SerializeLevel(FileBuffer, WorldContextObject, FilePath)) return false;

	//aquí falta algo
	return true;
}
