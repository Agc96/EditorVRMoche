/**
 * Funciones para la exportación de los niveles creados con el Editor de Niveles al servidor Web.
 * @author Sebastián Solari
 * @author Anthony Gutiérrez
 */

#include "EditorVRFunctions.h"

TArray<bool> UEditorVRFunctions::ExportedLevelStatus = TArray<bool>();

/** Función que guarda y exporta al servidor Web un nivel ya existente desde el Editor de Niveles. */
bool UEditorVRFunctions::ExportExistingLevel(UObject* WorldContextObject, const FString& FilePath)
{
	FBufferArchive FileBuffer;
	if (!SerializeEditableLevel(FileBuffer, WorldContextObject, FilePath)) return false;
	
	UE_LOG(LogTemp, Log, TEXT("FileBuffer size: %d"), FileBuffer.Num());
	TSharedRef<IHttpRequest> Request = PostRequest("upload", FileBuffer, FPaths::GetCleanFilename(FilePath));
	ExportedLevelStatus.Add(false);
	
	//Vincular el objeto con el delegado ResponseDownload y ejecutar la petición.
	Request->OnProcessRequestComplete().BindStatic(ExportMessage);
	if (!SendRequest(Request))
	{
		UE_LOG(LogTemp, Log, TEXT("Hubo un problema con el request."));
		ExportedLevelStatus.Empty();
		return false;
	}
	
	bool WasExported = (ExportedLevelStatus.Num() > 0) ? ExportedLevelStatus[0] : false;
	ExportedLevelStatus.Empty();
	return WasExported;
}

bool UEditorVRFunctions::SendRequest(TSharedRef<IHttpRequest>& Request)
{
	//Solicitar la ejecución de la petición HTTP, si no funciona, devolver falso.
	if (!Request->ProcessRequest()) return false;

	//Esperar a que se termine de ejecutar la petición HTTP, incluyendo OnProcessRequestComplete.
	double LastTime = FPlatformTime::Seconds();
	while (EHttpRequestStatus::Processing == Request->GetStatus())
	{
		const double AppTime = FPlatformTime::Seconds();
		FHttpModule::Get().GetHttpManager().Tick(AppTime - LastTime);
		LastTime = AppTime;
		UE_LOG(LogTemp, Log, TEXT("Esperando..."));
		FPlatformProcess::Sleep(0.5f);
	}

	EHttpRequestStatus::Type RequestStatus = Request->GetStatus();
	UE_LOG(LogTemp, Log, TEXT("Estado del request: %s"), EHttpRequestStatus::ToString(RequestStatus));
	if (RequestStatus == EHttpRequestStatus::Succeeded) return true;
	return false;
}

/** Función que guarda y exporta al servidor Web un nivel nuevo creado con el Editor de Niveles. */
bool UEditorVRFunctions::ExportNewLevel(UObject* WorldContextObject, const FString& FilePath)
{
	if (!CheckIfCreateOrReplaceFile(FilePath)) return false;

	FBufferArchive FileBuffer;
	if (!SerializeEditableLevel(FileBuffer, WorldContextObject, FilePath)) return false;
	
	UE_LOG(LogTemp, Log, TEXT("FileBuffer size: %d"), FileBuffer.Num());
	TSharedRef<IHttpRequest> Request = PostRequest("upload", FileBuffer, FPaths::GetCleanFilename(FilePath));
	ExportedLevelStatus.Add(false);
	
    Request->OnProcessRequestComplete().BindStatic(ExportMessage);
	if (!SendRequest(Request))
	{
		UE_LOG(LogTemp, Log, TEXT("Hubo un problema con el request."));
		ExportedLevelStatus.Empty();
		//Se guardó el archivo así que se devuelve verdadero.
		return true;
	}
	
	bool WasExported = (ExportedLevelStatus.Num() > 0) ? ExportedLevelStatus[0] : false;
	UE_LOG(LogTemp, Log, TEXT("Estado: %s"), WasExported ? TEXT("true") : TEXT("false"));
	ExportedLevelStatus.Empty();
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

void UEditorVRFunctions::ExportMessage(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (ResponseIsValid(Response, bWasSuccessful))
	{
        FString respuesta = Response->GetContentAsString();
        UE_LOG(LogTemp, Log, TEXT("%s"), *respuesta);
        DisplayMessage(EAppMsgType::Ok, TEXT("Se ha exportado el nivel correctamente."), TEXT("Editor VR Moche"));
		ExportedLevelStatus.Emplace(true);
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
