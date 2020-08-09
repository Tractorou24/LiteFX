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

#ifdef SPIRV
[[vk::input_attachment_index(0)]] SubpassInput<float4> gDiffuse;
[[vk::input_attachment_index(1)]] SubpassInput<float>  gDepth;
#elif DXIL
#endif

FragmentData main(VertexData input)
{
    FragmentData fragment;
  
#ifdef SPIRV
    fragment.Color = gDiffuse.SubpassLoad();
#elif DXIL
    fragment.Color = float4(1.0, 1.0, 1.0, 1.0);
#endif  
    
    return fragment;
}