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
//    28.01.16   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  GSMT_SheafData <interface>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::SheafData
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2008-2016
//
// =================================================================
//

#include <string.h>

#include "gsmt_sheafdata.h"

// =================================================================

gsmt::SheafData::SheafData ( int n_struct, int n_atoms )  {
  initSheafData ( n_struct,n_atoms );
}

gsmt::SheafData::~SheafData()  {
  freeMemory();
}

void gsmt::SheafData::clear()  {
  freeMemory();
  initSheafData ( 0,0 );
}

void gsmt::SheafData::initSheafData ( int n_struct, int n_atoms )  {

  nStruct = n_struct;
  nAtoms  = n_atoms;

  if (nStruct>0)  {
    mmdb::GetVectorMemory ( sId,nStruct,0 );
    S = new PStructure [nStruct];
    T = new mmdb::mat44[nStruct];
  } else  {
    sId = NULL;
    S   = NULL;
    T   = NULL;
  }

  if (nAtoms>0)  {
    mmdb::GetVectorMemory ( mask,nAtoms,0 );  // atom mask
    mmdb::GetVectorMemory ( cx  ,nAtoms,0 );  // consensus x
    mmdb::GetVectorMemory ( cy  ,nAtoms,0 );  // consensus y
    mmdb::GetVectorMemory ( cz  ,nAtoms,0 );  // consensus z
    mmdb::GetVectorMemory ( var2,nAtoms,0 );  // distance variance
  } else  {
    mask = NULL;
    cx   = NULL;
    cy   = NULL;
    cz   = NULL;
    var2 = NULL;
  }
  
  Q      = -1.0;
  rmsd   = -1.0;
  seqId  = -1.0;   // average sequence identity
  Nalign = -1;

}


void gsmt::SheafData::freeMemory()  {

  mmdb::FreeVectorMemory ( sId,0 );
  if (S)  {
    delete[] S;
    S = NULL;
  }
  if (T)  {
    delete[] T;
    T = NULL;
  }

  mmdb::FreeVectorMemory ( mask,0 );
  mmdb::FreeVectorMemory ( cx  ,0 );
  mmdb::FreeVectorMemory ( cy  ,0 );
  mmdb::FreeVectorMemory ( cz  ,0 );
  mmdb::FreeVectorMemory ( var2,0 );

}


void gsmt::SheafData::addStructure ( PStructure st, int structNo )  {
mmdb::ivector sId1;
PPStructure   S1;
mmdb::mat44  *T1;
int           ns1;

  ns1 = nStruct + 1;
  mmdb::GetVectorMemory ( sId1,ns1,0 );
  S1 = new PStructure[ns1];
  T1 = new mmdb::mat44[ns1];
  for (int i=0;i<nStruct;i++)  {
    sId1[i] = sId[i];
    S1  [i] = S  [i];
    mmdb::Mat4Copy ( T[i],T1[i] );
  }
  sId1[nStruct] = structNo;
  S1  [nStruct] = st;
  mmdb::Mat4Init ( T1[nStruct] );
  
  mmdb::FreeVectorMemory ( sId,0 );
  if (S)  delete[] S;
  if (T)  delete[] T;
  
  sId = sId1;
  S   = S1;
  T   = T1;
  
  nStruct = ns1;

}


bool gsmt::SheafData::hasStructure ( int structNo )  {
bool has = false;
  for (int i=0;(i<nStruct) && (!has);i++)
    has = (sId[i]==structNo);
  return has;
}

bool gsmt::SheafData::hasStructure ( PSheafData sd )  {
bool has = false;
  if (sd)
    for (int i=0;(i<nStruct) && (!has);i++)
      for (int j=0;(j<sd->nStruct) && (!has);j++)
        has = (sId[i]==sd->sId[j]);
  return has;
}

void gsmt::SheafData::copyFrom ( PSheafData source )  {

  freeMemory();
  initSheafData ( source->nStruct,source->nAtoms );

  for (int i=0;i<nStruct;i++)  {
    sId[i] = source->sId[i];
    S  [i] = source->S  [i];
    mmdb::Mat4Copy ( source->T[i],T[i] );
  }
  
  for (int i=0;i<nAtoms;i++)  {
    mask[i] = source->mask[i];
    cx  [i] = source->cx  [i];
    cy  [i] = source->cy  [i];
    cz  [i] = source->cz  [i];
    var2[i] = source->var2[i];
  }
  
  Q      = source->Q;
  rmsd   = source->rmsd;
  seqId  = source->seqId;
  Nalign = source->Nalign;

}


void gsmt::SheafData::writeResAlign ( mmdb::io::RFile f )  {
mmdb::PPAtom a;
char         L[100],SS[4];
int          i,j, nat,sseType;

  f.Write ( "      Var.  | " );
  for (i=0;i<nStruct;i++)  {
    f.Write ( "| " );
    sprintf ( L,"   %4s    ",S[i]->getRefName() );
    f.Write ( L );
  }
  f.LF();
  f.Write ( " -----------+-" );
  for (i=0;i<nStruct;i++)
    f.Write ( "+------------" );
  f.LF();
  
  for (i=0;i<nAtoms;i++)  {
    sprintf ( L," %10.3f ",sqrt(var2[i]) );
    f.Write ( L );
    if (mask[i])  f.Write ( "|*" );
            else  f.Write ( "| " ); 
    for (j=0;j<nStruct;j++)  {
      S[j]->getCalphas ( a,nat );
      sseType = a[i]->GetSSEType();
      if (sseType==mmdb::SSE_Helix)       strcpy ( SS,"H " );
      else if (sseType==mmdb::SSE_Strand) strcpy ( SS,"S " );
                                     else strcpy ( SS,"  " );
      sprintf ( L,"|%2s%1s:%3s%4i%1s",SS,a[i]->GetChainID(),
                  a[i]->GetResName(),a[i]->GetSeqNum(),
                  a[i]->GetInsCode() );
      f.Write ( L );
    }
    f.LF();
  }

  f.Write ( " -----------'-" );
  for (i=0;i<nStruct;i++)
    f.Write ( "'------------" );
  f.LF();

}

// =================================================================

gsmt::SortSheafData::SortSheafData() : ShiftSort() {}
gsmt::SortSheafData::~SortSheafData() {}
      
int gsmt::SortSheafData::Compare ( void * di, void * dj )  {
// sorting by increasing Q-score
  if (PSheafData(di)->Q<PSheafData(dj)->Q)      return  1;
  else if (PSheafData(di)->Q>PSheafData(dj)->Q) return -1;
                                           else return  0;
}

void gsmt::SortSheafData::Sort ( PPSheafData d, int dataLen )  {
  ShiftSort::Sort ( (void**)d,dataLen );
}


/*
void gsmt::SheafData::writeLogFile ( mmdb::io::RFile f )  {
char L[300];
  
  sprintf ( L,"Q-score : %.4f\n"
              "R.m.s.d.: %.4f\n"
              "Nalign  : %i",
              Q,rmsd,Nalign );
  f.WriteLine ( L );

}
*/
