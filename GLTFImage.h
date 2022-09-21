/*****************************************************************//**
 * \file   GLTFImage.h
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
 * \brief  Encapsulate a glTF image
 * 
 * \author joechamm
 * \date   September 2022
 *********************************************************************/
#ifndef __GLTF_IMAGE_H__
#define __GLTF_IMAGE_H__

#include <QObject>
#include <QJsonObject>
#include <QImage>
#include <QUrl>
#include <QDir>

#include "GLTFException.h"

namespace jcqt
{
	struct Image
	{
		QImage m_image;
		QJsonObject m_jsonObject;
	};

	bool isValidJsonGLTFImage ( const QJsonObject& jsonObj )
	{
		bool hasUri = jsonObj.contains ( "uri" );
		bool hasBufferView = jsonObj.contains ( "bufferView" );
		bool hasMimeType = jsonObj.contains ( "mimeType" );

		if ( hasUri )
		{
			return !hasBufferView;
		}

		if ( hasBufferView )
		{
			return hasMimeType;
		}

		return false;
	}

	bool isInternal ( const QJsonObject& jsonObj )
	{
		// TODO: implement
		return false;
	}

	QString getFilename ( const QJsonObject& jsonObj )
	{
		// TODO: implement
		return QString ();
	}

	Image createImageFromJson ( const QJsonObject& jsonObj )
	{
		Image image;
		image.m_jsonObject = jsonObj;
		
		try
		{
			if ( !isValidJsonGLTFImage ( jsonObj ) )
			{
				throw new GLTFException ( "INVALID JSON EXCEPTION" );
			}

			if ( jsonObj.contains ( "uri" ) )
			{
				if ( isInternal ( jsonObj ) )
				{
					// TODO: handle case
				}
				else
				{
					QString filename = getFilename ( jsonObj );
					image.m_image.load ( filename );
				}
			}

			return image;
		}
		catch ( const GLTFException& e )
		{
			qWarning () << e.what () << Qt::endl;
			return image;
		}
	}
}

#endif // __GLTF_IMAGE_H__