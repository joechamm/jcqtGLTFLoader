/*****************************************************************//**
 * \file   GLTFSampler.h
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
 * \brief  Encapsulate a glTF bufferView 
 * 
 * \author joechamm
 * \date   September 2022
 *********************************************************************/
#ifndef __GLTF_SAMPLER_H__
#define __GLTF_SAMPLER_H__

#include <QJsonObject>

namespace jcqt
{
	enum class eSamplerFilter : quint16
	{
		eSamplerFilter_NOT_SET = 1,
		eSamplerFilter_NEAREST = 9728,
		eSamplerFilter_LINEAR = 9729, 
		eSamplerFilter_NEAREST_MIPMAP_NEAREST = 9984,
		eSamplerFilter_LINEAR_MIPMAP_NEAREST = 9985,
		eSamplerFilter_NEAREST_MIPMAP_LINEAR = 9986,
		eSamplerFilter_LINEAR_MIPMAP_LINEAR = 9987,
		eSamplerFilter_INVALID = 9999
	};

	enum class eSamplerWrapMode : quint16
	{
		eSamplerWrapMode_NOT_SET = 1,
		eSamplerWrapMode_REPEAT = 10497, // default value
		eSamplerWrapMode_CLAMP_TO_EDGE = 33071,
		eSamplerWrapMode_MIRRORED_REPEAT = 33648,
		eSamplerWrapMode_INVALID = 9999
	};

	struct Sampler
	{
		eSamplerFilter m_magFilter = eSamplerFilter::eSamplerFilter_NOT_SET;
		eSamplerFilter m_minFilter = eSamplerFilter::eSamplerFilter_NOT_SET;
		eSamplerWrapMode m_wrapS = eSamplerWrapMode::eSamplerWrapMode_REPEAT;
		eSamplerWrapMode m_wrapT = eSamplerWrapMode::eSamplerWrapMode_REPEAT;
		QJsonObject m_jsonObject;
	};

	Sampler createSamplerFromJson ( const QJsonObject& jsonObj )
	{
		Sampler sampler;
		quint64 magFilter = jsonObj.contains ( "magFilter" ) ? jsonObj.value ( "magFilter" ).toInteger () : 0;
		quint64 minFilter = jsonObj.contains ( "minFilter" ) ? jsonObj.value ( "minFilter" ).toInteger () : 0;
		quint64 wrapS = jsonObj.contains ( "wrapS" ) ? jsonObj.value ( "wrapS" ).toInteger () : 10497;
		quint64 wrapT = jsonObj.contains ( "wrapT" ) ? jsonObj.value ( "wrapT" ).toInteger () : 10497;

		switch ( magFilter )
		{
		case 0:
			sampler.m_magFilter = eSamplerFilter::eSamplerFilter_NOT_SET;
			break;
		case 9728: // NEAREST
			sampler.m_magFilter = eSamplerFilter::eSamplerFilter_NEAREST;
			break;
		case 9729: // LINEAR
			sampler.m_magFilter = eSamplerFilter::eSamplerFilter_LINEAR;
			break;
		default:
			sampler.m_magFilter = eSamplerFilter::eSamplerFilter_INVALID;
			break;
		}

		switch ( minFilter )
		{
		case 0:
			sampler.m_minFilter = eSamplerFilter::eSamplerFilter_NOT_SET;
			break;
		case 9728: // NEAREST
			sampler.m_minFilter = eSamplerFilter::eSamplerFilter_NEAREST;
			break;
		case 9729: // LINEAR
			sampler.m_minFilter = eSamplerFilter::eSamplerFilter_LINEAR;
			break;
		case 9984: // NEAREST_MIPMAP_NEAREST
			sampler.m_minFilter = eSamplerFilter::eSamplerFilter_NEAREST_MIPMAP_NEAREST;
			break;
		case 9985: // LINEAR_MIPMAP_NEAREST
			sampler.m_minFilter = eSamplerFilter::eSamplerFilter_LINEAR_MIPMAP_NEAREST;
			break;
		case 9986: // NEAREST_MIPMAP_LINEAR
			sampler.m_minFilter = eSamplerFilter::eSamplerFilter_NEAREST_MIPMAP_LINEAR;
			break;
		case 9987: // LINEAR_MIPMAP_LINEAR
			sampler.m_minFilter = eSamplerFilter::eSamplerFilter_LINEAR_MIPMAP_LINEAR;
			break;
		default:
			sampler.m_minFilter = eSamplerFilter::eSamplerFilter_INVALID;
			break;
		}

		switch ( wrapS )
		{
		case 0:
			sampler.m_wrapS = eSamplerWrapMode::eSamplerWrapMode_NOT_SET;
			break;
		case 10497: // REPEAT
			sampler.m_wrapS = eSamplerWrapMode::eSamplerWrapMode_REPEAT;
			break;
		case 33071: // CLAMP_TO_EDGE
			sampler.m_wrapS = eSamplerWrapMode::eSamplerWrapMode_CLAMP_TO_EDGE;
			break;
		case 33648: // MIRRORED_REPEAT
			sampler.m_wrapS = eSamplerWrapMode::eSamplerWrapMode_MIRRORED_REPEAT;
			break;
		default:
			sampler.m_wrapS = eSamplerWrapMode::eSamplerWrapMode_INVALID;
			break;
		}

		switch ( wrapT )
		{
		case 0:
			sampler.m_wrapT = eSamplerWrapMode::eSamplerWrapMode_NOT_SET;
			break;
		case 10497: // REPEAT
			sampler.m_wrapT = eSamplerWrapMode::eSamplerWrapMode_REPEAT;
			break;
		case 33071: // CLAMP_TO_EDGE
			sampler.m_wrapT = eSamplerWrapMode::eSamplerWrapMode_CLAMP_TO_EDGE;
			break;
		case 33648: // MIRRORED_REPEAT
			sampler.m_wrapT = eSamplerWrapMode::eSamplerWrapMode_MIRRORED_REPEAT;
			break;
		default:
			sampler.m_wrapT = eSamplerWrapMode::eSamplerWrapMode_INVALID;
			break;
		}
		
		sampler.m_jsonObject = jsonObj;
		return sampler;
	}
}

#endif // !__GLTF_BUFFER_VIEW_H__