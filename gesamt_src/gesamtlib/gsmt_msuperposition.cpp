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
//    21.05.16   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  GSMT_MSuperposition <implementation>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::MSuperposition
//       ~~~~~~~~~
//
//  (C) E. Krissinel, 2008-2016
//
// =================================================================
//

#include "gsmt_msuperposition.h"

// =================================================================

gsmt::MSuperposition::MSuperposition()  {
  Init();
}

gsmt::MSuperposition::MSuperposition ( PSuperposition SD )  {
  Init();
  CopyFrom ( SD );
}

gsmt::MSuperposition::~MSuperposition()  {
  FreeMemory();
}


void gsmt::MSuperposition::Init()  {

  nStruct = 0;     // number of structures
  T       = NULL;  // transformation matrices
  rmsd    = 0.0;   // best superposition r.m.s.d.
  Q       = 0.0;   // Q-score
  seqId   = 0.0;   // sequence identity
  sSize   = NULL;  // structure sizes
  dSize   = NULL;  // domain sizes
  Nalgn   = 0;     // alignment length
  c       = NULL;  // atom correspondence vectors
  var     = NULL;  // distance variances  i <-> c[0][i]

}

void gsmt::MSuperposition::FreeMemory()  {

  if (T)  {
    delete[] T;
    T = NULL;
  }
  
  mmdb::FreeVectorMemory ( sSize,0 );
  mmdb::FreeVectorMemory ( dSize,0 );
  mmdb::FreeMatrixMemory ( c    ,nStruct,0,0 );
  mmdb::FreeVectorMemory ( var  ,0 );

}


void gsmt::MSuperposition::CopyFrom ( PSuperposition SD )  {

  FreeMemory();
  
  nStruct = 2;

  T = new mmdb::mat44[nStruct];
  mmdb::Mat4Init ( T[0] );
  mmdb::Mat4Copy ( SD->T,T[1] );

  rmsd  = SD->rmsd;
  Q     = SD->Q;
  seqId = SD->seqId;
  Nalgn = SD->Nalgn;

  mmdb::GetVectorMemory ( sSize,2,0 );
  sSize[0] = SD->natoms1;
  sSize[1] = SD->natoms2;

  mmdb::GetVectorMemory ( dSize,2,0 );
  dSize[0] = SD->natoms1;
  dSize[1] = SD->natoms2;

  c = new mmdb::ivector[2];
  mmdb::GetVectorMemory ( c[0],sSize[0],0 );
  mmdb::GetVectorMemory ( c[1],sSize[1],0 );
  mmdb::GetVectorMemory ( var ,sSize[0],0 );

  for (int i=0;i<sSize[0];i++)  {
    c[0][i] = SD->c1[i];
    var[i]  = SD->dist1[i];
  }
  for (int i=0;i<sSize[1];i++)
    c[1][i] = SD->c2[i];

/*
  printf ( " >>> nat1=%i  nat2=%i\n",natoms[0],natoms[1]);
  for (int i=0;i<natoms[0];i++)
    if (c[0][i]>=0)  printf ( "*" );
               else  printf ( "-" );
  printf ( "\n\n");
  for (int i=0;i<natoms[1];i++)
    if (c[1][i]>=0)  printf ( "*" );
               else  printf ( "-" );
  printf ( "\n\n");
*/

}



void gsmt::MSuperposition::CopyTo ( PSuperposition SD )  {

  SD->FreeMemory();

  if (nStruct>=2)
    SD->SetSuperposition ( T[1],rmsd,seqId,Q,Nalgn,c[0],var,sSize[0],
                           c[1],sSize[1] );
}

