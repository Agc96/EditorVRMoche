/**
 * Funciones para la exportación de los niveles creados con el Editor de Niveles al servidor Web.
 * @author Sebastián Solari
 */

#include "EditorVRFunctions.h"

// Función que guarda y exporta al servidor Web un nivel ya existente desde el Editor de Niveles.
bool UEditorVRFunctions::ExportExistingLevel(UObject* WorldContextObject, const FString& FilePath)
{
	FBufferArchive FileBuffer;
	if (!SerializeLevel(FileBuffer, WorldContextObject, FilePath)) return false;
	
	TSharedRef<IHttpRequest> Request = PostRequest("upload", FileBuffer, FPaths::GetCleanFilename(FilePath));
    Request->OnProcessRequestComplete().BindStatic(ResponseMessage);
	return Request->ProcessRequest();
}

// Función que guarda y exporta al servidor Web un nivel nuevo creado con el Editor de Niveles.
bool UEditorVRFunctions::ExportNewLevel(UObject* WorldContextObject, const FString& FilePath)
{
	if (!CheckIfCreateOrReplaceFile(FilePath)) return false;

	FBufferArchive FileBuffer;
	if (!SerializeLevel(FileBuffer, WorldContextObject, FilePath)) return false;
	
	TSharedRef<IHttpRequest> Request = PostRequest("upload", FileBuffer, FPaths::GetCleanFilename(FilePath));
    Request->OnProcessRequestComplete().BindStatic(ResponseMessage);
	
	//En este caso "true" significa que se guardó el archivo, no que se exportó correctamente. Si hay un error al exportar,
	//solo se muestra un dialog, no se usa el boolean de ProcessRequest().
	Request->ProcessRequest();
	return true;
}

// https://support.gamesparks.net/support/discussions/topics/1000079052
// Lo que hace aquí es construir manualmente un request multipart.
TSharedRef<IHttpRequest> UEditorVRFunctions::PostRequest(FString Subroute, const TArray<uint8>& FileBuffer, FString NombreArchivo)
{
    TSharedRef<IHttpRequest> Request = RequestWithRoute(Subroute);
    Request->SetVerb("POST");
    Request->SetHeader("Content-Type", "multipart/form-data; boundary=blahblahsomeboundary");

    // TODO: buscar el orden correcto de estas líneas
    FString a = "\r\n--blahblahsomeboundary\r\n";
    FString ContentDisposition = "Content-Disposition: form-data; name=\"file\";  filename=\"" + NombreArchivo + "\"\r\n";
    FString ContentType = "Content-Type: application/octet-stream\r\n\r\n";
    FString e = "\r\n--blahblahsomeboundary--\r\n";

    TArray<uint8> Data;
    Data.Append((uint8*)TCHAR_TO_UTF8(*a), a.Len());
    Data.Append((uint8*)TCHAR_TO_UTF8(*ContentDisposition), ContentDisposition.Len());
    Data.Append((uint8*)TCHAR_TO_UTF8(*ContentType), ContentType.Len());
    Data.Append(FileBuffer);
    Data.Append((uint8*)TCHAR_TO_UTF8(*e), e.Len());

    Request->SetContent(Data);
    Request->SetHeader(TEXT("User-Agent"), TEXT("X-UnrealEngine-Agent"));

    //Request->SetContentAsString(ContentJsonString);
    return Request;
}

TSharedRef<IHttpRequest> UEditorVRFunctions::RequestWithRoute(FString Subroute)
{
	FHttpModule* HttpModule = &FHttpModule::Get();
    TSharedRef<IHttpRequest> Request = HttpModule->CreateRequest();
    Request->SetURL(ApiBaseUrl + Subroute);
    return Request;
}

void UEditorVRFunctions::ResponseMessage(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (ResponseIsValid(Response, bWasSuccessful))
	{
        FString respuesta = Response->GetContentAsString();
        UE_LOG(LogTemp, Log, TEXT("%s"), *respuesta);
        DisplayMessage(EAppMsgType::Ok, TEXT("Se ha exportado el nivel correctamente."), TEXT("Editor VR Moche"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No se pudo subir el archivo"));
		DisplayMessage(EAppMsgType::Ok, TEXT("No se pudo exportar el nivel."), TEXT("Error"));
    }
}

bool UEditorVRFunctions::ResponseIsValid(FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (!bWasSuccessful || !Response.IsValid()) return false;
    if (EHttpResponseCodes::IsOk(Response->GetResponseCode())) return true;
	
	UE_LOG(LogTemp, Warning, TEXT("Http Response returned error code: %d"), Response->GetResponseCode());
    return false;
}
