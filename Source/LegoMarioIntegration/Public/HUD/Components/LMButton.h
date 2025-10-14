// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/Button.h"
#include "LMButton.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FButtonHoveredDelegate, ULMButton*, button);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FButtonUnHoveredDelegate, ULMButton*, button);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FButtonClickedDelegate, ULMButton*, button);

UCLASS()
class LEGOMARIOINTEGRATION_API ULMButton : public UButton
{
	GENERATED_BODY()

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	UFUNCTION()
	void HandleCustomHovered();
	UFUNCTION()
	void HandleCustomUnHovered();
	UFUNCTION()
	void HandleCustomClicked();
	UPROPERTY(BlueprintAssignable)
	FButtonUnHoveredDelegate OnButtonHoveredDelegate;
	UPROPERTY(BlueprintAssignable)
	FButtonUnHoveredDelegate OnButtonUnHoveredDelegate;
	UPROPERTY(BlueprintAssignable)
	FButtonClickedDelegate OnButtonClickedDelegate;

private:
	UPROPERTY(EditAnywhere, Category="PixelProperties", BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	UDataTable* DT_Layout;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true), Category="PixelProperties")
	TArray<FGameplayTag> LevelTags;

	UFUNCTION(BlueprintCallable, Category="PixelProperties")
	void GatherLevelPixelTags();

public:
	FORCEINLINE UDataTable* GetLayoutDataTable() const { return DT_Layout; };
};
