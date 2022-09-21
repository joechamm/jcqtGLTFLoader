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
#include <QJsonArray>
#include <QList>
#include <array>

namespace jcqt
{
	enum class eAccessorComponentType : qint16
	{
		eComponentType_BYTE = 5120,
		eComponentType_UNSIGNED_BYTE = 5121,
		eComponentType_SHORT = 5122,
		eComponentType_UNSIGNED_SHORT = 5123,
		eComponentType_UNSIGNED_INT = 5125,
		eComponentType_FLOAT = 5126,
		eComponentType_INVALID = 9999
	};

	enum class eAccessorType : qint8
	{
		eType_SCALAR = 1,
		eType_VEC2 = 2,
		eType_VEC3 = 3,
		eType_VEC4 = 4,
		eType_MAT2 = 5,
		eType_MAT3 = 6,
		eType_MAT4 = 7,
		eType_INVALID = 8
	};

	struct SparseValues
	{
		qsizetype m_bufferViewIndex = -1;
		qsizetype m_byteOffset = 0;
	};

	struct SparseIndices
	{
		qsizetype m_bufferViewIndex = -1;
		qsizetype m_byteOffset = 0;
		eAccessorComponentType m_componentType = eAccessorComponentType::eComponentType_INVALID;
	};

	struct AccessorSparse
	{
		qsizetype		m_count = -1;
		SparseValues	m_values;
		SparseIndices	m_indices;
		// contains all the extras and extensions for sparse values and sparse indices
		QJsonObject		m_jsonObject;
	};

	struct Accessor
	{
		qsizetype					m_bufferViewIndex = -1;
		qsizetype					m_byteOffset = 0;
		eAccessorComponentType		m_componentType = eAccessorComponentType::eComponentType_INVALID;
		qsizetype					m_count = -1;
		eAccessorType				m_type = eAccessorType::eType_INVALID;
		bool						m_normalized = false;
		AccessorSparse				m_sparse;
		QJsonObject					m_jsonObject;		
	};

	SparseValues createSparseValuesFromJson ( const QJsonObject& jsonObj )
	{
		SparseValues sparseValues;
		sparseValues.m_bufferViewIndex = jsonObj.value ( "bufferView" ).toInteger ();
		sparseValues.m_byteOffset = jsonObj.contains ( "byteOffset" ) ? jsonObj.value ( "byteOffset" ).toInteger () : 0;
		return sparseValues;
	}

	SparseIndices createSparseIndicesFromJson ( const QJsonObject& jsonObj )
	{
		SparseIndices sparseIndices;
		sparseIndices.m_bufferViewIndex = jsonObj.value ( "bufferView" ).toInteger ();
		qint64 compType = jsonObj.value ( "componentType" ).toInteger ();
		switch ( compType )
		{	
		case 5121:
			sparseIndices.m_componentType = eAccessorComponentType::eComponentType_UNSIGNED_BYTE;
			break;		
		case 5123:
			sparseIndices.m_componentType = eAccessorComponentType::eComponentType_UNSIGNED_SHORT;
			break;
		case 5125:
			sparseIndices.m_componentType = eAccessorComponentType::eComponentType_UNSIGNED_INT;
			break;		
		default:
			sparseIndices.m_componentType = eAccessorComponentType::eComponentType_INVALID;
			break;
		}

		sparseIndices.m_byteOffset = jsonObj.contains ( "byteOffset" ) ? jsonObj.value ( "byteOffset" ).toInteger () : 0;
		return sparseIndices;
	}

	AccessorSparse createAccessorSparseFromJson ( const QJsonObject& jsonObj )
	{
		AccessorSparse accessorSparse;
		accessorSparse.m_count = jsonObj.value ( "count" ).toInteger ();
		accessorSparse.m_indices = createSparseIndicesFromJson ( jsonObj.value ( "indices" ).toObject () );
		accessorSparse.m_values = createSparseValuesFromJson ( jsonObj.value ( "values" ).toObject () );
		accessorSparse.m_jsonObject = jsonObj;
		return accessorSparse;
	}

	Accessor createAccessorFromJson ( const QJsonObject& jsonObj )
	{
		Accessor accessor;
		accessor.m_bufferViewIndex = jsonObj.contains ( "bufferView" ) ? jsonObj.value ( "bufferView" ).toInteger () : -1;
		accessor.m_byteOffset = jsonObj.contains ( "byteOffset" ) ? jsonObj.value ( "byteOffset" ).toInteger () : 0;

		qint64 compType = jsonObj.value ( "componentType" ).toInteger ();
		switch ( compType )
		{
		case 5120:
			accessor.m_componentType = eAccessorComponentType::eComponentType_BYTE;
			break;
		case 5121:
			accessor.m_componentType = eAccessorComponentType::eComponentType_UNSIGNED_BYTE;
			break;
		case 5122:
			accessor.m_componentType = eAccessorComponentType::eComponentType_SHORT;
			break;
		case 5123:
			accessor.m_componentType = eAccessorComponentType::eComponentType_UNSIGNED_SHORT;
			break;		
		case 5125:
			accessor.m_componentType = eAccessorComponentType::eComponentType_UNSIGNED_INT;
			break;
		case 5126:
			accessor.m_componentType = eAccessorComponentType::eComponentType_FLOAT;
			break;
		default:
			accessor.m_componentType = eAccessorComponentType::eComponentType_INVALID;
			break;
		}

		accessor.m_normalized = jsonObj.value ( "normalized" ).toBool ( false );
		accessor.m_count = jsonObj.value ( "count" ).toInteger ();

		QString type = jsonObj.value ( "type" ).toString ();
		
		if ( type.compare ( "SCALAR", Qt::CaseInsensitive ) == 0 )
		{
			accessor.m_type = eAccessorType::eType_SCALAR;
		}
		else if ( type.compare ( "VEC2", Qt::CaseInsensitive ) == 0 )
		{
			accessor.m_type = eAccessorType::eType_VEC2;
		}
		else if ( type.compare ( "VEC3", Qt::CaseInsensitive ) == 0 )
		{
			accessor.m_type = eAccessorType::eType_VEC3;
		}
		else if ( type.compare ( "VEC4", Qt::CaseInsensitive ) == 0 )
		{
			accessor.m_type = eAccessorType::eType_VEC4;
		}
		else if ( type.compare ( "MAT2", Qt::CaseInsensitive ) == 0 )
		{
			accessor.m_type = eAccessorType::eType_MAT2;
		}
		else if ( type.compare ( "MAT3", Qt::CaseInsensitive ) == 0 )
		{
			accessor.m_type = eAccessorType::eType_MAT3;
		}
		else if ( type.compare ( "MAT4", Qt::CaseInsensitive ) == 0 )
		{
			accessor.m_type = eAccessorType::eType_MAT4;
		}
		else
		{
			accessor.m_type = eAccessorType::eType_INVALID;
		}

		if ( jsonObj.contains ( "sparse" ) )
		{
			accessor.m_sparse = createAccessorSparseFromJson ( jsonObj.value ( "sparse" ).toObject () );
		}

		accessor.m_jsonObject = jsonObj;
		return accessor;		
	}

	template<typename T>
	QList<std::array<T, 2>> getAccessorMinMax ( const QJsonObject& jsonObj )
	{
		QList<std::array<T, 2>> minMaxList;
		bool hasMinMax = jsonObj.contains ( "min" ) && jsonObj.contains ( "max" );
		if ( !hasMinMax )
		{
			qWarning () << "MIN/MAX NOT AVAILABLE. RETURNING EMPTY LIST" << Qt::endl;
			return minMaxList;
		}

		quint64 compType = jsonObj.value ( "componentType" ).toInteger ();
		switch ( compType )
		{
		case 5120: // BYTE
			if ( !( std::is_same<T, qint8>::value ) )
			{
				qWarning () << "COMPONENT TYPE MISMATCH. RETURNING EMPTY LIST" << Qt::endl;
				return minMaxList;
			}
			break;
		case 5121: // UNSIGNED_BYTE
			if ( !( std::is_same<T, quint8>::value ) )
			{
				qWarning () << "COMPONENT TYPE MISMATCH. RETURNING EMPTY LIST" << Qt::endl;
				return minMaxList;
			}
			break;
		case 5122: // SHORT
			if ( !( std::is_same<T, qint16>::value ) )
			{
				qWarning () << "COMPONENT TYPE MISMATCH. RETURNING EMPTY LIST" << Qt::endl;
				return minMaxList;
			}
			break;
		case 5123: // UNSIGNED_SHORT
			if ( !( std::is_same<T, quint16>::value ) )
			{
				qWarning () << "COMPONENT TYPE MISMATCH. RETURNING EMPTY LIST" << Qt::endl;
				return minMaxList;
			}
			break;
		case 5125: // UNSIGNED_INT
			if ( !( std::is_same<T, quint32>::value ) )
			{
				qWarning () << "COMPONENT TYPE MISMATCH. RETURNING EMPTY LIST" << Qt::endl;
				return minMaxList;
			}
			break;
		case 5126: // FLOAT
			if ( !( std::is_same<T, float>::value ) )
			{
				qWarning () << "COMPONENT TYPE MISMATCH. RETURNING EMPTY LIST" << Qt::endl;
				return minMaxList;
			}
			break;
		default:
			qWarning () << "INVALID COMPONENT TYPE. RETURNING EMPTY LIST" << Qt::endl;
			return minMaxList;
			break;
		}
				
		QJsonArray minList = jsonObj.value ( "min" ).toArray ();
		QJsonArray maxList = jsonObj.value ( "max" ).toArray ();
		QString type = jsonObj.value ( "type" ).toString ();

		if ( type.compare ( "SCALAR", Qt::CaseInsensitive ) == 0 )
		{
			constexpr qsizetype COMP_COUNT = 1;
			minMaxList.resize ( COMP_COUNT );
			for ( qsizetype i = 0; i < COMP_COUNT; i++ )
			{
				T minVal, maxVal;
				if ( std::is_same<T, float>::value )
				{
					minVal = static_cast< T >( minList.at ( i ).toDouble () );
					maxVal = static_cast< T >( maxList.at ( i ).toDouble () );
				}
				else
				{
					minVal = static_cast< T >( minList.at ( i ).toInteger () );
					maxVal = static_cast< T >( maxList.at ( i ).toInteger () );
				}
				
				minMaxList [ i ] = std::array<T, 2> ( { minVal, maxVal } );
			}
		}
		else if ( type.compare ( "VEC2", Qt::CaseInsensitive ) == 0 )
		{
			constexpr qsizetype COMP_COUNT = 2;
			minMaxList.resize ( COMP_COUNT );
			for ( qsizetype i = 0; i < COMP_COUNT; i++ )
			{
				T minVal, maxVal;
				if ( std::is_same<T, float>::value )
				{
					minVal = static_cast< T >( minList.at ( i ).toDouble () );
					maxVal = static_cast< T >( maxList.at ( i ).toDouble () );
				}
				else
				{
					minVal = static_cast< T >( minList.at ( i ).toInteger () );
					maxVal = static_cast< T >( maxList.at ( i ).toInteger () );
				}

				minMaxList [ i ] = std::array<T, 2> ( { minVal, maxVal } );
			}
		}
		else if ( type.compare ( "VEC3", Qt::CaseInsensitive ) == 0 )
		{
			constexpr qsizetype COMP_COUNT = 3;
			minMaxList.resize ( COMP_COUNT );
			for ( qsizetype i = 0; i < COMP_COUNT; i++ )
			{
				T minVal, maxVal;
				if ( std::is_same<T, float>::value )
				{
					minVal = static_cast< T >( minList.at ( i ).toDouble () );
					maxVal = static_cast< T >( maxList.at ( i ).toDouble () );
				}
				else
				{
					minVal = static_cast< T >( minList.at ( i ).toInteger () );
					maxVal = static_cast< T >( maxList.at ( i ).toInteger () );
				}
				minMaxList [ i ] = std::array<T, 2> ( { minVal, maxVal } );
			}
		}
		else if ( type.compare ( "VEC4", Qt::CaseInsensitive ) == 0 )
		{
			constexpr qsizetype COMP_COUNT = 4;
			minMaxList.resize ( COMP_COUNT );
			for ( qsizetype i = 0; i < COMP_COUNT; i++ )
			{
				T minVal, maxVal;
				if ( std::is_same<T, float>::value )
				{
					minVal = static_cast< T >( minList.at ( i ).toDouble () );
					maxVal = static_cast< T >( maxList.at ( i ).toDouble () );
				}
				else
				{
					minVal = static_cast< T >( minList.at ( i ).toInteger () );
					maxVal = static_cast< T >( maxList.at ( i ).toInteger () );
				}
				minMaxList [ i ] = std::array<T, 2> ( { minVal, maxVal } );
			}
		}
		else if ( type.compare ( "MAT2", Qt::CaseInsensitive ) == 0 )
		{
			constexpr qsizetype COMP_COUNT = 4;
			minMaxList.resize ( COMP_COUNT );
			for ( qsizetype i = 0; i < COMP_COUNT; i++ )
			{
				T minVal, maxVal;
				if ( std::is_same<T, float>::value )
				{
					minVal = static_cast< T >( minList.at ( i ).toDouble () );
					maxVal = static_cast< T >( maxList.at ( i ).toDouble () );
				}
				else
				{
					minVal = static_cast< T >( minList.at ( i ).toInteger () );
					maxVal = static_cast< T >( maxList.at ( i ).toInteger () );
				}
				minMaxList [ i ] = std::array<T, 2> ( { minVal, maxVal } );
			}
		}
		else if ( type.compare ( "MAT3", Qt::CaseInsensitive ) == 0 )
		{
			constexpr qsizetype COMP_COUNT = 9;
			minMaxList.resize ( COMP_COUNT );
			for ( qsizetype i = 0; i < COMP_COUNT; i++ )
			{
				T minVal, maxVal;
				if ( std::is_same<T, float>::value )
				{
					minVal = static_cast< T >( minList.at ( i ).toDouble () );
					maxVal = static_cast< T >( maxList.at ( i ).toDouble () );
				}
				else
				{
					minVal = static_cast< T >( minList.at ( i ).toInteger () );
					maxVal = static_cast< T >( maxList.at ( i ).toInteger () );
				}
				minMaxList [ i ] = std::array<T, 2> ( { minVal, maxVal } );
			}
		}
		else if ( type.compare ( "MAT4", Qt::CaseInsensitive ) == 0 )
		{
			constexpr qsizetype COMP_COUNT = 16;
			minMaxList.resize ( COMP_COUNT );
			for ( qsizetype i = 0; i < COMP_COUNT; i++ )
			{
				T minVal, maxVal;
				if ( std::is_same<T, float>::value )
				{
					minVal = static_cast< T >( minList.at ( i ).toDouble () );
					maxVal = static_cast< T >( maxList.at ( i ).toDouble () );
				}
				else
				{
					minVal = static_cast< T >( minList.at ( i ).toInteger () );
					maxVal = static_cast< T >( maxList.at ( i ).toInteger () );
				}
				minMaxList [ i ] = std::array<T, 2> ( { minVal, maxVal } );
			}
		}

		return minMaxList;
	}
}

#endif // !__GLTF_ACCESSOR_H__

