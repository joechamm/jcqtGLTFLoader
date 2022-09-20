#include "GLTFImage.h"

namespace jcqt
{
	GLTFImage::GLTFImage ( QObject* parent )
	{}
	GLTFImage::GLTFImage ( const QJsonObject & jsonObj, QObject * parent )
	{}
	GLTFImage::~GLTFImage ()
	{}

	bool GLTFImage::isValidJson ( const QJsonObject& jsonObj )
	{
		bool hasUri = jsonObj.contains ( "uri" );
		bool hasBufferView = jsonObj.contains ( "bufferView" );
		bool hasMimeType = jsonObj.contains ( "mimeType" );

		if ( hasUri )
		{
			return ( !hasBufferView );
		}

		if ( hasBufferView )
		{
			return hasMimeType;
		}

		return false;
	}

	QString GLTFImage::getUriFromJson ( const QJsonObject & jsonObj )
	{
		return jsonObj.value ( "uri" ).toString ();		
	}

	QString GLTFImage::getMimeTypeFromJson ( const QJsonObject& jsonObj )
	{
		return jsonObj.value ( "mimeType" ).toString ();
	}

	qsizetype GLTFImage::getBufferViewIndexFromJson ( const QJsonObject& jsonObj )
	{
		return qsizetype (jsonObj.value("bufferView").toInteger());
	}

	QString GLTFImage::getNameFromJson ( const QJsonObject& jsonObj )
	{
		return jsonObj.value ( "name" ).toString ();
	}

	QJsonObject GLTFImage::getExtensionsFromJson ( const QJsonObject& jsonObj )
	{
		return jsonObj.value ( "extensions" ).toObject ();		
	}

	QJsonObject GLTFImage::getExtrasFromJson ( const QJsonObject& jsonObj )
	{
		return jsonObj.value ( "extras" ).toObject ();
	}

	bool GLTFImage::loadImageFromFilename ( const QString& filename, QImage& image )
	{
		return image.load ( filename );
	}
}
