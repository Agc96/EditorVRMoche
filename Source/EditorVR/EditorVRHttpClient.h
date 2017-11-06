// Copyright (C) 2017 DP2 VR Moche - PUCP. Todos los derechos reservados.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "Serialization/BufferArchive.h"
#include "Serialization/MemoryReader.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

#include "EditorVRHttpClient.generated.h"

/**
 *
 */
UCLASS()
class EDITORVR_API UEditorVRHttpClient : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    UFUNCTION()
        static void DescargarArchivo(FString NombreArchivo);
    UFUNCTION()
        static void SubirArchivo(FString Path);

private:
    static TSharedRef<IHttpRequest> RequestWithRoute(FString Subroute);
    static TSharedRef<IHttpRequest> GetRequest(FString Subroute);
    static TSharedRef<IHttpRequest> PostRequest(FString Subroute, const TArray<uint8>& DatosASubir, FString NombreArchivo);
    static void Send(TSharedRef<IHttpRequest>& Request);
    static bool ResponseIsValid(FHttpResponsePtr Response, bool bWasSuccessful);

    static void RespuestaDescarga(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
    static void RespuestaSubida(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

    static FHttpModule* Http;
    static FString ApiBaseUrl;
};
