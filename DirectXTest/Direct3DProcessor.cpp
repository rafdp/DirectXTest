
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
}

Direct3DProcessor::Direct3DProcessor (WindowClass* wnd, uint8_t buffers)
try :
	swapChain_	   (nullptr),
	device_        (nullptr),
	deviceContext_ (nullptr),
	wnd_           (wnd),
	nBuffers_      (buffers),
	currentBuffer_ (nullptr)
{
	if (wnd == nullptr)
		_EXC_N (NULL_WND,
				"D3D: Cannot create environment over a null window")


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

	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = wnd_->width ();
	viewport.Height = wnd_->height ();
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	deviceContext_->RSSetViewports (1, &viewport);

}
_END_EXCEPTION_HANDLING (CTOR)

Direct3DProcessor::~Direct3DProcessor ()
{
	try
	{
		ok ();
#define free(var) var->Release (); var = nullptr
		
		free (swapChain_);
		free (device_);
		free (deviceContext_);

#undef free
	}
	_END_EXCEPTION_HANDLING (DTOR)
}

void Direct3DProcessor::ProcessDrawing ()
{
	BEGIN_EXCEPTION_HANDLING
	deviceContext_->ClearRenderTargetView (currentBuffer_,
										   D3DXCOLOR (0.0f, 0.1f, 0.2f, 0.5f));
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