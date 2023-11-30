
#include "GlobalShaderDeclaration.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFrameWork/Actor.h"

#include "ShaderParameterUtils.h"
#include "PipelineStateCache.h"
#include "GlobalShader.h"

/*
FGlobalVertexBuffer
*/

class FGlobalShaderTest : public FGlobalShader
{
	DECLARE_INLINE_TYPE_LAYOUT(FGlobalShaderTest, NonVirtual);
public:
	FGlobalShaderTest() {}
	FGlobalShaderTest(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
		SimpleColorVal.Bind(Initializer.ParameterMap, TEXT("SimpleColor"));
		TestTextureVal.Bind(Initializer.ParameterMap, TEXT("MyTexture"));
		TestTextureSampler.Bind(Initializer.ParameterMap, TEXT("MyTextureSampler"));
	}

	static bool ShouldCache(EShaderPlatform Platform)
	{
		return true;
	}

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return true;
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
		OutEnvironment.SetDefine(TEXT("TEST_MICRO"), 1);
	}

	void SetParameters(FRHIBatchedShaderParameters& RHICmdList, const FLinearColor& MyColor
		, FTextureReferenceRHIRef MyTextureRHI, FGlobalShaderStructData& InShaderStructData)
	{
		SetShaderValue(RHICmdList, SimpleColorVal, MyColor);
		SetTextureParameter(RHICmdList, TestTextureVal, TestTextureSampler
			, TStaticSamplerState<SF_Trilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI(), MyTextureRHI);

		FGlobalUniformStructData UniformData;
		UniformData.ColorOne = InShaderStructData.ColorOne;
		UniformData.ColorTwo = InShaderStructData.ColorTwo;
		UniformData.ColorThree = InShaderStructData.ColorThree;
		UniformData.ColorFour = InShaderStructData.ColorFour;
		UniformData.ColorIndex = InShaderStructData.ColorIndex;
		SetUniformBufferParameterImmediate(RHICmdList, GetUniformBufferParameter<FGlobalUniformStructData>(), UniformData);
	}

	//virtual bool Serialize(FArchive& Ar) override
	//{
	//	bool bShaderHasOutdatedParameters = FGlobalShader::Serialize(Ar);
	//	Ar << SimpleColorVal << TestTextureVal;
	//	return bShaderHasOutdatedParameters;
	//}

private:
	LAYOUT_FIELD(FShaderParameter, SimpleColorVal);
	LAYOUT_FIELD(FShaderResourceParameter, TestTextureVal);
	LAYOUT_FIELD(FShaderResourceParameter, TestTextureSampler);
};

class FGlobalShaderTestVS : public FGlobalShaderTest
{
	DECLARE_SHADER_TYPE(FGlobalShaderTestVS, Global);

public:
	FGlobalShaderTestVS() {}

	FGlobalShaderTestVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShaderTest(Initializer)
	{

	}
};

class FGlobalShaderTestPS : public FGlobalShaderTest
{
	DECLARE_SHADER_TYPE(FGlobalShaderTestPS, Global);

public:
	FGlobalShaderTestPS() {}

	FGlobalShaderTestPS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShaderTest(Initializer)
	{

	}
};

IMPLEMENT_SHADER_TYPE(, FGlobalShaderTestVS, TEXT("/Plugin/GlobalShaderTest/Private/GlobalShaderTest.usf"), TEXT("MainVS"), SF_Vertex)
IMPLEMENT_SHADER_TYPE(, FGlobalShaderTestPS, TEXT("/Plugin/GlobalShaderTest/Private/GlobalShaderTest.usf"), TEXT("MainPS"), SF_Pixel)

// RenderTarget에 렌더링하는 코드

TGlobalResource<FMyVertexBuffer> GMyVertexBuffer;


static void DrawTestShaderRenderTarget_RenderThread(
	FRHICommandListImmediate& RHICmdList,
	FTextureRenderTargetResource* OutputRenderTargetResource,
	ERHIFeatureLevel::Type FeatureLevel,
	FName TextureRenderTargetName,
	FLinearColor MyColor,
	FTextureReferenceRHIRef MyTextureRHI,
	FGlobalShaderStructData ShaderStructData
)
{
	check(IsInRenderingThread());

#if WANTS_DRAW_MESH_EVENTS
	FString EventName;
	TextureRenderTargetName.ToString(EventName);
	SCOPED_DRAW_EVENTF(RHICmdList, SceneCapture, TEXT("GlobalSahderTest %s"), *EventName);
#else
	SCOPED_DRAW_EVENT(RHICmdList, DrawTestShaderRenderTarget_RenderThread);
#endif

	// Set render target.
	FRHITexture2D* RenderTargetTexture = OutputRenderTargetResource->GetRenderTargetTexture();
	RHICmdList.Transition(FRHITransitionInfo(RenderTargetTexture, ERHIAccess::SRVMask, ERHIAccess::RTV));

	FRHITextureDesc TextureDesc = OutputRenderTargetResource->TextureRHI->GetDesc();
	TextureDesc.Reset();
	if (TextureDesc.IsMultisample())
	{
		int a=0;
	}

	FRHITexture* ColorRTs[1] = { RenderTargetTexture };
	FRHIRenderPassInfo RPInfo(1, ColorRTs, ERenderTargetActions::Clear_Store);
	//FRHIRenderPassInfo RPInfo(RenderTargetTexture, ERenderTargetActions::Clear_Store, OutputRenderTargetResource->TextureRHI, FExclusiveDepthStencil::DepthNop_StencilNop);
	RHICmdList.BeginRenderPass(RPInfo, TEXT("DrawTestShaderRenderTarget_RenderThread"));

	//FRHITexture2D* RenderTargetTexture = OutputRenderTargetResource->GetRenderTargetTexture();
	//RHICmdList.Transition(FRHITransitionInfo(RenderTargetTexture, ERHIAccess::SRVMask, ERHIAccess::RTV));

	//FRHIRenderPassInfo RPInfo(RenderTargetTexture, ERenderTargetActions::Clear_Store, OutputRenderTargetResource->TextureRHI, FExclusiveDepthStencil::DepthNop_StencilNop);
	//RHICmdList.BeginRenderPass(RPInfo, TEXT("DrawTestShaderRenderTarget_RenderThread"));
	{
		// Update viewport.
		const FIntPoint DisplacementMapResolution(OutputRenderTargetResource->GetSizeX(), OutputRenderTargetResource->GetSizeY());
		RHICmdList.SetViewport(0, 0, 0.f, DisplacementMapResolution.X, DisplacementMapResolution.Y, 1.f);

		FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(FeatureLevel);
		TShaderMapRef<FGlobalShaderTestVS> VertexShader(GlobalShaderMap);
		TShaderMapRef<FGlobalShaderTestPS> PixelShader(GlobalShaderMap);

		FMyTextureVertexDeclaration VertexDec;
		VertexDec.InitRHI(RHICmdList);

		// Set the graphic pipeline state.  
		FGraphicsPipelineStateInitializer GraphicsPSOInit;
		RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);
		GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
		GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();
		GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
		GraphicsPSOInit.PrimitiveType = PT_TriangleStrip;
		GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GetVertexDeclarationFVector4();  //VertexDec.VertexDeclarationRHI;
		GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
		GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
		SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit, 0);

		// Update shader uniform parameters.

		FRHIBatchedShaderParameters& BatchedParameters = RHICmdList.GetScratchShaderParameters();
		PixelShader->SetParameters(BatchedParameters, MyColor, MyTextureRHI, ShaderStructData);

		RHICmdList.SetStreamSource(0, GMyVertexBuffer.VertexBufferRHI, 0);
		RHICmdList.DrawPrimitive(0, 2, 1);

	}
	RHICmdList.EndRenderPass();
	RHICmdList.Transition(FRHITransitionInfo(RenderTargetTexture, ERHIAccess::RTV, ERHIAccess::SRVMask));
	//TransitionAndCopyTexture(RHICmdList, OutputRenderTargetResource->GetRenderTargetTexture(), OutputRenderTargetResource->TextureRHI, {});
}

void UGlobalTestShaderBlueprintLibrary::DrawGlobalTestShaderRenderTarget(class UTextureRenderTarget2D* OutputRenderTarget
	, AActor* Actor, FLinearColor MyColor, UTexture* MyTexture, FGlobalShaderStructData ShaderStructData)
{
	check(IsInGameThread());

	if (!OutputRenderTarget)
		return;

	if (!Actor)
		return;

	FTextureRenderTargetResource* TextureRenderTargetResource = OutputRenderTarget->GameThread_GetRenderTargetResource();
	FTextureReferenceRHIRef MyTextureRHI = MyTexture->TextureReference.TextureReferenceRHI;
	UWorld* World = Actor->GetWorld();
	ERHIFeatureLevel::Type FeatureLevel = World->Scene->GetFeatureLevel();
	FName TextureRenderTargetName = OutputRenderTarget->GetFName();
	ENQUEUE_RENDER_COMMAND(CaptureCommand)(
		[TextureRenderTargetResource, FeatureLevel, MyColor, TextureRenderTargetName, MyTextureRHI, ShaderStructData]
	(FRHICommandListImmediate& RHICmdList)
		{
			DrawTestShaderRenderTarget_RenderThread(RHICmdList, TextureRenderTargetResource
				, FeatureLevel, TextureRenderTargetName, MyColor, MyTextureRHI, ShaderStructData);
		}
	);
}