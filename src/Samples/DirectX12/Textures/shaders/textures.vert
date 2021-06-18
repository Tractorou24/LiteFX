#pragma pack_matrix(row_major)

struct VertexData 
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float2 TextureCoordinate : TEXCOORD0;
}; 

struct VertexInput
{
    float3 Position : POSITION;
    float4 Color : COLOR;
    float2 TextureCoordinate : TEXCOORD0;
};

struct CameraData
{
    float4x4 ViewProjection;
};

struct TransformData
{
    float4x4 Model;
};

ConstantBuffer<CameraData>    camera    : register(b0, space0);
ConstantBuffer<TransformData> transform : register(b0, space2);

VertexData main(in VertexInput input)
{
    VertexData vertex;
    
    float4 position = mul(float4(input.Position, 1.0), transform.Model);
    vertex.Position = mul(position, camera.ViewProjection);
    
    vertex.Color = input.Color;
    vertex.TextureCoordinate = input.TextureCoordinate;
 
    return vertex;
}