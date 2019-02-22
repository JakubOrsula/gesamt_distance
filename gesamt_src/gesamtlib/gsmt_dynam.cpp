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
//  **** Module  :  GSMT_Dynam <implementation>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::Dynam
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2008-2013
//
// =================================================================
//

#include "gsmt_dynam.h"

// =================================================================

gsmt::Dynam::Dynam()  {
  InitClass();
}

gsmt::Dynam::~Dynam()  {
  FreeMemory();
}

void gsmt::Dynam::InitClass()  {

  maxCont2 = 0.0;

  n1       = 0;
  n2       = 0;

  V        = NULL;
  mi       = NULL;
  c1       = NULL;
  c2       = NULL;

  n1alloc  = 0;
  n2alloc  = 0;

}


void gsmt::Dynam::FreeMemory()  {

  mmdb::FreeMatrixMemory ( V ,n1alloc,0,0 );
  mmdb::FreeMatrixMemory ( mi,n1alloc,0,0 );
  mmdb::FreeVectorMemory ( c1,0 );
  mmdb::FreeVectorMemory ( c2,0 );

  n1alloc = 0;
  n2alloc = 0;
  n1      = 0;
  n2      = 0;

}

void gsmt::Dynam::GetMemory ( int natoms1, int natoms2 )  {

  if ((n1alloc<natoms1) || (n2alloc<natoms2))  {

    FreeMemory();

    mmdb::GetMatrixMemory ( V ,natoms1,natoms2,0,0 );
    mmdb::GetMatrixMemory ( mi,natoms1,natoms2,0,0 );
    mmdb::GetVectorMemory ( c1,natoms1,0 );
    mmdb::GetVectorMemory ( c2,natoms2,0 );

    n1alloc = natoms1;
    n2alloc = natoms2;

  }

  n1 = natoms1;
  n2 = natoms2;

}

void gsmt::Dynam::Align ( mmdb::PPAtom A1, int natoms1,
                          mmdb::PPAtom A2, int natoms2,
                          mmdb::realtype max_cont )  {

  maxCont2 = max_cont*max_cont;

  GetMemory ( natoms1,natoms2 );

  CalcScores    ( A1,A2 );
  BuildPathTable();
  Backtrace     ();

}

void gsmt::Dynam::MLAlign ( mmdb::PPAtom A1, int natoms1,
                            mmdb::PPAtom A2, int natoms2,
                            mmdb::realtype sigma )  {

  maxCont2 = 10.0*sigma*sigma;
    
  GetMemory ( natoms1,natoms2 );

  CalcMLScores  ( A1,A2,sigma*sigma );
  BuildPathTable();
  Backtrace     ();

}

void gsmt::Dynam::Align ( int natoms1, int natoms2,
                          mmdb::PContact contact, int nContacts,
                          mmdb::realtype max_cont )  {

  maxCont2 = max_cont*max_cont;

  GetMemory ( natoms1,natoms2 );

  CalcScores    ( contact,nContacts );
  BuildPathTable();
  Backtrace     ();

}

void gsmt::Dynam::Align ( mmdb::PPAtom   A1, int natoms1,
                          mmdb::PPAtom   A2, int natoms2,
                          mmdb::realtype R0,
                          mmdb::realtype rmsd_est )  {

  maxCont2 = R0*R0 + rmsd_est*rmsd_est;

  GetMemory ( natoms1,natoms2 );

  CalcScores    ( A1,A2 );
  BuildPathTable();
  Backtrace     ();

}

void gsmt::Dynam::Align ( int natoms1, int natoms2,
                          mmdb::PContact contact, int nContacts,
                          mmdb::realtype R0, mmdb::realtype rmsd_est ) {

  maxCont2 = R0*R0 + rmsd_est*rmsd_est;

  GetMemory ( natoms1,natoms2 );

  CalcScores    ( contact,nContacts );
  BuildPathTable();
  Backtrace     ();

}


void gsmt::Dynam::CalcScores ( mmdb::PPAtom A1, mmdb::PPAtom A2 )  {
mmdb::ivector  Vi1;
mmdb::realtype dx,dy,dz, d2;
int            i1,i2;

  for (i1=0;i1<n1;i1++)  {
    Vi1 = V[i1];
    for (i2=0;i2<n2;i2++)  {
      Vi1[i2] = 0;
      dx = A1[i1]->x - A2[i2]->x;
      dx = dx*dx;
      if (dx<maxCont2)  {
        dy = A1[i1]->y - A2[i2]->y;
        dy = dy*dy;
        if (dy<maxCont2)  {
          dz = A1[i1]->z - A2[i2]->z;
          d2 = dx + dy + dz*dz;
          if (d2<maxCont2)
            Vi1[i2] = mmdb::mround(1000.0*(maxCont2-d2));
        }
      }
    }
  }

}

void gsmt::Dynam::CalcMLScores ( mmdb::PPAtom A1, mmdb::PPAtom A2,
                                 mmdb::realtype sigma2 )  {
mmdb::ivector  Vi1;
mmdb::realtype dx,dy,dz, d2;
int            i1,i2;

  for (i1=0;i1<n1;i1++)  {
    Vi1 = V[i1];
    for (i2=0;i2<n2;i2++)  {
      Vi1[i2] = 0;
      dx = A1[i1]->x - A2[i2]->x;
      dx = dx*dx;
      if (dx<maxCont2)  {
        dy = A1[i1]->y - A2[i2]->y;
        dy = dy*dy;
        if (dy<maxCont2)  {
          dz = A1[i1]->z - A2[i2]->z;
          d2 = dx + dy + dz*dz;
          if (d2<maxCont2)
            Vi1[i2] = mmdb::mround(10000.0*mmdb::Exp(-d2/sigma2));
        }
      }
    }
  }

}

void gsmt::Dynam::CalcScores ( mmdb::PContact contact, int nContacts ) {
mmdb::ivector  Vi1;
mmdb::realtype Score;
int            i1,i2,i;

  for (i1=0;i1<n1;i1++)  {
    Vi1 = V[i1];
    for (i2=0;i2<n2;i2++)
      Vi1[i2] = 0;
  }

  for (i=0;i<nContacts;i++)  {
    Score = maxCont2 - contact[i].dist;
    if (Score>0.0)
      V[contact[i].id1][contact[i].id2] = mmdb::mround(1000.0*Score);
  }

}

void gsmt::Dynam::BuildPathTable()  {
mmdb::ivector V0,V1;
mmdb::ivector mi0,mi1;
int           i1,i2, d2, mk;

  //  V[i][j]  is the maximal accumulated score of all possible paths
  //           from pair (0,0) to pair (i,j)
  //  mi[i][j] is index of first occurence of the maximal accumulated
  //           score from pair (i,0) to pair (i,j)

  V0  = V[0];
  mi0 = mi[0];

  d2  = 0;
  mk  = 0;
  for (i2=0;i2<n2;i2++)  {
    if (V0[i2]>d2)  {
      d2 = V0[i2];
      mk = i2;
    }
    mi0[i2] = mk;
  }

  for (i1=1;i1<n1;i1++)  {
    V1  = V0;
    V0  = V[i1];
    mi1 = mi0;
    mi0 = mi[i1];
    if (V1[0]>V0[0])  V0[0] = V1[0];
    d2     = V0[0];
    mk     = 0;
    mi0[0] = 0;
    for (i2=1;i2<n2;i2++)  {
      //  at this point, V1[k]==V[i1-1][k] and V0[i2]==V[i1][i2].
      //  V[i1][i2] is calculated as accumulated score that would be
      //  achieved if pair (i1,i2) is added to the path with maximal
      //  accumulated score achieved for pairs with lower indexes.
      //  Last pair of such path is (i1-1,my[i1-1][i2-1]).
      V0[i2] = mmdb::IMax ( V1[i2],V1[mi1[i2-1]]+V0[i2] );
      if (V0[i2]>d2)  {
        d2 = V0[i2];
        mk = i2;
      }
      mi0[i2] = mk;
    }
  }

}


void gsmt::Dynam::Backtrace()  {
int  i1,i2, d2;

  for (i1=0;i1<n1;i1++)  c1[i1] = -1;
  for (i2=0;i2<n2;i2++)  c2[i2] = -1;

  i1 = n1 - 1;
  i2 = n2 - 1;
  while ((i1>=0) && (i2>=0))  {
    d2 = V[i1][mi[i1][i2]];
    if (d2<=0)  break;
    while (i1>=0)  {
      if (V[i1][mi[i1][i2]]<d2)
        break;
      i1--;
    }
    i1++;
    i2 = mi[i1][i2];
    c1[i1] = i2;
    c2[i2] = i1;
    i1--;
    i2--;
  }

}

void gsmt::Dynam::getAlignment ( mmdb::ivector & ac1,
                                 mmdb::ivector & ac2,
                                 mmdb::rvector   scores )  {
UNUSED_ARGUMENT(scores);
  ac1 = c1;
  ac2 = c2;
//  if (scores)  {
//  }
}

