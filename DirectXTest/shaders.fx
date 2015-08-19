
//#define ENHANCE_PERFORMANCE

cbuffer cbPerObject : register(b0)
{
	float4x4 VP;
	float4x4 World;
};

cbuffer Cam : register(b1)
{
	float4 CamPos;
}

cbuffer Ray : register(b2)
{
	float4 PointColor;
	float4 RayColor;
	float  CosPow;
	float  Scale;
	float  RayOnly;
	float  RayRange;
}

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 worldPos: POSITION;
	float4 color : COLOR;
};


VS_OUTPUT VShaderCube (float4 inPos : POSITION, float4 inColor : COLOR)
{
	VS_OUTPUT output;
	output.worldPos = mul (inPos, World);
	output.position = mul (output.worldPos, VP);
	output.color = inColor;

	return output;
}

float4 PShaderCube (VS_OUTPUT input) : SV_TARGET
{
	return input.color;
}


struct GSInput
{
	float4 worldPos : POSITION;
	float4 color : COLOR;
};

struct PSInput
{
	float4 position : SV_POSITION; 
#ifndef ENHANCE_PERFORMACE
	float4 texCoord : TEXCOORD;
#endif
	float4 color : COLOR;
};

GSInput ParticleSystemVShader (float4 inColor : COLOR)
{
	GSInput ret;
	ret.worldPos = mul (float4(inColor.xyz, 1.0f), World);
	ret.worldPos.w = Scale;
	if (RayOnly > 0.0f && inColor.a < -0.5f)
	{
		ret.color.r = -1.0f;
		return ret;
	}
	if (inColor.a > -0.5f)
	{
		float tempCos = cos (inColor.a / RayRange);
		tempCos = pow (tempCos, CosPow);
		if (tempCos < abs (RayOnly))
			ret.color.r = -1.0f;
		else
		{
			ret.worldPos.w *= -1.0f;
			ret.color = RayColor * tempCos + PointColor * (1 - tempCos);
		}
		return ret;
	}
	if (RayOnly < 0.0f)
	{
		ret.color = PointColor;
		return ret;
	}
	return ret;
}
//#ifndef ENHANCE_PERFORMACE
[maxvertexcount (12)]
//#else
//[maxvertexcount (9)]
//#endif
void ParticleSystemGShader (point GSInput input[1],
							inout TriangleStream<PSInput> OutputStream)
{
	if (input[0].color.r < -0.5f) return;

	float3 normal = normalize (CamPos.xyz - input[0].worldPos.xyz);
	float zPerpendicular = -normal.y / normal.z;
	float3 upAxis = normalize (float3 (0.0f, 1.0f, zPerpendicular));
	float3 rightAxis = normalize (cross (normal, upAxis));
	float scale = abs (input[0].worldPos.w);
	if (input[0].worldPos.w < -0.5f) scale *= 1.5;
	scale /= 100.0f;

	PSInput outputVert[4];

	uint size;
#ifdef ENHANCE_PERFORMACE
	size = 4;
#else
	size = 3;
#endif

	for (uint i1 = 0; i1 < size; i1++)
	{
		outputVert[i1].position = mul (float4(input[0].worldPos.xyz -
											  ((i1 < 2 ? (+1) : (-1)) * upAxis +
											  ((i1 == 1 || i1 == 3) ? (+1) : (-1)) * rightAxis) * scale, 1.0f),
									   VP);
#ifndef ENHANCE_PERFORMACE
		outputVert[i1].texCoord = float4 ((i1 == 1 || i1 == 3) ? 0.0f : 1.0f,
										  (i1 < 2) ? 0.0f : 1.0f,
										  0.0f,
										  0.0f);
#endif
		outputVert[i1].color = input[0].color;
		OutputStream.Append (outputVert[i1]);
	}
	if (input[0].worldPos.w > 0.0f) return;
	OutputStream.RestartStrip ();


	float3 shift = float3 (scale*0.5f*(sin (1024.0f * input[0].worldPos.x) / 2.0f + 0.5f),
						   scale*0.5f*(cos (1124.0f * input[0].worldPos.y) / 2.0f + 0.5f),
						   scale*0.5f*(sin (1224.0f * input[0].worldPos.z) / 2.0f + 0.5f));

	for (uint i2 = 0; i2 < size; i2++)
	{
		outputVert[i2].position = mul (float4(input[0].worldPos.xyz + shift -
											  ((i2 < 2 ? (+1) : (-1)) * upAxis +
											  ((i2 == 0 || i2 == 3) ? (+1) : (-1)) * rightAxis) * scale, 
											  1.0f),
									   VP);
		OutputStream.Append (outputVert[i2]);
	}
	OutputStream.RestartStrip ();

	for (uint i3 = 0; i3 < size; i3++)
	{
		outputVert[i3].position = mul (float4(input[0].worldPos.xyz - shift -
											  ((i3 < 2 ? (+1) : (-1)) * upAxis +
											  ((i3 == 0 || i3 == 3) ? (+1) : (-1)) * rightAxis) * scale, 
											  1.0f),
									   VP);
		OutputStream.Append (outputVert[i3]);
	}
}
#ifndef ENHANCE_PERFORMANCE
Texture2D ObjTexture : register (t7);
SamplerState ObjSamplerState : register (s7);

float4 ParticleSystemPShader (PSInput input) : SV_TARGET
{
	float4 diffuse = ObjTexture.Sample (ObjSamplerState, input.texCoord.xy);
	clip (diffuse.a - .2);
	float3 finalColor = diffuse.xyz * input.color.xyz;
	return float4 (finalColor, input.color.a * diffuse.a);
}
#else
float4 ParticleSystemPShader (PSInput input) : SV_TARGET
{ 
	return input.color;
}
#endif

float4 main () : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}