cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct VertexInputType
{
    float4 position : POSITION;
    float4 color : COLOR;
	float3 normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
	float3 normal : NORMAL;
};

PixelInputType main( VertexInputType input )
{
	PixelInputType output;

	/*Change the potision vector to be 4 units for proper matrix calculations*/
	input.position.w = 1.0f;

	output.position = mul( input.position, worldMatrix );
	output.position = mul( output.position, viewMatrix );
	output.position = mul( output.position, projectionMatrix );

	/*Store the input color for the pixel shader to use*/
	output.color = input.color;

	output.normal = mul( input.normal, worldMatrix );
    // Normalize the normal vector.
    output.normal = normalize( output.normal );

	return output;
}