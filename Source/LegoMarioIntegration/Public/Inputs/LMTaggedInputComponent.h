// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EnhancedInputComponent.h"
#include "LMInputConfig.h"
#include "LMTaggedInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class LEGOMARIOINTEGRATION_API ULMTaggedInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	template<class UserClass, typename FuncType>
	void BindActionsByTag(const ULMInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func)
	{
		check(InputConfig)
		if(const UInputAction* IA = InputConfig->FindInputActionForTag(InputTag))
		{
			BindAction(IA,TriggerEvent,Object,Func);
		}
	}
};
