/*****************************************************************//**
 * \file   GLTFSkin.h
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
#ifndef __GLTF_SKIN_H__
#define __GLTF_SKIN_H__

#include <QJsonObject>
#include <QJsonArray>
#include <QSet>
#include <QList>

#include "GLTFException.h"

namespace jcqt
{
	struct Skin
	{
		qsizetype			m_inverseBindMatricesIdx = -1;	// index of the accessor containing the floating-point 4x4 inverse-bind matrices (use identity if not set)
		qsizetype			m_skeletonIdx = -1;				// index of the node used as a skeleton root
		QList<qsizetype>	m_joints;
		QJsonObject			m_jsonObject;
	};

	Skin createSkinFromJson ( const QJsonObject& jsonObj )
	{
		Skin skin;

		try
		{
			if ( !jsonObj.contains ( "joints" ) )
			{
				throw new GLTFException ( "INVALID JSON EXCEPTION" );
			}
		
			if ( jsonObj.contains ( "inverseBindMatrices" ) )
				skin.m_inverseBindMatricesIdx = jsonObj.value ( "inverseBindMatrices" ).toInteger ();

			if ( jsonObj.contains ( "skeleton" ) )
				skin.m_skeletonIdx = jsonObj.value ( "skeleton" ).toInteger ();

			QJsonArray joints = jsonObj.value ( "joints" ).toArray ();
			qsizetype count = joints.count ();
			skin.m_joints.resize ( count );
			for ( qsizetype i = 0; i < count; i++ )
			{
				skin.m_joints [ i ] = joints.at ( i ).toInteger ();
			}

			skin.m_jsonObject = jsonObj;
			return skin;
		}
		catch ( const GLTFException& e )
		{
			qWarning () << e.what () << Qt::endl;
		}

		return skin;
	}
}

#endif // !__GLTF_SKIN_H__
