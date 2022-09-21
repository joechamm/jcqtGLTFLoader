/*****************************************************************//**
 * \file   GLTFCamera.h
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
 * \brief  Encapsulate a glTF camera
 * 
 * \author joechamm
 * \date   September 2022
 *********************************************************************/
#ifndef __GLTF_CAMERA_H__
#define __GLTF_CAMERA_H__

#include <QJsonObject>
#include <QRect>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "GLTFException.h"

namespace jcqt
{
	enum class eCameraType : quint8
	{
		eCameraType_NOT_SET = 1,
		eCameraType_PERSPECTIVE = 2,
		eCameraType_ORTHOGRAPHIC = 3,
		eCameraType_INVALID = 99
	};

	struct CameraProjection
	{
		glm::mat4 m_proj;
	};

	struct CameraPerspective
	{
		float m_aspectRatio = 4.0f / 3.0f;
		float m_yFovRads = glm::radians ( 45.0f ); // REQUIRED
		float m_zFar = 100.0f;
		float m_zNear = 0.01f; // REQUIRED
	};

	struct CameraOrthographic
	{
		float m_xMag = 1.0f; // REQUIRED
		float m_yMag = 1.0f; // REQUIRED
		float m_zFar = 1.0f; // REQUIRED
		float m_zNear = 0.f; // REQUIRED
	};

	struct Camera
	{
		eCameraType m_type = eCameraType::eCameraType_NOT_SET;
		CameraProjection m_cameraProjection = { .m_proj = glm::mat4 ( 1.0f ) };
		QJsonObject m_jsonObject;
	};

	struct Viewport
	{
		QRect m_view;
	};

	CameraProjection getCameraProjectionFromPerspective ( const CameraPerspective& persp )
	{
		CameraProjection proj;
		proj.m_proj = glm::perspective ( persp.m_yFovRads, persp.m_aspectRatio, persp.m_zNear, persp.m_zFar );
		return proj;
	}

	CameraProjection getCameraProjectionFromOrthographic ( const CameraOrthographic& ortho, Viewport* view )
	{
		CameraProjection proj;
		try
		{
			if ( !view )
			{
				throw new GLTFException ( "SET PROJECTION FAILURE" );
			}

			float left = static_cast<float>(view->m_view.left ());			
			float top = static_cast< float >( view->m_view.top ());
			
			float width = static_cast< float >( view->m_view.width ());
			float height = static_cast< float >( view->m_view.height ());

			float right = left + width * ortho.m_xMag;
			float bottom = top + height * ortho.m_yMag;

			proj.m_proj = glm::ortho ( left, right, bottom, top, ortho.m_zNear, ortho.m_zFar );
			return proj;			
		}
		catch ( const GLTFException& e )
		{
			qWarning () << e.what () << Qt::endl;
		}

		return proj;
	}

	CameraPerspective getCameraPerspectiveFromJson ( const QJsonObject& jsonObj, Viewport* view )
	{
		CameraPerspective perspective;

		try
		{
			if ( jsonObj.contains ( "aspectRatio" ) )
			{
				perspective.m_aspectRatio = static_cast< float >( jsonObj.value ( "aspectRatio" ).toDouble () );
			}
			else if ( view )
			{
				int w = view->m_view.width ();
				int h = view->m_view.height ();
				perspective.m_aspectRatio = w / ( float ) h;
			}
			else
			{
				throw new GLTFException ( "ASPECT RATIO NOT SET EXCEPTION" );
			}

			perspective.m_yFovRads = static_cast<float>(jsonObj.value ( "yfov" ).toDouble ( glm::radians ( 45.0f ) ));
			perspective.m_zFar = static_cast< float >( jsonObj.value ( "zfar" ).toDouble ( 100.0 ) );
			perspective.m_zNear = static_cast< float >( jsonObj.value ( "znear" ).toDouble ( 0.0 ) );

			return perspective;
		}
		catch ( const GLTFException& e )
		{
			qWarning () << e.what () << Qt::endl;
		}
		
		return perspective;
	}

	CameraOrthographic getCameraOrthographicFromJson ( const QJsonObject& jsonObj )
	{
		CameraOrthographic orthographic;

		orthographic.m_xMag = static_cast< float >( jsonObj.value ( "xmag" ).toDouble ( 1.0 ) );
		orthographic.m_yMag = static_cast< float >( jsonObj.value ( "ymag" ).toDouble ( 1.0 ) );
		orthographic.m_zFar = static_cast< float >( jsonObj.value ( "zfar" ).toDouble ( 1.0 ) );
		orthographic.m_zNear = static_cast< float >( jsonObj.value ( "znear" ).toDouble ( 0.0 ) );
		return orthographic;
	}

	Camera createCameraFromJson ( const QJsonObject& jsonObj, Viewport * view = nullptr )
	{
		Camera camera;
		
		try
		{
			if ( !jsonObj.contains ( "type" ) )
			{
				throw new GLTFException ( "INVALID JSON EXCEPTION" );
			}

			QString camType = jsonObj.value ( "type" ).toString ();
			if ( camType.compare ( "perspective", Qt::CaseInsensitive ) == 0 )
			{
				CameraPerspective camPerspective = getCameraPerspectiveFromJson ( jsonObj.value ( "perspective" ).toObject (), view );
				camera.m_type = eCameraType::eCameraType_PERSPECTIVE;
				camera.m_cameraProjection = getCameraProjectionFromPerspective ( camPerspective );
				camera.m_jsonObject = jsonObj;
				return camera;
			}
			else if ( camType.compare ( "orthographic", Qt::CaseInsensitive ) == 0 )
			{
				CameraOrthographic camOrtho = getCameraOrthographicFromJson ( jsonObj.value ( "orthographic" ).toObject () );
				camera.m_type = eCameraType::eCameraType_ORTHOGRAPHIC;
				camera.m_cameraProjection = getCameraProjectionFromOrthographic ( camOrtho, view );
				camera.m_jsonObject = jsonObj;
				return camera;
			}
		}
		catch ( const GLTFException& e )
		{
			qWarning () << e.what () << Qt::endl;
		}
	}
}

#endif // !__GLTF_CAMERA_H__
