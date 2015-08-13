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

class Vertex_t
{
	float x, y, z;
	float nx, ny, nz;
	union
	{
		struct { float u, v, w; };
		struct { float r, g, b; };
	};
	

	void SetPos (float x_, 
				 float y_,
				 float z_);

	void SetNormal (float nx_,
				    float ny_,
				    float nz_);

	void SetTexture (float u_,
					 float v_,
					 float w_ = 0.0f);

	void SetColor (float r_,
				   float g_,
				   float b_);
};

struct Direct3DObjectBuffer
{
	XMMATRIX WVP, World;
};

class Direct3DObject : NZA_t
{
	std::vector<Vertex_t> vertices_;
	std::vector<UINT> indices_;
	D3D11_PRIMITIVE_TOPOLOGY topology_;
	bool drawIndexed_;
	UINT objectId_;
	float alpha_;
	Direct3DObjectBuffer objData_;
	XMMATRIX world_;

	ID3D11Buffer* vertexBuffer_;
	ID3D11Buffer* indexBuffer_;
	ID3D11Buffer* objectBuffer_;
	bool buffersSet_;

	ShaderDesc_t vertexShader_;
	ShaderDesc_t pixelShader_;

	friend class Direct3DProcessor;

	void SetID (UINT id);

	void ok ();
	
public:
	Direct3DObject (XMMATRIX& world,
					D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
					bool drawIndexed = false);
	~Direct3DObject ();

	void AddVertexArray (Vertex_t* vert, 
						 size_t n);

	void AddIndexArray (UINT* ind,
						size_t n);

	void SetupBuffers (ID3D11Device* device);

	void SetWVP (XMMATRIX& matrix);
	void SetWorld (XMMATRIX& matrix);
	XMMATRIX& GetWVP ();
	XMMATRIX& GetWorld ();

	void Draw (ID3D11DeviceContext* deviceContext,
			   Direct3DCamera* cam);

	void AttachVertexShader (ShaderDesc_t desc);
	void AttachPixelShader  (ShaderDesc_t desc);

};