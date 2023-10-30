#pragma once
#include "ShaderCore.h"
#include "RenderResource.h"

struct FTextureVertex
{
	FVector4 Position;
	FVector2D UV;
};

class FGlobalVertexBuffer : public FVertexBuffer
{
	public:
		virtual void InitRHI(FRHICommandListBase& RHICmdList) override;

};

class FTextureVertexDeclaration : public FRenderResource
{
public:
	FVertexDeclarationRHIRef VertexDeclarationRHI;
	virtual void InitRHI(FRHICommandListBase& RHICmdList) override;
	virtual void ReleaseRHI() override;
};

class GlobalShaderDeclaration
{
};

