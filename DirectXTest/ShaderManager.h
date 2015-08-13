#pragma once

#include "includes.h"

enum SHADER_TYPES
{
	SHADER_VERTEX  = 1,
	SHADER_PIXEL   = 2,
	SHADER_NOT_SET = 3
};

struct ShaderDesc_t
{
	UINT type;
	UINT blobIndex;
	UINT index;

	bool operator == (const ShaderDesc_t& that);
};

class Direct3DShaderManager : NZA_t
{
	std::vector<ID3D10Blob*>		    blobs_;
	std::vector<ID3D11VertexShader*>    vertexShaders_;
	std::vector<ID3D11PixelShader*>     pixelShaders_;
	std::map<std::string, ShaderDesc_t> loaded_;

public:
	Direct3DShaderManager ();
	~Direct3DShaderManager ();

	void ok ();

	ShaderDesc_t LoadShader (std::string filename,
							 std::string function,
							 SHADER_TYPES shaderType,
							 ID3D11Device* device);

	void** GetShader (ShaderDesc_t desc);
	ID3D10Blob* GetBlob (ShaderDesc_t desc);
};