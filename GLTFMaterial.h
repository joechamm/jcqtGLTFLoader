/*****************************************************************//**
 * \file   GLTFMaterial.h
 * \licence MIT License

Copyright (c) 2022 Joseph Cunningham

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 * \brief  Encapsulate a glTF material
 * 
 * \author joechamm
 * \date   September 2022
 *********************************************************************/
#ifndef __GLTF_MATERIAL_H__
#define __GLTF_MATERIAL_H__

#include <QJsonObject>
#include <QJsonArray>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "GLTFException.h"

namespace jcqt
{
	enum class eAlphaMode : quint8
	{
		eAlphaMode_OPAQUE = 1,
		eAlphaMode_MASK = 2,
		eAlphaMode_BLEND = 3,
		eAlphaMode_INVALID = 99
	};

	struct Texture
	{
		qsizetype	m_samplerIdx = -1;
		qsizetype	m_sourceIdx = -1;
		QJsonObject m_jsonObject;
	};

	struct TextureInfo
	{
		qsizetype		m_index = - 1;
		qsizetype		m_texCoord = 0;
		QJsonObject		m_jsonObject;
	};

	struct PBRMetalicRoughness
	{
		float			m_metallicFactor = 1.0f;
		float			m_roughnessFactor = 1.0f;
		glm::vec4		m_baseColorFactor = glm::vec4 ( 1.0f, 1.0f, 1.0f, 1.0f );
		TextureInfo		m_baseColorTexture;
		TextureInfo		m_metallicRoughnessTexture;
		QJsonObject		m_jsonObject;
	};

	struct NormalTextureInfo
	{
		qsizetype		m_index = 0;
		qsizetype		m_texCoord = 0;
		float			m_scale = 1.0f;
		QJsonObject		m_jsonObject;
	};

	struct OcclusionTextureInfo
	{
		qsizetype		m_index = 0;
		qsizetype		m_texCoord = 0;
		float			m_strength = 1.0f;
		QJsonObject		m_jsonObject;
	};

	struct Material
	{
		glm::vec3				m_emissiveFactor = glm::vec3 ( 0.0f, 0.0f, 0.0f );
		bool					m_doubleSided = false;
		float					m_alphaCutoff = 0.5f;
		eAlphaMode				m_alphaMode = eAlphaMode::eAlphaMode_OPAQUE;
		PBRMetalicRoughness		m_pbrMetallicRoughness;
		NormalTextureInfo		m_normalTexture;
		OcclusionTextureInfo	m_occlusionTexture;
		TextureInfo				m_emissiveTexture;
		QJsonObject				m_jsonObject;
	};

	Texture createTextureFromJson ( const QJsonObject& jsonObj )
	{
		Texture tex;

		if ( jsonObj.contains ( "sampler" ) )
			tex.m_samplerIdx = jsonObj.value ( "sampler" ).toInteger ();

		if ( jsonObj.contains ( "source" ) )
			tex.m_sourceIdx = jsonObj.value ( "source" ).toInteger ();

		tex.m_jsonObject = jsonObj;
		return tex;
	}

	TextureInfo createTextureInfoFromJson ( const QJsonObject& jsonObj )
	{
		TextureInfo texInfo;
		texInfo.m_index = jsonObj.value ( "index" ).toInteger ();
		if(jsonObj.contains("texCoord"))
			texInfo.m_texCoord = jsonObj.value ( "texCoord" ).toInteger ();

		texInfo.m_jsonObject = jsonObj;
		return texInfo;
	}

	NormalTextureInfo createNormalTextureInfoFromJson ( const QJsonObject& jsonObj )
	{
		NormalTextureInfo normalTex;
		normalTex.m_index = jsonObj.value ( "index" ).toInteger ();
		if ( jsonObj.contains ( "texCoord" ) )
			normalTex.m_texCoord = jsonObj.value ( "texCoord" ).toInteger ();

		if ( jsonObj.contains ( "scale" ) )
			normalTex.m_scale = static_cast< float >( jsonObj.value ( "scale" ).toDouble ( 1.0 ) );

		normalTex.m_jsonObject = jsonObj;
		return normalTex;
	}

	PBRMetalicRoughness createPBRMetallicRoughnessFromJson ( const QJsonObject& jsonObj )
	{
		PBRMetalicRoughness pbrMtl;
		if ( jsonObj.contains ( "baseColorFactor" ) )
		{
			QJsonArray baseColorFactor = jsonObj.value ( "baseColorFactor" ).toArray ();
			for ( qsizetype i = 0; i < 4; i++ )
			{
				pbrMtl.m_baseColorFactor [ i ] = static_cast< float >( baseColorFactor.at ( i ).toDouble ( 1.0 ) );
			}
		}

		if ( jsonObj.contains ( "baseColorTexture" ) )
			pbrMtl.m_baseColorTexture = createTextureInfoFromJson ( jsonObj.value ( "baseColorTexture" ).toObject () );

		if ( jsonObj.contains ( "metallicFactor" ) )
			pbrMtl.m_metallicFactor = static_cast< float >( jsonObj.value ( "metallicFactor" ).toDouble ( 1.0 ) );

		if ( jsonObj.contains ( "roughnessFactor" ) )
			pbrMtl.m_roughnessFactor = static_cast< float >( jsonObj.value ( "roughnessFactor" ).toDouble ( 1.0 ) );

		if ( jsonObj.contains ( "metallicRoughnessTexture" ) )
			pbrMtl.m_metallicRoughnessTexture = createTextureInfoFromJson ( jsonObj.value ( "metallicRoughnessTexture" ).toObject () );

		pbrMtl.m_jsonObject = jsonObj;
		return pbrMtl;
	}

	OcclusionTextureInfo createOcclusionTextureInfoFromJson ( const QJsonObject& jsonObj )
	{
		OcclusionTextureInfo  occTex;
		occTex.m_index = jsonObj.value ( "index" ).toInteger ();
		if ( jsonObj.contains ( "texCoord" ) )
			occTex.m_texCoord = jsonObj.value ( "texCoord" ).toInteger ();

		if ( jsonObj.contains ( "strength" ) )
			occTex.m_strength = static_cast< float >( jsonObj.value ( "strength" ).toDouble ( 1.0 ) );

		occTex.m_jsonObject = jsonObj;
		return occTex;
	}

	Material createMaterialFromJson ( const QJsonObject& jsonObj )
	{
		Material mat;
		if ( jsonObj.contains ( "pbrMetallicRoughness" ) )
			mat.m_pbrMetallicRoughness = createPBRMetallicRoughnessFromJson ( jsonObj.value ( "pbrMetallicRoughness" ).toObject () );

		if ( jsonObj.contains ( "normalTexture" ) )
			mat.m_normalTexture = createNormalTextureInfoFromJson ( jsonObj.value ( "normalTexture" ).toObject () );

		if ( jsonObj.contains ( "occlusionTexture" ) )
			mat.m_occlusionTexture = createOcclusionTextureInfoFromJson ( jsonObj.value ( "occlusionTexture" ).toObject () );

		if ( jsonObj.contains ( "emissiveTexture" ) )
			mat.m_emissiveTexture = createTextureInfoFromJson ( jsonObj.value ( "emissiveTexture" ).toObject () );

		if ( jsonObj.contains ( "emissiveFactor" ) )
		{
			QJsonArray emissiveFactor = jsonObj.value ( "emissiveFactor" ).toArray ();
			for ( qsizetype i = 0; i < 3; i++ )
			{
				mat.m_emissiveFactor [ i ] = static_cast< float >( emissiveFactor.at ( i ).toDouble ( 0.0 ) );
			}
		}

		if ( jsonObj.contains ( "alphaMode" ) )
		{
			QString alphaMode = jsonObj.value ( "alphaMode" ).toString ();
			if ( alphaMode.compare ( "mask", Qt::CaseInsensitive ) == 0 )
			{
				mat.m_alphaMode = eAlphaMode::eAlphaMode_MASK;
			}
			else if ( alphaMode.compare ( "blend", Qt::CaseInsensitive ) == 0 )
			{
				mat.m_alphaMode = eAlphaMode::eAlphaMode_BLEND;
			}
			else if ( alphaMode.compare ( "opaque", Qt::CaseInsensitive ) == 0 )
			{
				mat.m_alphaMode = eAlphaMode::eAlphaMode_OPAQUE;
			}
			else
			{
				mat.m_alphaMode = eAlphaMode::eAlphaMode_INVALID;
			}
		}

		mat.m_alphaCutoff = static_cast< float >( jsonObj.value ( "alphaCutoff" ).toDouble ( 0.5 ) );
		mat.m_doubleSided = jsonObj.value ( "doubleSided" ).toBool ( false );
		mat.m_jsonObject = jsonObj;
		return mat;
	}
}

#endif // !__GLTF_MATERIAL_H__
