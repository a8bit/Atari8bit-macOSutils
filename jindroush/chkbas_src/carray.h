//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#include "wintypes.h"

#ifndef __CARRAY_H__
#define __CARRAY_H__

#define CARRAY_SUB_SIZE 16

typedef struct sa
{
	struct sa* pPrev;
	struct sa* pNext;
	void* array[ CARRAY_SUB_SIZE ];
	
} SubArray;

class CArray
{
public:
	CArray();
	~CArray();
	void* GetAt( int );
	BOOL SetAt( int, void* );
	int GetSize() { return m_iArrayItems; };
	BOOL Add( void* );
	BOOL DeleteLast();
	void Destroy();
private:
	BOOL AddNewBlock();
	int m_iAllocatedBlocks;
	int m_iArrayItems;
	SubArray * m_pFirstBlock;
};

#endif
