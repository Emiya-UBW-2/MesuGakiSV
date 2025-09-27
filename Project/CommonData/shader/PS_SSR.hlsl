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
	float4 color0           : SV_TARGET0;	// �F
};

// �萔�o�b�t�@�s�N�Z���V�F�[�_�[��{�p�����[�^
struct DX_D3D11_PS_CONST_BUFFER_BASE {
	float4		FactorColor;			// �A���t�@�l��

	float		MulAlphaColor;			// �J���[�ɃA���t�@�l����Z���邩�ǂ���( 0.0f:��Z���Ȃ�  1.0f:��Z���� )
	float		AlphaTestRef;			// �A���t�@�e�X�g�Ŏg�p�����r�l
	float2		Padding1;

	int		    AlphaTestCmpMode;		// �A���t�@�e�X�g��r���[�h( DX_CMP_NEVER �Ȃ� )
	int3		Padding2;

	float4		IgnoreTextureColor;	// �e�N�X�`���J���[���������p�J���[
};

// ��{�p�����[�^
cbuffer cbD3D11_CONST_BUFFER_PS_BASE				: register(b1) {
	DX_D3D11_PS_CONST_BUFFER_BASE		g_Base;
};


// �v���O�����Ƃ̂����̂��߂Ɏg�����W�X�^1
cbuffer cbMULTIPLYCOLOR_CBUFFER1 : register(b2)
{
    float2 dispsize;
}

// �v���O�����Ƃ̂����̂��߂Ɏg�����W�X�^
cbuffer cbMULTIPLYCOLOR_CBUFFER2 : register(b3) {
	float4 caminfo;
}


//�e�N�X�`��
SamplerState g_Register0MapSampler : register(s0); // �f�B�t���[�Y�}�b�v�T���v��
Texture2D g_Register0MapTexture : register(t0); // �f�B�t���[�Y�}�b�v�e�N�X�`��

SamplerState g_Register1MapSampler : register(s1); // �@���}�b�v�T���v��
Texture2D g_Register1MapTexture : register(t1); // �@���}�b�v�e�N�X�`��

SamplerState g_Register2MapSampler : register(s2); // �[�x�}�b�v�T���v��
Texture2D g_Register2MapTexture : register(t2); // �[�x�}�b�v�e�N�X�`��

//�֐�
float4 GetTexColor0(float2 texCoord, int2 offset = int2(0, 0)) {
    texCoord.y *= -1.f;
	return g_Register0MapTexture.Sample(g_Register0MapSampler, texCoord, offset);
}
float4 GetTexColor1(float2 texCoord, int2 offset = int2(0, 0)) {
	return g_Register1MapTexture.Sample(g_Register1MapSampler, texCoord, offset);
}
float4 GetTexColor2(float2 texCoord, int2 offset = int2(0, 0)) {
	return g_Register2MapTexture.Sample(g_Register2MapSampler, texCoord, offset);
}

float3 DisptoProjNorm(float2 screenUV) {
	float2 pos = screenUV;

	pos /= 0.5f;
	pos.x = pos.x - 1.f;
	pos.y = 1.f - pos.y;

	float3 position;
    position.x = pos.x * caminfo.z * dispsize.x / dispsize.y;
    position.y = pos.y * caminfo.z;
	position.z = 1.f;

	return position;
}

float2 ProjtoDisp(float3 position) {
	position = position / position.z;

	float2 screenUV;
	screenUV.x = position.x / caminfo.z * dispsize.y / dispsize.x;
	screenUV.y = position.y / caminfo.z;

	screenUV.x = screenUV.x + 1.f;
	screenUV.y = 1.f - screenUV.y;
	screenUV *= 0.5f;
	return screenUV;
}

bool Hitcheck(float3 position) {
	float2 screenUV = ProjtoDisp(position);
	if (
		(abs(screenUV.x) <= 1.f) &&
		(abs(screenUV.y) <= 1.f)
		) {
		float depth = GetTexColor2(screenUV).r;
		float z = depth / (caminfo.y * 0.005f);
        return (position.z < z && z < position.z + (caminfo.y * 1.0f));
    }
	else {
		return false;
	}
}

static float maxLength = 12.5f *100.f; // ���ˍő勗��
static int BinarySearchIterations = 24; //2���T���ő吔

float4 applySSR(float3 normal, float2 screenUV) {
    float pixelStride;
    float3 delta;
    float3 position;

    float depth = GetTexColor2(screenUV).r;
    float z = depth / (caminfo.y * 0.005f);

    float4 color = float4(0.f, 0.f, 0.f, 0.f);

    bool IsCalc = (depth > 0.f);
    if (IsCalc)
    {
        float3 NormPos = DisptoProjNorm(screenUV);
		pixelStride = maxLength / caminfo.x;
		delta = reflect(NormPos, normal); // ���˃x�N�g��*�P��Ői�ދ���
        position = NormPos * z; //����
    }
	[fastopt]
	for (int i = 0; i < caminfo.x; i++) {
        if (IsCalc) {
            position += delta * pixelStride;

            if (Hitcheck(position)) { //���������̂œ񕪒T��
                position -= delta * pixelStride; //���ɖ߂�
                delta /= BinarySearchIterations; //�i�ޗʂ�������
    			[unroll] // attribute
                for (int j = 0; j < BinarySearchIterations; j++) {
                    if (IsCalc) {
                        pixelStride *= 0.5f;
                        position += delta * pixelStride;
                        if (Hitcheck(position)) {
                            pixelStride = -pixelStride;
                        }
                        if (length(pixelStride) < 1.f) {
                            IsCalc = false;
                        }
                    }
                }
				color = GetTexColor0(ProjtoDisp(position));// ���������̂ŐF���u�����h����
                IsCalc = false;
            }
        }
    }
    return color;
}

PS_OUTPUT main(PS_INPUT PSInput) {
	//�߂�l
	PS_OUTPUT PSOutput;
	//���˂��ǂꂾ�������邩
    float uvFactor = 2.0 * length(PSInput.TextureCoord0 - float2(0.5, 0.5));
    uvFactor *= uvFactor;
    float edge = max(0.0, 1.0 - uvFactor);
    float Per = GetTexColor2(PSInput.TextureCoord0).g * edge;
	//�m�[�}�����W�擾
    float3 normal = GetTexColor1(PSInput.TextureCoord0).xyz * 2.f - 1.f;
	
    float4 color = applySSR(normal, PSInput.TextureCoord0);
	//SSR
    if (Per > 0.f)
    {
        PSOutput.color0 = lerp(float4(0.f, 0.f, 0.f, 0.f), color, Per);
    }
    else
    {
        PSOutput.color0 = float4(0.f, 0.f, 0.f, 0.f);
    }
	//�����������o�͂���ꍇ�͂�����
    float3 Color = GetTexColor0(PSInput.TextureCoord0).xyz;
	if (
		PSOutput.color0.r == Color.r &&
		PSOutput.color0.g == Color.g &&
		PSOutput.color0.b == Color.b
		) {
		PSOutput.color0.a = 0.0;
	}
	return PSOutput;
}
