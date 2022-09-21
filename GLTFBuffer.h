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
#include <QDir>

#include "GLTFException.h"

namespace jcqt
{
	struct jcqtBuffer
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

	jcqtBuffer getBufferFromJson ( const QJsonObject& jsonObj )
	{
		jcqtBuffer buffer;

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

	class GLTFBuffer : public QObject
	{
		Q_OBJECT

	public:
		GLTFBuffer ( QObject* parent = nullptr );
		GLTFBuffer ( const QJsonObject& jsonObj, QObject* parent = nullptr );
//		GLTFBuffer ( const QString& uri, qsizetype byteLength, QObject* parent = nullptr );
		~GLTFBuffer ();

		/**
		 * isValidJson
		 * bool
		 * \brief quick check that the QJsonObject has valid gltf buffer format
		 *
		 * \param jsonObj
		 * \return true if jsonObj contains the keys 'byteLength' and 'uri'
		 */
		static bool isValidJson ( const QJsonObject& jsonObj );
		/**
		 * isUriInternal
		 * bool
		 * \brief check if the uri points to internal data or external.
		 *
		 * \param jsonObj
		 * \return true if
		 */
		static bool isUriInternal ( const QJsonObject& jsonObj );
		
		// glTF buffer
		static bool jsonHasUri ( const QJsonObject& jsonObj );
		static bool jsonHasByteLength ( const QJsonObject& jsonObj );
		static bool jsonHasName ( const QJsonObject& jsonObj );
		static bool jsonHasExtensions ( const QJsonObject& jsonObj );
		static bool jsonHasExtras ( const QJsonObject& jsonObj );

		static QString uriStringFromJson ( const QJsonObject& jsonObj );
		static QString nameStringFromJson ( const QJsonObject& jsonObj );
		static QUrl urlFromJson ( const QJsonObject& jsonObj );
		static qsizetype byteLengthFromJson ( const QJsonObject& jsonObj );

		static bool externalUriExists ( const QJsonObject& jsonObj );
		static bool isRelativeFromJson ( const QJsonObject& jsonObj );
		static QString uriSchemeFromJson ( const QJsonObject& jsonObj );		
		
		/**
		 * setFromJson
		 * bool
		 * Use this to set the m_jsonObject member variable.
		 * 
		 * \param jsonObj
		 * \return true if hasData returns false and jsonObj is valid
		 */
		bool setFromJson ( const QJsonObject& jsonObj );

		bool hasData () const;
		bool jsonValid () const;

		const char* constData () const;
		const char* data () const;

		bool loadData ();
		bool loadDataFromExternalFile (const QString& filename, qsizetype byteLength);
		bool loadDataFromInternalString ( const QString& uriString, qsizetype byteLength );
		/**
		 * handleInternalData
		 * bool
		 * Assumes that m_jsonObject contains 'uri' key, and the value is a base64 encoded string.
		 * 
		 * \return 
		 */
		bool handleInternalData ();
		void clearBufferData ();
		
		bool setDirPath ( const QString& path );
		QString getDirPath () const;
		QString getFilePath () const;
	private:
		qsizetype m_byteLength;
		QByteArray m_bufferData;
		QJsonObject m_jsonObject;
		
		QDir m_dirPath;
	};
}

#endif // !__GLTF_BUFFER_H__



