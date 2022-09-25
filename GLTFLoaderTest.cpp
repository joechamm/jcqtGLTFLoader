/*****************************************************************//**
 * \file   GLTFLoaderTest.cpp
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

#include <QTest>
#include "GLTFLoader.h"
#include "GLTFBuffer.h"

class GLTFLoaderTest : public QObject
{
	Q_OBJECT

private slots:
	void initTestCase ()
	{
		qDebug ( "GLTFLoaderTest initTestCase" );
	}
	
	void testLoadGLTF ()
	{
		bool gotAccessors, gotAnimations, gotBuffers, gotBufferViews, gotCameras, gotImages, gotMaterials, gotMeshes, gotNodes, gotSamplers, gotScenes, gotSkins, gotTextures;
		QString testFilename = ":/test/test.gltf";
		jcqt::GLTFLoader loader;
		bool loadSuccess = loader.loadGLTF ( testFilename );
		if ( loadSuccess )
		{
			gotAccessors = loader.getAccessors ();
			gotAnimations = loader.getAnimations ();
			gotBuffers = loader.getBuffers ();
			gotBufferViews = loader.getBufferViews ();
			gotCameras = loader.getCameras ();
			gotImages = loader.getImages ();
			gotMaterials = loader.getMaterials ();
			gotMeshes = loader.getMeshes ();
			gotNodes = loader.getNodes ();
			gotSamplers = loader.getSamplers ();
			gotScenes = loader.getScenes ();
			gotSkins = loader.getSkins ();
			gotTextures = loader.getTextures ();
			
			qDebug () << "loadGLTF SUCCESS!" << Qt::endl;
			loader.printJsonDocument ( 8 );
		}
		else
		{
			qDebug () << "loadGLTF FAILED!" << Qt::endl;
			gotAccessors = false;
			gotAnimations = false;
			gotBuffers = false;
			gotBufferViews = false;
			gotCameras = false;
			gotImages = false;
			gotMaterials = false;
			gotMeshes = false;
			gotNodes = false;
			gotSamplers = false;
			gotScenes = false;
			gotSkins = false;
			gotTextures = false;
		}
		QVERIFY ( loadSuccess );
		QVERIFY ( gotAccessors );
		QVERIFY ( gotAnimations );
		QVERIFY ( gotBuffers );
		QVERIFY ( gotBufferViews );
		QVERIFY ( gotCameras );
		QVERIFY ( gotImages );
		QVERIFY ( gotMaterials );
		QVERIFY ( gotMeshes );
		QVERIFY ( gotNodes );
		QVERIFY ( gotSamplers );
		QVERIFY ( gotScenes );
		QVERIFY ( gotSkins );
		QVERIFY ( gotTextures );
	}

	void cleanupTestCase ()
	{
		qDebug ( "GLTFLoaderTest cleanupTestCase" );
	}
};

QTEST_MAIN(GLTFLoaderTest)
#include "gltfloadertest.moc"
