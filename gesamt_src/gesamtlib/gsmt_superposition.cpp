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
//    10.02.13   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  GSMT_Superposition <implementation>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  Superposition
//       ~~~~~~~~~
//
//  (C) E. Krissinel, 2008-2013
//
// =================================================================
//

#include "gsmt_superposition.h"

// =================================================================

gsmt::Superposition::Superposition()  {
  Init();
}

gsmt::Superposition::Superposition ( PSuperposition SD )  {
  Init();
  CopyFrom ( SD );
}

gsmt::Superposition::~Superposition()  {
  FreeMemory();
}

void gsmt::Superposition::FreeMemory()  {
  mmdb::FreeVectorMemory ( c1   ,0 );
  mmdb::FreeVectorMemory ( dist1,0 );
  mmdb::FreeVectorMemory ( c2   ,0 );
}

void gsmt::Superposition::Init()  {
  mmdb::Mat4Init ( T ); // transformation matrix
  rmsd    = 0.0;          // best superposition r.m.s.d.
  seqId   = 0.0;          // sequence identity
  Q       = 0.0;          // Q-score
  natoms1 = 0;            // length of 1st structure
  natoms2 = 0;            // length of 2nd structure
  Nalgn   = 0;            // alignment length
  c1      = NULL;         // correspondence vector for structure #1
  c2      = NULL;         // correspondence vector for structure #2
  dist1   = NULL;         // distances  i <->c1[i]
}


void gsmt::Superposition::SetSuperposition ( mmdb::mat44  & sup_T,
                                             mmdb::realtype sup_rmsd,
                                             mmdb::realtype sup_seqId,
                                             mmdb::realtype sup_Q,
                                             int            sup_Nalgn,
                                             mmdb::ivector  sup_c1,
                                             mmdb::rvector  sup_dist1,
                                             int            sup_nat1,
                                             mmdb::ivector  sup_c2,
                                             int            sup_nat2 ) {
int  i;

  FreeMemory();

  mmdb::Mat4Copy ( sup_T,T ); // transformation matrix
  rmsd  = sup_rmsd;           // best superposition r.m.s.d.
  seqId = sup_seqId;          // sequence identity
  Q     = sup_Q;              // Q-score
  Nalgn = sup_Nalgn;          // alignment length

  mmdb::GetVectorMemory ( c1   ,sup_nat1,0 );
  mmdb::GetVectorMemory ( dist1,sup_nat1,0 );
  mmdb::GetVectorMemory ( c2   ,sup_nat2,0 );

  for (i=0;i<sup_nat1;i++)  {
    c1[i] = sup_c1[i];
    if (c1[i]>=0)  dist1[i] = sqrt(sup_dist1[i]);
             else  dist1[i] = -1.0;
  }
  for (i=0;i<sup_nat2;i++)
    c2[i] = sup_c2[i];

  natoms1 = sup_nat1;
  natoms2 = sup_nat2;

}


void gsmt::Superposition::CopyFrom ( PSuperposition SDfrom )  {

  FreeMemory();

  if (SDfrom)  {

    mmdb::Mat4Copy ( SDfrom->T,T );
    rmsd    = SDfrom->rmsd;
    Q       = SDfrom->Q;
    seqId   = SDfrom->seqId;
    natoms1 = SDfrom->natoms1;
    natoms2 = SDfrom->natoms2;
    Nalgn   = SDfrom->Nalgn;

    mmdb::GetVectorMemory ( c1   ,natoms1,0 );
    mmdb::GetVectorMemory ( dist1,natoms1,0 );
    mmdb::GetVectorMemory ( c2   ,natoms2,0 );

    for (int i=0;i<natoms1;i++)  {
      c1   [i] = SDfrom->c1   [i];
      dist1[i] = SDfrom->dist1[i];
    }
    for (int i=0;i<natoms2;i++)
      c2[i] = SDfrom->c2[i];

  }

}
