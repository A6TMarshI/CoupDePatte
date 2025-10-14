// Fill out your copyright notice in the Description page of Project Settings.


#include "HelperLibrary/LMReadWriteFile.h"

#include "JsonObjectConverter.h"
#include "Misc/FileHelper.h"
#include "PlayerStates/LMPlayerState.h"


bool ULMReadWriteFile::SaveTelemetryAsJson(FString FilePath, TSharedPtr<FJsonObject> Object, FString& OutMessage, const bool& bAppendFile)
{
	FString JsonString;

	if (!FJsonSerializer::Serialize(Object.ToSharedRef(), TJsonWriterFactory<>::Create(&JsonString, 0)))
	{
		OutMessage = FString::Printf(TEXT("Failed to write Json : %s"), *FilePath);
		return false;;
	}
	if (!WriteToFile(FilePath, JsonString, bAppendFile))return false;
	OutMessage = FString::Printf(TEXT("Successfully write Json : %s"), *FilePath);
	return true;
}

bool ULMReadWriteFile::WriteToFile(FString FilePath, FString String, const bool& bAppendFile)
{
	if (!FFileHelper::SaveStringToFile(bAppendFile ? ReadFromFile(FilePath).Append("\n" + String) : String, *FilePath))
	{
		return false;;
	}
	return true;
}

FString ULMReadWriteFile::ReadFromFile(FString FilePath)
{
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*FilePath))return FString();
	FString OutString = FString();
	FFileHelper::LoadFileToString(OutString, *FilePath);
	return OutString;
}

TSharedPtr<FJsonObject> ULMReadWriteFile::ReadJson(FString JsonFilePath)
{
	const FString JsonString = ReadFromFile(JsonFilePath);
	TSharedPtr<FJsonObject> JsonObject;
	if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(JsonString), JsonObject))
	{
		return nullptr;
	}
	return JsonObject;
}