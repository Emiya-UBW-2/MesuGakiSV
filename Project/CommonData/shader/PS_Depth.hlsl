// �s�N�Z���V�F�[�_�[�̓���
struct PS_INPUT
{
    float4 Position : SV_POSITION; // ���W
    float4 DiffuseColor : COLOR0; // �f�B�t���[�Y�J���[
    float4 SpecularColor : COLOR1; // �X�y�L�����J���[
    float2 TextureCoord0 : TEXCOORD0; // �e�N�X�`�����W�O
    float2 TextureCoord1 : TEXCOORD1; // �e�N�X�`�����W�P
};

// �s�N�Z���V�F�[�_�[�̏o��
struct PS_OUTPUT {
    float4 color0 : SV_TARGET0; // �F
};

// �萔�o�b�t�@�s�N�Z���V�F�[�_�[��{�p�����[�^
struct DX_D3D11_PS_CONST_BUFFER_BASE {
    float4 FactorColor; // �A���t�@�l��

    float MulAlphaColor; // �J���[�ɃA���t�@�l����Z���邩�ǂ���( 0.0f:��Z���Ȃ�  1.0f:��Z���� )
    float AlphaTestRef; // �A���t�@�e�X�g�Ŏg�p�����r�l
    float2 Padding1;

    int AlphaTestCmpMode; // �A���t�@�e�X�g��r���[�h( DX_CMP_NEVER �Ȃ� )
    int3 Padding2;

    float4 IgnoreTextureColor; // �e�N�X�`���J���[���������p�J���[
};

// ��{�p�����[�^
cbuffer cbD3D11_CONST_BUFFER_PS_BASE : register(b1) {
    DX_D3D11_PS_CONST_BUFFER_BASE g_Base;
};

// �v���O�����Ƃ̂����̂��߂Ɏg�����W�X�^1
cbuffer cbMULTIPLYCOLOR_CBUFFER1 : register(b2)
{
    float2 dispsize;
}

SamplerState g_Tex1Sampler : register(s0); // �f�B�t���[�Y�}�b�v�T���v��
Texture2D g_Tex1Texture : register(t0); // �f�B�t���[�Y�}�b�v�e�N�X�`��

SamplerState g_Tex2Sampler : register(s1); // �f�B�t���[�Y�}�b�v�T���v��
Texture2D g_Tex2Texture : register(t1); // �f�B�t���[�Y�}�b�v�e�N�X�`��

// main�֐�
PS_OUTPUT main(PS_INPUT PSInput)
{
    PS_OUTPUT PSOutput;
    float2 TexPos = PSInput.TextureCoord0;
    TexPos.y = 1.f - TexPos.y;

	// �e�N�X�`���J���[�̓ǂݍ���
    float4 Depth = g_Tex1Texture.Sample(g_Tex1Sampler, TexPos);
    float Tex1 = Depth.r;
    float Tex2 = g_Tex2Texture.Sample(g_Tex2Sampler, TexPos).r;

    PSOutput.color0 = g_Tex1Texture.Sample(g_Tex1Sampler, TexPos);
    
    if (Depth.r == 0.f)
    {
        PSOutput.color0 = float4(0, 0, 0, 0);
        return PSOutput;
    }
    
    float Diff = abs(Tex1 - Tex2);
    PSOutput.color0.rgb = float3(1, 1, 1);
    PSOutput.color0.a = (Diff > 12.5f * 1.5f) ? 1.f : 0.f;

    return PSOutput;
}

