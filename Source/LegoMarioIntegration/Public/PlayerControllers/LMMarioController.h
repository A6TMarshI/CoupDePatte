// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/LMMarioCharacter.h"
#include "Engine/DataTable.h"
#include "GameFramework/PlayerController.h"
#include "LMMarioController.generated.h"

class ALMMarioHUD;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDirectionChangedDelegate, float, X, float, Z);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMoveForwardBackwardDelegate, float, HorizontalVector);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRotateLeftRightDelegate, float, VerticalVector);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPantChangedDelegate, EBinaryPantsValue, PantsValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRotationEventDelegate, bool, IsRotating);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FProtectiveShieldDisplayDelegate, FLinearColor, ShieldProtection);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FJumpingDelegate);

UENUM()
enum class EGroudAndTileColor:uint8
{
	EGATC_White UMETA(DisplayName = "White"),
	EGATC_Red UMETA(DisplayName = "Red"),
	EGATC_Blue UMETA(DisplayName = "Blue"),
	EGATC_Yellow UMETA(DisplayName = "Yellow"),
	EGATC_Green UMETA(DisplayName = "Green"),
	EGATC_Purple UMETA(DisplayName = "Purple"),

	EGATC_Black UMETA(DisplayName = "Black"),
	EGATC_Cyan UMETA(DisplayName = "Cyan"),
	EGATC_Brown UMETA(DisplayName = "Brown"),
	EGATC_BrownNouggat UMETA(DisplayName = "BrownNouggat"),

	EGATC_RotationTile UMETA(DisplayName = "RotationTile"),
	EGATC_StartTile UMETA(DisplayName = "StartTile"),
	EGATC_FlagTile UMETA(DisplayName = "FlagTile"),
	EGATC_BlockTile UMETA(DisplayName = "BlockTile"),
	EGATC_SeesawTile UMETA(DisplayName = "SeesawTile"),
};


UCLASS()
class LEGOMARIOINTEGRATION_API ALMMarioController : public APlayerController
{
	GENERATED_BODY()

public:
	ALMMarioController();
	/// Not in use anywhere, only available for further possible work. Was used to controller character movements in game. So far it was working well but since working with cerebral palsy patient result in high compensation movement which trouble LEGO tilt orientation, Movement was almost impossible to process.
	/// Even through offset calibrations. Nonetheless, this is working as a joystick compensating movement could be fixed from building a physical platform responsible to lock Lego into a standard standing position just like joysticks
	/// @param DirectionVector Value received from LEGO's tilt through the socket communication 
	virtual void SetMarioDirection(FVector DirectionVector);
	virtual void OnMatchStateSet(const FName MatchState);
	/// This was responsible to change this controller's pawn color based on the binary value received. Since our character is never visible through Whack-a-Mole concept, this is not relevant anymore, but could still be usable for further improvement of the game
	/// @param BinaryPantsValue Value received from LEGO's binary pants code through the socket communication 
	void SetMarioOutfit(const int BinaryPantsValue);
	/// This function is responsible to process the first color shift received as multiple color could be received almost the same time due to the bright white led underneath the lego. This ensure White in-game event does not get triggered faulty
	/// @param HexadecimalColorCode Value received from LEGO's Hexadecimal code generated from color recognition through the socket communication 
	void VerifyGroundColor(const FString HexadecimalColorCode);
	/// This is for internal use only. Since any ground data verification gets blocked once a validation timer is set, the last color could be missed, as well as being the right one to validate so if the CurrentGroundData does not match the verified one, send the last known one for a second step validation.
	/// @param LastSentGroundCode LastGroundData that was cached when the validation steps occurs
	void VerifyGroundColor(const EGroudAndTileColor LastSentGroundCode);
	void ClearAllHUD();

	UPROPERTY()
	FDirectionChangedDelegate OnDirectionChangedDelegate;

	UPROPERTY()
	FMoveForwardBackwardDelegate OnMoveForwardBackwardDelegate;

	UPROPERTY()
	FRotateLeftRightDelegate OnRotateLeftRightDelegate;

	UPROPERTY()
	FJumpingDelegate OnJumpingDelegate;

	UPROPERTY()
	FPantChangedDelegate OnPantChangedDelegate;

	FVector SpawnLocation = FVector(0, 0, 0);

	UPROPERTY(EditAnywhere, Category="StringProperties")
	UStringTable* ST_String;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PostInitializeComponents() override;
	virtual void OnPossess(APawn* InPawn) override;

	/// This is responsible for validating the LastGroundData match the CurrentGroundData. If not, initiate the secondStepValidation only if a new Color has been received meanwhile.
	/// @param LastGroundData Value to validate
	void ValidateGroundColor(EGroudAndTileColor LastGroundData);
	/// This function can be overriden following needs for groud data.
	/// @param GroundData Ground data to trigger event
	/// @param GroundColor Color matching ground data if applied
	virtual void TriggerMarioGroundOrTileEvent(const EGroudAndTileColor GroundData, FLinearColor& GroundColor);

	UPROPERTY()
	ALMMarioHUD* MarioHUD;

	void WinLooseUIInfo(FLinearColor TextColor, FString TextInfo);
	FTimerHandle VictoryTimerHandle;
	UPROPERTY(EditAnywhere, Category="SoundProperties")
	USoundBase* VictorySound;
	UPROPERTY(EditAnywhere, Category="SoundProperties")
	USoundBase* DefeatSound;

	bool bIsStunned = false;
	bool bIsShielded = false;

	UPROPERTY(EditAnywhere, Category="MovementProperties")
	int JumpThreshold = 18;
	float LastJumpHeight;

	UPROPERTY(EditAnywhere, Category="GameplayProperties")
	FGameplayTagContainer GameplayTags;

private:
	FTimerHandle WaitForSocketBindingTimerHandle;

	UPROPERTY(EditAnywhere, Category="MovementProperties")
	float ChangingDirectionThreshold = 6.f;
	UPROPERTY(EditAnywhere, Category="MovementProperties")
	float ChangingRotationThreshold = 6.f;;
	UPROPERTY(EditAnywhere, Category="MovementProperties")
	float ChangingDirectionSmoothness = .04f;
	UPROPERTY(EditAnywhere, Category="MovementProperties")
	float StopMovementThreshold;

	FTimerHandle CheckGroundColorTimerHandle;
	float CheckGroundColorDelay = 1.f / 30.f;
	EGroudAndTileColor CurrentGroundData;
	TMap<EGroudAndTileColor, FString> GroundAndTileColorMap;
	FTimerHandle PollTimer;
	bool bSecondStepValidationRequire = false;

	int InitialBinaryPantsValue = 0;

public:
	FORCEINLINE float GetChangingDirectionThreshold() const { return ChangingDirectionThreshold; }
	FORCEINLINE float GetChangingRotationThreshold() const { return ChangingRotationThreshold; }
	FORCEINLINE float GetChangingDirectionSmoothness() const { return ChangingDirectionSmoothness; }
	FORCEINLINE float GetStopMovementThreshold() const { return StopMovementThreshold; }
	FORCEINLINE int GetInitialBinaryPantsValue() const { return InitialBinaryPantsValue; }
	FORCEINLINE bool GetIsShielded() const { return bIsShielded; }
	FORCEINLINE bool GetIsStunned() const { return bIsStunned; }
	UFUNCTION(BlueprintCallable)
	virtual bool HasRemainingHealth();
	FORCEINLINE ALMMarioHUD* GetMarioHUD() const { return MarioHUD; }
	FORCEINLINE FGameplayTagContainer GetGameplayTags() const { return GameplayTags; }
	FORCEINLINE void ClearAllTags()
	{
		FGameplayTagContainer GameplayTagsToRemove = GameplayTags;
		GameplayTags.RemoveTags(GameplayTagsToRemove);
	}

protected:
};
