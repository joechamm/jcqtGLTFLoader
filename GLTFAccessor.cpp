#include "GLTFAccessor.h"

namespace jcqt
{
	GLTFAccessor::GLTFAccessor ( QObject* parent )
		: QObject ( parent )
	{}

	GLTFAccessor::~GLTFAccessor ()
	{}

	GLTFAccessor::SparseValues GLTFAccessor::createSparseValuesFromJson ( const QJsonObject& jsonObj )
	{
		SparseValues sparseValues;
		sparseValues.m_bufferViewIndex = jsonObj.value ( "bufferView" ).toInteger ();
		sparseValues.m_byteOffset = jsonObj.contains ( "byteOffset" ) ? jsonObj.value ( "byteOffset" ).toInteger () : 0;
		return sparseValues;
	}

	GLTFAccessor::SparseIndices GLTFAccessor::createSparseIndicesFromJson ( const QJsonObject& jsonObj )
	{
		SparseIndices sparseIndices;
		sparseIndices.m_bufferViewIndex = jsonObj.value ( "bufferView" ).toInteger ();
		sparseIndices.m_componentType = jsonObj.value ( "componentType" ).toInteger ();
		sparseIndices.m_byteOffset = jsonObj.contains ( "byteOffset" ) ? jsonObj.value ( "byteOffset" ).toInteger () : 0;
		return sparseIndices;
	}

	GLTFAccessor::AccessorSparse GLTFAccessor::createAccessorSparseFromJson ( const QJsonObject& jsonObj )
	{
		AccessorSparse accessorSparse;
		accessorSparse.m_count = jsonObj.value ( "count" ).toInteger ();
		accessorSparse.m_indices = createSparseIndicesFromJson ( jsonObj.value ( "indices" ).toObject () );
		accessorSparse.m_values = createSparseValuesFromJson ( jsonObj.value ( "values" ).toObject () );
		accessorSparse.m_jsonObject = jsonObj;
		return accessorSparse;
	}

}
