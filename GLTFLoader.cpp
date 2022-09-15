#include "GLTFLoader.h"

#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonObject>

GLTFLoader::GLTFLoader(QObject *parent)
	: QObject(parent)
{}

GLTFLoader::~GLTFLoader()
{}

bool GLTFLoader::loadGLTF ( const QString& filename )
{
	// check the extension is gltf first
	QFileInfo fi ( filename );
	QString ext = fi.suffix ();
	if ( ext.compare ( "gltf" ) != 0 )
	{
		qWarning () << "Filename must use 'gltf' extension" << Qt::endl;
		return false;
	}

	QFile loadFile ( filename );

	if ( !loadFile.open ( QIODevice::ReadOnly ) )
	{
		qWarning () << "Couldn't open " << filename << Qt::endl;
		return false;
	}

	QByteArray data = loadFile.readAll ();

	QJsonParseError errParse;

	m_document = QJsonDocument::fromJson ( data, &errParse );

	if ( m_document.isNull () )
	{
		qWarning () << "Failed to parse JSON document from " << filename << Qt::endl << "JsonParseError: " << errParse.errorString () << Qt::endl;
		return false;
	}

	return true;
}

bool GLTFLoader::isJsonArray ()
{
	if ( m_document.isNull () )
		return false;

	return m_document.isArray ();	
}

bool GLTFLoader::isJsonObject ()
{
	if ( m_document.isNull () )
		return false;

	return m_document.isObject ();
}

QJsonArray GLTFLoader::jsonArray () const
{
	if ( m_document.isNull () )
		return QJsonArray ();

	return m_document.array ();
}

QJsonObject GLTFLoader::jsonObject () const
{
	if ( m_document.isNull () )
		return QJsonObject ();

	return m_document.object ();
}

void GLTFLoader::printJsonDocument (int maxDepth) const
{
	qDebug () << "Attempting to print JSON document" << Qt::endl;

	if ( m_document.isNull () )
	{
		qDebug () << "Document is NULL" << Qt::endl;
		return;
	}		

	if ( m_document.isEmpty () )
	{
		qDebug () << "Document is EMPTY" << Qt::endl;
		return;
	}

	if ( m_document.isObject () )
	{
		qDebug () << "JSON OBJECT: " << Qt::endl;
		printJsonObject ( m_document.object (), maxDepth );
		return;
	}

	if ( m_document.isArray () )
	{
		qDebug () << "JSON ARRAY: " << Qt::endl;
		printJsonArray ( m_document.array (), maxDepth );
		return;
	}		
}

void GLTFLoader::printJsonObject ( const QJsonObject& obj, int maxDepth ) const
{
	if ( maxDepth < 0 )
	{
		qDebug () << "MAX DEPTH REACHED" << Qt::endl;
		return;
	}
	QStringList objKeys = obj.keys ();
	for ( const QString& key : objKeys )
	{
		qDebug () << key << ": ";
		QJsonValue childVal = obj [ key ];
		switch ( childVal.type() )
		{
		case QJsonValue::Bool:
			qDebug () << (childVal.toBool () ? "TRUE" : "FALSE");
			break;
		case QJsonValue::Double:
			qDebug () << QString::number ( childVal.toDouble () );
			break;
		case QJsonValue::String:
			qDebug () << childVal.toString ();
			break;
		case QJsonValue::Array:
			printJsonArray ( childVal.toArray (), maxDepth - 1 );
			break;
		case QJsonValue::Object:
			printJsonObject ( childVal.toObject (), maxDepth - 1 );
			break;
		case QJsonValue::Null:
			qDebug () << "NULL";
			break;
		default:
			qDebug () << "UNKNOWN TYPE";
			break;
		}

		qDebug () << Qt::endl;
	}

	qDebug () << Qt::endl;
}

void GLTFLoader::printJsonArray ( const QJsonArray& arr, int maxDepth ) const
{
	if ( maxDepth < 0 )
	{
		qDebug () << "MAX DEPTH REACHED" << Qt::endl;
		return;
	}

	const qsizetype arrSize = arr.size ();
	for ( qsizetype i = 0; i < arrSize; ++i )
	{
		QJsonValue arrElem = arr [ i ];
		switch ( arrElem.type () )
		{
		case QJsonValue::Bool:
			qDebug () << ( arrElem.toBool () ? "TRUE" : "FALSE" );
			break;
		case QJsonValue::Double:
			qDebug () << QString::number ( arrElem.toDouble () );
			break;
		case QJsonValue::String:
			qDebug () << arrElem.toString ();
			break;
		case QJsonValue::Array:
			printJsonArray ( arrElem.toArray (), maxDepth - 1 );
			break;
		case QJsonValue::Object:
			printJsonObject ( arrElem.toObject (), maxDepth - 1 );
			break;
		case QJsonValue::Null:
			qDebug () << "NULL";
			break;
		default:
			qDebug () << "UNKNOWN TYPE";
			break;
		}

		qDebug () << Qt::endl;
	}

	qDebug () << Qt::endl;
}
