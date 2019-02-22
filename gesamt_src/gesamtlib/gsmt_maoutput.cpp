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
//    15.11.15   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  GSMT_MAOutput <implementation>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::MAOutput
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2008-2015
//
// =================================================================
//

#include <string.h>
#include <ctype.h>

#include "gsmt_maoutput.h"
#include "mmdb2/mmdb_tables.h"

// -----------------------  gsmt::MAOutput  ------------------------

void gsmt::MAOutput::Init()  {
  name[0]    = char(0);
  chID[0]    = char(0);
  seqNum     = 0;
  insCode[0] = char(0);
  rmsd       = 0.0;
  sseType    = mmdb::SSE_None;
  aligned    = false;
}

void gsmt::MAOutput::Fill ( mmdb::PAtom A, bool align )  {
mmdb::PResidue res;
  res = A->GetResidue();
  if (res)  {
    strcpy ( name,res->GetResName() );
    strcpy ( chID,res->GetChainID() );
    if (!chID[0])  strcpy ( chID," " );
    seqNum = res->GetSeqNum();
    strcpy ( insCode,res->GetInsCode() );
    sseType = res->SSE;
  }
  aligned = align;
}

void  gsmt::MAOutput::Copy ( RMAOutput M )  {
  strcpy ( name,M.name );
  strcpy ( chID,M.chID );
  seqNum  = M.seqNum;
  sseType = M.sseType;
  strcpy ( insCode,M.insCode );
  rmsd    = M.rmsd;
  aligned = M.aligned;
}

void  gsmt::MAOutput::write ( mmdb::io::RFile f )  {
  f.WriteFile ( name,sizeof(name) );
  f.WriteFile ( chID,sizeof(chID) );
  f.WriteInt  ( &seqNum  );
  f.WriteInt  ( &sseType );
  f.WriteFile ( insCode,sizeof(insCode) );
  f.WriteReal ( &rmsd    );
  f.WriteBool ( &aligned );
}

void  gsmt::MAOutput::read ( mmdb::io::RFile f )  {
  f.ReadFile ( name,sizeof(name) );
  f.ReadFile ( chID,sizeof(chID) );
  f.ReadInt  ( &seqNum  );
  f.ReadInt  ( &sseType );
  f.ReadFile ( insCode,sizeof(insCode) );
  f.ReadReal ( &rmsd    );
  f.ReadBool ( &aligned );
}


int gsmt::MAOutput::getWriteSize()  {
  return   sizeof(mmdb::realUniBin) +
           sizeof(mmdb::ChainID)    +
           sizeof(mmdb::ResName)    +
           sizeof(mmdb::InsCode)    +
         3*sizeof(mmdb::intUniBin)  + 2;
}

void gsmt::MAOutput::mem_write ( mmdb::pstr S, int & l )  {
int version=1;
  mmdb::mem_write ( version,S,l );
  mmdb::mem_write ( name   ,sizeof(name)   ,S,l );
  mmdb::mem_write ( chID   ,sizeof(chID)   ,S,l );
  mmdb::mem_write ( insCode,sizeof(insCode),S,l );
  mmdb::mem_write ( rmsd   ,S,l );
  mmdb::mem_write ( sseType,S,l );
  mmdb::mem_write ( seqNum ,S,l );
  mmdb::mem_write ( aligned,S,l );
}


void gsmt::MAOutput::mem_read ( mmdb::cpstr S, int & l )  {
int version;
  mmdb::mem_read ( version,S,l );
  mmdb::mem_read ( name   ,sizeof(name)   ,S,l );
  mmdb::mem_read ( chID   ,sizeof(chID)   ,S,l );
  mmdb::mem_read ( insCode,sizeof(insCode),S,l );
  mmdb::mem_read ( rmsd   ,S,l );
  mmdb::mem_read ( sseType,S,l );
  mmdb::mem_read ( seqNum ,S,l );
  mmdb::mem_read ( aligned,S,l );
}


namespace gsmt  {

  void FreeMSOutput ( PPMAOutput & MAOutput, int & nrows )  {
  int i;
    if (MAOutput)  {
      for (i=0;i<nrows;i++)
        if (MAOutput[i])  delete[] MAOutput[i];
      delete[] MAOutput;
    }
    MAOutput = NULL;
    nrows    = 0;
  }

}
