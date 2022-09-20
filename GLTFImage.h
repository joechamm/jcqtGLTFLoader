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

namespace jcqt
{
	class GLTFImage : public QObject
	{
		Q_OBJECT

	public:
		GLTFImage ( QObject* parent = nullptr );
		GLTFImage ( const QJsonObject& jsonObj, QObject* parent = nullptr );
		~GLTFImage ();

		/*
			According to the glTF reference page https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-image, the 'bufferView' field MUST NOT be defined when the 'uri' field is defined.
			If the 'bufferView' field is defined, then the 'mimeType' field must also be defined.
		*/
		static bool isValidJson ( const QJsonObject& jsonObj );
		static QString getUriFromJson ( const QJsonObject& jsonObj );
		static QString getMimeTypeFromJson ( const QJsonObject& jsonObj );
		static qsizetype getBufferViewIndexFromJson ( const QJsonObject& jsonObj );
		static QString getNameFromJson ( const QJsonObject& jsonObj );
		static QJsonObject getExtensionsFromJson ( const QJsonObject& jsonObj );
		static QJsonObject getExtrasFromJson ( const QJsonObject& jsonObj );

		static bool loadImageFromFilename ( const QString& filename, QImage& image );

	private:
		QJsonObject m_jsonObject;
		QImage m_image;
	};
}

#endif // __GLTF_IMAGE_H__