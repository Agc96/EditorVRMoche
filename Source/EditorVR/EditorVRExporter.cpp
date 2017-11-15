/**
 * Funciones para la exportaci�n de los niveles creados con el Editor de Niveles al servidor Web.
 * @author Sebasti�n Solari
 * @author Anthony Guti�rrez
 */

#include "EditorVRFunctions.h"

// Funci�n que guarda y exporta al servidor Web un nivel ya existente desde el Editor de Niveles.
bool UEditorVRFunctions::ExportExistingLevel(UObject* WorldContextObject, const FString& FilePath)
{
	FBufferArchive FileBuffer;
	if (!SerializeLevel(FileBuffer, WorldContextObject, FilePath)) return false;
	//aqu� falta algo
	return true;
}

// Funci�n que guarda y exporta al servidor Web un nivel nuevo creado con el Editor de Niveles.
bool UEditorVRFunctions::ExportNewLevel(UObject* WorldContextObject, const FString& FilePath)
{
	if (!CheckIfCreateOrReplaceFile(FilePath)) return false;

	FBufferArchive FileBuffer;
	if (!SerializeLevel(FileBuffer, WorldContextObject, FilePath)) return false;

	//aqu� falta algo
	return true;
}
