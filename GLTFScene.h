/*****************************************************************//**
 * \file   GLTFScene.h
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
 * \brief  GLTFScene class and helpers to instantiate glTF scene
 * 
 * \author joechamm
 * \date   September 2022
 *********************************************************************/
#ifndef __GLTF_SCENE_H__
#define __GLTF_SCENE_H__

#include <QList>
#include <QString>
#include <QHash>
#include "vec4.h"

namespace jcqt
{
	constexpr const qint32 MAX_NODE_LEVEL = 16;

	struct Hierarchy
	{
		// parent for this node (or -1 for root)
		qint32 parent_;
		// first child for a node (or - 1)
		qint32 firstChild_;
		// next sibling for a node (or - 1)
		qint32 nextSibling_;
		// last added node (or - 1)
		qint32 lastSibling_;
		// cached node level
		qint32 level_;
	};

	struct SceneNode
	{
		qint32 mesh_;
		qint32 material_;
		qint32 parent_;
		qint32 firstChild_;
		qint32 rightSibling_;
	};

	struct Scene
	{
		/* Local transformations for each node and global transforms and an array of 'dirty/changed' local transforms */
		QList<gpumat4> localTransforms_;
		QList<gpumat4> globalTransforms_;

		// list of nodes whose global transform must be recalculated
		QList<qint32> changedAtThisFrame_ [ MAX_NODE_LEVEL ];

		// Hierarchy components
		QList<Hierarchy> hierarchy_;

		// Meshes for nodes (Node -> Mesh)
		QHash<quint32, quint32> meshes_;

		// Materials for nodes (Node -> Material) 
		QHash<quint32, quint32> materialForNode_;

		/* Useful for debugging */
		// Node names: which name is assigned to the node
		QHash<quint32, quint32> nameForNode_;

		// Collection of scene node names
		QStringList names_;

		// Collection of debug material names
		QStringList materialNames_;
	};

	qint32 addNode ( Scene& scene, qint32 parent, qint32 level );

	// markAsChanged() routine starts with a given node and recursively descends to each and every child node, adding it to the changedAtLevel_ arrays. 
	void markAsChanged ( Scene& scene, qint32 node );

	int findNodeByName ( const Scene& scene, const QString& name );

	inline QString getNodeName ( const Scene& scene, qint32 node )
	{	
		qint32 strID = scene.nameForNode_.value ( node, -1 );	
		return ( strID > -1 ) ? scene.names_.at ( strID ) : QString ();
	}

	inline void setNodeName ( Scene& scene, int node, const QString& name )
	{
		quint32 stringID = ( quint32 ) scene.names_.size ();
		scene.names_.append ( name );
		scene.nameForNode_ [ node ] = stringID;
	}

	qint32 getNodeLevel ( const Scene& scene, qint32 n );

	void recalculateGlobalTransforms ( Scene& scene );

	void loadScene ( const QString& filename, Scene& scene );
	void saveScene ( const QString& filename, const Scene& scene );

	void dumpTransformations ( const QString& filename, const Scene& scene );
	void printChagedNodes ( const Scene& scene );

	void dumpSceneToDot ( const QString& filename, const Scene& scene, qint32* visited = nullptr );

	void mergeScenes ( Scene& scene, const QList<Scene*>& scenes, const QList<gpumat4>& rootTransforms, const QList<quint32>& meshCounts, bool mergeMeshes = true, bool mergeMaterials = true );

	// Delete a collection of nodes from a scenegraph
	void deleteSceneNodes ( Scene& scene, const QList<quint32>& nodesToDelete );
}


#endif // !__GLTF_SCENE_H__
