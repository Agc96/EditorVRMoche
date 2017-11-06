// Copyright (C) 2017 DP2 VR Moche - PUCP. Todos los derechos reservados.

#include "EditorVRHttpClient.h"

// https://wiki.unrealengine.com/Http-requests
FHttpModule* UEditorVRHttpClient::Http = &FHttpModule::Get();
FString UEditorVRHttpClient::ApiBaseUrl = "http://localhost:8080/api/"; // cambiar localhost con la URL del servidor

TSharedRef<IHttpRequest> UEditorVRHttpClient::RequestWithRoute(FString Subroute)
{
    TSharedRef<IHttpRequest> Request = Http->CreateRequest();
    Request->SetURL(ApiBaseUrl + Subroute);
    return Request;
}

TSharedRef<IHttpRequest> UEditorVRHttpClient::GetRequest(FString Subroute)
{
    TSharedRef<IHttpRequest> Request = RequestWithRoute(Subroute);
    Request->SetVerb("GET");
    Request->SetHeader(TEXT("User-Agent"), TEXT("X-UnrealEngine-Agent"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/octet-stream"));
    return Request;
}

// https://support.gamesparks.net/support/discussions/topics/1000079052
// Lo que hace aquí es construir manualmente un request multipart.
TSharedRef<IHttpRequest> UEditorVRHttpClient::PostRequest(FString Subroute, const TArray<uint8>& DatosASubir, FString NombreArchivo)
{
    TSharedRef<IHttpRequest> Request = RequestWithRoute(Subroute);
    Request->SetVerb("POST");
    Request->SetHeader("Content-Type", "multipart/form-data; boundary=blahblahsomeboundary");

    // TODO: buscar el orden correcto de estas líneas

    FString a = "\r\n--blahblahsomeboundary\r\n";
    FString b = "Content-Disposition: form-data; name=\"file\";  filename=\"" + NombreArchivo + "\"\r\n";

    FString c = "Content-Type: application/octet-stream\r\n\r\n";
    FString e = "\r\n--blahblahsomeboundary--\r\n";

    TArray<uint8> data;
    data.Append((uint8*)TCHAR_TO_UTF8(*a), a.Len());
    data.Append((uint8*)TCHAR_TO_UTF8(*b), b.Len());
    data.Append((uint8*)TCHAR_TO_UTF8(*c), c.Len());
    data.Append(DatosASubir);
    data.Append((uint8*)TCHAR_TO_UTF8(*e), e.Len());

    Request->SetContent(data);
    Request->SetHeader(TEXT("User-Agent"), TEXT("X-UnrealEngine-Agent"));

    //Request->SetContentAsString(ContentJsonString);
    return Request;
}

void UEditorVRHttpClient::Send(TSharedRef<IHttpRequest>& Request)
{
    Request->ProcessRequest();
}

bool UEditorVRHttpClient::ResponseIsValid(FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (!bWasSuccessful || !Response.IsValid()) return false;
    if (EHttpResponseCodes::IsOk(Response->GetResponseCode())) return true;
    else {
        UE_LOG(LogTemp, Warning, TEXT("Http Response returned error code: %d"), Response->GetResponseCode());
        return false;
    }
}

void UEditorVRHttpClient::RespuestaDescarga(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (ResponseIsValid(Response, bWasSuccessful)) {
        TArray<uint8> contenido = Response->GetContent();
        UE_LOG(LogTemp, Log, TEXT("Peso del archivo descargado: %d"), contenido.Num());
        // TODO: hacer lo que se necesite con el archivo (primero deserializarlo).
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("No se pudo descargar el archivo"));
    }
}

void UEditorVRHttpClient::RespuestaSubida(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (ResponseIsValid(Response, bWasSuccessful)) {
        FString respuesta = Response->GetContentAsString();
        UE_LOG(LogTemp, Log, TEXT("%s"), *respuesta);
        // TODO: hacer lo que se necesite con el archivo luego de subirse.
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("No se pudo subir el archivo"));
    }
}

void UEditorVRHttpClient::DescargarArchivo(FString NombreArchivo)
{
    TSharedRef<IHttpRequest> Request = GetRequest("download/" + NombreArchivo);
    Request->OnProcessRequestComplete().BindStatic(RespuestaDescarga);
    Send(Request);
}


void UEditorVRHttpClient::SubirArchivo(FString Path)
{
    //FString Path = "C:\\Users\\Slsvcn\\Desktop\\mascapo (3).bin";
    TArray<uint8> TheBinaryArray;
    FFileHelper::LoadFileToArray(TheBinaryArray, *Path);

    TSharedRef<IHttpRequest> Request = PostRequest("upload", TheBinaryArray, FPaths::GetCleanFilename(Path));
    Request->OnProcessRequestComplete().BindStatic(RespuestaSubida);
    Send(Request);
}
