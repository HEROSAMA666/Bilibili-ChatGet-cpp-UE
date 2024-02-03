// Fill out your copyright notice in the Description page of Project Settings.



#include "ChatStructure.h"
#include "Http.h"
#include "StreamListenerAndSpawner.h"
#include "HttpRetrySystem.h"
#include "JsonObjectConverter.h"
#include "Interfaces/IHttpResponse.h"
#include "WebSocketsModule.h"
#include "IWebSocket.h"

// Sets default values



AStreamListenerAndSpawner::AStreamListenerAndSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Spawnposition = GetActorLocation();

}

// Called when the game starts or when spawned
void AStreamListenerAndSpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStreamListenerAndSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AStreamListenerAndSpawner::MessagebodySet(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{	
	if (HttpRequest.IsValid()&&HttpResponse.IsValid()) 
	{
		//UE_LOG(LogTemp, Warning, TEXT("request is complete %s"), (bSucceeded ? TEXT("True") : TEXT("False")));

		MessageBody = HttpResponse->GetContentAsString();//set global variable "MessageBody" after successfully complete request

		//UE_LOG(LogTemp, Warning, TEXT("%s"), *MessageBody);
	}
	
}

TArray <FChatData> AStreamListenerAndSpawner::MessageProcesser()
{
	TArray<FChatData> PackageArray;//Local variable to pass the data
	FChatData Newpackage;//Local variable to pass the data(or to added into array¡ü)
	//Only for pass or return data¡ü

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);//used to storage Reader deserialized data
	
	TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(MessageBody);//Read Global variable "Messbody"<- include raw stream chat

	TArray< TSharedPtr<FJsonValue> > JsonValueDeliver; //(Array)used to storage chat message data struct

	if (FJsonSerializer::Deserialize(Reader, JsonObject))//This function can deserialize raw data(reader) to a manipulate jsonobject and return a boolean value
	{
		JsonValueDeliver = JsonObject->GetObjectField("data")->GetArrayField("room");//Let JsonValueDeliver actually get the data

		for (TSharedPtr<FJsonValue> item : JsonValueDeliver) 
		{
			IdentifyCodeArray.Add(item->AsObject()->GetObjectField("check_info")->GetStringField("ct"));
		}

		if (IdentifyCodeArray.Find(PriviousLastMsgID,SameValuePos))
		{//see if reciving the same content
			if(PriviousLastMsgID == JsonValueDeliver[9]->AsObject()->GetObjectField("check_info")->GetStringField("ct"))
			{
				UE_LOG(LogTemp, Warning, TEXT("Same json pack!"));
				UE_LOG(LogTemp, Warning, TEXT("same position is %d"), SameValuePos);
				IdentifyCodeArray.Empty();
				return PackageArray;
			}
			else
			{
				PriviousLastMsgID = JsonValueDeliver[9]->AsObject()->GetObjectField("check_info")->GetStringField("ct");
				for (SameValuePos;SameValuePos<JsonValueDeliver.Num();SameValuePos++) 
				{
					Newpackage.UserID = JsonValueDeliver[SameValuePos]->AsObject()->GetIntegerField("uid");
					Newpackage.UserName = FName(JsonValueDeliver[SameValuePos]->AsObject()->GetStringField("nickname"));
					Newpackage.Message = JsonValueDeliver[SameValuePos]->AsObject()->GetStringField("text");
					Newpackage.IdentifyCode = JsonValueDeliver[SameValuePos]->AsObject()->GetStringField("ct");
					PackageArray.Add(Newpackage);
				}
				IdentifyCodeArray.Empty();
				return PackageArray;//This is for the situation that new array has the same elements with last array
			}
		}
		else 
		{
			Identifier = JsonValueDeliver[9];//used to indentify if same array is acquired
			PriviousLastMsgID = JsonValueDeliver[9]->AsObject()->GetObjectField("check_info")->GetStringField("ct");
			for(TSharedPtr<FJsonValue> item: JsonValueDeliver)
			{
				TSharedPtr<FJsonObject>itemObject = item->AsObject();

				Newpackage.UserID = itemObject->GetIntegerField("uid");
				Newpackage.UserName = FName(itemObject->GetStringField("nickname"));
				Newpackage.Message = itemObject->GetStringField("text");
				Newpackage.IdentifyCode = itemObject->GetObjectField("check_info")->GetStringField("ct");

				PackageArray.Add(Newpackage);//Add single object into the return array
			}
			IdentifyCodeArray.Empty();
			return PackageArray;//This is for the new array is completely from the last array
		}
		
	}
	return PackageArray;
}

void AStreamListenerAndSpawner::ChatGet(FString RoomID) 
{
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();//Request declaration
	
	HttpRequest->SetVerb("GET");
	HttpRequest->SetURL("http://api.live.bilibili.com/ajax/msg?roomid=" + RoomID);//Set Room url 
	//UE_LOG(LogTemp, Warning, TEXT("Stream room url is %s"), *url);
	HttpRequest->SetHeader("Content-Type", "application/json;charset=UTF-8");
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &AStreamListenerAndSpawner::MessagebodySet);
	//HttpRequest settings¡ü

	HttpRequest->ProcessRequest();

}


void AStreamListenerAndSpawner::OnConnected()
{
	UE_LOG(LogTemp, Warning, TEXT("Server connected!"));
}

void AStreamListenerAndSpawner::OnConnectionError(const FString& Error)
{
	UE_LOG(LogTemp, Warning, TEXT("Server unsuccessfully connected,code is %s"),*Error);
}

void AStreamListenerAndSpawner::OnClosed(int32 StatusCode, const FString& Reason, bool bWasClean)
{
	UE_LOG(LogTemp, Warning, TEXT("Connection closed! StatusCode:%d  Reason:%s"),StatusCode, *Reason);
}

void AStreamListenerAndSpawner::OnMessage(const FString& Message)
{
	UE_LOG(LogTemp, Warning, TEXT("Server connected!"));
	

}

void AStreamListenerAndSpawner::GetUrl(const FString RoomID)
{
	FString Link,host,wss_port;
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	
	HttpRequest->SetVerb("GET");
	HttpRequest->SetURL("https://api.live.bilibili.com/room/v1/Danmu/getConf?room_id="+ RoomID + "&platform=pc&player=web");
	HttpRequest->SetHeader("Content-Type", "application/json;charset=UTF-8");
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &AStreamListenerAndSpawner::OnRequestComplete);

	HttpRequest->ProcessRequest();
}//Send request to get the server link that stream room belongs to

void AStreamListenerAndSpawner::OnRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	FString Link, host, wss_port;

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

	MessageBody = HttpResponse->GetContentAsString();

	TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(MessageBody);

	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		TSharedPtr<FJsonValue> JsonDeliver = JsonObject->GetObjectField("data")->GetArrayField("host_server_list")[URLFailedTime];
		host = JsonDeliver->AsObject()->GetStringField("host");
		wss_port = JsonDeliver->AsObject()->GetStringField("wss_port");
	}
	Link = "wss://" + host + ":" + wss_port + "/sub";
	//UE_LOG(LogTemp, Warning, TEXT("server link is %s"), *Link);  Link get successful

}

void AStreamListenerAndSpawner::ConnectToServer(const FString ServerLink)
{
	FModuleManager::Get().LoadModuleChecked("WebSockets");
	Socket = FWebSocketsModule::Get().CreateWebSocket(ServerLink, "ws");
	//GetUrl(RoomID);
	//UE_LOG(LogTemp, Warning,TEXT("URL is %s"),*GetUrl(RoomID));
	
}
