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
		QImage		m_image;
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

	bool isInternal ( const QString& uriStr )
	{
		return uriStr.startsWith ( "data:", Qt::CaseInsensitive );
	}

	bool getImageFromInternal ( const QString& uriStr, QImage& image, const char* mimeType )
	{
		try
		{
			const QString base64Str = "base64,";
			qsizetype idxBase64 = uriStr.indexOf ( base64Str );
			if ( -1 != idxBase64 )
			{
				QString dataStr = uriStr.sliced ( idxBase64 + base64Str.size () );
				QByteArray bufferData = dataStr.toUtf8 ();
				QByteArray::FromBase64Result bufferDataDecoded = QByteArray::fromBase64Encoding ( bufferData );
				if ( bufferDataDecoded.decodingStatus != QByteArray::Base64DecodingStatus::Ok )
				{
					throw new GLTFException ( "BASE 64 DECODING FAILED" );
				}

				QByteArray decodedData = bufferDataDecoded.decoded;

				image = QImage::fromData ( decodedData, mimeType );
				return true;
			}

			const QString octStreamStr = "application/octet-stream,";
			qsizetype idxOctStream = uriStr.indexOf ( octStreamStr );
			if ( -1 != idxOctStream )
			{
				QString dataStr = uriStr.sliced ( idxOctStream + octStreamStr.size () );
				QByteArray bufferData = dataStr.toUtf8 ();
				QByteArray::FromBase64Result bufferDataDecoded = QByteArray::fromBase64Encoding ( bufferData );
				if ( bufferDataDecoded.decodingStatus != QByteArray::Base64DecodingStatus::Ok )
				{
					throw new GLTFException ( "BASE 64 DECODING FAILED" );
				}

				QByteArray decodedData = bufferDataDecoded.decoded;

				image = QImage::fromData ( decodedData, mimeType );
				return true;
			}

			const QString gltfBufferStr = "application/gltf-buffer,";
			qsizetype gltfBufferIdx = uriStr.indexOf ( gltfBufferStr );
			if ( -1 != gltfBufferIdx )
			{
				QString dataStr = uriStr.sliced ( gltfBufferIdx + gltfBufferStr.size () );
				QByteArray bufferData = dataStr.toUtf8 ();
				QByteArray::FromBase64Result bufferDataDecoded = QByteArray::fromBase64Encoding ( bufferData );
				if ( bufferDataDecoded.decodingStatus != QByteArray::Base64DecodingStatus::Ok )
				{
					throw new GLTFException ( "BASE 64 DECODING FAILED" );
				}

				QByteArray decodedData = bufferDataDecoded.decoded;

				image = QImage::fromData ( decodedData, mimeType );
				return true;
			}
		}
		catch ( const GLTFException& e )
		{
			qWarning () << e.what () << Qt::endl;
			return false;
		}		

		return false;
	}

	QString getFilename ( const QString& uriStr )
	{
		QUrl url ( uriStr );
		return url.path ();
	}

	Image createImageFromJson ( const QJsonObject& jsonObj )
	{
		Image image;
				
		try
		{
			if ( !isValidJsonGLTFImage ( jsonObj ) )
			{
				throw new GLTFException ( "INVALID JSON EXCEPTION" );
			}

			if ( jsonObj.contains ( "uri" ) )
			{
				QString uriStr = jsonObj.value ( "uri" ).toString ();
				if ( isInternal ( uriStr))
				{
					QString mimeType = jsonObj.value ( "mimeType" ).toString ();
					if ( mimeType.isEmpty () ) // use image/jpeg for default mimeType
					{
						getImageFromInternal ( uriStr, image.m_image, "image/jpeg");
					}
					else
					{
						getImageFromInternal ( uriStr, image.m_image, mimeType.toStdString ().c_str () );
					}					
				}
				else
				{
					QString filename = getFilename ( uriStr );
					image.m_image.load ( filename );
				}
			}

			image.m_jsonObject = jsonObj;
			return image;
		}
		catch ( const GLTFException& e )
		{
			qWarning () << e.what () << Qt::endl;			
		}

		return image;
	}
}

#endif // __GLTF_IMAGE_H__