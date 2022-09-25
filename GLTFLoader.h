/*****************************************************************//**
 * \file   GLTFLoader.h
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
#ifndef __GLTF_LOADER_H__
#define __GLTF_LOADER_H__


#include <QObject>
#include <QJsonDocument>

#include "GLTFAccessor.h"
#include "GLTFAnimation.h"
#include "GLTFBuffer.h"
#include "GLTFBufferView.h"
#include "GLTFCamera.h"
#include "GLTFImage.h"
#include "GLTFMaterial.h"
#include "GLTFMesh.h"
#include "GLTFNode.h"
#include "GLTFSampler.h"
#include "GLTFSkin.h"
#include "GLTFException.h"

#include <QList>

namespace jcqt
{
	class GLTFLoader : public QObject
	{
		Q_OBJECT

	public:
		GLTFLoader ( QObject* parent = nullptr );
		~GLTFLoader ();

		bool loadGLTF ( const QString& filename );
		bool isJsonArray () const;
		bool isJsonObject () const;
		QJsonArray jsonArray () const;
		QJsonArray jsonArray ( const QString& arrKey ) const;
		QJsonObject jsonObject () const;
		QJsonObject jsonObject ( const QString& objKey ) const;

		bool containsAtTopLevel ( const QString& key ) const;

		QJsonObject getBuffer ( qsizetype bufferIndex ) const;
		QJsonArray getBuffers () const;

		void printJsonObject ( const QJsonObject& obj, int maxDepth = 8 ) const;
		void printJsonArray ( const QJsonArray& arr, int maxDepth = 8 ) const;
		void printJsonDocument ( int maxDepth = 8 ) const;

		bool getScenes ();
		bool getNodes ();
		bool getMeshes ();
		bool getBuffers ();
		bool getBufferViews ();
		bool getAccessors ();
		bool getCameras ();
		bool getSkins ();
		bool getAnimations ();
		bool getMaterials ();
		bool getTextures ();
		bool getSamplers ();
		bool getImages ();
		void getAll ();

	private:
		QJsonDocument		m_document;

		QList<Scene>		m_scenes;
		QList<Node>			m_nodes;
		QList<Mesh>			m_meshes;
		QList<Buffer>		m_buffers;
		QList<BufferView>	m_bufferViews;
		QList<Accessor>		m_accessors;
		QList<Camera>		m_cameras;
		QList<Skin>			m_skins;
		QList<Animation>	m_animations;
		QList<Material>		m_materials;
		QList<Texture>		m_textures;
		QList<Sampler>		m_samplers;
		QList<Image>		m_images;
	};
}

#endif // !__GLTF_LOADER_H__

