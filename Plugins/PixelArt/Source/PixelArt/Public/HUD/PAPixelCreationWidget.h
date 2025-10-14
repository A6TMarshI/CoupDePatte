// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Engine/DataTable.h"
#include "PAPixelCreationWidget.generated.h"

#define PIXEL_ART_FILE_PATH FString("PixelArtJson/PixelArtData.json")

class UHorizontalBox;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPANativeConstructFinishedDelegate);

class UEditableText;
class UVerticalBox;

UENUM()
enum class EPixelState : uint8
{
	EPS_Hidden,
	EPS_Unlocked,
	EPS_Locked,
};

USTRUCT(BlueprintType)
struct FPAPixelState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FLinearColor Color = FLinearColor::Black;
	UPROPERTY(BlueprintReadWrite)
	FLinearColor HitColor = FLinearColor::Black;
	UPROPERTY(BlueprintReadWrite)
	EPixelState PixelState = EPixelState::EPS_Hidden;
};

USTRUCT(BlueprintType)
struct FPAPixelArtDataInfos : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag PixelArtTag;
	UPROPERTY(BlueprintReadOnly)
	TMap<FVector2D, FPAPixelState> PixelColorMap = TMap<FVector2D, FPAPixelState>();
	UPROPERTY(BlueprintReadWrite)
	int Rows = 4.f;
	UPROPERTY(BlueprintReadWrite)
	int Columns = 4.f;

	static TArray<FPAPixelArtDataInfos>::SizeType FindByTag(const TArray<FPAPixelArtDataInfos>& Array, const FGameplayTag& Tag);
};

/**
 * 
 */
UCLASS()
class PIXELART_API UPAPixelCreationWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	bool TryGetPixelFromJson(UPARAM(ref)FPAPixelArtDataInfos& PixelArt);
	static bool TryGetPixelFromJson(FPAPixelArtDataInfos& PixelArt, const FGameplayTag& PixelArtTag);

protected:
	virtual void NativeConstruct() override;
	void SynchronizePixelProperties();

	UFUNCTION(BlueprintCallable)
	void GeneratePixelArtScale();
	UFUNCTION(BlueprintCallable)
	virtual void GeneratePixelArtFromJson(const FPAPixelArtDataInfos& PixelArt, const bool& bOverrideScale);
	UPROPERTY(EditAnywhere, Category="PixelSettings", BlueprintReadWrite, meta=(AllowPrivateAccess))
	uint8 Column = 4;
	UPROPERTY(EditAnywhere, Category="PixelSettings", BlueprintReadWrite, meta=(AllowPrivateAccess))
	uint8 Row = 4;
	UPROPERTY(meta=(BindWidget))
	UVerticalBox* VB_Row = nullptr;
	UPROPERTY(EditAnywhere, Category="PixelSettings", BlueprintReadWrite, meta=(AllowPrivateAccess))
	FGameplayTag PixelTag;
	UPROPERTY(EditAnywhere, Category="PixelSettings", BlueprintReadWrite)
	UDataTable* DataTable;

private:
	UFUNCTION(BlueprintCallable)
	bool AddNewGameplayTag();
	UFUNCTION(BlueprintCallable)
	bool RemoveGameplayTag();

	UFUNCTION(BlueprintCallable)
	FGameplayTag RequestPixelArtTag(const FName& TagName);

	UPROPERTY(meta=(BindWidget, OptionalWidget, AllowPrivateAccess), BlueprintReadWrite)
	UEditableText* ETXT_R;
	UPROPERTY(meta=(BindWidget, OptionalWidget, AllowPrivateAccess), BlueprintReadWrite)
	UEditableText* ETXT_G;
	UPROPERTY(meta=(BindWidget, OptionalWidget, AllowPrivateAccess), BlueprintReadWrite)
	UEditableText* ETXT_B;
	UPROPERTY(meta=(BindWidget, OptionalWidget, AllowPrivateAccess), BlueprintReadWrite)
	UEditableText* ETXT_GameplayTag;
	UPROPERTY(meta=(BindWidget, OptionalWidget, AllowPrivateAccess), BlueprintReadWrite)
	UImage* IMG_ColorSample;

	UPROPERTY(BlueprintAssignable)
	FPANativeConstructFinishedDelegate OnNativeConstructFinishedDelegate;

	UFUNCTION()
	void ColorOnMouseButtonDown(UImage* Image);
	UFUNCTION()
	void ClearOnMouseButtonDown(UImage* Image);
	UFUNCTION(BlueprintCallable)
	void UpdateColor();

	UFUNCTION(BlueprintCallable)
	bool SavePixelArt();
	UFUNCTION(BlueprintCallable)
	bool SaveAsPixelArt();
	UFUNCTION(BlueprintCallable)
	bool LoadPixelArt();

	UPROPERTY(EditAnywhere, Category="PixelSettings")
	FMargin InMargin = FMargin{5.f};
	UPROPERTY(VisibleAnywhere, Category="PixelSettings", BlueprintReadWrite, meta=(AllowPrivateAccess))
	FLinearColor Color = FLinearColor::Black;
	UPROPERTY(Category="PixelSettings", VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FGameplayTagContainer AvailablePixelArtTags;

public:
	virtual bool IsEditorOnly() const override { return true; }
};
