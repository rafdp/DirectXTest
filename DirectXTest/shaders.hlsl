
cbuffer cbPerObject : register(b0)
{
	float4x4 WVP;
	float4x4 World;
};

cbuffer testBuffer : register(b8)
{
	float4 Color;
	float4 Position;
	float4 Direction;
	float  Min;
}

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 worldPos: POSITION;
	float4 color : COLOR;
};

VS_OUTPUT VShader (float4 inPos : POSITION, float4 inColor : COLOR)
{
	VS_OUTPUT output;
	output.worldPos = mul (inPos, World);

	float3 m0m1 = Position - output.worldPos;

	float d = length (cross (m0m1, Direction.xyz)) / length (Direction.xyz);

	if (d < Min) output.color = Color * (1.0f - d / Min);
	else output.color = inColor * 0.5f;

	output.position = mul (inPos, WVP);
	//output.color = inColor;

	return output;
}

float4 PShader (VS_OUTPUT input) : SV_TARGET
{
	return input.color;
}