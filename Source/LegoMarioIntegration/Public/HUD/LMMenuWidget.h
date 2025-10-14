// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "LMMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class LEGOMARIOINTEGRATION_API ULMMenuWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="SoundProperties")
	TObjectPtr<USoundBase> HoveredSound;

	UPROPERTY(EditAnywhere, Category="SoundProperties")
	TObjectPtr<USoundBase> ClickedSound;

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void PlayHoveredSound();
	UFUNCTION()
	void PlayClickedSound();

private:
	void AddButton(UPanelWidget* UlmMenuWidget, TArray<UButton*>& Array);
	UFUNCTION(BlueprintCallable)
	TArray<UButton*> FindAllButtons();
};
