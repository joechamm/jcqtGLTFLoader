/*****************************************************************//**
 * \file   GLTFException.h
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
 * \brief  Exception handling
 * 
 * \author joechamm
 * \date   September 2022
 *********************************************************************/
#ifndef __GLTF_EXCEPTION_H__
#define __GLTF_EXCEPTION_H__

#include <QException>

namespace jcqt
{
	class GLTFException : public QException
	{
	public:
		GLTFException ( const QByteArray& message ) : m_message ( message ) {}
		explicit GLTFException ( const char* message ) : m_message ( message ) {}
		explicit GLTFException ( const QString& message ) : m_message ( message.toLocal8Bit () ) {}

		GLTFException* clone () const override
		{
			return new GLTFException ( *this );
		}

		void raise () const override
		{
			throw* this;
		}

		const char* what () const
		{
			return m_message.constData ();
		}
	protected:
		QByteArray m_message;
	};
}

#endif // !__JCQT_EXCEPTION_H__
