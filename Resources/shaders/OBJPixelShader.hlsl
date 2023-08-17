
#include "OBJShaderHeader.hlsli"


PSOutput main(VSOutput input)
{
    PSOutput output;

	// �e�N�X�`���}�b�s���O
    float4 texcolor = tex.Sample(smp, input.uv);

	// ����x
    const float SHININESS = 20.0f;
	// ���_���王�_�ւ̕����x�N�g��
    float3 eyedir = normalize(cameraPos - input.worldpos.xyz);

	// �����ˌ�
    float3 ambient = float3(ambientColor * m_ambient);

	// �V�F�[�f�B���O�ɂ��F
    float4 shadecolor = { 0, 0, 0, m_alpha };
    
    //�X�y�L�����}�b�v�̐F
    float4 specularMapCol = { 0, 0, 0, 0 };
    
    matrix worldL = mul(world, worldMesh);
    
    //���[���h�̖@��
    float3 wNormal = normalize(mul(worldL, float4(input.normal, 0)));
    
    float3 lightNormal = wNormal;
          
    //�m�[�}���}�b�v���L���Ȃ烉�C�g�̌v�Z�Ɏg���@�����Z�o
    if (isNormalMap)
    {
        //���[�J���̖@�����g��Ȃ��Ƃ����Ȃ�
        lightNormal = GetNormalMapWorldNormalVec(tex4.Sample(smp, input.uv), input.tangent,
            input.binormal, input.normal, worldL);
    }
    
	//���s����
    for (int i = 0; i < S_DIRLIGHT_NUM; i++)
    {
        if (dirLights[i].active)
        {
			// ���C�g�Ɍ������x�N�g���Ɩ@���̓���
            float3 dotlightnormal = dot(dirLights[i].lightv, lightNormal);
			// ���ˌ��x�N�g��
            float3 reflect = normalize(-dirLights[i].lightv + 2 * dotlightnormal * lightNormal);
            float3 specular = pow(saturate(dot(reflect, eyedir)), SHININESS);
			//�g�D�[��
            if (isToon)
            {
                specular = smoothstep(0.5f, 0.55f, specular);
                dotlightnormal = (1.0f - specular) * smoothstep(0.5f, 0.55f, dotlightnormal);
                ambient = (1.0f - dotlightnormal - specular) * ambient;
            }

			// �g�U���ˌ�
            float3 diffuse = dotlightnormal * m_diffuse * diffuseColor;
			// ���ʔ��ˌ�
            specular = specular * m_specular * specularColor;

			// �S�ĉ��Z����
            shadecolor.rgb += (diffuse + specular) * dirLights[i].lightcolor;
            
            //�X�y�L�����}�b�v
            if (isSpecularMap)
            {
                float4 specularMap = GetSpecularMapColor(specular, diffuse, tex2.Sample(smp, input.uv));
                specularMapCol += specularMap;
            }
        }
    }

	//�_����
    for (int i = 0; i < S_POINTLIGHT_NUM; i++)
    {
        if (pointLights[i].active)
        {
			//���C�g�ւ̃x�N�g��
            float3 lightv = pointLights[i].lightpos - input.worldpos.xyz;
			//�x�N�g���̒���
            float d = length(lightv);
			//���K�����A�P�ʃx�N�g���ɂ���
            lightv = normalize(lightv);
			//���������W��
            float atten = 1.0f / (pointLights[i].lightatten.x + pointLights[i].lightatten.y * d +
				pointLights[i].lightatten.z * d * d);
                       
			// ���C�g�Ɍ������x�N�g���Ɩ@���̓���
            float3 dotlightnormal = dot(lightv, lightNormal);
            
			// ���ˌ��x�N�g��
            float3 reflect = normalize(-lightv + 2 * dotlightnormal * lightNormal);
                        
            float3 specular = pow(saturate(dot(reflect, eyedir)), SHININESS);
			//�g�D�[��
            if (isToon)
            {
                specular = smoothstep(0.5f, 0.55f, specular);
                dotlightnormal = (1.0f - specular) * smoothstep(0.5f, 0.55f, dotlightnormal);
                ambient = (1.0f - dotlightnormal - specular) * ambient;
            }

			// �g�U���ˌ�
            float3 diffuse = dotlightnormal * m_diffuse * diffuseColor;
			// ���ʔ��ˌ�
            specular = specular * m_specular * specularColor;
            
			// �S�ĉ��Z����
            shadecolor.rgb += atten * (diffuse + specular) * pointLights[i].lightcolor;
                       
            //�X�y�L�����}�b�v
            if (isSpecularMap)
            {
                float4 specularMap = GetSpecularMapColor(specular, diffuse, tex2.Sample(smp, input.uv));
                specularMapCol += specularMap;
            }

        }
    }
    

	//�X�|�b�g���C�g
    for (int i = 0; i < S_SPOTLIGHT_NUM; i++)
    {
        if (spotLights[i].active)
        {
			//���C�g�ւ̕����x�N�g��
            float3 lightv = spotLights[i].lightpos - input.worldpos.xyz;
            float d = length(lightv);
            lightv = normalize(lightv);
			//���������W��
            float atten = saturate(1.0f / (spotLights[i].lightatten.x + spotLights[i].lightatten.y
				* d + spotLights[i].lightatten.z * d * d));
			//�p�x����
            float cos = dot(lightv, spotLights[i].lightv);
			//�����J�n�p�x����A�����I���p�x�ɂ����Č���
			//�����J�n�p�x�̓����́A1�{ �����I���p�x�̊O����0�{�̋P�x
            float angleatten = smoothstep(spotLights[i].lightfactoranglecos.y,
				spotLights[i].lightfactoranglecos.x, cos);
			//�p�x��������Z
            atten *= angleatten;
			//���C�g�Ɍ������x�N�g���Ɩ@���̓���
            float3 dotlightnormal = dot(lightv, lightNormal);
			//���ˌ��x�N�g���@
            float3 reflect = normalize(-lightv + 2 * dotlightnormal * lightNormal);
            float3 specular = pow(saturate(dot(reflect, eyedir)), SHININESS);
			//�g�D�[��
            if (isToon)
            {
                specular = smoothstep(0.5f, 0.55f, specular);
                dotlightnormal = (1.0f - specular) * smoothstep(0.5f, 0.55f, dotlightnormal);
                ambient = (1.0f - dotlightnormal - specular) * ambient;
            }

			// �g�U���ˌ�
            float3 diffuse = dotlightnormal * m_diffuse * diffuseColor;
			// ���ʔ��ˌ�
            specular = specular * m_specular * specularColor;
			//�S�ĉ��Z����
            shadecolor.rgb += atten * (diffuse + specular) * spotLights[i].lightcolor;
            
            //�X�y�L�����}�b�v
            if (isSpecularMap)
            {
                float4 specularMap = GetSpecularMapColor(specular, diffuse, tex2.Sample(smp, input.uv));
                specularMapCol += specularMap;
            }
        }
    }

	//�ۉe
    for (int i = 0; i < S_CIRCLESHADOW_NUM; i++)
    {
        if (circleShadows[i].active)
        {
			//���C�g�ւ̕����x�N�g��
            float3 casterv = circleShadows[i].casterPos - input.worldpos.xyz;
			//���e�����ł̋���
            float d = dot(casterv, circleShadows[i].dir);
			//���������W��
            float atten = saturate(1.0f / (circleShadows[i].atten.x + circleShadows[i].atten.y
				* d + circleShadows[i].atten.z * d * d));
			//�������}�C�i�X�Ȃ�0�ɂ���
            atten *= step(0, d);
			//���z���C�g�̍��W
            float3 lightpos = circleShadows[i].casterPos + circleShadows[i].dir
				* circleShadows[i].distanceCasterLight;
			//�I�u�W�F�N�g�\�ʂ��烉�C�g�ւ̃x�N�g���i�P�ʃx�N�g���j
            float3 lightv = normalize(lightpos - input.worldpos.xyz);
			//�p�x����
            float cos = dot(lightv, circleShadows[i].dir);
			//�����J�n�p�x����A�����I���p�x�ɂ����Č���
			//�����J�n�p�x�̓����́A1�{ �����I���p�x�̊O����0�{�̋P�x
            float angleatten = smoothstep(circleShadows[i].factorAngleCos.y,
				circleShadows[i].factorAngleCos.x, cos);
			//�p�x��������Z
            atten *= angleatten;

			//�S�Č��Z����
            shadecolor.rgb -= atten;
        }
    }

	//����
    shadecolor.rgb += ambient;

	//�������C�g
	//����
    float dotL = 1.0f;
    if (isRimLight)
    {
        dotL = smoothstep(0.5f, 0.55f, dot(eyedir, wNormal));
    }

	// �V�F�[�f�B���O�ɂ��F�ŕ`��
    float4 DSC = dotL * shadecolor;
    float4 RIM = float4(rimColor.rgb, 1.0f) * (1.0f - dotL);
    float4 RGBA = (DSC * texcolor * color) + RIM;
    float4 RGBA2 = (DSC * color) + RIM;
    float3 RGB = RGBA.rgb;
    float A = RGBA.a;
    
      
    //�X�y�L�����}�b�v
    if (isSpecularMap)
    {
        RGBA += saturate(specularMapCol);
    }
    
		//�^���V���G�b�g
    if (isSilhouette)
    {
        RGBA = float4(1, 0, 0, 1);
    }
	
    if (isFog == true)
    {
		//�t�H�O
        float4 m_FogColor = float4(1.0f, 1.0f, 1.0f, 1.0f); //�t�H�O�J���[
        float m_Near = 100.0f; //�t�H�O�̊J�n�ʒu
        float m_Far = 500.0f; //�t�H�O�̏I���ʒu
        float m_FogLen = m_Far - m_Near; //m_Far - m_Near�̌���

		//���_�Ǝ��_�Ƃ̋������v�Z����
        float d = distance(input.worldpos.xyz, cameraPos);

        float f = (m_Far - d) / (m_Far - m_Near);
        f = clamp(f, 0.0f, 1.0f);
		//�I�u�W�F�N�g�̃����o�[�g�g�U�Ɩ��̌v�Z���ʂƃt�H�O�J���[����`��������

        RGBA = RGBA * f + m_FogColor * (1.0f - f);
    }
	
	//�f�B�]���u
    if (isDissolve)
    {
        float4 mask = tex2.Sample(smp, input.uv);
		//R�l��0.5������ƕ`��j������
        clip(mask.r - dissolveT);
    }
   

	//�ꖇ�ڂ̈��
    output.col = RGBA;

    return output;
}