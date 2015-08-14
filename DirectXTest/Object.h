#pragma once

#include "includes.h"

class Direct3DCamera : NZA_t
{
	WindowClass* wnd_;
	XMVECTOR position_;
	XMVECTOR target_;
	XMVECTOR up_;
	XMFLOAT3 projectionSettings_;

	XMMATRIX view_;
	XMMATRIX projection_;

public:
	void ok ();

	Direct3DCamera (WindowClass* wnd,
					float posX, float posY, float posZ,
					float tgtX, float tgtY, float tgtZ,
					float upX, float upY, float upZ,
					float fov = 0.3f, float zNear = 1.0f, float zFar = 1000.0f);

	void Update ();

	const XMMATRIX& GetView ();
	const XMMATRIX& GetProjection ();
};
struct Vertex_t
{
	float x, y, z;
	//float nx, ny, nz;
	//float u, v, w;
	float r, g, b, a;
	

	void SetPos (float x_, 
				 float y_,
				 float z_);

	/*void SetNormal (float nx_,
				    float ny_,
				    float nz_);

	void SetTexture (float u_,
					 float v_,
					 float w_ = 0.0f);*/

	void SetColor (float r_,
				   float g_,
				   float b_,
				   float a_ = 0.0f);
};
struct Direct3DObjectBuffer
{
	XMMATRIX WVP;
	XMMATRIX World;
};

__declspec (align (16))
struct CurrentMatrices
{
	Direct3DObjectBuffer objData_;
	XMMATRIX world_;
};

__declspec (align (16))
class Direct3DObject : NZA_t
{
	bool drawIndexed_;
	std::vector<Vertex_t> vertices_;
	std::vector<UINT> indices_;
	CurrentMatrices currM_;
	D3D11_PRIMITIVE_TOPOLOGY topology_;
	bool blending_;
	UINT objectId_;

	ID3D11Buffer* vertexBuffer_;
	ID3D11Buffer* indexBuffer_;
	Direct3DConstantBufferManager* cbManager_;
	UINT objectBufferN_;
	bool buffersSet_;
	bool objectBufferSet_;

	ShaderDesc_t vertexShader_;
	ShaderDesc_t pixelShader_;
	UINT layoutN_;

	friend class Direct3DProcessor;

	void SetID (UINT id);

	void ok ();

	void SaveLayout (UINT n);

	void SetCBManager (Direct3DConstantBufferManager* cbManager);
	
public:
	Direct3DObject (XMMATRIX& world, 
					bool blending = false,
					bool drawIndexed = false,
					D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	~Direct3DObject ();

	void AddVertexArray (Vertex_t* vert, 
						 size_t n);

	void AddIndexArray (UINT* ind,
						size_t n);

	void SetupBuffers (ID3D11Device* device);

	void SetWVP (XMMATRIX& matrix);
	void SetWorld (XMMATRIX& matrix);
	XMMATRIX& GetWorld ();

	void Draw (ID3D11DeviceContext* deviceContext,
			   Direct3DCamera* cam);

	void AttachVertexShader (ShaderDesc_t desc);
	void AttachPixelShader  (ShaderDesc_t desc);

	void ClearBuffers ();
	void SetVertexBuffer (ID3D11Device* device);
	void SetIndexBuffer  (ID3D11Device* device);
	void SetObjectBuffer (ID3D11Device* device);
};


void* GetValidObjectPtr ();