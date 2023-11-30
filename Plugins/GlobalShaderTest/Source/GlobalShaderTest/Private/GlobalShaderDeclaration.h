#pragma once

#include "GlobalShader.h"
#include "CoreMinimal.h"
#include "DataDrivenShaderPlatformInfo.h"
#include "MaterialShader.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "RHI.h"
#include "RHIStaticStates.h"
#include "RenderGraphResources.h"
#include "RenderGraphResources.h"
#include "RenderResource.h"
#include "SceneManagement.h"
#include "ShaderParameterUtils.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/BlueprintFunctionLibrary.h"
// FScreenPassTextureViewportParameters and FScreenPassTextureInput
#include "ScreenPass.h"
#include "SceneTexturesConfig.h"
#include "GlobalShaderDeclaration.generated.h"

class AActor;
class UTextureRenderTarget2D;

BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FGlobalUniformStructData, )
	SHADER_PARAMETER(FVector4f, ColorOne)
	SHADER_PARAMETER(FVector4f, ColorTwo)
	SHADER_PARAMETER(FVector4f, ColorThree)
	SHADER_PARAMETER(FVector4f, ColorFour)
	SHADER_PARAMETER(uint32, ColorIndex)
END_GLOBAL_SHADER_PARAMETER_STRUCT()

IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FGlobalUniformStructData, "FGlobalShaderTestUniform");

struct FMyTextureVertex
{
    FVector4 Position;
    FVector2D UV;
};

// Blueprint에서 UniformBuffer의 데이터를 입력받아올 구조체
USTRUCT(BlueprintType)
struct FGlobalShaderStructData
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = ShaderData)
		FLinearColor ColorOne;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = ShaderData)
		FLinearColor ColorTwo;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = ShaderData)
		FLinearColor ColorThree;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = ShaderData)
		FLinearColor ColorFour;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = ShaderData)
		int32 ColorIndex;
};

class FMyTextureVertexDeclaration : public FRenderResource
{
public:
	FVertexDeclarationRHIRef VertexDeclarationRHI;

	virtual void InitRHI(FRHICommandListBase& RHICmdList) override
	{
		FVertexDeclarationElementList Elements;
		uint32 Stride = sizeof(FMyTextureVertex);
		Elements.Add(FVertexElement(0, STRUCT_OFFSET(FMyTextureVertex, Position), VET_Float4, 0, Stride));
		Elements.Add(FVertexElement(0, STRUCT_OFFSET(FMyTextureVertex, UV), VET_Float2, 1, Stride));
		VertexDeclarationRHI = RHICreateVertexDeclaration(Elements);
	}

	virtual void ReleaseRHI() override
	{
		VertexDeclarationRHI->Release();
	}
};

class FMyVertexBuffer : public FVertexBuffer
{
public:
	/**
	* Initialize the RHI for this rendering resource
	*/
	virtual void InitRHI(FRHICommandListBase& RHICmdList) override
	{
		// create a static vertex buffer
		FRHIResourceCreateInfo CreateInfo(TEXT("FRHIResourceCreateInfo"));
		VertexBufferRHI = RHICmdList.CreateVertexBuffer(sizeof(FMyTextureVertex) * 4, BUF_Static, CreateInfo);

		void* VoidPtr = RHICmdList.LockBuffer(VertexBufferRHI, 0, sizeof(FMyTextureVertex) * 4, RLM_WriteOnly);

		// Generate the vertices used
		FMyTextureVertex* Vertices = reinterpret_cast<FMyTextureVertex*>(VoidPtr);

		Vertices[0].Position = FVector4(-1.0f, 1.0f, 0.0f, 1.0f);
		Vertices[1].Position = FVector4(1.0f, 1.0f, 0.0f, 1.0f);
		Vertices[2].Position = FVector4(-1.0f, -1.0f, 0.0f, 1.0f);
		Vertices[3].Position = FVector4(1.0f, -1.0f, 0.0f, 1.0f);
		Vertices[0].UV = FVector2D(0.0f, 1.0f);
		Vertices[1].UV = FVector2D(1.0f, 1.0f);
		Vertices[2].UV = FVector2D(0.0f, 0.0f);
		Vertices[3].UV = FVector2D(1.0f, 0.0f);
		RHICmdList.UnlockBuffer(VertexBufferRHI);
	}
};

extern TGlobalResource<FMyVertexBuffer> GMyVertexBuffer;

// Blueprint에 노출시킬 함수 선언
UCLASS(MinimalAPI, meta = (ScriptName = "TestShaderLibrary"))
class UGlobalTestShaderBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

		UFUNCTION(BlueprintCallable, Category = "GlobalShaderTestPlugin", meta = (WorldContext = "WorldContextObject"))
		static void DrawGlobalTestShaderRenderTarget(class UTextureRenderTarget2D* OutputRenderTarget
			, AActor* Actor, FLinearColor MyColor, UTexture* MyTexture, FGlobalShaderStructData ShaderStructData);
};
