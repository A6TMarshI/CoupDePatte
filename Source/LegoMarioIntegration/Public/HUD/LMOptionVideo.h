// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LMOptionsMenu.h"
#include "LMOptionVideo.generated.h"

/**
 * 
 */
UCLASS()
class LEGOMARIOINTEGRATION_API ULMOptionVideo : public ULMOptionsMenu
{
	GENERATED_BODY()

protected:
	
	virtual void NativeConstruct() override;
	
	virtual void LoadSettingOptions() override;

private:
	UPROPERTY(BlueprintReadWrite,meta=(BindWidget,AllowPrivateAccess))
	UHorizontalBox* HB_WindowMode;
	UPROPERTY(BlueprintReadWrite,meta=(BindWidget,AllowPrivateAccess))
	UHorizontalBox* HB_Resolution;
	UPROPERTY(BlueprintReadWrite,meta=(BindWidget,AllowPrivateAccess))
	UHorizontalBox* HB_FrameLimit;
	UPROPERTY(BlueprintReadWrite,meta=(BindWidget,AllowPrivateAccess))
	UHorizontalBox* HB_ViewDistance;
	UPROPERTY(BlueprintReadWrite,meta=(BindWidget,AllowPrivateAccess))
	UHorizontalBox* HB_PostProcess;
	UPROPERTY(BlueprintReadWrite,meta=(BindWidget,AllowPrivateAccess))
	UHorizontalBox* HB_AntiAliasing;
	UPROPERTY(BlueprintReadWrite,meta=(BindWidget,AllowPrivateAccess))
	UHorizontalBox* HB_ShadowQuality;
	
	UFUNCTION(BlueprintCallable)
	void SetScreenResolution(UButton* ResolutionButton);
	UFUNCTION(BlueprintCallable)
	void SetFPSCap(float FPSCap);
	UFUNCTION(BlueprintCallable)
	void SetViewDistance(int ViewDistance);
	UFUNCTION(BlueprintCallable)
	void SetPostProcessQuality(int PostProcessQuality);
	UFUNCTION(BlueprintCallable)
	void SetAntiAliasingQuality(int AntiAliasingQuality);
	UFUNCTION(BlueprintCallable)
	void SetShadowQuality(int ShadowQuality);
	UFUNCTION(BlueprintCallable)
	void SetWindowMode(EWindowMode::Type bFullScreen);
};
