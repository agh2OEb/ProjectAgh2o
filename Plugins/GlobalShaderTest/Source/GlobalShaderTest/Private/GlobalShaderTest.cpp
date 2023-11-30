// Copyright Epic Games, Inc. All Rights Reserved.

#include "GlobalShaderTest.h"
#include "Interfaces/IPluginManager.h"
#include "GlobalShaderDeclaration.h"
#include "ShaderParameterUtils.h"
#include "Misc/Paths.h"
#include "ShaderCore.h"

#define LOCTEXT_NAMESPACE "FGlobalShaderTestModule"

void FGlobalShaderTestModule::StartupModule()
{
	//Plugin 폴더에 있는 usf파일을 감지할 수 있도록 Plugin Module 시작지점에 Shader 폴더를 추가합니다. 
	auto Plugin = IPluginManager::Get().FindPlugin(TEXT("GlobalShaderTest"));
	FString PluginShaderDirectory = FPaths::Combine(Plugin->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Plugin/GlobalShaderTest"), PluginShaderDirectory);

}

void FGlobalShaderTestModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FGlobalShaderTestModule, GlobalShaderTest)