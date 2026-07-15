#include "VertexShader.hlsli"

Texture2D<float32_t4> g_texture : register(t0); //SRV
SamplerState g_sampler : register(s0);//sampler
struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float32_t2 uv = input.texcoord;
    float32_t4 textureColor = g_texture.Sample(g_sampler, uv);
    
    //grayscale 
    float32_t value = dot(textureColor.rgb, float32_t3(0.2125f, 0.7154f, 0.0721f));
    
    output.color = float32_t4(value, value, value, textureColor.a);
    return output;
}