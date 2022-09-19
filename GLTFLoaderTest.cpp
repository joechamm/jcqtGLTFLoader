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
		QString testFilename = ":/test/test.gltf";
		jcqt::GLTFLoader loader;
		bool loadSuccess = loader.loadGLTF ( testFilename );
		if ( loadSuccess )
		{
			qDebug () << "loadGLTF SUCCESS!" << Qt::endl;
			loader.printJsonDocument ( 8 );
		}
		else
		{
			qDebug () << "loadGLTF FAILED!" << Qt::endl;
		}
		QVERIFY ( loadSuccess );
	}

	void testLoadGLTFBuffers ()
	{
		bool bufferSuccess = false;
		QString testFilename = ":/test/test.gltf";
		jcqt::GLTFLoader loader;
		bool loadSuccess = loader.loadGLTF ( testFilename );
		if ( loadSuccess )
		{
			QJsonObject jsonBuff0 = loader.getBuffer ( 0 );
			if (!jsonBuff0.isEmpty () )
			{
				jcqt::GLTFBuffer gltfBuff0 ( jsonBuff0 );
				bufferSuccess = gltfBuff0.loadData ();
				if ( bufferSuccess )
				{
					qDebug () << "GLTFBuffer::loadData() SUCCESS!" << Qt::endl << "Data:" << Qt::endl << gltfBuff0.constData () << Qt::endl;
				}
			}
		}
		QVERIFY ( bufferSuccess );
	}

	void cleanupTestCase ()
	{
		qDebug ( "GLTFLoaderTest cleanupTestCase" );
	}
};

QTEST_MAIN(GLTFLoaderTest)
#include "gltfloadertest.moc"
