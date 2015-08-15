
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
	float4 worldPos: POSITION;
	float4 color : COLOR;
};

[maxvertexcount (3)]
void GShader (point VS_OUTPUT input[1],
			  inout TriangleStream<VS_OUTPUT> OutputStream)
{
	
	float3 normal = normalize (CamPos.xyz - input[0].worldPos.xyz);
	float zPerpendicular = -(normal.x * normal.x + 
							 normal.y * (normal.y + 1.0f)) / 
							 normal.z;
	float3 upAxis = normalize (float3 (input[0].worldPos.x, 
									   input[0].worldPos.y + 1.0f, 
									   input[0].worldPos.z + zPerpendicular));
	float3 rightAxis = normalize (cross (normal, upAxis));
	
	VS_OUTPUT outputVert[3];
	outputVert[0].worldPos = float4(input[0].worldPos.xyz - (upAxis * 0.577f - rightAxis) * 0.01f * Scale, 1.0f);
	outputVert[1].worldPos = float4(input[0].worldPos.xyz - (upAxis * 0.577f + rightAxis) * 0.01f * Scale, 1.0f);
	outputVert[2].worldPos = float4(input[0].worldPos.xyz + (upAxis * 1.154f) * 0.01f * Scale, 1.0f);
	
	//outputVert[0].worldPos = float4(input[0].worldPos.xyz + float3 (0.05f, 0.0f, 0.0f), 1.0f);
	//outputVert[1].worldPos = float4(input[0].worldPos.xyz + float3 (0.0f, 0.05f, 0.0f), 1.0f);
	//outputVert[2].worldPos = float4(input[0].worldPos.xyz + float3 (0.0f, 0.0f, 0.05f), 1.0f);

	outputVert[0].position = mul (outputVert[0].worldPos, VP);
	outputVert[1].position = mul (outputVert[1].worldPos, VP);
	outputVert[2].position = mul (outputVert[2].worldPos, VP);
	/*
	float3 m0m1 = Position.xyz - input[0].worldPos.xyz;
	float d = length (cross (m0m1, Direction.xyz)) / length (Direction.xyz);
	if (d < Range)
	{
		float tempCos = cos (d / Range);
		tempCos = pow (tempCos, Pow);
		float4 colorEnd = Color * tempCos + input[0].color * (1 - tempCos);
		outputVert[0].color = colorEnd;
		outputVert[1].color = colorEnd;
		outputVert[2].color = colorEnd;

		VS_OUTPUT outputVertNew[3];

		float3 shift = float3 (sin (1024.0f * input[0].worldPos.x)/2.0f + 0.5f,
							   cos (1124.0f * input[0].worldPos.y) / 2.0f + 0.5f,
							   sin (1224.0f * input[0].worldPos.z) / 2.0f + 0.5f);
		
		outputVertNew[0].worldPos = float4(input[0].worldPos.xyz - (upAxis * 0.577f - rightAxis + shift) * 0.01f * Scale, 1.0f);
		outputVertNew[1].worldPos = float4(input[0].worldPos.xyz - (upAxis * 0.577f + rightAxis + shift) * 0.01f * Scale, 1.0f);
		outputVertNew[2].worldPos = float4(input[0].worldPos.xyz + (upAxis * 1.154f + shift) * 0.01f * Scale, 1.0f);

		outputVertNew[0].position = mul (outputVertNew[0].worldPos, VP);
		outputVertNew[1].position = mul (outputVertNew[1].worldPos, VP);
		outputVertNew[2].position = mul (outputVertNew[2].worldPos, VP);

		outputVertNew[0].color = colorEnd;
		outputVertNew[1].color = colorEnd;
		outputVertNew[2].color = colorEnd;

		OutputStream.Append (outputVertNew[0]);
		OutputStream.Append (outputVertNew[1]);
		OutputStream.Append (outputVertNew[2]);

	}
	else*/
	//{
		outputVert[0].color = Color;
		outputVert[1].color = Color;
		outputVert[2].color = Color;
	//}
	
	OutputStream.Append (outputVert[0]);
	OutputStream.Append (outputVert[1]);
	OutputStream.Append (outputVert[2]);
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

float4 PShader (VS_OUTPUT input) : SV_TARGET
{
	return input.color;
}

float rand (in float2 uv)
{
	float2 noise = (frac (sin (dot (uv, float2(12.9898, 78.233)*2.0)) * 43758.5453));
	return abs (noise.x + noise.y) * 0.5;
}

VS_OUTPUT VShaderCube (float4 inPos : POSITION, float4 inColor : COLOR)
{
	VS_OUTPUT output;
	output.worldPos = mul (inPos, World);
	output.position = mul (output.worldPos, VP);
	output.color = inColor;

	return output;
}
