// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/DataTable.h"
#include "PAReadWriteFile.generated.h"

struct FPAPixelArtDataInfos;
/**
 * 
 */
UCLASS()
class PIXELART_API UPAReadWriteFile : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="HelperFunction")
	static bool CreatePixelArtSave(FString FilePath, const TArray<FPAPixelArtDataInfos>& StructArray);

	static void SaveJsonToFile(const FString& JsonString);
	static void SerializeToTableJson(const TArray<FPAPixelArtDataInfos>& StructArray, FString& OutJsonString);
	
	UFUNCTION(BlueprintCallable, Category="HelperFunction")
	static bool WriteToFile(FString FilePath, FString String, const bool& bAppendFile);

#if WITH_EDITOR
	static UDataTable* ImportDataTableFromJson(const FString& SourcePath, const FString& DestinationPath, UScriptStruct* StructClass, bool& bOutSuccess, FString& OutInfoMessage);
#endif

	UFUNCTION(BlueprintCallable, Category="HelperFunction")
	static FString ReadFromFile(FString FilePath);

	static TSharedPtr<FJsonObject> ReadJson(FString JsonFilePath);

	static TArray<FPAPixelArtDataInfos> ReadJsonToMultipleStruct(const FString& FilePath);
	static TArray<FPAPixelArtDataInfos> JsonToMultipleStruct(const FString& JsonString);

	static bool ExportDataTableToJson(const FString& FilePath, UDataTable* DataTable);

#if WITH_EDITOR
	static UAssetImportTask* CreateImportTask(const FString& SourcePath, const FString& DestinationPath, UFactory* Factory, UObject* Options);
	static UObject* ProcessImportTask(UAssetImportTask* ImportTask);
#endif
	static UObject* ImportAsset(const FString& SourcePath, const FString& DestinationPath);
};
