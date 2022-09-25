/*****************************************************************//**
 * \file   GLTFMesh.h
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
 * \brief  Encapsulate a glTF mesh
 * 
 * \author joechamm
 * \date   September 2022
 *********************************************************************/
#ifndef __GLTF_MESH_H__
#define __GLTF_MESH_H__

#include <QJsonObject>
#include <QJsonArray>
#include <QList>

#include "GLTFException.h"
#include "GLTFNode.h"

namespace jcqt
{
	enum class ePrimitiveMode : quint8
	{
		ePrimitiveMode_POINTS = 0,
		ePrimitiveMode_LINES = 1,
		ePrimitiveMode_LINE_LOOP = 2,
		ePrimitiveMode_LINE_STRIP = 3,
		ePrimitiveMode_TRIANGLES = 4,  // DEFAULT
		ePrimitiveMode_TRIANGLE_STRIP = 5,
		ePrimitiveMode_TRIANGLE_FAN = 6,
		ePrimitiveMode_INVALID = 99
	};

	struct MeshPrimitive
	{
		qsizetype m_indicesIdx = 0; // index of the accessor that contains the vertex indices
		qsizetype m_materialIdx = 0;
		ePrimitiveMode m_mode = ePrimitiveMode::ePrimitiveMode_TRIANGLES;
		QJsonObject m_jsonObject;
	};

	struct Mesh
	{
		QList<MeshPrimitive>	m_primitives;
		QList<float>			m_weights;
		QJsonObject				m_jsonObject;
	};

	MeshPrimitive getMeshPrimitiveFromJson ( const QJsonObject& jsonObj )
	{
		MeshPrimitive prim;
		if ( jsonObj.contains ( "indices" ) )
			prim.m_indicesIdx = jsonObj.value ( "indices" ).toInteger ();

		if ( jsonObj.contains ( "material" ) )
			prim.m_materialIdx = jsonObj.value ( "material" ).toInteger ();

		if ( jsonObj.contains ( "mode" ) )
		{
			qsizetype mode = jsonObj.value ( "mode" ).toInteger ();
			switch ( mode )
			{
			case 0: // POINTS
				prim.m_mode = ePrimitiveMode::ePrimitiveMode_POINTS;
				break;
			case 1: // LINES
				prim.m_mode = ePrimitiveMode::ePrimitiveMode_LINES;
				break;
			case 2: // LINE_LOOP
				prim.m_mode = ePrimitiveMode::ePrimitiveMode_LINE_LOOP;
				break;
			case 3: // LINE_STRIP
				prim.m_mode = ePrimitiveMode::ePrimitiveMode_LINE_STRIP;
				break;
			case 4: // TRIANGLES
				prim.m_mode = ePrimitiveMode::ePrimitiveMode_TRIANGLES;
				break;
			case 5: // TRIANGLE_STRIP
				prim.m_mode = ePrimitiveMode::ePrimitiveMode_TRIANGLE_STRIP;
				break;
			case 6: // TRIANGLE_FAN
				prim.m_mode = ePrimitiveMode::ePrimitiveMode_TRIANGLE_FAN;
				break;
			default:
				prim.m_mode = ePrimitiveMode::ePrimitiveMode_INVALID;
				break;
			}
		}

		prim.m_jsonObject = jsonObj;
		return prim;
	}

	void getPrimitivesFromJsonArray ( const QJsonArray& jsonArr, QList<MeshPrimitive>& prims )
	{
		qsizetype count = jsonArr.size ();
		prims.resize ( count );
		for ( qsizetype i = 0; i < count; i++ )
		{
			prims [ i ] = getMeshPrimitiveFromJson ( jsonArr.at ( i ).toObject () );
		}
	}

	//void getWeightsFromJsonArray ( const QJsonArray& jsonArr, QList<float>& weights )
	//{
	//	qsizetype count = jsonArr.size ();
	//	weights.resize ( count );
	//	for ( qsizetype i = 0; i < count; i++ )
	//	{
	//		weights [ i ] = static_cast< float >( jsonArr.at ( i ).toDouble ( 1.0 ) );
	//	}
	//}

	Mesh createMeshFromJson ( const QJsonObject& jsonObj )
	{
		Mesh mesh;
		try
		{
			if ( !jsonObj.contains ( "primitives" ) )
			{
				throw new GLTFException ( "INVALID JSON EXCEPTION" );
			}

			getPrimitivesFromJsonArray ( jsonObj.value ( "primitives" ).toArray (), mesh.m_primitives );

			if ( jsonObj.contains ( "weights" ) )
			{
				getWeightsFromJsonArray ( jsonObj.value ( "weights" ).toArray (), mesh.m_weights );
			}
				
			mesh.m_jsonObject = jsonObj;
			return mesh;
		}
		catch ( const GLTFException& e )
		{
			qWarning () << e.what () << Qt::endl;
		}		

		return mesh;
	}
}

#endif // !__GLTF_MESH_H__
