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
//    03.02.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  GSMT_SegCluster <implementation>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::SegCluster
//       ~~~~~~~~~
//
//  (C) E. Krissinel, 2008-2014
//
// =================================================================
//

#include "mmdb2/mmdb_atom.h"
#include "gsmt_segcluster.h"

// =================================================================

gsmt::SegCluster::SegCluster ( int cid ) {
  InitClass ( cid );
}

gsmt::SegCluster::~SegCluster()  {
  FreeMemory();
}

void gsmt::SegCluster::InitClass ( int cid )  {
int i;

  id     = cid;     // cluster id
  n      = 0;       // cluster size
  segn   = NULL;    // lengths of segments
  seg1   = NULL;    // starting indices of segments in 1st structure
  seg2   = NULL;    // starting indices of segments in 2nd structure
  nSegs  = 0;       // number of segment sections
  mc1[0] = mmdb::MaxReal; // mass center #1 - x
  mc1[1] = mmdb::MaxReal; // mass center #1 - y
  mc1[2] = mmdb::MaxReal; // mass center #1 - z
  mc2[0] = mmdb::MaxReal; // mass center #2 - x
  mc2[1] = mmdb::MaxReal; // mass center #2 - y
  mc2[2] = mmdb::MaxReal; // mass center #2 - z
  rmsd2  = 0.0;           // square rmsd of the cluster
  Q      = 0.0;           // Q-score of the cluster

  for (i=0;i<9;i++)
    C[i] = 0.0;           // correlation matrix

  mmdb::Mat4Init ( T );   // transformation matrix
  SD     = NULL;          // superposition data
  nalloc = 0;             // allocated length of segn, seg1 and seg2

}

void gsmt::SegCluster::FreeMemory()  {
  mmdb::FreeVectorMemory ( segn,0 );
  mmdb::FreeVectorMemory ( seg1,0 );
  mmdb::FreeVectorMemory ( seg2,0 );
  FreeSuperposition();
  nalloc = 0;
}

void gsmt::SegCluster::FreeSuperposition()  {
  if (SD)  {
    delete SD;
    SD = NULL;
  }
}

void gsmt::SegCluster::Reset ( int cid )  {
  FreeMemory();
  InitClass ( cid );
}

mmdb::realtype gsmt::SegCluster::getRMSD2 ( mmdb::PPAtom A1,
                                            mmdb::PPAtom A2,
                                            int natoms )  {
//  calculates RMSD2 = |A1 - T*A2|^2 for first natoms
mmdb::realtype rmsd2, dx, dy,dz;
int            i;
  rmsd2 = 0.0;
  for (i=0;i<natoms;i++)  {
    dx = A1[i]->x - T[0][0]*A2[i]->x - T[0][1]*A2[i]->y -
                    T[0][2]*A2[i]->z - T[0][3];
    dy = A1[i]->y - T[1][0]*A2[i]->x - T[1][1]*A2[i]->y -
                    T[1][2]*A2[i]->z - T[1][3];
    dz = A1[i]->z - T[2][0]*A2[i]->x - T[2][1]*A2[i]->y -
                    T[2][2]*A2[i]->z - T[2][3];
    rmsd2 += dx*dx + dy*dy + dz*dz;
  }
  return rmsd2/natoms;
}

bool gsmt::SegCluster::isSuperposable ( mmdb::PPAtom   A1,
                                        mmdb::PPAtom   A2,
                                        int            natoms,
                                        mmdb::realtype square_rmsd )  {
// Returns True if rmsd2=|A1 - T*A2|^2 for first natoms of A1 and A2
// is less than square_rmsd. This function has proved to have 4x
// speed-up when used instead getRMSD2() and explicit comparison
// with the tolerance threshold.
mmdb::realtype rmsd2, rms2n, d;
int            i;
bool           B;

  rms2n = natoms*square_rmsd;
  rmsd2 = 0.0;
  B     = true;

  for (i=0;(i<natoms) && B;i++)  {
    d = A1[i]->x - T[0][0]*A2[i]->x - T[0][1]*A2[i]->y -
                   T[0][2]*A2[i]->z - T[0][3];
    rmsd2 += d*d;
    if (rmsd2<=rms2n)  {
      d = A1[i]->y - T[1][0]*A2[i]->x - T[1][1]*A2[i]->y -
                     T[1][2]*A2[i]->z - T[1][3];
      rmsd2 += d*d;
      if (rmsd2<=rms2n)  {
        d = A1[i]->z - T[2][0]*A2[i]->x - T[2][1]*A2[i]->y -
                       T[2][2]*A2[i]->z - T[2][3];
        rmsd2 += d*d;
        if (rmsd2>rms2n)
          B = false;
      } else
        B = false;
    } else
      B = false;
  }

  return B;

}

void gsmt::SegCluster::AddSegment ( int i1, int i2, int segLen,
                                    mmdb::rvector  CM,
                                    mmdb::mat44  & TM,
                                    mmdb::rvector  cm1,
                                    mmdb::rvector  cm2,
                                    mmdb::realtype rms2,
                                    mmdb::realtype Qscore )  {
//
//  [i1,i2]  - begining of segment in structures 1 and 2
//  segLen   - length of the segment
//  TM       - new matrix of best superposition for the whole cluster
//  cm1,cm2  - new mass centers for the whole cluster
mmdb::ivector  sn,s1,s2;
int            i;

  if (nSegs>=nalloc)  {

    sn = segn;
    s1 = seg1;
    s2 = seg2;

    nalloc = nSegs+4; // funny but 4 is faster than >4 and there is
                      // no difference between 1 and 4 (?!). Though
                      // the effect is marginal (2%).

    mmdb::GetVectorMemory ( segn,nalloc,0 );
    mmdb::GetVectorMemory ( seg1,nalloc,0 );
    mmdb::GetVectorMemory ( seg2,nalloc,0 );

    for (i=0;i<nSegs;i++)  {
      segn[i] = sn[i];
      seg1[i] = s1[i];
      seg2[i] = s2[i];
    }

    mmdb::FreeVectorMemory ( sn,0 );
    mmdb::FreeVectorMemory ( s1,0 );
    mmdb::FreeVectorMemory ( s2,0 );

  }

  segn[nSegs] = segLen;
  seg1[nSegs] = i1;
  seg2[nSegs] = i2;

  nSegs++;
  n += segLen;

  for (i=0;i<9;i++)
    C[i] = CM[i];

  mmdb::Mat4Copy ( TM,T );
  for (i=0;i<3;i++)  {
    mc1[i] = cm1[i];
    mc2[i] = cm2[i];
  }

  rmsd2 = rms2;
  Q     = Qscore;

}


void gsmt::SegCluster::GetAlignment ( mmdb::ivector c1, int nat1,
                                      mmdb::ivector c2, int nat2 )  {
int  i,j, i1,i2, k;

  for (i=0;i<nat1;i++)  c1[i] = -1;
  for (i=0;i<nat2;i++)  c2[i] = -1;

  for (i=0;i<nSegs;i++)  {
    i1 = seg1[i];
    i2 = seg2[i];
    k  = segn[i];
    for (j=0;j<k;j++)  {
      c1[i1] = i2;
      c2[i2] = i1;
      i1++;
      i2++;
    }
  }

}


void gsmt::SegCluster::SetSuperposition ( mmdb::mat44  & T,
                                          mmdb::realtype rmsd,
                                          mmdb::realtype seqId,
                                          mmdb::realtype Q,
                                          int            Nalgn,
                                          mmdb::ivector  c1,
                                          mmdb::rvector  sdist1,
                                          int            nat1,
                                          mmdb::ivector  c2,
                                          int            nat2 )  {

  if (!SD)
    SD = new Superposition();

  SD->SetSuperposition ( T,rmsd,seqId,Q,Nalgn,c1,sdist1,nat1,c2,nat2 );

/*

int  i;

  if (SD)  SD->FreeMemory();
     else  SD = new Superposition();

  mmdb::Mat4Copy ( T,SD->T ); // transformation matrix
  SD->rmsd  = rmsd;           // best superposition r.m.s.d.
  SD->seqId = seqId;          // sequence identity
  SD->Q     = Q;              // Q-score
  SD->Nalgn = Nalgn;          // alignment length

  mmdb::GetVectorMemory ( SD->c1   ,nat1,0 );
  mmdb::GetVectorMemory ( SD->dist1,nat1,0 );
  mmdb::GetVectorMemory ( SD->c2   ,nat2,0 );

  for (i=0;i<nat1;i++)  {
    SD->c1[i] = c1[i];
    if (c1[i]>=0)  SD->dist1[i] = sqrt(sdist1[i]);
             else  SD->dist1[i] = -1.0;
  }
  for (i=0;i<nat2;i++)
    SD->c2[i] = c2[i];

*/

}


void gsmt::SegCluster::calcCorrMatrix ( mmdb::rvector       CM,
                                        mmdb::PPAtom        A1,
                                        mmdb::PPAtom        A2,
                                        int                 i1,
                                        int                 i2,
                                        int                 n,
                                        const mmdb::vect3 & cm1,
                                        const mmdb::vect3 & cm2 )  {
mmdb::realtype dx1,dy1,dz1, dx2,dy2,dz2;
int            i,j,j1,j2; //,k,p,q;

  for (i=0;i<9;i++)
    CM[i] = 0.0;

  for (i=0;i<nSegs;i++)  {
    j1 = seg1[i];
    j2 = seg2[i];
    for (j=0;j<segn[i];j++)  {
      dx1   = A1[j1]->x - cm1[0];
      dy1   = A1[j1]->y - cm1[1];
      dz1   = A1[j1]->z - cm1[2];
      dx2   = A2[j2]->x - cm2[0];
      dy2   = A2[j2]->y - cm2[1];
      dz2   = A2[j2]->z - cm2[2];
      CM[0] += dx2*dx1;
      CM[1] += dx2*dy1;
      CM[2] += dx2*dz1;
      CM[3] += dy2*dx1;
      CM[4] += dy2*dy1;
      CM[5] += dy2*dz1;
      CM[6] += dz2*dx1;
      CM[7] += dz2*dy1;
      CM[8] += dz2*dz1;
      j1++;
      j2++;
    }
  }

  j1 = i1;
  j2 = i2;
  for (j=0;j<n;j++)  {
    dx1   = A1[j1]->x - cm1[0];
    dy1   = A1[j1]->y - cm1[1];
    dz1   = A1[j1]->z - cm1[2];
    dx2   = A2[j2]->x - cm2[0];
    dy2   = A2[j2]->y - cm2[1];
    dz2   = A2[j2]->z - cm2[2];
    CM[0] += dx2*dx1;
    CM[1] += dx2*dy1;
    CM[2] += dx2*dz1;
    CM[3] += dy2*dx1;
    CM[4] += dy2*dy1;
    CM[5] += dy2*dz1;
    CM[6] += dz2*dx1;
    CM[7] += dz2*dy1;
    CM[8] += dz2*dz1;  
    j1++;
    j2++;
  }
  
}

