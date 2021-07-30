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

#include "carray.h"

//constructs new object - 0 size
CArray::CArray()
{
	m_iAllocatedBlocks = 0;
	m_iArrayItems = 0;
	m_pFirstBlock = NULL;
}

//destructor calls destroy
CArray::~CArray()
{
	Destroy();
}

//destructs all sub array blocks
void CArray::Destroy()
{
	if ( m_iAllocatedBlocks )
	{
		SubArray* pCurr = m_pFirstBlock;
		SubArray* pNext;

		while ( pCurr )
		{
			pNext = pCurr->pNext;
			delete pCurr;
			pCurr = pNext;
		}
	}

	m_iAllocatedBlocks = 0;
	m_iArrayItems = 0;
	m_pFirstBlock = NULL;
}

//deletes last item
BOOL CArray::DeleteLast()
{
	m_iArrayItems--;
	return TRUE;
}

//gets value from array
void* CArray::GetAt( int iItem )
{
	if ( ( iItem + 1 ) > m_iArrayItems )
		return NULL;

	int iBlockNeeded = iItem / CARRAY_SUB_SIZE;

	int iBlockCurr = 0;

	SubArray* pCurr = m_pFirstBlock;

	while( iBlockCurr < iBlockNeeded )
	{
		pCurr = pCurr->pNext;
		iBlockCurr++;

		if ( !pCurr )
			return NULL;
	}

	return pCurr->array[ iItem % CARRAY_SUB_SIZE ];
}

//sets array's value - must exist before
BOOL CArray::SetAt( int iItem, void* pPtr )
{
	if ( ( iItem + 1 ) > m_iArrayItems )
		return FALSE;

	int iBlockNeeded = iItem / CARRAY_SUB_SIZE;

	int iBlockCurr = 0;

	SubArray* pCurr = m_pFirstBlock;

	while( iBlockCurr < iBlockNeeded )
	{
		pCurr = pCurr->pNext;
		iBlockCurr++;
		if ( !pCurr )
			return FALSE;
	}

	pCurr->array[ iItem % CARRAY_SUB_SIZE ] = pPtr;
	return TRUE;
}

//adds array's value
BOOL CArray::Add( void* pPtr )
{
	if ( ((( m_iArrayItems ) / CARRAY_SUB_SIZE ) + 1 ) > m_iAllocatedBlocks )
	{
		//if we need new block, we try to add them
		if ( !AddNewBlock() )
			return FALSE;
	}

	m_iArrayItems++;

	if ( !SetAt( m_iArrayItems - 1, pPtr ) )
	{
		m_iArrayItems--;
		return FALSE;
	}

	return TRUE;

}

//adds new sub array block
BOOL CArray::AddNewBlock()
{
	SubArray* pCurr = m_pFirstBlock;
	SubArray* pNext = m_pFirstBlock;

	//find end
	while ( pNext )
	{
		pCurr = pNext;
		pNext = pCurr->pNext;
	}

	if ( !( pNext = new SubArray ) )
		return FALSE;

	memset( pNext, 0, sizeof( SubArray) );

	if ( !m_pFirstBlock )
		m_pFirstBlock = pNext;
	else
		pCurr->pNext = pNext;

	m_iAllocatedBlocks++;

	return TRUE;
}


