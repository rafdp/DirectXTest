
#include "Builder.h"

void Direct3DProcessor::ok ()
{
	DEFAULT_OK_BLOCK
	if (swapChain_ == nullptr)
		_EXC_N (NULL_SWAP_CHAIN, "D3D: Null swap chain");
	if (device_ == nullptr)
		_EXC_N (NULL_DEVICE, "D3D: Null device");
	if (deviceContext_ == nullptr)
		_EXC_N (NULL_DEVICE_CONTEXT, "D3D: Null device context");
	if (wnd_ == nullptr)
		_EXC_N (NULL_WND, "D3D: Null window pointer");

}

Direct3DProcessor::Direct3DProcessor (WindowClass* wnd, uint8_t buffers)
try :
	swapChain_	         (nullptr),
	device_              (nullptr),
	deviceContext_       (nullptr),
	wnd_                 (wnd),
	nBuffers_            (buffers),
	currentBuffer_	     (nullptr),
	depthBuffer_         (nullptr),
	depthStencilView_    (nullptr),
	depthStencilStates_  (),
	rasterizerStates_    (),
	layouts_             (),
	objects_             (),
	shaderManager_       (),
	currentVertexShader_ ({ SHADER_NOT_SET }),
	currentPixelShader_  ({ SHADER_NOT_SET })
{
	if (wnd == nullptr)
		_EXC_N (NULL_WND,
				"D3D: Cannot create environment over a null window");
	InitDeviceAndSwapChain ();
	InitViewport ();
	InitDepthStencilView ();
	ApplyDepthStencilState (AddDepthStencilState ());
	ApplyRasterizerState   (AddRasterizerState   (true));
	AddRasterizerState   (false);



}
_END_EXCEPTION_HANDLING (CTOR)

Direct3DProcessor::~Direct3DProcessor ()
{
#define free(var) var->Release (); var = nullptr
		
		free (swapChain_);
		free (device_);
		free (deviceContext_);

		free (currentBuffer_);
		free (depthBuffer_);
		free (depthStencilView_);

		for (auto i = depthStencilStates_.begin ();
			      i < depthStencilStates_.end ();
			      i++)
			(*i)->Release ();

		for (auto i = rasterizerStates_.begin ();
			      i < rasterizerStates_.end ();
				  i++)
				 (*i)->Release ();

		for (auto i = objects_.begin ();
				  i < objects_.end ();
			      i++)
			delete *i;

		depthStencilStates_.clear ();
		rasterizerStates_.clear ();
		objects_.clear ();

#undef free
}

void Direct3DProcessor::ProcessDrawing (Direct3DCamera* cam)
{
	BEGIN_EXCEPTION_HANDLING
	deviceContext_->ClearRenderTargetView (currentBuffer_,
										   D3DXCOLOR (0.0f, 
													  0.1f, 
													  0.2f, 
													  0.0f));

	deviceContext_->ClearDepthStencilView (depthStencilView_, 
										   D3D11_CLEAR_DEPTH | 
										   D3D11_CLEAR_STENCIL, 
										   1.0f, 
										   0);

	for (auto i = objects_.begin ();
			  i < objects_.end ();
			  i++)
	{
		EnableShader ((*i)->vertexShader_);
		EnableShader ((*i)->pixelShader_);
		(*i)->Draw (deviceContext_, cam);
	}
	END_EXCEPTION_HANDLING (PROCESS_DRAWING)
}


void Direct3DProcessor::Present ()
{
	BEGIN_EXCEPTION_HANDLING
	HRESULT result = S_OK;
	result = swapChain_->Present (0, 0);
	if (result != S_OK)
		_EXC_N (SWAP_PRESENT,
				"D3D: Swap chain present failed (0x%x)" _
				result);
	END_EXCEPTION_HANDLING (PRESENT)
}

UINT Direct3DProcessor::AddDepthStencilState (bool enableDepth, 
											  bool enableStencil)
{
	BEGIN_EXCEPTION_HANDLING
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	ID3D11DepthStencilState* newDepthStencilState = nullptr;

	depthStencilDesc.DepthEnable = enableDepth;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = enableStencil;
	if (enableStencil)
	{
		depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	}
	
	HRESULT result = S_OK;
	result = device_->CreateDepthStencilState (&depthStencilDesc, &newDepthStencilState);
	if (result != S_OK)
		_EXC_N (CREATE_DEPTH_STENCIL_STATE,
				"D3D: Failed to create depth stencil state (0x%x)" _
				result);
	depthStencilStates_.push_back (newDepthStencilState);
	return depthStencilStates_.size () - 1;
	END_EXCEPTION_HANDLING (ADD_DEPTH_STENCIL_STATE)
}

void Direct3DProcessor::ApplyDepthStencilState (UINT n)
{
	BEGIN_EXCEPTION_HANDLING
	if (n >= depthStencilStates_.size ())
		_EXC_N (OUT_OF_RANGE, "D3D: Cannot apply depth stencil state (%d of %d available)" _
				n _
				depthStencilStates_.size ());
	deviceContext_->OMSetDepthStencilState (depthStencilStates_[n], 0);
	END_EXCEPTION_HANDLING (APPLY_DEPTH_STENCIL_STATE)
}

void Direct3DProcessor::InitDeviceAndSwapChain ()
{
	try {

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};

	swapChainDesc.BufferDesc.Width = wnd_->width ();
	swapChainDesc.BufferDesc.Height = wnd_->height ();
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	if (nBuffers_ == 0) nBuffers_ = 1;
	if (nBuffers_ > D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT)
		nBuffers_ = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT;
	swapChainDesc.BufferCount = nBuffers_;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = wnd_->hwnd ();
	swapChainDesc.SampleDesc = SAMPLE_DESC;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	HRESULT result = S_OK;
	result = D3D11CreateDeviceAndSwapChain (NULL,
											D3D_DRIVER_TYPE_HARDWARE,
											NULL,
											NULL,
											NULL,
											NULL,
											D3D11_SDK_VERSION,
											&swapChainDesc,
											&swapChain_,
											&device_,
											NULL,
											&deviceContext_);
	
	if (result != S_OK)
		_EXC_N (DEVICE_SWAP_CHAIN,
				"D3D: Failed to create device and swap chain (0x%x)" _ result)

	ID3D11Texture2D* tempBufferPtr = nullptr;

	result = swapChain_->GetBuffer (0,
									_uuidof (ID3D11Texture2D),
									(void**)&tempBufferPtr);


	if (result != S_OK)
		_EXC_N (DEVICE_SWAP_CHAIN_GET_BUFFER,
				"D3D: Failed to get buffer from swap chain (0x%x)" _
				result);

	result = device_->CreateRenderTargetView (tempBufferPtr,
											  nullptr,
											  &currentBuffer_);
	if (result != S_OK)
		_EXC_N (CREATE_RENDER_TARGET,
				"D3D: Failed to create render target from swap chain buffer (0x%x)" _
				result);

	tempBufferPtr->Release ();

	END_EXCEPTION_HANDLING (INIT_DEVICE_AND_SWAPCHAIN)
}

void Direct3DProcessor::InitViewport ()
{
	BEGIN_EXCEPTION_HANDLING

	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = wnd_->width ();
	viewport.Height = wnd_->height ();
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	deviceContext_->RSSetViewports (1, &viewport);

	END_EXCEPTION_HANDLING (INIT_VIEWPORT)
}

void Direct3DProcessor::InitDepthStencilView ()
{
	BEGIN_EXCEPTION_HANDLING

	D3D11_TEXTURE2D_DESC depthBufferDesc = {};

	depthBufferDesc.Width = wnd_->width ();
	depthBufferDesc.Height = wnd_->height ();
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	depthBufferDesc.SampleDesc = SAMPLE_DESC;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	HRESULT result = S_OK;

	result = device_->CreateTexture2D (&depthBufferDesc,
									   NULL,
									   &depthBuffer_);
	if (result != S_OK)
		_EXC_N (CREATE_TEXTURE,
				"D3D: Failed to create texture for depth stencil buffer (0x%x)" _
				result);

	result = device_->CreateDepthStencilView (depthBuffer_,
											  NULL,
											  &depthStencilView_);

	if (result != S_OK)
		_EXC_N (CREATE_DEPTH_STENCIL_VIEW,
				"D3D: Failed to create depth stencil view (0x%x)" _
				result);

	deviceContext_->OMSetRenderTargets (1,
										&currentBuffer_,
										depthStencilView_);

	END_EXCEPTION_HANDLING (INIT_DEPTH_STENCIL_VIEW)
}

UINT Direct3DProcessor::AddRasterizerState (bool clockwise, bool wireframe, bool cullNone)
{
	BEGIN_EXCEPTION_HANDLING
	
	D3D11_RASTERIZER_DESC rasterDesc = {};

	rasterDesc.FillMode = wireframe ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
	rasterDesc.CullMode = cullNone ? D3D11_CULL_NONE : D3D11_CULL_BACK;
	rasterDesc.FrontCounterClockwise = !clockwise;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0;
	rasterDesc.SlopeScaledDepthBias = 0;

	rasterDesc.DepthClipEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.MultisampleEnable = true;
	rasterDesc.AntialiasedLineEnable = false;

	HRESULT result = S_OK;


	ID3D11RasterizerState* newRasterState = nullptr;
	result = device_->CreateRasterizerState (&rasterDesc, &newRasterState);
	if (result != S_OK)
		_EXC_N (CREATE_RASTERIZER_STATE,
				"D3D: Failed to create rasterizer state (0x%x)" _
				result);

	rasterizerStates_.push_back (newRasterState);

	return rasterizerStates_.size () - 1;

	
	END_EXCEPTION_HANDLING (ADD_RASTERIZER_STATE)
}

void Direct3DProcessor::ApplyRasterizerState (UINT n)
{
	BEGIN_EXCEPTION_HANDLING
	if (n >= rasterizerStates_.size ())
		_EXC_N (OUT_OF_RANGE, "D3D: Cannot apply rasterizer state (%d of %d available)" _
				n _
				rasterizerStates_.size ());
	deviceContext_->RSSetState (rasterizerStates_[n]);
	END_EXCEPTION_HANDLING (APPLY_DEPTH_STENCIL_STATE)
}


void Direct3DProcessor::RegisterObject (Direct3DObject* obj)
{
	BEGIN_EXCEPTION_HANDLING

	obj->SetID (objects_.size ());
	objects_.push_back (obj);

	END_EXCEPTION_HANDLING (REGISTER_OBJECT)
}

std::vector<Direct3DObject*>& Direct3DProcessor::GetObjectsVector ()
{
	return objects_;
}

void Direct3DProcessor::ProcessObjects ()
{
	BEGIN_EXCEPTION_HANDLING
	for (auto i = objects_.begin ();
			  i < objects_.end ();
		      i++)
		(*i)->SetupBuffers (device_);
	END_EXCEPTION_HANDLING (PROCESS_OBJECTS)
}

ShaderDesc_t Direct3DProcessor::LoadShader (std::string filename,
											std::string function,
											SHADER_TYPES shaderType)
{
	return shaderManager_.LoadShader (filename, 
									  function, 
									  shaderType, 
									  device_);
}

ID3D11VertexShader* Direct3DProcessor::GetVertexShader (ShaderDesc_t desc)
{
	return reinterpret_cast<ID3D11VertexShader*> (shaderManager_.GetShader (desc));
}

ID3D11PixelShader* Direct3DProcessor::GetPixelShader (ShaderDesc_t desc)
{
	return reinterpret_cast<ID3D11PixelShader*> (shaderManager_.GetShader (desc));
}

void Direct3DProcessor::EnableShader (ShaderDesc_t desc)
{
	BEGIN_EXCEPTION_HANDLING
	if (desc.type != SHADER_VERTEX && desc.type != SHADER_PIXEL)
		_EXC_N (SHADER_TYPE, "D3D: Invalid shader type")

	if (desc.type == SHADER_VERTEX)
	{
		if (desc == currentVertexShader_) return;
		currentVertexShader_ = desc;
		deviceContext_->VSSetShader (GetVertexShader (desc), 0, 0);
		return;
	}

	if (desc.type == SHADER_PIXEL)
	{
		if (desc == currentPixelShader_) return;
		currentPixelShader_ = desc;
		deviceContext_->PSSetShader (GetPixelShader (desc), 0, 0);
		return;
	}
	END_EXCEPTION_HANDLING (ENABLE_SHADER)
}

UINT Direct3DProcessor::AddLayout (ShaderDesc_t desc,
								   bool position,
								   bool normal,
 								   bool texture,
	 							   bool color)
{
	BEGIN_EXCEPTION_HANDLING

	if (desc.type != SHADER_VERTEX)
		_EXC_N (SHADER_TYPE, "D3D: Unable to create layout, invalid shader type")

	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements;
	if (position)
		inputElements.push_back ({ "POSITION", 
								   0, 
								   DXGI_FORMAT_R32G32B32_FLOAT, 
								   0, 
								   offsetof (Vertex_t, x), 
								   D3D11_INPUT_PER_VERTEX_DATA, 
								   0 });

	if (normal)
		inputElements.push_back ({ "NORMAL",
								   0,
								   DXGI_FORMAT_R32G32B32_FLOAT,
								   0,
								   offsetof (Vertex_t, nx),
								   D3D11_INPUT_PER_VERTEX_DATA,
								   0 });
	
	if (texture & !color)
		inputElements.push_back ({ "TEXCOORD",
								   0,
								   DXGI_FORMAT_R32G32_FLOAT,
								   0,
								   offsetof (Vertex_t, u),
								   D3D11_INPUT_PER_VERTEX_DATA,
								   0 });
	if (!texture & color)
		inputElements.push_back ({ "COLOR",
								   0,
								   DXGI_FORMAT_R32G32B32A32_FLOAT,
								   0,
								   offsetof (Vertex_t, r),
								   D3D11_INPUT_PER_VERTEX_DATA,
								   0 });
	HRESULT result = S_OK;
	layouts_.push_back (nullptr);

	result = device_->CreateInputLayout (inputElements.data (),
										 inputElements.size (),
										 shaderManager_.GetBlob (desc)->GetBufferPointer (),
										 shaderManager_.GetBlob (desc)->GetBufferSize (),
										 &layouts_.back ());

	if (result != S_OK)
		_EXC_N (CREATE_LAYOUT, "D3D: Failed to create layout (0x%x)" _
				result)
	return layouts_.size () - 1;
	END_EXCEPTION_HANDLING (ADD_LAYOUT)
}

void Direct3DProcessor::EnableLayout (UINT n)
{
	BEGIN_EXCEPTION_HANDLING

	if (n >= layouts_.size ())
		_EXC_N (LAYOUT_INDEX, "D3D: Invalid layout index (%d of %d available)" _
				n _
				layouts_.size ())

	deviceContext_->IASetInputLayout (layouts_[n]);

	END_EXCEPTION_HANDLING (ENABLE_LAYOUT)
}


void Direct3DProcessor::SetLayout (Direct3DObject* obj, UINT n)
{
	BEGIN_EXCEPTION_HANDLING

	if (obj == nullptr)
		_EXC_N (NULL_OBJECT, "D3D: Cannot set layout to null object");

	if (n >= layouts_.size () || layouts_[n] == nullptr)
		_EXC_N (OUT_OF_RANGE, "D3D: Cannot set layout (%d of %d available) to object (obj %d)" _
				n _
				layouts_.size () _ 
				obj->objectId_);
	obj->SaveLayout (n);
	END_EXCEPTION_HANDLING (SET_LAYOUT)
}
