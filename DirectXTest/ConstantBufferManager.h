#pragma once

#include "includes.h"

struct BufferInfo_t
{
	ID3D11Buffer* buffer;
	void* data;
	UINT slot;
};

class Direct3DConstantBufferManager : NZA_t
{
	std::vector<BufferInfo_t> buffers_;
public:
	void ok ();
	Direct3DConstantBufferManager ();
	~Direct3DConstantBufferManager ();

	UINT Bind (void* data, 
			   size_t size, 
			   UINT slot,
			   ID3D11Device* device);

	void Update (UINT n, ID3D11DeviceContext* deviceContext);

	void SendVSBuffer (UINT n, ID3D11DeviceContext* deviceContext);

	void SendPSBuffer (UINT n, ID3D11DeviceContext* deviceContext);
};