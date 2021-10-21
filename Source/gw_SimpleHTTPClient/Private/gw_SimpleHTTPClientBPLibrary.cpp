// Copyright GIWIG, Inc. All Rights Reserved.
// https://ilyahartmann.com
// https://blog.giwig.online


#include "gw_SimpleHTTPClientBPLibrary.h"
#include "gw_SimpleHTTPClient.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "JsonObjectConverter.h"

Ugw_SimpleHTTPClientBPLibrary::Ugw_SimpleHTTPClientBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}
/******************************************************************************************


*******************************************************************************************/
void Ugw_SimpleHTTPClientBPLibrary::HttpRequest(const FString& URL, FText _post_data, FOnRequestSuccessed OnSuccessed, FOnRequestFailed OnFaild, TEnumAsByte<ERequestType> _request_type, TEnumAsByte<ERequestContentType> _content_type, TMap<FString, FString> _headers)
{
	TSharedRef<class IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindStatic(Ugw_SimpleHTTPClientBPLibrary::OnRequestContentReady, OnSuccessed, OnFaild);

	HttpRequest->SetURL(*URL);

	for (auto& e : _headers)
	{
		HttpRequest->SetHeader(*e.Key, *e.Value);
	}
	HttpRequest->SetHeader("Content-Type", StaticEnum<ERequestContentType>()->GetDisplayNameTextByValue(_content_type.GetValue()).ToString());
	HttpRequest->SetVerb(StaticEnum<ERequestType>()->GetDisplayNameTextByValue(_request_type.GetValue()).ToString());

	if (_request_type == ERequestType::Post) {
		HttpRequest->SetContentAsString(_post_data.ToString());
	}

	HttpRequest->ProcessRequest();
}
/******************************************************************************************


*******************************************************************************************/
void Ugw_SimpleHTTPClientBPLibrary::OnRequestContentReady(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Successed, FOnRequestSuccessed OnSuccessed, FOnRequestFailed OnFaild)
{
	if (!Successed || !Response.IsValid())
	{
		OnFaild.ExecuteIfBound(TEXT("Faild"), -1);
		return;
	}
	int32 ResponseCode = Response->GetResponseCode();
	if (!EHttpResponseCodes::IsOk(ResponseCode))
	{
		OnFaild.ExecuteIfBound(FString::Printf(TEXT("HttpDownloadRequest faild, Respose Code is %d."), ResponseCode), ResponseCode);
		return;
	}

	//TArray<uint8> ResponseContent = Response->GetContent();
	FText ResponseContent = FText::FromString(Response->GetContentAsString());
	OnSuccessed.ExecuteIfBound(ResponseContent);
	return;

}
/******************************************************************************************


*******************************************************************************************/
/*
void Ugw_SimpleHTTPClientBPLibrary::HttpTextToJWT(FText _content, FJWTTokens& tokens)
{
	if (!FJsonObjectConverter::JsonObjectStringToUStruct(_content.ToString(), &tokens, 0, 0))
	{
	}
}
*/
/******************************************************************************************


*******************************************************************************************/
void Ugw_SimpleHTTPClientBPLibrary::HttpJWTLogin(
	const FString& URL,
	FString							_user,
	FString							_password,
	FOnRequestSuccessedJWT			OnSuccessed,
	FOnRequestFailed				OnFaild
	//const TMap<FString, FString>	_custom_headers,
	//TMap<TEnumAsByte<EHeaderTypes>, FString> _default_headers
)
{
	TSharedRef<class IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindStatic(Ugw_SimpleHTTPClientBPLibrary::OnRequestContentReadyJWT, OnSuccessed, OnFaild);

	HttpRequest->SetURL(*URL);

	/*
	for (auto& e : _custom_headers)
	{
		HttpRequest->SetHeader(*e.Key, *e.Value);
	}
	*/
	HttpRequest->SetHeader("Content-Type", "application/json");
	HttpRequest->SetVerb("POST");

	FString content = "{\"username\":\"" + _user + "\", \"password\":\"" + _password + "\"}";
	HttpRequest->SetContentAsString(content);
	HttpRequest->ProcessRequest();
}
/******************************************************************************************


*******************************************************************************************/
void Ugw_SimpleHTTPClientBPLibrary::OnRequestContentReadyJWT(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Successed, FOnRequestSuccessedJWT OnSuccessed, FOnRequestFailed OnFaild)
{
	if (!Successed || !Response.IsValid())
	{
		OnFaild.ExecuteIfBound(TEXT("Faild"), -1);
		return;
	}
	int32 ResponseCode = Response->GetResponseCode();
	if (!EHttpResponseCodes::IsOk(ResponseCode))
	{
		OnFaild.ExecuteIfBound(FString::Printf(TEXT("HttpDownloadRequest faild, Respose Code is %d."), ResponseCode), ResponseCode);
		return;
	}

	//TArray<uint8> ResponseContent = Response->GetContent();


	FString ResponseContent = Response->GetContentAsString();
	//OnSuccessed.ExecuteIfBound(ResponseContent);
	FStringMap strings;

	TSharedPtr<FJsonObject> SaveData = MakeShareable(new FJsonObject());
	TSharedRef< TJsonReader<> > JsonReader = TJsonReaderFactory<>::Create(ResponseContent);
	if (FJsonSerializer::Deserialize(JsonReader, SaveData))
	{
		for (auto& el : SaveData->Values)
		{

			strings.tokens.Add(*el.Key, *el.Value.Get()->AsString());
		}
	}
	OnSuccessed.ExecuteIfBound(strings);
}
/******************************************************************************************


*******************************************************************************************/
void Ugw_SimpleHTTPClientBPLibrary::HttpJWTMakeHeaderToken(
	TEnumAsByte<EHeaderAuthTypes>	_default_auth_types,
	FString token,
	TMap<FString, FString>& header
)
{
	//TMap<FString, FString>	header;

	//StaticEnum<EHeaderAuthTypes>()->GetDisplayNameTextByValue(_default_auth_types.GetValue()).ToString();
	header.Add("Authorization", StaticEnum<EHeaderAuthTypes>()->GetDisplayNameTextByValue(_default_auth_types.GetValue()).ToString() + " " + token);
}
/******************************************************************************************


*******************************************************************************************/

