// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LMMenuWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/WidgetSwitcher.h"
#include "LMOptionsMenu.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FConnectionClosedDelegate, const bool, bMarioConnected, const bool, bLuigiConnected);

USTRUCT(BlueprintType)
struct FLastOptionSelected
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<UButton*> OptionButtons;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess))
	UButton* LastOptionSelected = nullptr;
};

UCLASS()
class LEGOMARIOINTEGRATION_API ULMOptionsMenu : public ULMMenuWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget, AllowPrivateAccess, OptionalWidget))
	UWidgetSwitcher* Options_Switcher;

	UPROPERTY(BlueprintAssignable)
	FConnectionClosedDelegate OnConnectionClosedDelegate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FString Username;

protected:
	UFUNCTION()
	virtual void VisibilityChanged(ESlateVisibility InVisibility);
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void PopulateSettingOptionsMap(UHorizontalBox* HorizontalBox);

	UFUNCTION(BlueprintCallable)
	void SetSelectionColor(UHorizontalBox* SelectedOptionHorizontalBox, UButton* SelectedOptionButton);

	UPROPERTY(EditAnywhere, Category="SelectionOptions")
	FLinearColor UnselectedColor = FLinearColor::Blue;
	UPROPERTY(EditAnywhere, Category="SelectionOptions")
	FLinearColor SelectedColor = FLinearColor::Green;

	UPROPERTY(BlueprintReadOnly)
	TMap<UHorizontalBox*, FLastOptionSelected> SettingOptions;

	UPROPERTY(BlueprintReadWrite)
	UGameUserSettings* UserSettings;

	UFUNCTION(BlueprintCallable)
	virtual void LoadSettingOptions();

	UFUNCTION(BlueprintCallable)
	virtual void SaveSettingOptions(ULMOptionsMenu* OptionMenu);

	UFUNCTION(BlueprintCallable)
	void SetWidgetVisibilityOnClick(UUserWidget* InWidget);

	UFUNCTION(BlueprintCallable)
	void SetSettingsOptionButton(UButton* ActiveButton, int SwitcherIndex);

private:
	UFUNCTION(BlueprintCallable)
	void ResetToDefault();


	UPROPERTY(BlueprintReadWrite, meta=(BindWidget, AllowPrivateAccess, OptionalWidget))
	UHorizontalBox* HB_SettingsOption;

public:
	FORCEINLINE ULMOptionsMenu* GetActiveWidget() const { return Cast<ULMOptionsMenu>(Options_Switcher->GetActiveWidget()); }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int GetNonNumericIndex(TArray<FString> Array) const
	{
		for (int32 i = Array.Num() - 1; i >= 0; --i)
		{
			if (!Array[i].IsNumeric())
			{
				return i + 1;
			}
		}
		return -1;
	}
};
