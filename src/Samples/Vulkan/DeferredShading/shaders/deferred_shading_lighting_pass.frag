#pragma pack_matrix(row_major)

struct VertexData 
{
    float4 Position : SV_POSITION;
    float2 TextureCoordinate : TEXCOORD0;
}; 

struct FragmentData
{
    float4 Color : SV_TARGET0;
};

[[vk::input_attachment_index(0)]] SubpassInput<float4> gDiffuse;
[[vk::input_attachment_index(1)]] SubpassInput<float>  gDepth;

FragmentData main(VertexData input)
{
    FragmentData fragment;
  
    fragment.Color = gDiffuse.SubpassLoad();
    
    return fragment;
}