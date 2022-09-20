#include "GLTFBufferView.h"

/* 
qsizetype m_bufferIndex;
qsizetype m_byteOffset;
qsizetype m_byteLength;
qsizetype m_byteStride;
qsizetype m_target;
QJsonObject m_jsonObject;
*/

namespace jcqt
{
	GLTFBufferView::GLTFBufferView ( QObject* parent )
		: QObject ( parent )
		, m_bufferIndex(-1)
		, m_byteOffset(0)
		, m_byteLength(-1)
		, m_byteStride(-1)
		, m_target(-1)
	{}

	GLTFBufferView::GLTFBufferView ( const QJsonObject & jsonObj, QObject * parent )
		: QObject(parent)
		, m_jsonObject(jsonObj)
		, m_bufferIndex ( -1 )
		, m_byteOffset ( 0 )
		, m_byteLength ( -1 )
		, m_byteStride ( -1 )
		, m_target ( -1 )
	{
		if ( isValidJson ( m_jsonObject ) )
		{
			setValuesFromJson ();
		}
	}

	GLTFBufferView::~GLTFBufferView ()
	{}

	bool GLTFBufferView::isValidJson ( const QJsonObject & jsonObj )
	{
		return ( jsonObj.contains ( "buffer" ) && jsonObj.contains ( "byteLength" ) );
	}

	qsizetype GLTFBufferView::getBufferIndexFromJson ( const QJsonObject& jsonObj )
	{
		return qsizetype (jsonObj.value("buffer").toInteger());
	}

	qsizetype GLTFBufferView::getByteOffsetFromJson ( const QJsonObject& jsonObj )
	{
		return qsizetype (jsonObj.value("byteOffset").toInteger());
	}

	qsizetype GLTFBufferView::getByteLengthFromJson ( const QJsonObject& jsonObj )
	{
		return qsizetype (jsonObj.value("byteLength").toInteger());
	}

	qsizetype GLTFBufferView::getByteStrideFromJson ( const QJsonObject& jsonObj )
	{
		return qsizetype (jsonObj.value("byteStride").toInteger());
	}

	qsizetype GLTFBufferView::getTargetFromJson ( const QJsonObject& jsonObj )
	{
		return qsizetype (jsonObj.value("target").toInteger());
	}

	QString GLTFBufferView::getNameFromJson ( const QJsonObject& jsonObj )
	{
		return jsonObj.value ( "name" ).toString ();
	}

	QJsonObject GLTFBufferView::getExtensionsFromJson ( const QJsonObject& jsonObj )
	{
		return jsonObj.value ( "extensions" ).toObject ();
	}

	QJsonObject GLTFBufferView::getExtrasFromJson ( const QJsonObject& jsonObj )
	{
		return jsonObj.value ( "extras" ).toObject ();
	}

	bool GLTFBufferView::isValid () const
	{	
		return isValidJson ( m_jsonObject );
	}

	bool GLTFBufferView::hasByteOffset () const
	{
		return m_jsonObject.contains ( "byteOffset" );
	}

	bool GLTFBufferView::hasByteStride () const
	{
		return m_jsonObject.contains ( "byteStride" );
	}

	bool GLTFBufferView::hasTarget () const
	{
		return m_jsonObject.contains ( "target" );
	}

	bool GLTFBufferView::hasName () const
	{
		return m_jsonObject.contains ( "name" );
	}

	bool GLTFBufferView::hasExtensions () const
	{
		return m_jsonObject.contains ( "extensions" );
	}

	bool GLTFBufferView::hasExtras () const
	{
		return m_jsonObject.contains ( "extras" );
	}

	qsizetype GLTFBufferView::bufferIndex () const
	{
		return m_bufferIndex;
	}

	qsizetype GLTFBufferView::byteOffset () const
	{
		return m_byteOffset;
	}

	qsizetype GLTFBufferView::byteLength () const
	{
		return m_byteLength;
	}

	qsizetype GLTFBufferView::byteStride () const
	{
		return m_byteStride;
	}

	qsizetype GLTFBufferView::target () const
	{
		return m_target;
	}

	QString GLTFBufferView::name () const
	{
		return m_jsonObject.value ( "name" ).toString ();
	}

	QJsonObject GLTFBufferView::extensions () const
	{
		return m_jsonObject.value ( "extensions" ).toObject ();
	}

	QJsonObject GLTFBufferView::extras () const
	{
		return m_jsonObject.value ( "extras" ).toObject ();
	}

	bool GLTFBufferView::setJsonObject ( const QJsonObject& jsonObj )
	{
		if ( isValidJson ( jsonObj ) )
		{
			m_jsonObject = jsonObj;
			return setValuesFromJson ();
		}

		return false;
	}

	bool GLTFBufferView::setValuesFromJson ()
	{
		if ( m_jsonObject.contains ( "buffer" ) )
		{
			m_bufferIndex = qsizetype ( m_jsonObject.value ( "buffer" ).toInteger () );
		}
		else
		{
			return false;
		}

		if ( m_jsonObject.contains ( "byteOffset" ) )
		{
			m_byteOffset = qsizetype ( m_jsonObject.value ( "byteOffset" ).toInteger () );
		}
		else
		{
			m_byteOffset = 0;
		}

		if ( m_jsonObject.contains ( "byteLength" ) )
		{
			m_byteLength = qsizetype ( m_jsonObject.value ( "byteLength" ).toInteger () );
		}
		else
		{
			return false;
		}

		if ( m_jsonObject.contains ( "byteStride" ) )
		{
			m_byteStride = qsizetype ( m_jsonObject.value ( "byteStride" ).toInteger () );
		}
		else
		{
			m_byteStride = -1;
		}

		if ( m_jsonObject.contains ( "target" ) )
		{
			m_target = qsizetype ( m_jsonObject.value ( "target" ).toInteger () );
		}
		else
		{
			m_target = -1;
		}

		return true;
	}
}

