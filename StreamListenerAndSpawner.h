// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "Interfaces/IHttpRequest.h"
#include "ChatStructure.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WebSocketsModule.h"
#include "IWebSocket.h"
#include "StreamListenerAndSpawner.generated.h"

UCLASS()
class MYPROJECT_API AStreamListenerAndSpawner : public AActor
{
	GENERATED_BODY()


public:	
	// Sets default values for this actor's properties
	AStreamListenerAndSpawner();


	UFUNCTION(BlueprintCallable, Category = "C++ Functions")
		void ChatGet(FString RoomID);

	UFUNCTION(BlueprintCallable, Category = "C++ Functions")
		TArray <FChatData> MessageProcesser();

	void MessagebodySet(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	//¡üOld way to process message data(Functions)

	void OnConnected();
	void OnConnectionError(const FString& Error);
	void OnClosed(int32 StatusCode, const FString& Reason, bool bWasClean);
	void OnMessage(const FString& Message);//¡üWebsocket Bind functions
	void OnRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);//Called when HttpRequest completed(Httpmodule requests server link)

	void GetUrl(const FString RoomID);

	UFUNCTION(BlueprintCallable, Category = "C++ Functions")
		void ConnectToServer(const FString RoomID);
	//¡üNew way(Websocket) to connect to the chat server
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	int32 SameValuePos=0;
	FString MessageBody="None";
	FString PriviousLastMsgID = "LMAO";
	
	TSharedPtr <FJsonValue> Identifier;

	TArray <FString> IdentifyCodeArray;

	//¡üOld way data

	TSharedPtr<IWebSocket> Socket = nullptr;

	int32 URLFailedTime = 0;
	UPROPERTY(BlueprintReadWrite)
		FVector Spawnposition;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString BiliStreamingRoomID;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
