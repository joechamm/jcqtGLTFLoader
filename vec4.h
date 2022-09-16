/*****************************************************************//**
 * \file   vec4.h
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
 * \brief  helper classes for storing and loading vector data
 * 
 * \author joechamm
 * \date   September 2022
 *********************************************************************/
#ifndef __VEC_4_H__
#define __VEC_4_H__

#ifdef __GNUC__
#define PACKED_STRUCT __attribute__((packed,aligned(1)))
#else 
#define PACKED_STRUCT
#endif

#include <QMatrix4x4>

namespace jcqt
{
	struct PACKED_STRUCT gpuvec4
	{
		float x, y, z, w;

		gpuvec4 () = default;
		explicit gpuvec4 ( float v ) : x ( v ), y ( v ), z ( v ), w ( v ) {}
		gpuvec4 ( float a, float b, float c, float d ) : x ( a ), y ( b ), z ( c ), w ( d ) {}
		explicit gpuvec4 ( const QVector4D& v ) : x ( v.x () ), y ( v.y () ), z ( v.z () ), w ( v.w () ) {}
	};

	struct PACKED_STRUCT gpumat4
	{
		float data_ [ 16 ];

		gpumat4 () = default;
		explicit gpumat4 ( const QMatrix4x4& m )
		{
			memcpy ( data_, m.constData (), 16 * sizeof ( float ) );
		}

		/**
		 * operator()
		 * const float &
		 * \brief access data in column major order.
		 * 
		 * \param col
		 * \param row
		 * \return matrix element in column 'col' and row 'row'
		 */
		inline const float & operator()( int col, int row ) const
		{
			return data_ [ col * 4 + row ];
		}

		inline float & operator()( int col, int row )
		{
			return data_ [ col * 4 + row ];
		}
	};
}

#endif // !__VEC_4_H__
