// �s�N�Z���V�F�[�_�[�̓���
struct PS_INPUT
{
    float2 texCoords0 : TEXCOORD0; // �e�N�X�`�����W
};

// �s�N�Z���V�F�[�_�[�̏o��
struct PS_OUTPUT
{
    float4 color0 : SV_TARGET0; // �F
};


// �萔�o�b�t�@�s�N�Z���V�F�[�_�[��{�p�����[�^
struct DX_D3D11_PS_CONST_BUFFER_BASE
{
    float4 FactorColor; // �A���t�@�l��

    float MulAlphaColor; // �J���[�ɃA���t�@�l����Z���邩�ǂ���( 0.0f:��Z���Ȃ�  1.0f:��Z���� )
    float AlphaTestRef; // �A���t�@�e�X�g�Ŏg�p�����r�l
    float2 Padding1;

    int AlphaTestCmpMode; // �A���t�@�e�X�g��r���[�h( DX_CMP_NEVER �Ȃ� )
    int3 Padding2;

    float4 IgnoreTextureColor; // �e�N�X�`���J���[���������p�J���[
};

// ��{�p�����[�^
cbuffer cbD3D11_CONST_BUFFER_PS_BASE : register(b1)
{
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
	// �e�N�X�`���J���[�̓ǂݍ���
    float Tex1 = g_Tex1Texture.Sample(g_Tex1Sampler, PSInput.texCoords0).r;
    float Tex2 = g_Tex2Texture.Sample(g_Tex2Sampler, PSInput.texCoords0).r;
    
    float Diff = (Tex1 - Tex2) / 10.f;

	// �o�̓J���[ = �e�N�X�`���J���[ * �f�B�t���[�Y�J���[
    PSOutput.color0.rgb = float3(1, 1, 1);
    if (Tex2 > 0.f && 1.f > Diff && Diff > 0.f)
    {
        PSOutput.color0.a = 1.f - Diff;
   
    }
    else
    {
        PSOutput.color0.a = 0.f;
    }

	// �o�̓p�����[�^��Ԃ�
    return PSOutput;
}

