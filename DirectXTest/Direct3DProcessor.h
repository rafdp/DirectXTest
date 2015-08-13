#pragma once

#include "includes.h"

class WindowClass;

class Direct3DProcessor : NZA_t
{
	IDXGISwapChain*			  swapChain_;
	ID3D11Device*			  device_;
	ID3D11DeviceContext*      deviceContext_;
	WindowClass*              wnd_;

	uint8_t                   nBuffers_;
	ID3D11RenderTargetView*   currentBuffer_;
	ID3D11Texture2D*		  depthBuffer_;
	ID3D11DepthStencilView*   depthStencilView_;

	std::vector<ID3D11DepthStencilState*> depthStencilStates_;
	std::vector<ID3D11RasterizerState*>   rasterizerStates_;

	std::vector<Direct3DObject*> objects_;
	Direct3DShaderManager        shaderManager_;
	ShaderDesc_t				 currentVertexShader_;
	ShaderDesc_t                 currentPixelShader_;

	void InitDeviceAndSwapChain ();
	void InitViewport ();
	void InitDepthStencilView ();
	void EnableShader (ShaderDesc_t desc);
	

public:
	void ok ();

	Direct3DProcessor (WindowClass* wnd, uint8_t buffers = 1);
	~Direct3DProcessor ();

	void ProcessDrawing (Direct3DCamera* cam);
	void Present ();

	UINT AddDepthStencilState (bool enableDepth = true, 
							   bool enableStencil = false);
	void ApplyDepthStencilState (UINT n);

	UINT AddRasterizerState   (bool clockwise = true, 
							   bool wireframe = false, 
							   bool cullNone  = false);
	void ApplyRasterizerState (UINT n);

	void RegisterObject (Direct3DObject* obj);
	std::vector<Direct3DObject*>& GetObjectsVector ();

	void ProcessObjects ();

	ShaderDesc_t LoadShader (std::string filename, 
							 std::string function,
							 SHADER_TYPES shaderType);

	ID3D11VertexShader* GetVertexShader (ShaderDesc_t desc);
	ID3D11PixelShader*  GetPixelShader  (ShaderDesc_t desc);
};