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
	//Plugin ������ �ִ� usf������ ������ �� �ֵ��� Plugin Module ���������� Shader ������ �߰��մϴ�. 
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