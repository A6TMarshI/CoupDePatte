// Fill out your copyright notice in the Description page of Project Settings.


#include "Socket/LMTCPSocketListener.h"

#include "Sockets.h"
#include "Async/Async.h"
#include "GameInstances/LMGameInstance.h"
#include "Serialization/JsonSerializer.h"
#include "Socket/LMSocket.h"

bool FLMTCPSocketListener::Init()
{
	bStopRequested = false;
	return true;
}

uint32 FLMTCPSocketListener::Run()
{
	while (!bStopRequested)
	{
		if (!ConnectionSocket)
		{
			Stop();
			continue;
		}

		TArray<uint8> ReceivedData;

		uint32 Size = 0;
		const uint32 MaxBufferSize = 2 * 1024 * 1024;
		int32 Read = 0;
		while (ConnectionSocket->HasPendingData(Size))
		{
			ReceivedData.SetNumUninitialized(FMath::Min(Size, MaxBufferSize));

			ConnectionSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);
		}

		if (ReceivedData.Num() <= 0)continue;

		FUTF8ToTCHAR Converter((const ANSICHAR*)ReceivedData.GetData(), Read);
		FString JsonString(Converter.Get(), Converter.Length());


		TArray<FString> JsonArray;
		JsonString.ParseIntoArray(JsonArray, TEXT("}"), true);
		for (auto Json : JsonArray)
		{
			Json.Append("}");


			//UE_LOG(LogTemp, Warning, TEXT("%s Position from : %d"), *EncodeData, ConnectionSocket->GetPortNo());

			TSharedPtr<FJsonValue> JsonValue;
			TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Json);

			if (FJsonSerializer::Deserialize(JsonReader, JsonValue))
			{
				TSharedPtr<FJsonObject> JsonVector = JsonValue->AsObject();
				TSharedPtr<FJsonValue> DataField = JsonVector.Get()->TryGetField("Data");
				TSharedPtr<FJsonValue> IDField = JsonVector.Get()->TryGetField("ID");

				switch (DataField->AsArray().Num())
				{
				case 3: // Handle Accelerometer data for controlling movement direction and jump based on the 3D Vector received;
					/*AsyncTask(ENamedThreads::GameThread, [this, JsonVector]()
					{
						if (bStopRequested)return;
						if (IsValid(MarioController) && IsValid(GameInstance) && !bPauseRequested)
							MarioController->SetMarioDirection(FVector(
								(JsonVector[0]->AsNumber() - GameInstance->CalibrationOffset.X),
								(JsonVector[1]->AsNumber() - GameInstance->CalibrationOffset.Y),
								(JsonVector[2]->AsNumber() - GameInstance->CalibrationOffset.Z)
								));
					});*/
					break;
				case 1: // Handle binary data sent as a single block
					if (DataField->AsArray()[0]->AsString().Left(2) == TEXT("0b")) // Pants data received as Binary (Start with "0b")
					{
						AsyncTask(ENamedThreads::GameThread, [this, DataField]()
						{
							const FString DataFormatted = DataField->AsArray()[0]->AsString().Mid(2);
							const int DataAsNumber = FCString::Atoi(*DataFormatted);
							if (bStopRequested)return;
							if (IsValid(MarioController))
							{
								MarioController->SetMarioOutfit(DataAsNumber);
							}
							else
							{
								LegoOutfit = DataAsNumber;
							}
						});
					}
					else if (DataField->AsArray()[0]->AsString() == TEXT("CONNECTION_COMPLETED"))
					{
						LegoName = IDField->AsString();
						AsyncTask(ENamedThreads::GameThread, [this]()
						{
							if (bStopRequested)return;
							if (IsValid(GameInstance))
								GameInstance->OnConnectionEstablishedDelegate.Broadcast(LegoName, true);
						});
					}
					else if (DataField->AsArray()[0]->AsString() == TEXT("CONNECTION_FAILURE"))
					{
						AsyncTask(ENamedThreads::GameThread, [this]()
						{
							if (bStopRequested)return;
							if (bPauseRequested)return;
							if (IsValid(GameInstance))
								GameInstance->OnConnectionEstablishedDelegate.Broadcast(LegoName, false);
						});
					}
					else if (DataField->AsArray()[0]->AsString() == TEXT("CONNECTION_CLOSED"))
					{
						AsyncTask(ENamedThreads::GameThread, [this]()
						{
							if (IsValid(GameInstance))
								GameInstance->OnConnectionEstablishedDelegate.Broadcast(LegoName, false);
						});
					}
					else // Any other data sent as Hexadecimal, such as Ground color or Tiles
					{
						AsyncTask(ENamedThreads::GameThread, [this, DataField]()
						{
							if (bStopRequested)return;
							if (IsValid(MarioController))
							{
								MarioController->VerifyGroundColor(DataField->AsArray()[0]->AsString());
								if (LegoGroundColor != "")LegoGroundColor = "";
							}
							else
							{
								LegoGroundColor = DataField->AsArray()[0]->AsString();
							}
						});
					}
					break;
				default:
					UE_LOG(LogTemp, Warning, TEXT("Data could not be interpreted"))
					break;
				}
			}
		}
	}
	return 0;
}

void FLMTCPSocketListener::Stop()
{
	bStopRequested = true;
}

void FLMTCPSocketListener::Pause(bool bPaused)
{
	bPauseRequested = bPaused;
}
