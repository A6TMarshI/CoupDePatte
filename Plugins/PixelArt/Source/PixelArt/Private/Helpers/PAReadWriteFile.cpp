// Fill out your copyright notice in the Description page of Project Settings.


#include "Helpers/PAReadWriteFile.h"
#if WITH_EDITOR
#include "AssetImportTask.h"
#include "AssetToolsModule.h"
#include "Factories/ReimportDataTableFactory.h"
#endif

#include "Json.h"
#include "JsonUtilities.h"
#include "HUD/PAPixelCreationWidget.h"
#include "Misc/FileHelper.h"


bool UPAReadWriteFile::CreatePixelArtSave(FString FilePath, const TArray<FPAPixelArtDataInfos>& StructArray)
{
	UDataTable* Table = NewObject<UDataTable>();
	Table->RowStruct = FPAPixelArtDataInfos::StaticStruct();

	for (auto Struct : StructArray)
	{
		Table->AddRow(FName(Struct.PixelArtTag.ToString()), Struct);
	}

	if (ExportDataTableToJson(FilePath, Table))return true;
	return false;
}

void UPAReadWriteFile::SaveJsonToFile(const FString& JsonString)
{
	FString Path = FPaths::ProjectContentDir() + PIXEL_ART_FILE_PATH;
	FFileHelper::SaveStringToFile(JsonString, *Path);
}

void UPAReadWriteFile::SerializeToTableJson(const TArray<FPAPixelArtDataInfos>& StructArray, FString& OutJsonString)
{
	UDataTable* Table = NewObject<UDataTable>();
	Table->RowStruct = FPAPixelArtDataInfos::StaticStruct();

	for (auto Struct : StructArray)
	{
		Table->AddRow(FName(Struct.PixelArtTag.ToString()), Struct);
	}

	TArray<TSharedPtr<FJsonValue>> RowsJsonArray;

	for (const auto& Row : Table->GetRowMap())
	{
		FPAPixelArtDataInfos* RowData = reinterpret_cast<FPAPixelArtDataInfos*>(Row.Value);

		TSharedPtr<FJsonObject> RowJsonObject = MakeShareable(new FJsonObject);
		RowJsonObject->SetStringField("Name", Row.Key.ToString());

		FString PixelArtTagString = FString::Printf(TEXT("(TagName=\"%s\")"), *RowData->PixelArtTag.GetTagName().ToString());
		RowJsonObject->SetStringField("PixelArtTag", PixelArtTagString);

		TSharedPtr<FJsonObject> PixelColorMapJsonObject = MakeShareable(new FJsonObject);
		for (auto& Pixel : RowData->PixelColorMap)
		{
			FString PixelKeyString = FString::Printf(TEXT("(X=%f,Y=%f)"), Pixel.Key.X, Pixel.Key.Y);
			FString EnumName = "/Script/PixelArt.EPixelState";

			UEnum* EnumPtr = FindObject<UEnum>(nullptr, *EnumName, true);
			if (!EnumPtr)return;
			FString PixelStateName = EnumPtr->GetNameStringByValue(static_cast<int64>(Pixel.Value.PixelState));
			FString PixelDataString = FString::Printf(
				TEXT("(Color=(R=%f,G=%f,B=%f,A=%f),HitColor=(R=%f,G=%f,B=%f,A=%f),PixelState=%s)"),
				Pixel.Value.Color.R, Pixel.Value.Color.G, Pixel.Value.Color.B, Pixel.Value.Color.A,
				Pixel.Value.HitColor.R, Pixel.Value.HitColor.G, Pixel.Value.HitColor.B, Pixel.Value.HitColor.A,
				*PixelStateName);
			PixelColorMapJsonObject->SetStringField(PixelKeyString, PixelDataString);
		}
		RowJsonObject->SetObjectField("PixelColorMap", PixelColorMapJsonObject);

		RowJsonObject->SetStringField("Rows", FString::FromInt(RowData->Rows));

		RowJsonObject->SetStringField("Columns", FString::FromInt(RowData->Columns));

		RowsJsonArray.Add(MakeShareable(new FJsonValueObject(RowJsonObject)));
	}


	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutJsonString);
	FJsonSerializer::Serialize(RowsJsonArray, Writer);
}

bool UPAReadWriteFile::WriteToFile(FString FilePath, FString String, const bool& bAppendFile)
{
	if (!FFileHelper::SaveStringToFile(bAppendFile ? ReadFromFile(FilePath).Append("\n" + String) : String, *FilePath))
	{
		return false;;
	}
	return true;
}

#if WITH_EDITOR
UDataTable* UPAReadWriteFile::ImportDataTableFromJson(const FString& SourcePath, const FString& DestinationPath, UScriptStruct* StructClass, bool& bOutSuccess, FString& OutInfoMessage)
{
	UReimportDataTableFactory* Factory = NewObject<UReimportDataTableFactory>();
	Factory->AutomatedImportSettings.ImportType = ECSVImportType::ECSV_DataTable;
	Factory->AutomatedImportSettings.ImportRowStruct = StructClass;

	UAssetImportTask* ImportTask = CreateImportTask(SourcePath, DestinationPath, Factory, nullptr);
	if (!ImportTask)return nullptr;

	UObject* ImportedAsset = ProcessImportTask(ImportTask);
	if (!ImportedAsset)return nullptr;

	return Cast<UDataTable>(ImportedAsset);
}
#endif

bool UPAReadWriteFile::ExportDataTableToJson(const FString& FilePath, UDataTable* DataTable)
{
#if WITH_EDITOR
	if (!DataTable)return false;

	FString TableString = DataTable->GetTableAsJSON();

	if (WriteToFile(FilePath, TableString, false))return true;
#endif

	return false;
}
#if WITH_EDITOR
UAssetImportTask* UPAReadWriteFile::CreateImportTask(const FString& SourcePath, const FString& DestinationPath, UFactory* Factory, UObject* Options)
{
	UAssetImportTask* Task = NewObject<UAssetImportTask>();
	if (!Task) return nullptr;

	Task->Filename = SourcePath;
	Task->DestinationPath = FPaths::GetPath(DestinationPath);
	Task->DestinationName = FPaths::GetCleanFilename(DestinationPath);

	Task->bSave = false;
	Task->bAutomated = true;
	Task->bAsync = false;
	Task->bReplaceExisting = true;
	Task->bReplaceExistingSettings = false;

	Task->Factory = Factory;
	Task->Options = Options;

	return Task;
}

UObject* UPAReadWriteFile::ProcessImportTask(UAssetImportTask* ImportTask)
{
	if (!ImportTask)return nullptr;

	if (FAssetToolsModule* AssetToolsModule = FModuleManager::LoadModulePtr<FAssetToolsModule>("AssetTools"))
	{
		AssetToolsModule->Get().ImportAssetTasks({ImportTask});
		if (ImportTask->GetObjects().Num() > 0)
		{
			return StaticLoadObject(UObject::StaticClass(), nullptr, *FPaths::Combine(ImportTask->DestinationPath, ImportTask->DestinationName));
		}
	}
	return nullptr;
}
#endif

UObject* UPAReadWriteFile::ImportAsset(const FString& SourcePath, const FString& DestinationPath)
{
#if WITH_EDITOR
	UAssetImportTask* Task = CreateImportTask(SourcePath, DestinationPath, nullptr, nullptr);
	if (!Task)return nullptr;

	UObject* Asset = ProcessImportTask(Task);
	if (!Asset)return nullptr;

	return Asset;
#else
	return nullptr;
#endif
}

FString UPAReadWriteFile::ReadFromFile(FString FilePath)
{
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*FilePath))return FString();
	FString OutString = FString();
	FFileHelper::LoadFileToString(OutString, *FilePath);
	return OutString;
}

TSharedPtr<FJsonObject> UPAReadWriteFile::ReadJson(FString JsonFilePath)
{
	const FString JsonString = ReadFromFile(JsonFilePath);
	TSharedPtr<FJsonObject> JsonObject;
	if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(JsonString), JsonObject))
	{
		return nullptr;
	}
	return JsonObject;
}

TArray<FPAPixelArtDataInfos> UPAReadWriteFile::ReadJsonToMultipleStruct(const FString& FilePath)
{
	const FString JsonString = ReadFromFile(FilePath);
	return JsonToMultipleStruct(JsonString);
}

TArray<FPAPixelArtDataInfos> UPAReadWriteFile::JsonToMultipleStruct(const FString& JsonString)
{
	if (JsonString == FString())return TArray<FPAPixelArtDataInfos>();

	UDataTable* Table = NewObject<UDataTable>();
	Table->RowStruct = FPAPixelArtDataInfos::StaticStruct();

	Table->CreateTableFromJSONString(JsonString);

	FString Context;
	TArray<FPAPixelArtDataInfos*> OutRows;
	Table->GetAllRows<FPAPixelArtDataInfos>(Context, OutRows);

	TArray<FPAPixelArtDataInfos> OutArray;
	for (const auto Row : OutRows)
	{
		OutArray.Add(*Row);
	}

	return OutArray;
}
