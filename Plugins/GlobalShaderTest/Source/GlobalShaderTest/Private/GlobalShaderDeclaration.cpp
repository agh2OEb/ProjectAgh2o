#include "GlobalShaderDeclaration.h"


/*
FTextureVertexDeclaration
*/

void FTextureVertexDeclaration::InitRHI(FRHICommandListBase& RHICmdList)
{
	FVertexDeclarationElementList Elements;
	uint32 Stride = sizeof(FTextureVertex);

	Elements.Add(FVertexElement(0, STRUCT_OFFSET(FTextureVertex, Position), VET_Float4, 0, Stride));
	Elements.Add(FVertexElement(0, STRUCT_OFFSET(FTextureVertex, UV), VET_Float2, 1, Stride));

	VertexDeclarationRHI = RHICreateVertexDeclaration(Elements);
}

void FTextureVertexDeclaration::ReleaseRHI()
{
	VertexDeclarationRHI->Release();
}

/*
FGlobalVertexBuffer
*/

void FGlobalVertexBuffer::InitRHI(FRHICommandListBase& RHICmdList)
{
	// Static Vertex Buffer ����
	FRHIResourceCreateInfo CreateInfo(TEXT("GlobalVertex CreateInfo"));
	VertexBufferRHI = RHICmdList.CreateVertexBuffer(sizeof(FTextureVertex)*4, BUF_Static, CreateInfo);

	// ���ؽ����� �� 
	void* VoidPtr = RHILockBuffer(VertexBufferRHI, 0, sizeof(FTextureVertex)*4, RLM_WriteOnly);

	// ���� ���ý� ���� �����
	FTextureVertex* Vertices = reinterpret_cast<FTextureVertex*>(VoidPtr);

	Vertices[0].Position = FVector4(-1.0f, 1.0f, 0.0f, 1.0f);
	Vertices[1].Position = FVector4(1.0f, 1.0f, 0.0f, 1.0f);
	Vertices[2].Position = FVector4(-1.0f, -1.0f, 0.0f, 1.0f);
	Vertices[3].Position = FVector4(1.0f, -1.0f, 0.0f, 1.0f);
	Vertices[0].UV = FVector2D(0.0f, 1.0f);
	Vertices[1].UV = FVector2D(1.0f, 1.0f);
	Vertices[2].UV = FVector2D(0.0f, 0.0f);
	Vertices[3].UV = FVector2D(1.0f, 0.0f);

	// ���ؽ����� �� ����
	RHIUnlockBuffer(VertexBufferRHI);
}