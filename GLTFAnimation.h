/*****************************************************************//**
 * \file   GLTFAnimation.h
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
 * \brief  Encapsulate a glTF animation
 * 
 * \author joechamm
 * \date   September 2022
 *********************************************************************/
#ifndef __GLTF_ANIMATION_H__
#define __GLTF_ANIMATION_H__

#include <QJsonObject>
#include <QJsonArray>
#include <QList>

namespace jcqt
{
	enum class eAnimationChannelTargetPath : quint8
	{
		eTargetPath_NOT_SET = 0,
		eTargetPath_TRANSLATION = 1,
		eTargetPath_ROTATION = 2,
		eTargetPath_SCALE = 3,
		eTargetPath_WEIGHTS = 4,
		eTargetPath_INVALID = 99
	};

	enum class eAnimationSamplerInterpolation : quint8
	{
		eSamplerInterpolation_NOT_SET = 0,
		eSamplerInterpolation_LINEAR = 1,
		eSamplerInterpolation_STEP = 2,
		eSamplerInterpolation_CUBICSPLINE = 3,
		eSamplerInterpolation_INVALID = 99
	};

	struct AnimationChannelTarget
	{
		qsizetype						m_nodeIdx = -1;
		eAnimationChannelTargetPath		m_path = eAnimationChannelTargetPath::eTargetPath_NOT_SET;
		QJsonObject						m_jsonObject;
	};

	struct AnimationSampler
	{
		qsizetype						m_inputIdx = -1;
		qsizetype						m_outputIdx = -1;
		eAnimationSamplerInterpolation	m_interpolation = eAnimationSamplerInterpolation::eSamplerInterpolation_LINEAR;
		QJsonObject						m_jsonObject;
	};

	struct AnimationChannel
	{
		qsizetype				m_samplerIdx = -1;
		AnimationChannelTarget	m_target;
		QJsonObject				m_jsonObject;
	};

	struct Animation
	{
		QList<AnimationChannel>	m_channels;
		QList<AnimationSampler> m_samplers;
		QJsonObject				m_jsonObject;
	};

	AnimationChannelTarget createAnimationChannelTargetFromJson ( const QJsonObject& jsonObj )
	{
		AnimationChannelTarget channelTarget;
		if ( jsonObj.contains ( "node" ) )
			channelTarget.m_nodeIdx = jsonObj.value ( "node" ).toInteger ();

		QString path = jsonObj.value ( "path" ).toString ();
		if ( path.compare ( "translation", Qt::CaseInsensitive ) == 0 )
		{
			channelTarget.m_path = eAnimationChannelTargetPath::eTargetPath_TRANSLATION;
		}
		else if ( path.compare ( "rotation", Qt::CaseInsensitive ) == 0 )
		{
			channelTarget.m_path = eAnimationChannelTargetPath::eTargetPath_ROTATION;
		}
		else if ( path.compare ( "scale", Qt::CaseInsensitive ) == 0 )
		{
			channelTarget.m_path = eAnimationChannelTargetPath::eTargetPath_SCALE;
		}
		else if ( path.compare ( "weights", Qt::CaseInsensitive ) == 0 )
		{
			channelTarget.m_path = eAnimationChannelTargetPath::eTargetPath_WEIGHTS;
		}
		else
		{
			channelTarget.m_path = eAnimationChannelTargetPath::eTargetPath_INVALID;
		}

		channelTarget.m_jsonObject = jsonObj;
		return channelTarget;
	}

	AnimationSampler createAnimationSamplerFromJson ( const QJsonObject& jsonObj )
	{
		AnimationSampler animationSampler;
		animationSampler.m_inputIdx = jsonObj.value ( "input" ).toInteger ();
		animationSampler.m_outputIdx = jsonObj.value ( "output" ).toInteger ();
		if ( jsonObj.contains ( "interpolation" ) )
		{
			QString interpolation = jsonObj.value ( "interpolation" ).toString ();
			if ( interpolation.compare ( "linear", Qt::CaseInsensitive ) == 0 )
			{
				animationSampler.m_interpolation = eAnimationSamplerInterpolation::eSamplerInterpolation_LINEAR;
			}
			else if ( interpolation.compare ( "step", Qt::CaseInsensitive ) == 0 )
			{
				animationSampler.m_interpolation = eAnimationSamplerInterpolation::eSamplerInterpolation_STEP;
			}
			else if ( interpolation.compare ( "cubicspline", Qt::CaseInsensitive ) == 0 )
			{
				animationSampler.m_interpolation = eAnimationSamplerInterpolation::eSamplerInterpolation_CUBICSPLINE;
			}
			else
			{
				animationSampler.m_interpolation = eAnimationSamplerInterpolation::eSamplerInterpolation_INVALID;
			}
		}

		animationSampler.m_jsonObject = jsonObj;
		return animationSampler;
	}

	AnimationChannel createAnimationChannelFromJson ( const QJsonObject& jsonObj )
	{
		AnimationChannel animationChannel;
		animationChannel.m_samplerIdx = jsonObj.value ( "sampler" ).toInteger ();
		animationChannel.m_target = createAnimationChannelTargetFromJson ( jsonObj.value ( "target" ).toObject() );
		animationChannel.m_jsonObject = jsonObj;
		return animationChannel;
	}

	Animation createAnimationFromJson ( const QJsonObject& jsonObj )
	{
		Animation animation;

		QJsonArray channels = jsonObj.value ( "channels" ).toArray ();
		
		qsizetype count = channels.size ();
		animation.m_channels.resize ( count );
		
		for ( qsizetype i = 0; i < count; i++ )
		{
			animation.m_channels [ i ] = createAnimationChannelFromJson ( channels.at ( i ).toObject () );
		}
		
		QJsonArray samplers = jsonObj.value ( "samplers" ).toArray ();

		count = samplers.size ();
		animation.m_samplers.resize ( count );

		for ( qsizetype i = 0; i < count; i++ )
		{
			animation.m_samplers [ i ] = createAnimationSamplerFromJson ( samplers.at ( i ).toObject () );
		}

		animation.m_jsonObject = jsonObj;
		return animation;
	}
}

#endif // !__GLTF_ANIMATION_H__
