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

#include "cfs.h"

CDirEntry::CDirEntry()
{
	*m_szFname = '\0';
	*m_szAscData = '\0';
	m_dwFlags = 0;
	m_pSubdir = NULL;
	m_pNext = NULL;
	m_pPrev = NULL;

	#ifdef _MEMORY_DUMP_
		printf( "CDirEntry constructed: %08X\n", this );
	#endif
}

CDirEntry::~CDirEntry()
{
	#ifdef _MEMORY_DUMP_
		printf( "CDirEntry destructed: %08X\n", this );
	#endif
}

CFs::CFs()
{
	#ifdef _MEMORY_DUMP_
		printf( "CFs constructed: %08X\n", this );
	#endif
}

CFs::~CFs()
{
	#ifdef _MEMORY_DUMP_
		printf( "CFs destructed: %08X\n", this );
	#endif
}

void CFs::DeleteList( CDirEntry* pList )
{
	CDirEntry* pCurr = pList;
	CDirEntry* pNext;

	while( pCurr )
	{
		if ( pCurr->m_pSubdir )
			DeleteList( pCurr->m_pSubdir );

		pNext = pCurr->m_pNext;
		delete pCurr;

		pCurr = pNext;
	}
}

