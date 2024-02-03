#pragma once

#include "CoreMinimal.h"
#include "ChatStructure.generated.h"

/**
 *
 */
USTRUCT(BlueprintType)
struct FChatData
{
	GENERATED_USTRUCT_BODY()

public:
    #define uid UserID 
	#define nickname UserName 
	#define text Message 
	#define ct IdentifyCode 

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int64 UserID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName UserName;	

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString Message;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString IdentifyCode;

};