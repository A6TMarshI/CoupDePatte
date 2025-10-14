// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LMReadWriteFile.generated.h"

class UDataTable;
/**
 * 
 */
UCLASS()
class LEGOMARIOINTEGRATION_API ULMReadWriteFile : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static bool SaveTelemetryAsJson(FString FilePath, TSharedPtr<FJsonObject> Object, FString& OutMessage,
	                                const bool& bAppendFile);

	UFUNCTION(BlueprintCallable, Category="HelperFunction")
	static bool WriteToFile(FString FilePath, FString String, const bool& bAppendFile);

	UFUNCTION(BlueprintCallable, Category="HelperFunction")
	static FString ReadFromFile(FString FilePath);

	static TSharedPtr<FJsonObject> ReadJson(FString JsonFilePath);
};
