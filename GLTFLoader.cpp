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

	QJsonArray GLTFLoader::jsonArray ( const QString& arrKey ) const
	{
		if ( m_document.isObject () )
		{
			if ( m_document.object ().contains ( arrKey ) )
			{
				return m_document.object ().value ( arrKey ).toArray ();
			}
		}		

		return QJsonArray ();
	}

	QJsonObject GLTFLoader::jsonObject () const
	{
		if ( m_document.isNull () )
			return QJsonObject ();

		return m_document.object ();
	}

	QJsonObject GLTFLoader::jsonObject ( const QString& objKey ) const
	{
		if ( m_document.isObject () )
		{
			if ( m_document.object ().contains ( objKey ) )
			{
				return m_document.object ().value ( objKey ).toObject ();
			}
		}

		return QJsonObject ();
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

	QJsonObject GLTFLoader::getBuffer ( qsizetype bufferIndex ) const
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

	bool GLTFLoader::getScenes ()
	{
		QJsonArray scenesArray = jsonArray ( "scenes" );
		qsizetype count = scenesArray.size ();
		if ( count > 0 )
		{
			m_scenes.resize ( count );
			for ( qsizetype i = 0; i < count; i++ )
			{
				m_scenes [ i ] = createSceneFromJson ( scenesArray.at ( i ).toObject () );
			}

			return true;
		}

		return false;
	}

	bool GLTFLoader::getNodes ()
	{
		QJsonArray nodesArray = jsonArray ( "nodes" );
		qsizetype count = nodesArray.size ();
		if ( count > 0 )
		{
			m_nodes.resize ( count );
			for ( qsizetype i = 0; i < count; i++ )
			{
				m_nodes [ i ] = createNodeFromJson ( nodesArray.at ( i ).toObject () );
			}

			return true;
		}

		return false;
	}

	bool GLTFLoader::getMeshes ()
	{
		QJsonArray meshesArray = jsonArray ( "meshes" );
		qsizetype count = meshesArray.size ();
		if ( count > 0 )
		{
			m_meshes.resize ( count );
			for ( qsizetype i = 0; i < count; i++ )
			{
				m_meshes [ i ] = createMeshFromJson ( meshesArray.at ( i ).toObject () );
			}

			return true;
		}

		return false;
	}

	bool GLTFLoader::getBuffers ()
	{
		QJsonArray buffersArray = jsonArray ( "buffers" );
		qsizetype count = buffersArray.size ();
		if ( count > 0 )
		{
			m_buffers.resize ( count );
			for ( qsizetype i = 0; i < count; i++ )
			{
				m_buffers [ i ] = createBufferFromJson ( buffersArray.at ( i ).toObject () );
			}

			return true;
		}

		return false;
	}

	bool GLTFLoader::getBufferViews ()
	{
		QJsonArray bufferViewsArray = jsonArray ( "bufferViews" );
		qsizetype count = bufferViewsArray.size ();
		if ( count > 0 )
		{
			m_bufferViews.resize ( count );
			for ( qsizetype i = 0; i < count; i++ )
			{
				m_bufferViews [ i ] = createBufferViewFromJson ( bufferViewsArray.at ( i ).toObject () );
			}

			return true;
		}

		return false;
	}

	bool GLTFLoader::getAccessors ()
	{
		QJsonArray accessorsArray = jsonArray ( "accessors" );
		qsizetype count = accessorsArray.size ();
		if ( count > 0 )
		{
			m_accessors.resize ( count );
			for ( qsizetype i = 0; i < count; i++ )
			{
				m_accessors [ i ] = createAccessorFromJson ( accessorsArray.at ( i ).toObject () );
			}

			return true;
		}

		return false;
	}

	bool GLTFLoader::getCameras ()
	{
		QJsonArray camerasArray = jsonArray ( "cameras" );
		qsizetype count = camerasArray.size ();
		if ( count > 0 )
		{
			m_cameras.resize ( count );
			for ( qsizetype i = 0; i < count; i++ )
			{
				m_cameras [ i ] = createCameraFromJson ( camerasArray.at ( i ).toObject () );
			}

			return true;
		}

		return false;
	}

	bool GLTFLoader::getSkins ()
	{
		QJsonArray skinsArray = jsonArray ( "skins" );
		qsizetype count = skinsArray.size ();
		if ( count > 0 )
		{
			m_skins.resize ( count );
			for ( qsizetype i = 0; i < count; i++ )
			{
				m_skins [ i ] = createSkinFromJson ( skinsArray.at ( i ).toObject () );
			}

			return true;
		}

		return false;
	}

	bool GLTFLoader::getAnimations ()
	{
		QJsonArray animationsArray = jsonArray ( "animations" );
		qsizetype count = animationsArray.size ();
		if ( count > 0 )
		{
			m_animations.resize ( count );
			for ( qsizetype i = 0; i < count; i++ )
			{
				m_animations [ i ] = createAnimationFromJson ( animationsArray.at ( i ).toObject () );
			}

			return true;
		}

		return false;
	}

	bool GLTFLoader::getMaterials ()
	{
		QJsonArray materialsArray = jsonArray ( "materials" );
		qsizetype count = materialsArray.size ();
		if ( count > 0 )
		{
			m_materials.resize ( count );
			for ( qsizetype i = 0; i < count; i++ )
			{
				m_materials [ i ] = createMaterialFromJson ( materialsArray.at ( i ).toObject () );
			}

			return true;
		}

		return false;
	}

	bool GLTFLoader::getTextures ()
	{
		QJsonArray texturesArray = jsonArray ( "textures" );
		qsizetype count = texturesArray.size ();
		if ( count > 0 )
		{
			m_textures.resize ( count );
			for ( qsizetype i = 0; i < count; i++ )
			{
				m_textures [ i ] = createTextureFromJson ( texturesArray.at ( i ).toObject () );
			}

			return true;
		}

		return false;
	}

	bool GLTFLoader::getSamplers ()
	{
		QJsonArray samplersArray = jsonArray ( "samplers" );
		qsizetype count = samplersArray.size ();
		if ( count > 0 )
		{
			m_samplers.resize ( count );
			for ( qsizetype i = 0; i < count; i++ )
			{
				m_samplers [ i ] = createSamplerFromJson ( samplersArray.at ( i ).toObject () );
			}

			return true;
		}

		return false;
	}

	bool GLTFLoader::getImages ()
	{
		QJsonArray imagesArray = jsonArray ( "bufferViews" );
		qsizetype count = imagesArray.size ();
		if ( count > 0 )
		{
			m_images.resize ( count );
			for ( qsizetype i = 0; i < count; i++ )
			{
				m_images [ i ] = createImageFromJson ( imagesArray.at ( i ).toObject () );
			}

			return true;
		}

		return false;
	}

	void GLTFLoader::getAll ()
	{
		getScenes ();
		getNodes ();
		getMeshes ();
		getBuffers ();
		getBufferViews ();
		getAccessors ();
		getCameras ();
		getSkins ();
		getAnimations ();
		getMaterials ();
		getTextures ();
		getSamplers ();
		getImages ();
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

