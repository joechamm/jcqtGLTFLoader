/*****************************************************************//**
 * \file   GLTFScene.cpp
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
 * \brief  
 * 
 * \author joechamm
 * \date   September 2022
 *********************************************************************/
#include "GLTFScene.h"

#include <QFile>
#include <q20algorithm.h>

namespace jcqt
{
	static constexpr qsizetype kSizeMat4 = 16 * sizeof ( float );

	static void saveStringList ( QFile* f, const QStringList& lines )
	{
		quint32 sz = ( quint32 ) lines.size ();
		qint32 bytesWritten = f->write ( ( const char* ) &sz, sizeof ( quint32 ) );

		if ( bytesWritten < 0 )
		{
			qDebug () << "WRITE operation returned -1. Failed to save StringList to file." << Qt::endl;
			return;
		}

		for ( const QString& s : lines )
		{
			sz = ( quint32 ) s.length ();
			bytesWritten = f->write ( ( const char* ) &sz, sizeof ( quint32 ) );
			if ( bytesWritten < 0 )
			{
				qDebug () << "WRITE operation returned -1. Failed to save StringList to file." << Qt::endl;
				return;
			}

			bytesWritten = f->write ( s.toStdString ().c_str (), sz + 1 );
			if ( bytesWritten < 0 )
			{
				qDebug () << "WRITE operation returned -1. Failed to save StringList to file." << Qt::endl;
				return;
			}
		}
	}

	static void loadStringList ( QFile* f, QStringList& lines )
	{
		{
			quint32 sz = 0;
			qint64 bytesRead = f->read ( ( char* ) &sz, sizeof ( quint32 ) );
			if ( bytesRead < 0 )
			{
				qDebug () << "READ operation returned -1. Failed to load StringList from file." << Qt::endl;
				return;
			}
			lines.resize ( sz );
		}
		QByteArray inBytes;
		for ( QString& s : lines )
		{
			quint32 sz = 0;
			qint64 bytesRead = f->read ( ( char* ) &sz, sizeof ( quint32 ) );
			if ( bytesRead < 0 )
			{
				qDebug () << "READ operation returned -1. Failed to load StringList from file." << Qt::endl;
				return;
			}
			inBytes.resize ( sz + 1 );
			bytesRead = f->read ( inBytes.data (), sz + 1 );
			if ( bytesRead < 0 )
			{
				qDebug () << "READ operation returned -1. Failed to load StringList from file." << Qt::endl;
				return;
			}
			s = QString ( inBytes );
		}
	}

	qint32 addNode ( Scene& scene, qint32 parent, qint32 level )
	{
		qint32 node = ( qint32 ) scene.hierarchy_.size ();
		{
			// TODO: resize aux arrays (local/global etc.)
			scene.localTransforms_.append ( gpumat4() );
			scene.globalTransforms_.append ( gpumat4 () );
		}

		scene.hierarchy_.append ( Hierarchy {
			.parent_ = parent, 
			.lastSibling_ = -1
		} );

		/*
		*	If we have a parent, we must fix its first child referenceand, potentially, the next sibling reference of some other node.
		*	If a parent node has no children, we must directly set its firstChild_ field; otherwise,
		*	we should run over the sibliings of this child to find out where to add the next sibling.
		*/
		if ( parent > -1 )
		{
			qint32 s = scene.hierarchy_ [ parent ].firstChild_;
			if ( s == -1 )
			{
				scene.hierarchy_ [ parent ].firstChild_ = node;
				scene.hierarchy_ [ node ].lastSibling_ = node;
			}
			else
			{
				qint32 dest = scene.hierarchy_ [ s ].lastSibling_;
				if ( dest < 0 )
				{
					// no cached lastSibling, iterate nextSibling indices
					for ( dest = s; scene.hierarchy_ [ dest ].nextSibling_ != -1; dest = scene.hierarchy_ [ dest ].nextSibling_ );
				}

				/*
				*	After the for loop, we assign our new node as the next sibling of the last child.
				*/

				scene.hierarchy_ [ dest ].nextSibling_ = node;
				scene.hierarchy_ [ s ].lastSibling_ = node;
			}
		}

		/* The level of this node is stored for correct global transformation updating. To keep the structure valid, we store the negative indices for the newly added node. */
		scene.hierarchy_ [ node ].level_ = level;
		scene.hierarchy_ [ node ].nextSibling_ = -1;
		scene.hierarchy_ [ node ].firstChild_ = -1;
		return node;
	}

	void markAsChanged ( Scene& scene, qint32 node )
	{
		// First, the node itself is marked as changed
		qint32 level = scene.hierarchy_ [ node ].level_;
		scene.changedAtThisFrame_ [ level ].append ( node );

		// start from the first child and advance to the next sibling, descending into the hierarchy.
		for ( qint32 s = scene.hierarchy_ [ node ].firstChild_; s != -1; s = scene.hierarchy_ [ s ].nextSibling_ )
		{
			markAsChanged ( scene, s );
		}
	}

	qint32 findNodeByName ( const Scene& scene, const QString& name )
	{
		// Extremely simple linear search without any hierarchreference
		// To support DFS/BFS searches separate travertsal routines are needed
		for ( quint32 i = 0; i < scene.localTransforms_.size (); i++ )
		{
			if ( scene.nameForNode_.contains ( i ) )
			{
				qint32 strID = scene.nameForNode_.value ( i );				
				if ( strID > -1 )
				{
					if ( scene.names_ [ strID ] == name )
						return ( qint32 ) i;
				}
			}
		}

		return -1;
	}

	qint32 getNodeLevel ( const Scene& scene, qint32 n )
	{
		qint32 level = -1;
		for ( qint32 p = 0; p != -1; p = scene.hierarchy_ [ p ].parent_, level++ );
		return level;
	}

//	bool mat4IsIdentity ( const gpumat4& m );
//	void fprintfMat4 ( QFile* f, const gpumat4& m );

	static bool mat4IsIdentity ( const gpumat4& m )
	{
		return ( m ( 0, 0 ) == 1.f && m ( 0, 1 ) == 0.f && m ( 0, 2 ) == 0.f && m ( 0, 3 ) == 0.f &&
			m ( 1, 0 ) == 0.f && m ( 1, 1 ) == 1.f && m ( 1, 2 ) == 0.f && m ( 1, 3 ) == 0.f &&
			m ( 2, 0 ) == 0.f && m ( 2, 1 ) == 0.f && m ( 2, 2 ) == 1.f && m ( 2, 3 ) == 0.f &&
			m ( 3, 0 ) == 0.f && m ( 3, 1 ) == 0.f && m ( 3, 2 ) == 0.f && m ( 3, 3 ) == 1.f );
	}

	static void fprintfMat4 ( QFile* f, const gpumat4& m )
	{
		qint64 bytesWritten = 0;
		if ( mat4IsIdentity ( m ) )
		{
			bytesWritten = f->write ( "IDENTITY" );
		}
		else
		{
			bytesWritten = f->write ( "\n" );
			for ( int col = 0; col < 4; col++ )
			{
				for ( int row = 0; row < 4; row++ )
				{
					QByteArray fval = QByteArray::number ( ( double ) m ( col, row ) ) + " ;";
					bytesWritten = f->write ( fval.constData () );
				}

				bytesWritten = f->write ( "\n" );
			}
		}
	}

	static QMatrix4x4 gpumat4ToQMatrix4x4 ( const gpumat4& mp )
	{
		return QMatrix4x4( mp ( 0, 0 ), mp ( 1, 0 ), mp ( 2, 0 ), mp ( 3, 0 ), mp ( 0, 1 ), mp ( 1, 1 ), mp ( 2, 1 ), mp ( 3, 1 ), mp ( 0, 2 ), mp ( 1, 2 ), mp ( 2, 2 ), mp ( 3, 2 ), mp ( 0, 3 ), mp ( 1, 3 ), mp ( 2, 3 ), mp ( 3, 3 ) );
	}

	static float gpumat4Determinant ( const gpumat4& mp )
	{
		QMatrix4x4 qmp = gpumat4ToQMatrix4x4 ( mp );
		return static_cast<float>(qmp.determinant ());
	}

	// CPU version of global transforms update []
	void recalculateGlobalTransforms ( Scene& scene )
	{
		// Start from the root layer of the list of changed scene nodes, supposing we have only one root node. This is because root node global transforms coincide with their local transforms. The changed nodes list is then cleared.
		if ( !scene.changedAtThisFrame_ [ 0 ].empty () )
		{
			qint32 c = scene.changedAtThisFrame_ [ 0 ][ 0 ];
			scene.globalTransforms_ [ c ] = scene.localTransforms_ [ c ];
			scene.changedAtThisFrame_ [ 0 ].clear ();
		}

		/*
		*	For all the lower levels, we must ensure that we have parents so that the loops are linear and there are no conditions inside.
		*	We will start from level 1 because the root level is already being handled. The exit condition is the emptiness of the list at the current level.
		*	We will avoid descending deeper than our list allows.
		*/

		for ( qint32 i = 1; i < MAX_NODE_LEVEL && ( !scene.changedAtThisFrame_ [ i ].empty () ); i++ )
		{
			// Iterate all the changed nodes at this level. For each of the iterated nodes, we fetch the parent transform and multiply it by the local node transform.
			for ( const qint32& c : scene.changedAtThisFrame_ [ i ] )
			{
				qint32 p = scene.hierarchy_ [ c ].parent_;

				// TODO: probably need a more efficient approach here
//				const gpumat4& mp = scene.globalTransforms_ [ p ];
//				const gpumat4& mc = scene.localTransforms_ [ c ];
//				QMatrix4x4 qmp ( mp ( 0, 0 ), mp ( 1, 0 ), mp ( 2, 0 ), mp ( 3, 0 ), mp ( 0, 1 ), mp ( 1, 1 ), mp ( 2, 1 ), mp ( 3, 1 ), mp ( 0, 2 ), mp ( 1, 2 ), mp ( 2, 2 ), mp ( 3, 2 ), mp ( 0, 3 ), mp ( 1, 3 ), mp ( 2, 3 ), mp ( 3, 3 ) );
//				QMatrix4x4 qmc ( mc ( 0, 0 ), mc ( 1, 0 ), mc ( 2, 0 ), mc ( 3, 0 ), mc ( 0, 1 ), mc ( 1, 1 ), mc ( 2, 1 ), mc ( 3, 1 ), mc ( 0, 2 ), mc ( 1, 2 ), mc ( 2, 2 ), mc ( 3, 2 ), mc ( 0, 3 ), mc ( 1, 3 ), mc ( 2, 3 ), mc ( 3, 3 ) );
//				scene.globalTransforms_ [ c ] = scene.globalTransforms_ [ p ] * scene.localTransforms_ [ c ];

				QMatrix4x4 qmp = gpumat4ToQMatrix4x4 ( scene.globalTransforms_ [ p ] );
				QMatrix4x4 qmc = gpumat4ToQMatrix4x4 ( scene.localTransforms_ [ c ] );

				scene.globalTransforms_ [ c ] = gpumat4 ( qmp * qmc );
			}

			// Clear the list for this level once we're done.
			scene.changedAtThisFrame_ [ i ].clear ();
		}

		/* Since we start from the root layer of the scen graph tree, all the changed layers below the root acquire a valid global transformation for thier parents, and we do not have to recalculate any of the global transformations multiple times. */
	}

	void loadMap ( QFile* f, QHash<quint32, quint32>& hashMap )
	{
		QList<quint32> ms;

		/* Read the count of {key, value} pairs and resize our temporary storage */
		quint32 sz = 0;
		qint64 bytesRead = f->read ((char*) & sz, sizeof (sz));
		if ( bytesRead < 0 )
		{
			qDebug () << "READ operation returned -1. Failed to load HashMap from file." << Qt::endl;
			return;
		}

		ms.resize ( sz );

		/* Read all the key-value pairs into ms */
		bytesRead = f->read ( ( char* ) ms.data (), sizeof ( qint32 ) * sz );
		if ( bytesRead < 0 )
		{
			qDebug () << "READ operation returned -1. Failed to load HashMap from file." << Qt::endl;
			return;
		}

		/* Convert the array into our hash table */
		for ( qint32 i = 0; i < ( sz / 2 ); i++ )
		{
			hashMap [ ms [ i * 2 + 0 ] ] = ms [ i * 2 + 1 ];
		}
	}

	void loadScene ( const QString& filename, Scene& scene )
	{
		QFile f ( filename );
		f.open ( QIODeviceBase::ReadOnly );

		if ( !f.isOpen () )
		{
			qDebug () << "Cannot open scene file " << filename << ". Please run SceneConverter and/or MergeMeshes" << Qt::endl;
			return;
		}

		quint32 sz;
		quint64 bytesRead = f.read ( ( char* ) &sz, sizeof ( sz ) );
		if ( bytesRead < 0 )
		{
			qDebug () << "READ operation returned -1. Failed to load Scene from file." << Qt::endl;
			return;
		}

		scene.hierarchy_.resize ( sz );
		scene.globalTransforms_.resize ( sz );
		scene.localTransforms_.resize ( sz );

		bytesRead = f.read ((char *) scene.localTransforms_.data (), kSizeMat4 * sz );
		if ( bytesRead < 0 )
		{
			qDebug () << "READ operation returned -1. Failed to load Scene from file." << Qt::endl;
			return;
		}

		bytesRead = f.read ( ( char* ) scene.globalTransforms_.data (), kSizeMat4 * sz );
		if ( bytesRead < 0 )
		{
			qDebug () << "READ operation returned -1. Failed to load Scene from file." << Qt::endl;
			return;
		}

		bytesRead = f.read ( ( char* ) scene.hierarchy_.data (), sizeof ( Hierarchy ) * sz );
		if ( bytesRead < 0 )
		{
			qDebug () << "READ operation returned -1. Failed to load Scene from file." << Qt::endl;
			return;
		}

		// Mesh for node [index to some list of buffers]
		loadMap ( &f, scene.materialForNode_ );
		loadMap ( &f, scene.meshes_ );

		if ( !f.atEnd () )
		{
			loadMap ( &f, scene.nameForNode_ );
			loadStringList ( &f, scene.names_ );
			loadStringList ( &f, scene.materialNames_ );
		}

		f.close ();		
	}

	void saveMap ( QFile* f, const QHash<quint32, quint32>& hashMap )
	{
		/* temporary storage structure */
		QList<quint32> ms;
		ms.reserve ( hashMap.size () * 2 );

		/* copy the elements ouf our map to our temporary storage */
		for ( QHash<quint32, quint32>::const_iterator it = hashMap.cbegin (), end = hashMap.cend (); it != end; ++it )
		{
			ms.append ( it.key () );
			ms.append ( it.value () );
		}

		/* write the number of {key,value} pairs to our file then write the data */
		const quint32 sz = static_cast< quint32 >( ms.size () );
		
		qint64 bytesWritten = f->write ( ( const char* ) &sz, sizeof ( sz ) );
		if ( bytesWritten < 0 )
		{
			qDebug () << "WRITE operation returned -1. Failed to save StringList to file." << Qt::endl;
			return;
		}
		
		bytesWritten = f->write ( ( const char* ) ms.data (), sizeof ( quint32 ) * ms.size () );
	}

	void saveScene ( const QString& filename, const Scene& scene )
	{
		/* At the beginning of the file, we must write the count of scene nodes. */
		QFile f(filename);

		if ( !f.open ( QIODeviceBase::WriteOnly ) )
		{
			qDebug () << "Failed to open " << filename << "! Cannot save scene." << Qt::endl;
			return;
		}

		const quint32 sz = ( quint32 ) scene.hierarchy_.size ();
		qint64 bytesWritten = f.write ( ( const char* ) &sz, sizeof ( sz ) );
		if ( bytesWritten < 0 )
		{
			qDebug () << "WRITE operation returned -1. Failed to save Scene to file." << Qt::endl;
			return;
		}

		bytesWritten = f.write ( ( const char* ) &sz, sizeof ( sz ) );
		if ( bytesWritten < 0 )
		{
			qDebug () << "WRITE operation returned -1. Failed to save Scene to file." << Qt::endl;
			return;
		}

		bytesWritten = f.write ( ( const char* ) scene.localTransforms_.constData (), kSizeMat4 * sz );
		if ( bytesWritten < 0 )
		{
			qDebug () << "WRITE operation returned -1. Failed to save Scene to file." << Qt::endl;
			return;
		}

		bytesWritten = f.write ( ( const char* ) scene.globalTransforms_.constData (), kSizeMat4 * sz );
		if ( bytesWritten < 0 )
		{
			qDebug () << "WRITE operation returned -1. Failed to save Scene to file." << Qt::endl;
			return;
		}

		bytesWritten = f.write ( ( const char* ) scene.hierarchy_.constData(), sizeof(Hierarchy) * sz);
		if ( bytesWritten < 0 )
		{
			qDebug () << "WRITE operation returned -1. Failed to save Scene to file." << Qt::endl;
			return;
		}

		// Mesh for node [index to some list of buffers]
		saveMap ( &f, scene.materialForNode_ );
		saveMap ( &f, scene.meshes_ );

		if ( !scene.names_.empty () && !scene.nameForNode_.empty () )
		{
			saveMap ( &f, scene.nameForNode_ );
			saveStringList ( &f, scene.names_ );
			saveStringList ( &f, scene.materialNames_ );
		}

		f.close ();
	}

	//bool mat4IsIdentity ( const gpumat4& m )
	//{
	//	return ( m ( 0, 0 ) == 1.f && m ( 0, 1 ) == 0.f && m ( 0, 2 ) == 0.f && m ( 0, 3 ) == 0.f &&
	//		m ( 1, 0 ) == 0.f && m ( 1, 1 ) == 1.f && m ( 1, 2 ) == 0.f && m ( 1, 3 ) == 0.f &&
	//		m ( 2, 0 ) == 0.f && m ( 2, 1 ) == 0.f && m ( 2, 2 ) == 1.f && m ( 2, 3 ) == 0.f &&
	//		m ( 3, 0 ) == 0.f && m ( 3, 1 ) == 0.f && m ( 3, 2 ) == 0.f && m ( 3, 3 ) == 1.f );
	//}

	//void fprintfMat4 ( QFile* f, const gpumat4& m )
	//{
	//	qint64 bytesWritten = 0;
	//	if ( mat4IsIdentity ( m ) )
	//	{
	//		bytesWritten = f->write ( "IDENTITY" );
	//	}
	//	else
	//	{
	//		bytesWritten = f->write ( "\n" );
	//		for ( int col = 0; col < 4; col++ )
	//		{
	//			for ( int row = 0; row < 4; row++ )
	//			{
	//				QByteArray fval = QByteArray::number ( ( double ) m ( col, row ) ) + " ;";
	//				bytesWritten = f->write ( fval.constData () );
	//			}

	//			bytesWritten = f->write ( "\n" );
	//		}
	//	}
	//}

	void dumpTransformations ( const QString& filename, const Scene& scene )
	{
		QFile f ( filename );

		if ( !f.open ( QIODeviceBase::WriteOnly | QIODeviceBase::Append ) )
		{
			qDebug () << "Failed to open " << filename << ". Cannot dump transformations." << Qt::endl;
			return;
		}

		for ( qsizetype i = 0; i < scene.localTransforms_.size (); i++ )
		{
			QByteArray str = "Node[" + QByteArray::number ( ( uint ) i ) + "].localTransform: ";
			qint64 bytesWritten = f.write ( str );
			fprintfMat4(&f, scene.localTransforms_.at ( i ));
			str = "Node[" + QByteArray::number ( ( uint ) i ) + "].globalTransform: ";
			bytesWritten = f.write ( str );
			fprintfMat4 ( &f, scene.globalTransforms_.at ( i ) );
			float det = gpumat4Determinant ( scene.globalTransforms_.at ( i ) );
			str = "Node[" + QByteArray::number ( ( uint ) i ) + "].globalDet = " + QByteArray::number ( ( double ) det ) + "; localDet = ";
			det = gpumat4Determinant ( scene.localTransforms_.at ( i ) );
			str += QByteArray::number ( ( double ) det ) + "\n";
			bytesWritten = f.write ( str );
		}

		f.close ();
	}

	void printChagedNodes ( const Scene& scene )
	{
		for ( qint32 i = 0; i < MAX_NODE_LEVEL && ( !scene.changedAtThisFrame_ [ i ].empty () ); i++ )
		{
			qDebug () << "Changed at level(" << i << "):" << Qt::endl;

			for ( const qint32& c : scene.changedAtThisFrame_ [ i ] )
			{
				qint32 p = scene.hierarchy_ [ c ].parent_;
				qDebug () << " Node " << c << ". Parent = " << p << "; LocalTransform: ";
				QFile f;
				f.open ( stdout, QIODevice::WriteOnly );
				fprintfMat4 ( &f, scene.localTransforms_.at ( i ) );
				f.close ();
				if ( p > -1 )
				{
					qDebug () << " ParentGlobalTransform: ";
					f.open ( stdout, QIODevice::WriteOnly );
					fprintfMat4 ( &f, scene.globalTransforms_.at ( p ) );
					f.close ();
				}
			}
		}
	}

	void dumpSceneToDot ( const QString& filename, const Scene& scene, qint32* visited )
	{
		QFile f ( filename );

		if ( !f.open ( QIODeviceBase::WriteOnly ) )
		{
			qDebug () << "Failed to open " << filename << ". Cannot dump scene." << Qt::endl;
			return;
		}

		f.write ( "digraph G\n{\n" );
		for ( qsizetype i = 0; i < scene.globalTransforms_.size (); i++ )
		{
			QByteArray name = "";
			QByteArray extra = "";
			if ( scene.nameForNode_.contains ( i ) )
			{
				qint32 strID = scene.nameForNode_.value ( i );
				name = scene.names_ [ strID ].toLocal8Bit();
			}

			if ( visited )
			{
				if ( visited [ i ] )
					extra = ", color = red";
			}

			QByteArray str = "n" + QByteArray::number ( ( uint ) i ) + " [label=\"" + name + "\" " + extra + "]\n";
			f.write ( str );
		}

		for ( qsizetype i = 0; i < scene.hierarchy_.size (); i++ )
		{
			qint32 p = scene.hierarchy_ [ i ].parent_;
			if ( p > -1 )
			{
				QByteArray str = "\t n" + QByteArray::number ( ( uint ) p ) + "\n";
				f.write ( str );
			}
		}

		f.write ( "}\n" );
		f.close ();
	}

	template <typename T>
	inline void mergeLists ( QList<T>& l1, const QList<T>& l2 )
	{
		l1.insert ( l1.end (), l2.begin (), l2.end () );
	}

	// Shift all hierarchy components in the nodes
	static void shiftNodes ( Scene& scene, qint32 startOffset, qint32 nodeCount, qint32 shiftAmount )
	{
		auto shiftNode = [shiftAmount] ( Hierarchy& node )
		{
			if ( node.parent_ > -1 )
				node.parent_ += shiftAmount;
			if ( node.firstChild_ > -1 )
				node.firstChild_ += shiftAmount;
			if ( node.nextSibling_ > -1 )
				node.nextSibling_ += shiftAmount;
			if ( node.lastSibling_ > -1 )
				node.lastSibling_ += shiftAmount;
		};

		for ( qint32 i = 0; i < nodeCount; i++ )
		{
			shiftNode ( scene.hierarchy_ [ i + startOffset ] );
		}
	}

	// Add the items from otherMap shifting indices and values along the way
	static void mergeMaps ( QHash<quint32, quint32>& m, const QHash<quint32, quint32>& otherMap, qint32 indexOffset, qint32 itemOffset )
	{
		QHashIterator<quint32, quint32> i ( otherMap );
		while ( i.hasNext () )
		{
			i.next ();
			m.insert ( i.key () + indexOffset, i.value () + itemOffset );
		}
	}

	void mergeScenes ( Scene& scene, const QList<Scene*>& scenes, const QList<gpumat4>& rootTransforms, const QList<quint32>& meshCounts, bool mergeMeshes, bool mergeMaterials )
	{
		// Create the new root node
		scene.hierarchy_ = {
			{
				.parent_ = -1,
				.firstChild_ = 1,
				.nextSibling_ = -1,
				.lastSibling_ = -1,
				.level_ = 0
			}
		};

		scene.nameForNode_ [ 0 ] = 0;
		scene.names_ = { "NewRoot" };

		QMatrix4x4 qidMat;
		qidMat.setToIdentity ();

		scene.localTransforms_.push_back ( gpumat4(qidMat) );
		scene.globalTransforms_.push_back ( gpumat4(qidMat) );

		if ( scenes.empty () )
		{
			return;
		}

		qint32 offs = 1;
		qint32 meshOffs = 0;
		qint32 nameOffs = ( int ) scene.names_.size ();
		qint32 materialOfs = 0;
		auto meshCount = meshCounts.begin ();

		if ( !mergeMaterials )
		{
			scene.materialNames_ = scenes [ 0 ]->materialNames_;
		}

		// FIXME: too much logic (for all the components in a scene, though mesh data and materials go separately - there are dedicated data lists)
		for ( const Scene* s : scenes )
		{
			mergeLists ( scene.localTransforms_, s->localTransforms_ );
			mergeLists ( scene.globalTransforms_, s->globalTransforms_ );

			mergeLists ( scene.hierarchy_, s->hierarchy_ );

			mergeLists ( scene.names_, s->names_ );
			if ( mergeMaterials )
			{
				mergeLists ( scene.materialNames_, s->materialNames_ );
			}

			qint32 nodeCount = ( qint32 ) s->hierarchy_.size ();

			shiftNodes ( scene, offs, nodeCount, offs );

			mergeMaps ( scene.meshes_, s->meshes_, offs, mergeMeshes ? meshOffs : 0 );
			mergeMaps ( scene.materialForNode_, s->materialForNode_, offs, mergeMaterials ? materialOfs : 0 );
			mergeMaps ( scene.nameForNode_, s->nameForNode_, offs, nameOffs );

			offs += nodeCount;

			materialOfs += ( qint32 ) s->materialNames_.size ();
			nameOffs += ( qint32 ) s->names_.size ();

			if ( mergeMeshes )
			{
				meshOffs += *meshCount;
				meshCount++;
			}
		}

		// fixing 'nextSibling' fields in the old roots (zero-index in all the scenes)
		offs = 1;
		qint32 idx = 0;
		for ( const Scene* s : scenes )
		{
			qint32 nodeCount = ( qint32 ) s->hierarchy_.size ();
			bool isLast = ( idx == scenes.size () - 1 );
			// calculate new next sibling for the old scene roots
			qint32 next = isLast ? -1 : offs + nodeCount;
			scene.hierarchy_ [ offs ].nextSibling_ = next;
			// attach to new root
			scene.hierarchy_ [ offs ].parent_ = 0;

			// transform old root nodes, if the transforms are given
			if ( !rootTransforms.empty () )
			{
				// TODO: need to find a better way to multiply gpumat4
				QMatrix4x4 qrtTrans = gpumat4ToQMatrix4x4 ( rootTransforms [ idx ] );
				QMatrix4x4 qlocTrans = gpumat4ToQMatrix4x4 ( scene.localTransforms_ [ offs ] );
//				scene.localTransforms_ [ offs ] = rootTransforms [ idx ] * scene.localTransforms_ [ offs ];
				scene.localTransforms_ [ offs ] = gpumat4 ( qrtTrans * qlocTrans );
			}

			offs += nodeCount;
			idx++;
		}

		// now shift levels of all nodes below the root
		for ( auto i = scene.hierarchy_.begin () + 1; i != scene.hierarchy_.end (); i++ )
		{
			i->level_++;
		}
	}

	/** A rather long algorithm (and the auxiliary routines) to delete a number of scene nodes from the hierarchy */


	// From https://stackoverflow.com/a/64152990/1182653
	// Delete a list of items from std::vector with indices in 'selection'
	template <class T, class Index = int> inline void eraseSelected ( QList<T>& v, const QList<Index>& selection )
		// e.g., eraseSelected({1, 2, 3, 4, 5}, {1, 3})  ->   {1, 3, 5}
		//                         ^     ^    2 and 4 get deleted
	{
		// cut off the elements moved to the end of the vector by std::stable_partition
		v.resize ( std::distance (
			v.begin (),
			// the stable_partition moves any element whose index is in 'selection' to the end
			std::stable_partition ( v.begin (), v.end (),
				[&selection, &v] ( const T& item ) {
					return !std::binary_search (
						selection.begin (), selection.end (),
						/* std::distance(std::find(v.begin(), v.end(), item), v.begin()) - if you don't like the pointer arithmetic below */
						static_cast< Index >( static_cast< const T* >( &item ) - &v [ 0 ] ) );
				} ) ) );
	}

	// Add an index to a sorted index array
	static void addUniqueIdx ( QList<quint32>& v, quint32 index )
	{
		if ( !std::binary_search ( v.begin (), v.end (), index ) )
		{
			v.push_back ( index );
		}
	}

	// Recurse down from a node and collect all nodes which are already marked for deletion
	static void collectNodesToDelete ( const Scene& scene, qint32 node, QList<quint32>& nodes )
	{
		for ( qint32 n = scene.hierarchy_ [ node ].firstChild_; n != -1; n = scene.hierarchy_ [ n ].nextSibling_ )
		{
			addUniqueIdx ( nodes, n );
			collectNodesToDelete ( scene, n, nodes );
		}
	}

	static qint32 findLastNonDeletedItem ( const Scene& scene, const QList<qint32>& newIndices, qint32 node )
	{
		// we have to be more subtle:
		// if the (newIndices[firstChild_] == - 1), we should follow the link and extract the last non-removed item
		if ( node == -1 )
		{
			return -1;
		}

		return ( newIndices [ node ] == -1 ) ? findLastNonDeletedItem ( scene, newIndices, scene.hierarchy_ [ node ].nextSibling_ ) : newIndices [ node ];
	}

	static void shiftMapIndices ( QHash<quint32, quint32>& items, const QList<qint32>& newIndices )
	{
		QHash<quint32, quint32> newItems;
		QHashIterator<quint32, quint32> m ( items );
		while ( m.hasNext () )
		{
			m.next ();
			qint32 newIndex = newIndices [ m.key () ];
			if ( newIndex != -1 )
			{
				newItems.insert ( newIndex, m.value () );
			}
		}

		items = newItems;
	}

	// Approximately an O ( N * Log(N) * Log(M)) algorithm (N = scene.size, M = nodesToDelete.size) to delete a collection of nodes from scene graph
	void deleteSceneNodes ( Scene& scene, const QList<quint32>& nodesToDelete )
	{
		// 0) Add all the nodes down below in the hierarchy
		auto indicesToDelete = nodesToDelete;
		for ( auto i : indicesToDelete )
		{
			collectNodesToDelete ( scene, i, indicesToDelete );
		}

		// aux array with node indices to keep track of the moved ones [moved = [](node) { return (node != nodes[node]); ]
		QList<qint32> nodes ( scene.hierarchy_.size () );
		std::iota ( nodes.begin (), nodes.end (), 0 );

		// 1.a) Move all the indicesToDelete to the end of 'nodes' array (and cut them off, a variation of swap'n'pop for multiple elements)
		auto oldSize = nodes.size ();
		eraseSelected ( nodes, indicesToDelete );

		// 1.b) Make a newIndices[oldIndex] mapping table
		QList<qint32> newIndices ( oldSize, -1 );
		for ( int i = 0; i < nodes.size (); i++ )
		{
			newIndices [ nodes [ i ] ] = i;
		}

		// 2) Replace all non-null parent/firstChild/nextSibling pointers in all the nodes by new positions
		auto nodeMover = [&scene, &newIndices] ( Hierarchy& h ) {
			return Hierarchy {
				.parent_ = ( h.parent_ != -1 ) ? newIndices [ h.parent_ ] : -1,
				.firstChild_ = findLastNonDeletedItem ( scene, newIndices, h.firstChild_ ),
				.nextSibling_ = findLastNonDeletedItem ( scene, newIndices, h.nextSibling_ ),
				.lastSibling_ = findLastNonDeletedItem ( scene, newIndices, h.lastSibling_ )
			};
		};

		std::transform ( scene.hierarchy_.begin (), scene.hierarchy_.end (), scene.hierarchy_.begin (), nodeMover );

		// 3) Finally throw away the hierarchy items
		eraseSelected ( scene.hierarchy_, indicesToDelete );

		// 4) As in mergeScenes() routine we also have to adjust all the "components" (i.e., meshes, materials, names and transformations)

		// 4a) Transformations are stored in arrays, so we just erase the items as we did iwith the scene.hierarchy_
		eraseSelected ( scene.localTransforms_, indicesToDelete );
		eraseSelected ( scene.globalTransforms_, indicesToDelete );

		// 4b) All the maps should change the key values with the newIndices[] array
		shiftMapIndices ( scene.meshes_, newIndices );
		shiftMapIndices ( scene.materialForNode_, newIndices );
		shiftMapIndices ( scene.nameForNode_, newIndices );

		// 5) scene node names list is not modified, but in principle it can be (remove all non-used items and adjust the nameForNode_ map)
		// 6) Material names list is not modified also, but if some materials fell out of use
	}
}
