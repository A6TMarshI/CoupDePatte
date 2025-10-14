// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class LegoMarioIntegration : ModuleRules
{
	public LegoMarioIntegration(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "UMG", "Networking", "Sockets", "Json", "JsonUtilities",
			"AIModule", "GameplayTasks", "NavigationSystem", "Niagara",
			"EnhancedInput", "GameplayTags", "Slate", "SlateCore", "PixelArt"
		});

		bUseUnity = false;	// Get Rid of annoying error that seems to come from unreal engine source code such as
							// G:\Unreal\UE_5.0\Engine\Source\Runtime\AudioMixer\Public\Quartz\AudioMixerQuantizedCommands.h(31): 
							// error C2838: 'PlaySoundW': illegal qualified name in member declaration
							// G:\Unreal\UE_5.0\Engine\Source\Runtime\AudioMixer\Public\Quartz\AudioMixerQuantizedCommands.h(31): 
							// error C2440: 'return': cannot convert from 'BOOL (__cdecl *)(LPCWSTR,HMODULE,DWORD)' to 'EQuartzCommandType'
							// G:\Unreal\UE_5.0\Engine\Source\Runtime\AudioMixer\Public\Quartz\AudioMixerQuantizedCommands.h(31): 
							// note: There is no context in which this conversion is possible

		if (base.Target.bBuildEditor)
		{
			PublicDependencyModuleNames.AddRange(new string[]
			{
				"UnrealEd"
			});
		}

		PrivateDependencyModuleNames.AddRange(new string[] { });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}