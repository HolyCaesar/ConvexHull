struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
	float3 normal : NORMAL;
};

cbuffer LightBuffer
{
    float4 diffuseColor;
    float3 lightDirection;
	float padding;
};

float4 main( PixelInputType input ) : SV_TARGET
{
	float4 color = input.color;
	float3 lightDir;
	float lightIntensity;

	// Invert the light direction for calculations.
    lightDir = -lightDirection;

    // Calculate the amount of light on this pixel.
    lightIntensity = saturate( dot( input.normal, lightDir ) );

    // Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
    color = saturate( diffuseColor * lightIntensity );
	color = color * input.color;
	//return color;
	return color;
}