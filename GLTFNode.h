/*****************************************************************//**
 * \file   GLTFNode.h
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
 * \brief  Encapsulate a glTF node
 * 
 * \author joechamm
 * \date   September 2022
 *********************************************************************/
#ifndef __GLTF_NODE_H__
#define __GLTF_NODE_H__

#include <QJsonObject>
#include <QJsonArray>
#include <QList>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/quaternion.hpp>

namespace jcqt
{
	struct Node
	{
		qsizetype			m_cameraIdx = -1;
		qsizetype			m_skinIdx = -1;
		qsizetype			m_meshIdx = -1;
		glm::mat4			m_matrix = glm::mat4 ( 1.0f );
		QList<qsizetype>	m_children;
		QList<float>		m_weights;
		QJsonObject			m_jsonObject;
	};

	glm::quat getQuatFromJsonArray ( const QJsonArray& jsonArr )
	{
		float x = static_cast< float >( jsonArr.at ( 0 ).toDouble () );
		float y = static_cast< float >( jsonArr.at ( 1 ).toDouble () );
		float z = static_cast< float >( jsonArr.at ( 2 ).toDouble () );
		float w = static_cast< float >( jsonArr.at ( 3 ).toDouble ( 1.0 ) );
		return glm::quat ( w, x, y, z );
	}

	glm::vec3 getTransFromJsonArray ( const QJsonArray& jsonArr )
	{
		float x = static_cast< float >( jsonArr.at ( 0 ).toDouble () );
		float y = static_cast< float >( jsonArr.at ( 1 ).toDouble () );
		float z = static_cast< float >( jsonArr.at ( 2 ).toDouble () );		
		return glm::vec3 ( x, y, z );
	}

	glm::vec3 getScaleFromJsonArray ( const QJsonArray& jsonArr )
	{
		float x = static_cast< float >( jsonArr.at ( 0 ).toDouble (1.0) );
		float y = static_cast< float >( jsonArr.at ( 1 ).toDouble (1.0) );
		float z = static_cast< float >( jsonArr.at ( 2 ).toDouble (1.0) );
		return glm::vec3 ( x, y, z );
	}

	glm::mat4 createMat4FromJsonArray ( const QJsonArray& jsonArr )
	{
		glm::mat4 matrix;

		for ( qsizetype col = 0; col < 4; col++ )
		{
			glm::vec4 columnVec;
			for ( qsizetype row = 0; row < 4; row++ )
			{
				columnVec [ row ] = static_cast< float >( jsonArr.at ( col * 4 + row ).toDouble ( ( row == col ) ? 1.0 : 0.0 ) );
			}

			matrix [ col ] = columnVec;
		}

		return matrix;
	}

	glm::mat4 createMat4FromTRS ( const QJsonArray& trans, const QJsonArray& rot, const QJsonArray& scale )
	{
		glm::vec3 scaleVec = getScaleFromJsonArray ( scale );
		glm::vec3 transVec = getTransFromJsonArray ( trans );
		glm::quat rotQuat = getQuatFromJsonArray ( rot );
		glm::mat4 scaleMat = glm::scale ( glm::mat4 ( 1.0f ), scaleVec );
		glm::mat4 transMat = glm::translate ( glm::mat4 ( 1.0f ), transVec );
		glm::mat4 rotMat = glm::mat4_cast ( rotQuat );
		return transMat * rotMat * scaleMat;
	}

	void getChildrenFromJsonArray ( const QJsonArray& jsonArr, QList<qsizetype>& children )
	{
		qsizetype count = jsonArr.size ();
		children.resize ( count );
		for ( qsizetype i = 0; i < count; i++ )
		{
			children [ i ] = jsonArr.at ( i ).toInteger ();
		}
	}

	void getWeightsFromJsonArray ( const QJsonArray& jsonArr, QList<float>& weights )
	{
		qsizetype count = jsonArr.size ();
		weights.resize ( count );
		for ( qsizetype i = 0; i < count; i++ )
		{
			weights [ i ] = static_cast<float>(jsonArr.at ( i ).toDouble (1.0));
		}
	}

	Node createNodeFromJson ( const QJsonObject& jsonObj )
	{
		Node node;
		if ( jsonObj.contains ( "camera" ) )
			node.m_cameraIdx = jsonObj.value ( "camera" ).toInteger ();

		if ( jsonObj.contains ( "skin" ) )
			node.m_skinIdx = jsonObj.value ( "skin" ).toInteger ();

		if ( jsonObj.contains ( "mesh" ) )
			node.m_meshIdx = jsonObj.value ( "mesh" ).toInteger ();

		if ( jsonObj.contains ( "matrix" ) )
		{
			node.m_matrix = createMat4FromJsonArray ( jsonObj.value ( "matrix" ).toArray () );
		}
		else
		{
			node.m_matrix = createMat4FromTRS ( jsonObj.value ( "translation" ).toArray (), jsonObj.value ( "rotation" ).toArray (), jsonObj.value ( "scale" ).toArray () );
		}

		if ( jsonObj.contains ( "children" ) )
		{
			getChildrenFromJsonArray ( jsonObj.value ( "children" ).toArray (), node.m_children );
		}

		if ( jsonObj.contains ( "weights" ) )
		{
			getWeightsFromJsonArray ( jsonObj.value ( "weights" ).toArray (), node.m_weights );
		}

		node.m_jsonObject = jsonObj;
		return node;
	}
}

#endif // !__GLTF_NODE_H__
