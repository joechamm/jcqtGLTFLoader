/*****************************************************************//**
 * \file   GLTFBufferView.h
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
#ifndef __GLTF_BUFFER_VIEW_H__
#define __GLTF_BUFFER_VIEW_H__

#include <QObject>
#include <QJsonObject>

namespace jcqt
{
	enum class eBufferViewTarget : quint16
	{
		eBufferViewTarget_NOT_SET = 1,
		eBufferViewTarget_ARRAY_BUFFER = 34962,
		eBufferViewTarget_ELEMENT_ARRAY_BUFFER = 34963,
		eBufferViewTarget_INVALID = 9999
	};

	struct BufferView
	{
		qsizetype m_bufferIndex = -1;
		qsizetype m_byteOffset = 0;
		qsizetype m_byteLength = - 1;
		qsizetype m_byteStride = - 1;
		eBufferViewTarget m_target = eBufferViewTarget::eBufferViewTarget_INVALID;
		QJsonObject m_jsonObject;
	};

	BufferView createBufferViewFromJson ( const QJsonObject& jsonObj )
	{
		BufferView bufferView;
		bufferView.m_bufferIndex = jsonObj.value ( "buffer" ).toInteger ();
		bufferView.m_byteLength = jsonObj.value ( "byteLength" ).toInteger ();
		bufferView.m_byteOffset = jsonObj.contains ( "byteOffset" ) ? jsonObj.value ( "byteOffset" ).toInteger () : 0;
		bufferView.m_byteStride = jsonObj.contains ( "byteStride" ) ? jsonObj.value ( "byteStride" ).toInteger () : 0;
		
		quint64 target = jsonObj.contains ( "target" ) ? jsonObj.value ( "target" ).toInteger () : 0;
		switch ( target )
		{
		case 0: // NOT SET
			bufferView.m_target = eBufferViewTarget::eBufferViewTarget_NOT_SET;
			break;
		case 34962: // ARRAY_BUFFER
			bufferView.m_target = eBufferViewTarget::eBufferViewTarget_ARRAY_BUFFER;
			break;
		case 34963: // ELEMENT_ARRAY_BUFFER
			bufferView.m_target = eBufferViewTarget::eBufferViewTarget_ELEMENT_ARRAY_BUFFER;
			break;
		default:
			bufferView.m_target = eBufferViewTarget::eBufferViewTarget_INVALID;
			break;
		}

		bufferView.m_jsonObject = jsonObj;
		return bufferView;
	}


	class GLTFBufferView : public QObject
	{
		Q_OBJECT

	public:
		GLTFBufferView ( QObject* parent = nullptr );
		GLTFBufferView ( const QJsonObject& jsonObj, QObject* parent = nullptr );
		~GLTFBufferView ();

		/*
			https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-bufferview
			'bufferView' requires the 'buffer' field and 'byteLength' field to be present
		*/
		static bool isValidJson ( const QJsonObject& jsonObj );
		static qsizetype getBufferIndexFromJson ( const QJsonObject& jsonObj );
		static qsizetype getByteOffsetFromJson ( const QJsonObject& jsonObj );
		static qsizetype getByteLengthFromJson ( const QJsonObject& jsonObj );
		static qsizetype getByteStrideFromJson ( const QJsonObject& jsonObj );
		static qsizetype getTargetFromJson ( const QJsonObject& jsonObj );
		static QString getNameFromJson ( const QJsonObject& jsonObj );
		static QJsonObject getExtensionsFromJson ( const QJsonObject& jsonObj );
		static QJsonObject getExtrasFromJson ( const QJsonObject& jsonObj );

		bool isValid () const;
		bool hasByteOffset () const;
		bool hasByteStride () const;
		bool hasTarget () const;
		bool hasName () const;
		bool hasExtensions () const;
		bool hasExtras () const;

		qsizetype bufferIndex () const;
		qsizetype byteOffset () const;
		qsizetype byteLength () const;
		qsizetype byteStride () const;
		qsizetype target () const;

		QString name () const;
		QJsonObject extensions () const;
		QJsonObject extras () const;

		bool setJsonObject ( const QJsonObject& jsonObj );
		bool setValuesFromJson ();

	private:
		qsizetype m_bufferIndex;
		qsizetype m_byteOffset;
		qsizetype m_byteLength;
		qsizetype m_byteStride;
		qsizetype m_target;
		QJsonObject m_jsonObject;
	};
}

#endif // !__GLTF_BUFFER_VIEW_H__

