//
// =================================================================
//  This code is distributed under the terms and conditions of the
//  CCP4 Program Suite Licence Agreement as 'Part 2' (Annex 2)
//  software. A copy of the CCP4 licence can be obtained by writing
//  to CCP4, Research Complex at Harwell, Rutherford Appleton
//  Laboratory, Didcot OX11 0FA, UK, or from
//  http://www.ccp4.ac.uk/ccp4license.php.
// =================================================================
//
//    27.12.15   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  GSMT_Sequence <implementation>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::Sequence
//       ~~~~~~~~~
//
//  (C) E. Krissinel, 2008-2015
//
// =================================================================
//

#include <string.h>

#include "gsmt_sequence.h"
#include "gsmt_structure.h"
#include "mmdb2/mmdb_io_file.h"

// =================================================================

gsmt::Sequence::Sequence() : mmdb::io::Stream()  {
  initClass();
}

gsmt::Sequence::Sequence ( mmdb::io::RPStream Object )
               : mmdb::io::Stream(Object)  {
  initClass();
}

gsmt::Sequence::~Sequence()  {
  freeMemory();
}

void  gsmt::Sequence::initClass()  {
  id[0] = char(0);
  seq   = NULL;
  title = NULL;
}

void  gsmt::Sequence::freeMemory() {
  if (seq)   delete seq;
  if (title) delete title;
  seq   = NULL;
  title = NULL;
}

gsmt::Sequence::RETURN_CODE gsmt::Sequence::loadSequence (
                                              mmdb::cpstr filePath )  {
mmdb::io::File f;
char           L[5001];
int            k;

  freeMemory();
  mmdb::CreateCopy ( seq  ,"" );
  mmdb::CreateCopy ( title,"" );

  f.assign ( filePath,true,false );
  if (f.reset())  {
    k = 0;
    while ((k>=0) && (!f.FileEnd()))  {
      f.ReadLine ( L,5000 );
      mmdb::CutSpaces ( L,mmdb::SCUTKEY_BEGEND );
      if (L[0]!='#')  {
        if ((k==0) && (L[0]=='>'))  {
          mmdb::CreateCopy ( title,&(L[1]) );
          k = 1;
        } else if (L[0]=='>')  {
          k = -2;
        } else if (L[0])  {
          if (k==1)  mmdb::CreateConcat ( seq,L );
               else  k = -1;
        }
      }
    }
    if (k>0)  {
      mmdb::DelSpaces ( seq );
      if (!seq[0])
        return RC_NoSequence;
    } else if (k==-1)
      return RC_WrongFormat;
    return RC_Ok;
  } else
    return RC_CantOpenFile;

}

int gsmt::Sequence::getSeqLength()  {
  if (seq)  return strlen(seq);
  return 0;
}

void gsmt::Sequence::setSequence ( PStructure structure )  {
  structure->getCoordSequence ( seq   );
  structure->getPDBTitle      ( title );
  strcpy ( id,structure->getMMDBManager()->GetEntryID() );
}

void gsmt::Sequence::setSequence ( mmdb::cpstr sequence )  {
  mmdb::CreateCopy ( seq,sequence );
}

void gsmt::Sequence::copy ( PSequence S )  {
// deep-copy from S to this
  mmdb::CreateCopy ( seq,S->getSequence() );
}

void gsmt::Sequence::read  ( mmdb::io::RFile f )  {
  f.ReadTerLine ( id    );
  f.CreateRead  ( seq   );
  f.CreateRead  ( title );
}

void gsmt::Sequence::write ( mmdb::io::RFile f )  {
  f.WriteTerLine ( id    );
  f.CreateWrite  ( seq   );
  f.CreateWrite  ( title );
}


mmdb::pstr gsmt::Sequence::getErrDesc ( RETURN_CODE rc,
                                        mmdb::pstr & S )  {

  switch (rc)  {
    case RC_Ok          :
          mmdb::CreateCopy ( S," no error\n\n" ); break;
    case RC_CantOpenFile:
          mmdb::CreateCopy ( S," cannot open file\n\n" ); break;
    case RC_WrongFormat :
          mmdb::CreateCopy ( S," wrong file format\n\n" ); break;
    case RC_NoSequence  :
          mmdb::CreateCopy ( S," no sequence found\n\n" ); break;
    default :
          mmdb::CreateCopy ( S," unknown error code\n\n" );
  }

  return S;

}

