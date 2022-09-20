/*****************************************************************//**
 * \file   GLTFAccessor.h
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
 * \brief  Encapsulate a glTF accessor
 * 
 * \author joechamm
 * \date   September 2022
 *********************************************************************/
#ifndef __GLTF_ACCESSOR_H__
#define __GLTF_ACCESSOR_H__

#include <QObject>
#include <QJsonObject>

namespace jcqt
{
	class GLTFAccessor : public QObject
	{
		Q_OBJECT

	public:
		struct SparseValues
		{
			qsizetype m_bufferViewIndex;
			qsizetype m_byteOffset;
		};

		struct SparseIndices
		{
			qsizetype m_bufferViewIndex;
			qsizetype m_byteOffset;
			qsizetype m_componentType;
		};

		struct AccessorSparse
		{
			qsizetype		m_count;
			SparseValues	m_values;
			SparseIndices	m_indices;
			// contains all the extras and extensions for sparse values and sparse indices
			QJsonObject		m_jsonObject; 
		};

	public:
		GLTFAccessor ( QObject* parent );
		~GLTFAccessor ();

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

		static SparseValues createSparseValuesFromJson ( const QJsonObject& jsonObj );
		static SparseIndices createSparseIndicesFromJson ( const QJsonObject& jsonObj );
		static AccessorSparse createAccessorSparseFromJson ( const QJsonObject& jsonObj );

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
		qsizetype		m_bufferViewIndex;
		qsizetype		m_byteOffset;
		qsizetype		m_componentType;
		qsizetype		m_count;
		qsizetype		m_max;
		qsizetype		m_min;
		bool			m_normalized;
		AccessorSparse	m_sparse;
		QJsonObject		m_jsonObject;
	};

}

#endif // !__GLTF_ACCESSOR_H__

