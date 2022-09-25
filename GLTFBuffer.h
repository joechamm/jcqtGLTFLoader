/*****************************************************************//**
 * \file   GLTFBuffer.h
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
 * \brief  Encapsulate a glTF buffer
 * 
 * \author joechamm
 * \date   September 2022
 *********************************************************************/
#ifndef __GLTF_BUFFER_H__
#define __GLTF_BUFFER_H__

#include <QByteArray>
#include <QJsonObject>
#include <QUrl>
#include <QFile>

#include "GLTFException.h"

namespace jcqt
{
	struct Buffer
	{
		QByteArray m_data;
		QJsonObject m_jsonObject;
	};

	bool isValidGLTFBufferFromJson ( const QJsonObject& jsonObj )
	{
		return ( jsonObj.contains ( "byteLength" ) );
	}

	QString getBufferFilenameFromUriString ( const QString& uriString )
	{
		QUrl url ( uriString );
		return url.path ();
	}

	Buffer createBufferFromJson ( const QJsonObject& jsonObj )
	{
		Buffer buffer;

		try
		{
			if ( !jsonObj.contains ( "byteLength" ) )
			{
				throw new GLTFException ( "INVALID JSON EXCEPTION" );
			}

			quint64 byteLength = jsonObj.value ( "byteLength" ).toInteger ();
			// if uri is not present, we just set the size of the buffer and return it
			if ( !jsonObj.contains ( "uri" ) )
			{
				buffer.m_data.resize ( byteLength );
				buffer.m_jsonObject = jsonObj;
				return buffer;
			}

			QString uriVal = jsonObj.value ( "uri" ).toString ();
			if ( uriVal.startsWith ( "data:" ) ) // buffer data is internal
			{
				// Start by checking if the uri string contains the substring 'base64,'. If so, the data portion will follow immediately after and we can base64 decode to our m_bufferData.
				const QString base64Str = "base64,";
				qsizetype idxBase64 = uriVal.indexOf ( base64Str );
				if ( -1 != idxBase64 )
				{
					QByteArray::FromBase64Result tempBuffer = QByteArray::fromBase64Encoding ( uriVal.sliced ( idxBase64 + base64Str.size () ).toUtf8 () );
					if ( tempBuffer.decodingStatus != QByteArray::Base64DecodingStatus::Ok )
					{
						throw new GLTFException ( "BASE 64 DECODING FAIL EXCEPTION" );
					}

					if ( tempBuffer.decoded.size () != byteLength )
					{
						throw new GLTFException ( "BYTE LENGTH MISMATCH EXCEPTION" );
					}

					buffer.m_data.setRawData ( tempBuffer.decoded.constData (), byteLength );
					buffer.m_jsonObject = jsonObj;
					return buffer;
				}

				// check if the optional 'base64, extension is not present, but media type is 'application/octet-stream' or 'application/gltf-buffer' 
				const QString octStreamStr = "application/octet-stream,";
				qsizetype octStreamIdx = uriVal.indexOf ( octStreamStr );
				if ( -1 != octStreamIdx )
				{
					// since we don't know if the data is base64 encoded here, we'll check if the byte length matches the sliced string portion.
					QByteArray dataBuffer = uriVal.sliced ( octStreamIdx + octStreamStr.size () ).toUtf8 ();
					if ( dataBuffer.size () == byteLength )
					{
						buffer.m_data.setRawData ( dataBuffer, byteLength );
						buffer.m_jsonObject = jsonObj;
						return buffer;
					}

					QByteArray::FromBase64Result tempBuffer = QByteArray::fromBase64Encoding ( dataBuffer );
					if ( tempBuffer.decodingStatus != QByteArray::Base64DecodingStatus::Ok )
					{
						throw new GLTFException ( "BASE 64 DECODING FAIL EXCEPTION" );
					}

					if ( tempBuffer.decoded.size () != byteLength )
					{
						throw new GLTFException ( "BYTE LENGTH MISMATCH EXCEPTION" );
					}

					buffer.m_data.setRawData ( tempBuffer.decoded.constData (), byteLength );
					buffer.m_jsonObject = jsonObj;
					return buffer;
				}

				// Same thing with application/gltf-buffer
				const QString gltfBufferStr = "application/gltf-buffer,";
				qsizetype gltfBufferIdx = uriVal.indexOf ( gltfBufferStr );
				if ( -1 != gltfBufferIdx )
				{
					QByteArray dataBuffer = uriVal.sliced ( gltfBufferIdx + gltfBufferStr.size () ).toUtf8 ();
					if ( dataBuffer.size () == byteLength )
					{
						buffer.m_data.setRawData ( dataBuffer, byteLength );
						buffer.m_jsonObject = jsonObj;
						return buffer;
					}

					QByteArray::FromBase64Result tempBuffer = QByteArray::fromBase64Encoding ( dataBuffer );
					if ( tempBuffer.decodingStatus != QByteArray::Base64DecodingStatus::Ok )
					{
						throw new GLTFException ( "BASE 64 DECODING FAIL EXCEPTION" );
					}

					if ( tempBuffer.decoded.size () != byteLength )
					{
						throw new GLTFException ( "BYTE LENGTH MISMATCH EXCEPTION" );
					}

					buffer.m_data.setRawData ( tempBuffer.decoded.constData (), byteLength );
					buffer.m_jsonObject = jsonObj;
					return buffer;
				}
				else
				{
					throw new GLTFException ( "INVALID DATA FROM JSON" );
				}
			}
			else // uri is filename
			{
				QString filename = getBufferFilenameFromUriString ( uriVal );
				QFile f ( filename );
				if ( !f.exists () )
				{
					throw new GLTFException ( "FILE NOT FOUND EXCEPTION" );
				}

				if ( !f.open ( QIODeviceBase::ReadOnly ) )
				{
					throw new GLTFException ( "OPEN FILE FAILURE EXCEPTION" );
				}

				buffer.m_data = f.readAll ();
				if ( buffer.m_data.size () != byteLength )
				{
					buffer.m_data.clear ();
					f.close ();
					throw new GLTFException ( "BYTE MISMATCH EXCEPTION" );
				}

				buffer.m_jsonObject = jsonObj;
				return buffer;
			}
		}
		catch ( const GLTFException& e )
		{
			qWarning () << e.what () << Qt::endl;
		}

		return buffer;
	}
}

#endif // !__GLTF_BUFFER_H__



