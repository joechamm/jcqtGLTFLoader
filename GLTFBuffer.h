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

namespace jcqt
{
	class GLTFBuffer : public QObject
	{
		Q_OBJECT

	public:
		GLTFBuffer ( QObject* parent = nullptr );
		GLTFBuffer ( const QJsonObject& jsonObj, QObject* parent = nullptr );
		GLTFBuffer ( const QString& uri, qsizetype byteLength, QObject* parent = nullptr );
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



