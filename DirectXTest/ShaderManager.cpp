#include "Builder.h"

bool ShaderDesc_t::operator == (const ShaderDesc_t& that)
{
	if (type != that.type)           return false;
	if (blobIndex != that.blobIndex) return false;
	if (index != that.index)         return false;
	return true;
}


void Direct3DShaderManager::ok ()
{
	DEFAULT_OK_BLOCK
}

Direct3DShaderManager::Direct3DShaderManager ()
try :
	blobs_ (),
	vertexShaders_ (),
	pixelShaders_ (),
	loaded_ ()
{

}
_END_EXCEPTION_HANDLING (CTOR)

Direct3DShaderManager::~Direct3DShaderManager ()
{
	for (auto i = blobs_.begin ();
			  i < blobs_.end ();
			  i++)
		(*i)->Release ();

	blobs_.clear ();

	for (auto i = vertexShaders_.begin ();
			  i < vertexShaders_.end ();
			  i++)
		(*i)->Release ();

	vertexShaders_.clear ();

	for (auto i = pixelShaders_.begin ();
			  i < pixelShaders_.end ();
			  i++)
		(*i)->Release ();

	pixelShaders_.clear ();

	loaded_.clear ();
}

ShaderDesc_t Direct3DShaderManager::LoadShader (std::string filename,
												std::string function,
												SHADER_TYPES shaderType,
												ID3D11Device* device)
{
	BEGIN_EXCEPTION_HANDLING
	std::string storing (filename);
	storing += ':';
	storing += function;
	auto found = loaded_.find (storing);
	if (found != loaded_.end ()) return found->second;

	HRESULT result = S_OK;

	blobs_.push_back (nullptr);
	UINT nBlob = blobs_.size () - 1;

	result = D3DX11CompileFromFileA (filename.c_str (), 
									 0, 
									 0, 
									 function.c_str (), 
									 "vs_4_0", 
									 0, 
									 0, 
									 0, 
									 &blobs_[nBlob], 
									 0, 
									 0);

	if (result != S_OK)
		_EXC_N (LOAD_SHADER_BLOB, "D3D: Failed to load shader (%s) from file (%s)" _ 
				function.c_str () _ 
				filename.c_str ())

	if (shaderType == SHADER_VERTEX)
	{
		vertexShaders_.push_back (nullptr);
		device->CreateVertexShader (blobs_[nBlob]->GetBufferPointer (), 
									blobs_[nBlob]->GetBufferSize (), 
									NULL, 
									&vertexShaders_.back ());
		return {SHADER_VERTEX, nBlob, static_cast <UINT> (vertexShaders_.size () - 1)};

	}

	if (shaderType == SHADER_PIXEL)
	{
		pixelShaders_.push_back (nullptr);
		device->CreatePixelShader (blobs_[nBlob]->GetBufferPointer (),
								   blobs_[nBlob]->GetBufferSize (),
								   NULL,
								   &pixelShaders_.back ());
		return { SHADER_PIXEL, nBlob, static_cast <UINT> (pixelShaders_.size () - 1) };

	}
	END_EXCEPTION_HANDLING (LOAD_SHADER)
}

void** Direct3DShaderManager::GetShader (ShaderDesc_t desc)
{
	BEGIN_EXCEPTION_HANDLING
	if (desc.type != SHADER_VERTEX && desc.type != SHADER_PIXEL)
		_EXC_N (SHADER_TYPE, "D3D: Invalid shader type")


	if (desc.blobIndex >= blobs_.size () || 
		blobs_[desc.blobIndex] == nullptr)
		_EXC_N (BLOB_INDEX, "D3D: Invalid blob index")

	if (desc.type == SHADER_VERTEX)
	{
		if (desc.index >= vertexShaders_.size () ||
			vertexShaders_[desc.index] == nullptr)
			_EXC_N (VERTEX_SHADER_INDEX, "D3D: Invalid vertex shader index")

		return reinterpret_cast<void**> (vertexShaders_[desc.index]);
	}

	if (desc.type == SHADER_PIXEL)
	{
		if (desc.index >= pixelShaders_.size () ||
			pixelShaders_[desc.index] == nullptr)
			_EXC_N (PIXEL_SHADER_INDEX, "D3D: Invalid pixel shader index")

			return reinterpret_cast<void**> (pixelShaders_[desc.index]);
	}

	END_EXCEPTION_HANDLING (GET_VERTEX_SHADER)
}


ID3D10Blob* Direct3DShaderManager::GetBlob (ShaderDesc_t desc)
{
	BEGIN_EXCEPTION_HANDLING

	if (desc.blobIndex >= blobs_.size () ||
		blobs_[desc.blobIndex] == nullptr)
		_EXC_N (BLOB_INDEX, "D3D: Invalid blob index")

		return blobs_[desc.blobIndex];

	END_EXCEPTION_HANDLING (GET_BLOB)
}