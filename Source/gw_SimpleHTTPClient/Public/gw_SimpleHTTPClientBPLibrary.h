// Copyright GIWIG, Inc. All Rights Reserved.
// https://ilyahartmann.com
// https://blog.giwig.online


#pragma once

//THIRD_PARTY_INCLUDES_START
//#include "openssl.h"
//THIRD_PARTY_INCLUDES_END


#include "Kismet/BlueprintFunctionLibrary.h"
#include "Misc/DateTime.h"
#include "Interfaces/IHttpRequest.h"
#include "gw_SimpleHTTPClientBPLibrary.generated.h"



DECLARE_DYNAMIC_DELEGATE_OneParam(FOnRequestSuccessed, const FText&, ResponseContent);


USTRUCT(BlueprintType)
struct FStringMap {
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		TMap<FString, FString> tokens;
};
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnRequestSuccessedJWT, const FStringMap, tokens);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnRequestFailed, FString, ErrorText, int32, ErrorCode);

UENUM()
enum ERequestContentType {
	Json		UMETA(DisplayName = "application/json"),
	Html		UMETA(DisplayName = "text/html"),
	Data		UMETA(DisplayName = "multipart/form-data"),
};

UENUM()
enum ERequestType {
	Get			UMETA(DisplayName = "GET"),
	Post		UMETA(DisplayName = "POST"),
};

UENUM()
enum EHeaderTypes {
	Authorization	UMETA(DisplayName = "Authorization"),
};

UENUM()
enum EHeaderAuthTypes {
	JWT			UMETA(DisplayName = "JWT"),
	Bearer		UMETA(DisplayName = "Bearer"),
};

USTRUCT(BlueprintType)
struct FJWTTokens {
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		FString access;
	UPROPERTY(BlueprintReadWrite)
		FString refresh;
};




UCLASS()
class Ugw_SimpleHTTPClientBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
public:

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "_headers"))
		static void HttpRequest(
			const FString& URL,
			FText _post_data,
			FOnRequestSuccessed OnSuccessed,
			FOnRequestFailed OnFaild,
			TEnumAsByte<ERequestType> _request_type,
			TEnumAsByte<ERequestContentType>
			_content_type,
			const TMap<FString, FString> _headers
		);
	static void OnRequestContentReady(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Successed, FOnRequestSuccessed OnSuccessed, FOnRequestFailed OnFaild);

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "_custom_headers,_default_headers"))
		static void HttpJWTLogin(
			const FString& URL,
			FString _user,
			FString _password,
			FOnRequestSuccessedJWT OnSuccessed,
			FOnRequestFailed OnFaild
		);
	static void OnRequestContentReadyJWT(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Successed, FOnRequestSuccessedJWT OnSuccessed, FOnRequestFailed OnFaild);


	//UFUNCTION(BlueprintCallable)
		//static void HttpTextToJWT(FText _content, struct FJWTTokens& tokens);

	UFUNCTION(BlueprintPure)
		static void HttpJWTMakeHeaderToken(
			TEnumAsByte<EHeaderAuthTypes>	_default_auth_types,
			FString token,
			TMap<FString, FString>& header
		);
};


