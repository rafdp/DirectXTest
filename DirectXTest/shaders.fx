
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
	float4 Color;
	float4 Position;
	float4 Direction;
	float  Range;
	float  Pow;
	float  Scale;
	float RayOnly;
}

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 worldPos: POSITION;
	float4 color : COLOR;
};

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float4 texCoord : TEXCOORD;
	float4 color : COLOR;
};


Texture2D ObjTexture : register (t7);
SamplerState ObjSamplerState : register (s7);


[maxvertexcount (4)]
void GShader (point VS_OUTPUT input[1],
			  inout TriangleStream<PS_INPUT> OutputStream)
{
	
	float3 normal = normalize (CamPos.xyz - input[0].worldPos.xyz);
	float zPerpendicular = -normal.y / normal.z;
	float3 upAxis = normalize (float3 (0.0f, 1.0f, zPerpendicular));
	float3 rightAxis = normalize (cross (normal, upAxis));
	
	PS_INPUT outputVert[4];
	
		
	
	float3 m0m1 = Position.xyz - input[0].worldPos.xyz;
	float d = length (cross (m0m1, Direction.xyz)) / length (Direction.xyz);
	//OutputStream.Append (input[0]);

	if (RayOnly < 0.5f && d > Range)
	{
		for (uint i0 = 0; i0 < 4; i0++)
		{
			
			outputVert[i0].position = mul (float4(input[0].worldPos.xyz -
												 ((i0 < 2 ? (+1) : (-1)) * upAxis +
												  ((i0 == 1 || i0 == 3) ? (+1) : (-1)) * rightAxis) * 0.01f * Scale, 1.0f),
										  VP);
			outputVert[i0].texCoord = float4 ((i0 == 1 || i0 == 3) ? 0.0f : 1.0f,
											  (i0 < 2) ? 0.0f : 1.0f, 
											  0.0f, 
											  0.0f);
			outputVert[i0].color = input[0].color;
			OutputStream.Append (outputVert[i0]);
		}
	}
	
	if (RayOnly > 0.5f && d < Range)
	{
		float tempCos = cos (d / Range);
		tempCos = pow (tempCos, Pow);
		if (tempCos < 0.5f) return;
		float4 colorEnd = Color * tempCos + input[0].color * (1 - tempCos);

		for (uint i1 = 0; i1 < 4; i1++)
		{
			outputVert[i1].position = mul (float4(input[0].worldPos.xyz -
												 ((i1 < 2 ? (+1) : (-1)) * upAxis +
												 ((i1 == 1 || i1 == 3) ? (+1) : (-1)) * rightAxis) * 0.015f * Scale, 1.0f),
											 VP);
			outputVert[i1].texCoord = float4 ((i1 == 1 || i1 == 3) ? 0.0f : 1.0f,
											  (i1 < 2) ? 0.0f : 1.0f,
											  0.0f,
											  0.0f);
			outputVert[i1].color = colorEnd;
			OutputStream.Append (outputVert[i1]);
		}
		OutputStream.RestartStrip ();
		
		float3 shift = float3 (Scale*0.02f*(sin (1024.0f * input[0].worldPos.x) / 2.0f + 0.5f),
							   Scale*0.02f*(cos (1124.0f * input[0].worldPos.y) / 2.0f + 0.5f),
							   Scale*0.02f*(sin (1224.0f * input[0].worldPos.z) / 2.0f + 0.5f));
		for (uint i2 = 0; i2 < 4; i2++)
		{
			outputVert[i2].position = mul (float4(input[0].worldPos.xyz + shift -
												  ((i2 < 2 ? (+1) : (-1)) * upAxis +
												  ((i2 == 0 || i2 == 3) ? (+1) : (-1)) * rightAxis) * 0.015f * Scale, 1.0f),
												VP);
			OutputStream.Append (outputVert[i2]);
		}
		OutputStream.RestartStrip ();

		for (uint i3 = 0; i3 < 4; i3++)
		{
			outputVert[i3].position = mul (float4(input[0].worldPos.xyz - shift -
												 ((i3 < 2 ? (+1) : (-1)) * upAxis +
												 ((i3 == 0 || i3 == 3) ? (+1) : (-1)) * rightAxis) * 0.015f * Scale, 1.0f),
												VP);
			OutputStream.Append (outputVert[i3]);
		}
		OutputStream.RestartStrip ();
	}
}

float4 main () : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
/*
VS_OUTPUT output;
output.worldPos = mul (inPos, World);

float3 m0m1 = Position - output.worldPos;

float d = length (cross (m0m1, Direction.xyz)) / length (Direction.xyz);

float tempCos = cos (d / Min);

if (d < Min) output.color = Color * pow (tempCos, Pow);
else output.color = inColor * 0.5f;

output.position = mul (inPos, WVP);
//output.color = inColor;

return output;*/

VS_OUTPUT VShader (float4 inPos : POSITION, float4 inColor : COLOR)
{
	VS_OUTPUT output;
	output.worldPos = mul (inPos, World);
	output.position = inPos;
	output.color = inColor;

	return output;
}

float4 PShader (PS_INPUT input) : SV_TARGET
{
	float4 diffuse = ObjTexture.Sample (ObjSamplerState, 
										input.texCoord.xy);
	clip (diffuse.a - .2);

	float3 finalColor = diffuse.xyz * input.color.xyz;
	return float4 (finalColor, input.color.a * diffuse.a);
}

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