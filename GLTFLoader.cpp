#include "GLTFLoader.h"
#include "GLTFException.h"

#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonObject>

namespace jcqt 
{

	GLTFLoader::GLTFLoader ( QObject* parent )
		: QObject ( parent )
	{}

	GLTFLoader::~GLTFLoader ()
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

	bool GLTFLoader::isJsonArray () const
	{
		if ( m_document.isNull () )
			return false;

		return m_document.isArray ();
	}

	bool GLTFLoader::isJsonObject () const
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

	bool GLTFLoader::containsAtTopLevel ( const QString& key ) const
	{
		try
		{
			if ( !isJsonObject () )
			{
				throw new GLTFException ( "INVALID JSON EXCEPTION" );
			}

			return m_document.object ().contains ( key );
		}
		catch ( const GLTFException& e )
		{
			qWarning () << e.what () << Qt::endl;
			return false;
		}
	}

	QJsonObject jcqt::GLTFLoader::getBuffer ( qsizetype bufferIndex ) const
	{
		try
		{
			if (!isJsonObject () )
			{
				throw new GLTFException ( "INVALID JSON EXCEPTION" );
			}

			if ( !containsAtTopLevel ( "buffers" ) )
			{
				throw new GLTFException ( "BUFFERS NOT PRESENT EXCEPTION" );
			}

			QJsonArray buffersArray = m_document.object ().value ( "buffers" ).toArray ();
			if ( buffersArray.count () <= bufferIndex )
			{
				throw new GLTFException ( "INDEX OUT OF RANGE EXCEPTION" );
			}

			return buffersArray [ bufferIndex ].toObject ();
		}
		catch ( const GLTFException& e )
		{
			qWarning () << e.what () << Qt::endl;
			return QJsonObject ();
		}
	}

	QJsonArray GLTFLoader::getBuffers () const
	{
		try
		{
			if ( !isJsonObject () )
			{
				throw new GLTFException ( "INVALID JSON EXCEPTION" );
			}

			if ( !containsAtTopLevel ( "buffers" ) )
			{
				throw new GLTFException ( "BUFFERS NOT PRESENT EXCEPTION" );
			}

			return m_document.object ().value ( "buffers" ).toArray ();
		}
		catch ( const GLTFException& e )
		{
			qWarning () << e.what () << Qt::endl;
			return QJsonArray ();
		}
	}

	void GLTFLoader::printJsonDocument ( int maxDepth ) const
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
			switch ( childVal.type () )
			{
			case QJsonValue::Bool:
				qDebug () << "Type: BOOL, Value: " << ( childVal.toBool () ? "TRUE" : "FALSE" );
				break;
			case QJsonValue::Double:
				qDebug () << "Type: DOUBLE, Value: " << QString::number ( childVal.toDouble () );
				break;
			case QJsonValue::String:
				qDebug () << "Type: STRING, Value: " << childVal.toString ();
				break;
			case QJsonValue::Array:
				qDebug () << "Type: ARRAY, printing elements... " << Qt::endl;
				printJsonArray ( childVal.toArray (), maxDepth - 1 );
				break;
			case QJsonValue::Object:
				qDebug () << "Type: OBJECT, printing keys..." << Qt::endl;
				printJsonObject ( childVal.toObject (), maxDepth - 1 );
				break;
			case QJsonValue::Null:
				qDebug () << "Type: NULL";
				break;
			default:
				qDebug () << "Type: UNKNOWN";
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
				qDebug () << "Type: BOOL, Value: " << ( arrElem.toBool () ? "TRUE" : "FALSE" );
				break;
			case QJsonValue::Double:
				qDebug () << "Type: DOUBLE, Value: " << QString::number ( arrElem.toDouble () );
				break;
			case QJsonValue::String:
				qDebug () << "Type: STRING, Value: " << arrElem.toString ();
				break;
			case QJsonValue::Array:
				qDebug () << "Type: ARRAY, printing elements... " << Qt::endl;
				printJsonArray ( arrElem.toArray (), maxDepth - 1 );
				break;
			case QJsonValue::Object:
				qDebug () << "Type: OBJECT, printing keys..." << Qt::endl;
				printJsonObject ( arrElem.toObject (), maxDepth - 1 );
				break;
			case QJsonValue::Null:
				qDebug () << "Type: NULL";
				break;
			default:
				qDebug () << "Type: UNKNOWN";
				break;
			}

			qDebug () << Qt::endl;
		}

		qDebug () << Qt::endl;
	}
}

