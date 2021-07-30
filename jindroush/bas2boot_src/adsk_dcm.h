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

#ifndef __CDCM_H__
#define __CDCM_H__

#include "errbase.h"
#include "wintypes.h"
#include "adsk.h"
#include "cfile.h"
#include "cprefile.h"

#define CDCM_FORMAT_VIOLATED ( CDCM_BASE + 0 )

class CDcm : public ADisk
{
public:
				CDcm();
				~CDcm();

				BOOL Load( char*, BOOL = FALSE, BOOL = FALSE );

				BOOL Save( char*, BOOL );

private:

				BOOL DecodeRec41( CGenFile& );
				BOOL DecodeRec42( CGenFile& );
				BOOL DecodeRec43( CGenFile& );
				BOOL DecodeRec44( CGenFile& );
				BOOL DecodeRec46( CGenFile& );
				BOOL DecodeRec47( CGenFile& );
				BOOL DecodeRecFA( CGenFile& );
				WORD ReadOffset( CGenFile& );

				void EncodeRec41( BYTE*, int*, BYTE*, BYTE*, int );
				void EncodeRec43( BYTE*, int*, BYTE*, int );
				void EncodeRec44( BYTE*, int*, BYTE*, BYTE*, int );

				void EncodeRec45();
				void EncodeRec46();
				void EncodeRec( BOOL );
				void EncodeRecFA( BOOL, int, int, int );

				BOOL	m_bLastPass;
				BYTE	m_abtCurrBuff[ 0x100 ];
				BYTE	m_abtPrevBuff[ 0x100 ];
				int	m_iSectorSize;
				WORD	m_wCurrentSector;
				long	m_lFileLength;
				BOOL	m_bAlreadyFormatted;


				BYTE* m_pbtCurr;
				BYTE* m_pbtPass;
				BYTE*	m_pbtLastRec;
};

#endif

