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
	

public:
	void ok ();

	Direct3DProcessor (WindowClass* wnd, uint8_t buffers = 1);
	~Direct3DProcessor ();

	void ProcessDrawing ();
	void Present ();
};