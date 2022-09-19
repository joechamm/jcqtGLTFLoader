#include "GLTFBuffer.h"
#include "GLTFException.h"

#include <QURLQuery>
#include <QFile>

namespace jcqt
{

	GLTFBuffer::GLTFBuffer ( QObject* parent )
		: QObject ( parent )
		, m_byteLength(0)
		, m_dirPath(QDir::current())
	{}

	GLTFBuffer::GLTFBuffer ( const QJsonObject & jsonObj, QObject * parent ) 
		: QObject(parent)
		, m_jsonObject(jsonObj)
		, m_byteLength(0)
		, m_dirPath(QDir::current())
	{}

	// TODO: implement
	GLTFBuffer::GLTFBuffer ( const QString & uri, qsizetype byteLength, QObject * parent )
	{}

	GLTFBuffer::~GLTFBuffer ()
	{}

	bool GLTFBuffer::isValidJson ( const QJsonObject& jsonObj )
	{
		return ( jsonObj.contains ( "byteLength" ) );
	}

	bool GLTFBuffer::isUriInternal ( const QJsonObject& jsonObj )
	{
		QJsonValue uriVal = jsonObj.value ( "uri" );
		if ( uriVal.isString () )
		{
			return uriVal.toString ().startsWith ( "data:", Qt::CaseInsensitive );
		}
		return false;
	}

	bool GLTFBuffer::jsonHasUri ( const QJsonObject& jsonObj )
	{
		return jsonObj.contains ( "uri" );
	}

	bool GLTFBuffer::jsonHasByteLength ( const QJsonObject& jsonObj )
	{
		return jsonObj.contains ( "byteLength" );
	}

	bool GLTFBuffer::jsonHasName ( const QJsonObject& jsonObj )
	{
		return jsonObj.contains ( "name" );
	}

	bool GLTFBuffer::jsonHasExtensions ( const QJsonObject& jsonObj )
	{
		return jsonObj.contains("extensions");
	}

	bool GLTFBuffer::jsonHasExtras ( const QJsonObject& jsonObj )
	{
		return jsonObj.contains("extras");
	}

	QString GLTFBuffer::uriStringFromJson ( const QJsonObject& jsonObj )
	{
		return jsonObj.value ( "uri" ).toString ();		
	}

	QString GLTFBuffer::nameStringFromJson ( const QJsonObject& jsonObj )
	{
		return jsonObj.value ( "name" ).toString ();
	}

	bool GLTFBuffer::externalUriExists ( const QJsonObject& jsonObj )
	{
		QJsonValue uriVal = jsonObj.value ( "uri" );
		if ( uriVal.isString () )
		{
			QString uriStr = uriVal.toString ();
			return QFile::exists ( uriStr );
		}

		return false;
	}

	bool GLTFBuffer::isRelativeFromJson ( const QJsonObject& jsonObj )
	{
		return QUrl ( jsonObj.value ( "uri" ).toString () ).isRelative ();
	}

	QString GLTFBuffer::uriSchemeFromJson ( const QJsonObject& jsonObj )
	{
		return QUrl ( jsonObj.value ( "uri" ).toString () ).scheme ();
	}

	QUrl GLTFBuffer::urlFromJson ( const QJsonObject& jsonObj )
	{
		return QUrl ( jsonObj.value ( "uri" ).toString () );
	}

	qsizetype GLTFBuffer::byteLengthFromJson ( const QJsonObject& jsonObj )
	{
		return qsizetype ( jsonObj.value ( "byteLength" ).toInteger () );
	}

	bool GLTFBuffer::setFromJson ( const QJsonObject& jsonObj )
	{
		if ( hasData () || !isValidJson ( jsonObj ) )
			return false;
		m_jsonObject = jsonObj;
		return true;
	}

	bool GLTFBuffer::hasData () const
	{
		return m_byteLength > 0;
	}

	bool GLTFBuffer::jsonValid () const
	{
		return isValidJson(m_jsonObject);
	}

	const char* GLTFBuffer::constData () const
	{
		return m_bufferData.constData ();
	}

	const char* GLTFBuffer::data () const
	{
		return m_bufferData.data ();
	}

	bool GLTFBuffer::loadData ()
	{
		try
		{
			if ( hasData () )
			{
				throw new GLTFException ( "BUFFER DATA ALREADY SET EXCEPTION" );
			}

			if ( !jsonValid () )
			{
				throw new GLTFException ( "INVALID JSON EXCEPTION" );
			}

			qsizetype byteLength = byteLengthFromJson ( m_jsonObject );
			QString uriString = m_jsonObject.value ( "uri" ).toString ();
			if ( uriString.startsWith ( "data:", Qt::CaseInsensitive ) )
			{
				return loadDataFromInternalString ( uriString, byteLength );
			}
			
			QString filePath = getFilePath ();
			return loadDataFromExternalFile ( filePath, byteLength );
		}
		catch ( const GLTFException& e )
		{
			qWarning () << e.what () << Qt::endl;
			return false;
		}
	}

	bool GLTFBuffer::loadDataFromExternalFile ( const QString& filename, qsizetype byteLength )
	{
		try
		{
			if ( m_byteLength > 0 )
			{
				throw new GLTFException ( "BUFFER DATA ALREADY SET EXCEPTION" );
			}

			QFile f ( filename );
			if ( !f.exists () )
			{
				throw new GLTFException ( "FILE DOES NOT EXIST EXCEPTION" );
			}

			if ( !f.open ( QIODeviceBase::ReadOnly ) )
			{
				throw new GLTFException ( "OPEN FILE FAIL EXCEPTION" );
			}

			if ( f.size () != byteLength )
			{
				f.close ();
				throw new GLTFException ( "BYTE LENGTH MISMATCH EXCEPTION" );
			}

			m_bufferData = f.readAll ();
			m_byteLength = byteLength;
			f.close ();
			return true;
		}
		catch ( const GLTFException& e )
		{
			qWarning () << e.what () << Qt::endl;
			return false;
		}
	}

	bool GLTFBuffer::loadDataFromInternalString ( const QString& uriString, qsizetype byteLength )
	{
		try
		{
			if ( m_byteLength > 0 )
			{
				throw new GLTFException ( "BUFFER DATA ALREADY SET EXCEPTION" );
			}

			// Start by checking if the uri string contains the substring 'base64,'. If so, the data portion will follow immediately after and we can base64 decode to our m_bufferData.
			const QString base64Str = "base64,";
			qsizetype idxBase64 = uriString.indexOf ( base64Str );
			if ( -1 != idxBase64 )
			{
				m_bufferData = QByteArray::fromBase64 ( uriString.sliced ( idxBase64 + base64Str.size () + 1 ).toLocal8Bit () );
				// Ensure that the data we read has the same size as the 'byteLength' value in our JSON object says.
				if ( m_bufferData.size () != byteLength )
				{
					m_bufferData.clear ();
					throw new GLTFException ( "BYTE LENGTH MISMATCH EXCEPTION" );
				}
				m_byteLength = byteLength;
				return true;
			}

			// check if the optional 'base64, extension is not present, but media type is 'application/octet-stream' or 'application/gltf-buffer' 
			const QString octStreamStr = "application/octet-stream,";
			qsizetype octStreamIdx = uriString.indexOf ( octStreamStr );
			if ( -1 != octStreamIdx )
			{
				// since we don't know if the data is base64 encoded here, we'll check if the byte length matches the sliced string portion.
				QString dataStr = uriString.sliced ( octStreamIdx + octStreamStr.size () + 1 );
				if ( dataStr.size () == byteLength )
				{
					m_bufferData = dataStr.toLocal8Bit ();
					m_byteLength = byteLength;
					return true;
				}

				// assume that dataStr is base64 encoded
				m_bufferData = QByteArray::fromBase64 ( dataStr.toLocal8Bit () );
				if ( m_bufferData.size () == byteLength )
				{
					m_byteLength = byteLength;
					return true;
				}

				throw new GLTFException ( "INVALID DATA FROM JSON" );
			}

			// Same thing with application/gltf-buffer
			const QString gltfBufferStr = "application/gltf-buffer,";
			qsizetype gltfBufferIdx = uriString.indexOf ( gltfBufferStr );
			if ( -1 != gltfBufferIdx )
			{
				// since we don't know if the data is base64 encoded here, we'll check if the byte length matches the sliced string portion.
				QString dataStr = uriString.sliced ( gltfBufferIdx + gltfBufferStr.size () + 1 );
				if ( dataStr.size () == byteLength )
				{
					m_bufferData = dataStr.toLocal8Bit ();
					m_byteLength = byteLength;
					return true;
				}

				// assume that dataStr is base64 encoded
				m_bufferData = QByteArray::fromBase64 ( dataStr.toLocal8Bit () );
				if ( m_bufferData.size () == byteLength )
				{
					m_byteLength = byteLength;
					return true;
				}

				throw new GLTFException ( "INVALID DATA FROM JSON" );
			}
		}
		catch ( const GLTFException& e )
		{
			qWarning () << e.what () << Qt::endl;
			return false;
		}

		return false;
	}

	bool GLTFBuffer::handleInternalData ()
	{
		try
		{
			if ( m_byteLength > 0 )
			{
				throw new GLTFException ( "BUFFER DATA ALREADY SET EXCEPTION" );
			}

			// convert the JSON value 'uri' to string and check it exists
			QString uriVal = m_jsonObject.value ( "uri" ).toString ();
			if ( uriVal.isEmpty () )
			{
				throw new GLTFException ( "JSON INVALID EXCEPTION" );
			}

			// get the byte size advertised by our json object for checking
			qsizetype byteLength = byteLengthFromJson ( m_jsonObject );

			// Start by checking if the uri string contains the substring 'base64,'. If so, the data portion will follow immediately after and we can base64 decode to our m_bufferData.
			const QString base64Str = "base64,";
			qsizetype idxBase64 = uriVal.indexOf ( base64Str );
			if ( -1 != idxBase64 )
			{
				m_bufferData = QByteArray::fromBase64 ( uriVal.sliced ( idxBase64 + base64Str.size () + 1 ).toLocal8Bit () );
				// Ensure that the data we read has the same size as the 'byteLength' value in our JSON object says.
				if ( m_bufferData.size () != byteLength )
				{
					m_bufferData.clear ();
					throw new GLTFException ( "BYTE LENGTH MISMATCH EXCEPTION" );
				}
				m_byteLength = byteLength;
				return true;
			}

			// check if the optional 'base64, extension is not present, but media type is 'application/octet-stream' or 'application/gltf-buffer' 
			const QString octStreamStr = "application/octet-stream,";
			qsizetype octStreamIdx = uriVal.indexOf ( octStreamStr );
			if ( -1 != octStreamIdx )
			{
				// since we don't know if the data is base64 encoded here, we'll check if the byte length matches the sliced string portion.
				QString dataStr = uriVal.sliced ( octStreamIdx + octStreamStr.size() + 1);
				if ( dataStr.size () == byteLength )
				{
					m_bufferData = dataStr.toLocal8Bit ();
					m_byteLength = byteLength;
					return true;
				}

				// assume that dataStr is base64 encoded
				m_bufferData = QByteArray::fromBase64 ( dataStr.toLocal8Bit () );
				if ( m_bufferData.size () == byteLength )
				{
					m_byteLength = byteLength;
					return true;
				}

				throw new GLTFException ( "INVALID DATA FROM JSON" );
			}
			
			// Same thing with application/gltf-buffer
			const QString gltfBufferStr = "application/gltf-buffer,";
			qsizetype gltfBufferIdx = uriVal.indexOf ( gltfBufferStr );
			if ( -1 != gltfBufferIdx )
			{
				// since we don't know if the data is base64 encoded here, we'll check if the byte length matches the sliced string portion.
				QString dataStr = uriVal.sliced ( gltfBufferIdx + gltfBufferStr.size () + 1 );
				if ( dataStr.size () == byteLength )
				{
					m_bufferData = dataStr.toLocal8Bit ();
					m_byteLength = byteLength;
					return true;
				}

				// assume that dataStr is base64 encoded
				m_bufferData = QByteArray::fromBase64 ( dataStr.toLocal8Bit () );
				if ( m_bufferData.size () == byteLength )
				{
					m_byteLength = byteLength;
					return true;
				}

				throw new GLTFException ( "INVALID DATA FROM JSON" );
			}
		}
		catch ( const GLTFException& e )
		{
			qWarning () << e.what () << Qt::endl;
			return false;
		}
	}

	void GLTFBuffer::clearBufferData ()
	{
		m_byteLength = 0;
		m_bufferData.clear ();		
	}

	bool GLTFBuffer::setDirPath ( const QString& path )
	{
		try
		{
			QDir dirPath ( path );

			if (dirPath.exists())
			{
				m_dirPath = dirPath;
				return true;
			}
			else
			{
				throw new GLTFException ( "DIR PATH DOES NOT EXIST EXCEPTION" );
			}
		}
		catch ( const GLTFException& e )
		{
			qWarning () << e.what () << Qt::endl;
			return false;
		}		
	}

	QString GLTFBuffer::getDirPath () const
	{
		return m_dirPath.path ();
	}

	QString GLTFBuffer::getFilePath () const
	{
		try
		{
			QUrl url ( m_jsonObject.value ( "uri" ).toString () );
			if ( !url.isValid () )
			{
				throw new GLTFException ( "INVALID URL EXCEPTION" );
			}

			if ( url.isRelative () )
			{
				return ( QDir::isAbsolutePath ( url.path () ) ) ? url.path () : m_dirPath.filePath ( url.path () );
			}
			
			QString localFile = url.toLocalFile ();
			return ( QDir::isAbsolutePath ( localFile ) ) ? localFile : m_dirPath.filePath ( localFile );
		}
		catch ( const GLTFException& e )
		{
			qWarning () << e.what () << Qt::endl;
			return QString ();
		}
	}
}
