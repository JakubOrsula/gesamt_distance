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
//    29.11.15   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  GSMT_Clusterer <implementation>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::Clusterer
//       ~~~~~~~~~
//
//  (C) E. Krissinel, 2008-2015
//
// =================================================================
//

#include <math.h>

#include "gsmt_clusterer.h"
#include "gsmt_sortclusters.h"
#include "gsmt_svd.h"
#include "gsmt_defs.h"

// =================================================================

gsmt::Clusterer::Clusterer() : Base()  {
  initClusterer();
}

gsmt::Clusterer::Clusterer ( mmdb::io::RPStream Object )
               : Base(Object)  {
  initClusterer();
}

gsmt::Clusterer::~Clusterer()  {
  freeMemory   ();
  /*
  mmdb::FreeVectorMemory ( svdRV1,1 );
  mmdb::FreeVectorMemory ( svdW  ,1 );
  mmdb::FreeMatrixMemory ( svdV  ,3,1,1 );
  mmdb::FreeMatrixMemory ( svdU  ,3,1,1 );
  mmdb::FreeMatrixMemory ( svdA  ,3,1,1 );
  */
}

void gsmt::Clusterer::initClusterer()  {

  //  --------- Fixed parameters

  QR0        = QR0_default; // Q-score parameter
  minSegLen  = seg_length_default; // minimal segment length to
                      // initiate a cluster, default is 9. Longer
                      // lengths make clustering faster but less
                      // specific. Lengths lower than 7 also lead
                      // to lower specificity.
  segTol     = 2.25;  // distance tolerance for segment matching
                      // segTol has a drastic effect on speed and
                      // "resolution" (ability to capture remote
                      // topologies). Lower segTol means a faster
                      // execution and lower resolution. Default 2.25
  clustTol2  = 16.0;  // square distance tolerance for cluster matching
                      // default 16.0
  filterTol2 = 4.0*clustTol2; // square distance tolerance for cluster
                              // filtering
  pruneSegLen = 0;            // maximal segment length for pruning
  pruneSegGap = mmdb::MaxInt; // minimal inter-segment gap for pruning

  //  --------- Input parameters

  minMatch1 = 0.5;    // minimal fraction of matched residues
                      //   in 1st structure
  minMatch2 = 0.5;    // minimal farction of matched residues
                      //   in 2nd structure

  //  --------- Data and derived parameters

  A1        = NULL;   // structure #1 C-alphas (copy pointer)
  natoms1   = 0;      // structure #1 length
  A2        = NULL;   // structure #2 C-alphas (copy pointer)
  natoms2   = 0;      // structure #2 length
  minMatch  = 0;      // minimal number of atoms to be matched

  //  --------- Internal data structures

  D1        = NULL;   // distance matrix of 1st structure (copy pointer)
  D2        = NULL;   // distance matrix of 2nd structure (copy pointer)
  sid       = NULL;   // segment ID matrix
  n1alloc   = 0;      // allocated dimension of 1st str-re dist matrix
  n2alloc   = 0;      // allocated dimension of 2nd str-re dist matrix

  Cluster   = NULL;   // segment superposition clusters
  nClusters = 0;      // number of clusters;
  nClAlloc  = 0;      // length of allocated cluster array

  stop = false;
}

void gsmt::Clusterer::freeMemory()  {
  A1      = NULL;  // structure #1 C-alphas
  A2      = NULL;  // structure #2 C-alphas
  natoms1 = 0;     // structure #1 length
  natoms2 = 0;     // structure #2 length
  mmdb::FreeMatrixMemory ( sid,n1alloc,0,0 );
  n1alloc = 0;     // allocated dimension of 1st structure distance matrix
  n2alloc = 0;     // allocated dimension of 2nd structure distance matrix
  FreeClusters ();
}

void gsmt::Clusterer::GetDMatMemory ( int nat1, int nat2 )  {
  if ((nat1>n1alloc) || (nat2>n2alloc))  {
    mmdb::FreeMatrixMemory ( sid,n1alloc,0,0 );
    n1alloc = nat1;
    n2alloc = nat2;
    mmdb::GetMatrixMemory  ( sid,n1alloc,n2alloc,0,0 );
  }
}

void gsmt::Clusterer::FreeClusters()  {
int i;
  if (Cluster)  {
    for (i=0;i<nClAlloc;i++)
      if (Cluster[i])  delete Cluster[i];
    delete[] Cluster;
    Cluster = NULL;
  }
  nClusters = 0;   // number of clusters;
  nClAlloc  = 0;   // length of allocated cluster array
}


void gsmt::Clusterer::calcMassCenters ( mmdb::PPAtom  atom1,
                                        mmdb::PPAtom  atom2,
                                        int           n,
                                        mmdb::rvector mc1,
                                        mmdb::rvector mc2 )  {
mmdb::realtype xc1,yc1,zc1, xc2,yc2,zc2;
int            i;

  xc1 = 0.0;    xc2 = 0.0;
  yc1 = 0.0;    yc2 = 0.0;
  zc1 = 0.0;    zc2 = 0.0;

  for (i=0;i<n;i++)  {
    xc1 += atom1[i]->x;
    yc1 += atom1[i]->y;
    zc1 += atom1[i]->z;
    xc2 += atom2[i]->x;
    yc2 += atom2[i]->y;
    zc2 += atom2[i]->z;
  }

  mc1[0] = xc1/n;
  mc1[1] = yc1/n;
  mc1[2] = zc1/n;
  mc2[0] = xc2/n;
  mc2[1] = yc2/n;
  mc2[2] = zc2/n;

}


bool gsmt::Clusterer::isRotoinversion ( mmdb::PPAtom  atom1,
                                        mmdb::PPAtom  atom2,
                                        int           n,
                                        mmdb::rvector mc1,
                                        mmdb::rvector mc2 ) {
mmdb::realtype C[9];
mmdb::vect3    vc1,vc2;
int            i,j, p,q;

  for (i=0;i<9;i++)
    C[i] = 0.0;

  for (i=0;i<n;i++)  {
    vc1[0] = atom1[i]->x - mc1[0];
    vc1[1] = atom1[i]->y - mc1[1];
    vc1[2] = atom1[i]->z - mc1[2];
    vc2[0] = atom2[i]->x - mc2[0];
    vc2[1] = atom2[i]->y - mc2[1];
    vc2[2] = atom2[i]->z - mc2[2];
    j = 0;
    for (p=0;p<3;p++)
      for (q=0;q<3;q++)
        C[j++] += vc1[q]*vc2[p];
  }

  return ( C[0]*C[4]*C[8] + C[1]*C[5]*C[6] + C[3]*C[7]*C[2] -
           C[2]*C[4]*C[6] - C[0]*C[5]*C[7] - C[8]*C[1]*C[3] ) <= 0.0;

}


bool gsmt::Clusterer::checkAndAdd ( PSegCluster C,
                                    int i1, int i2, int n,
                                    mmdb::rvector mc1,
                                    mmdb::rvector mc2 )  {
//
//  This function checks whether segments A1[i1..i1+n-1] and
//  A2[i2..i2+n-1] belong to cluster C, and if they do then
//  they are addedd to the cluster. The segments belong to
//  the cluster if they superpose with cluster's transformation
//  matrix T (applied to A2), or one close to it. Upon addition
// of the segment, the transformation matrix is recalculated.
//
mmdb::realtype CM[9];  // correlation matrix
mmdb::mat44    T;
mmdb::vect3    cm1,cm2;  // new mass centers
mmdb::realtype det,B, rmsd2;
int            i,j, j1,j2, p,q, np;
bool           added;

  //    Try to superpose all cluster with the new segment, and if it
  //  superposes well, accept the new segment and add it to the
  //  cluster

  //  1. Calculate the mass centers

  if (C->n>0)  {
    p  = C->n;
    np = n + p;  // total number of pairs
    for (i=0;i<3;i++)  {
      cm1[i] = (p*C->mc1[i] + n*mc1[i]) / np;
      cm2[i] = (p*C->mc2[i] + n*mc2[i]) / np;
    }
  } else  {
    np = n;  // total number of pairs
    for (i=0;i<3;i++)  {
      cm1[i] = mc1[i];
      cm2[i] = mc2[i];
    }
  }

  //  2. Calculate the correlation matrix

  C->calcCorrMatrix ( CM,A1,A2,i1,i2,n,cm1,cm2 );

  //  3. Calculate the transformation matrix (to be applied to Seg2)

  j = 0;
  for (p=0;p<3;p++)
    for (q=0;q<3;q++)
      svd_A[p][q] = CM[j++];

  SVD3 ( svd_A,svd_U,svd_V,svd_W,svd_RV1,p );

  if (p)
    added = false;
  else  {

    //  The determinant

    det = CM[0]*CM[4]*CM[8] + CM[1]*CM[5]*CM[6] + CM[3]*CM[7]*CM[2] -
          CM[2]*CM[4]*CM[6] - CM[0]*CM[5]*CM[7] - CM[8]*CM[1]*CM[3];

    if (det<=0.0)  {
      // rotoinversion
      i = -1;
      B = mmdb::MaxReal;
      for (j=0;j<3;j++)
        if (svd_W[j]<B)  {
          B = svd_W[j];
          i = j;
      }
      for (j=0;j<3;j++)
        svd_V[j][i] = -svd_V[j][i];
    }

    //  4.1 Calculate rotational part of T

    for (p=0;p<3;p++)
      for (q=0;q<3;q++)  {
        B = 0.0;
        for (i=0;i<3;i++)
          B += svd_U[p][i]*svd_V[q][i];
        T[q][p] = B;
      }

    //  4.2 Add translational part to T

    T[0][3] = cm1[0] - T[0][0]*cm2[0] - T[0][1]*cm2[1] - T[0][2]*cm2[2];
    T[1][3] = cm1[1] - T[1][0]*cm2[0] - T[1][1]*cm2[1] - T[1][2]*cm2[2];
    T[2][3] = cm1[2] - T[2][0]*cm2[0] - T[2][1]*cm2[1] - T[2][2]*cm2[2];

    //  5. Calculate square rmsd and decide on acceptance

    rmsd2 = C->n*C->rmsd2;

//    rmsd2 = 0.0;
//    for (i=0;i<C->nSegs;i++)  {
//      j1 = C->seg1[i];
//      j2 = C->seg2[i];
//      for (j=0;j<C->segn[i];j++)
//        rmsd2 += A1[j1++]->GetDist2 ( A2[j2++],T );
//    }

    j1 = i1;
    j2 = i2;
    for (i=0;i<n;i++)
      rmsd2 += A1[j1++]->GetDist2 ( A2[j2++],T );

    rmsd2 /= np;
    added  = (rmsd2<clustTol2);
    if (added)
      C->AddSegment ( i1,i2,n,CM,T,cm1,cm2,rmsd2,
                      np*np/(1.0+rmsd2/(QR0*QR0))/(natoms1*natoms2) );

  }

  return added;

}


void gsmt::Clusterer::pruneClusters ( int i1 )  {
// removes minimal-length clusters if they did not grow for too long
PSegCluster clust;
int         i,j,j1;

  i  = 0;
  while (i<nClusters)  {
    if (Cluster[i]->n<=pruneSegLen)  {
      j  = Cluster[i]->nSegs - 1;
      j1 = mmdb::IMax ( Cluster[i]->seg1[j],Cluster[i]->seg2[j] ) +
           Cluster[i]->segn[j];
      if (i1-j1>pruneSegGap)  {
        nClusters--;
        clust = Cluster[i];
        Cluster[i] = Cluster[nClusters];
        Cluster[nClusters] = clust;
        i--;
      }
    }
    i++;
  }

}

void gsmt::Clusterer::addSegment ( int i1, int i2, int n,
                                   int & clusterID,
                                   mmdb::rvector mc1,
                                   mmdb::rvector mc2 )  {
PPSegCluster   C;
PSegCluster    clust;
mmdb::realtype Q;
int            i,j,k, j1,j2;

  //  1. Find out whether the segment may belong to already
  //     existing cluster

  k  = -1;
  Q  = -1.0;
  i  = 0;
  while (i<nClusters)  {
    j  = Cluster[i]->nSegs - 1;
    j1 = Cluster[i]->seg1[j] + Cluster[i]->segn[j];
    j2 = Cluster[i]->seg2[j] + Cluster[i]->segn[j];
    if ((j1<=i1) && (j2<=i2))  {
      if (Cluster[i]->isSuperposable ( &(A1[i1]),&(A2[i2]),n,
                                       filterTol2 ))  {
        if (checkAndAdd(Cluster[i],i1,i2,n,mc1,mc2))  {
          // segment added
          if (Cluster[i]->Q>Q)  {
            // the Q-score has improved
            Q = Cluster[i]->Q;
            if (k<0)
              k = i; // first addition; further we try all of them
            else  {
              //  kth cluster had lower Q-score for adding the segment.
              //  Remove kth cluster and replace it with cluster i.
              //  Note that ith cluster is renumbered into kth cluster,
              //  therefore k does not change for further workflow.
              nClusters--;
              clust      = Cluster[k];
              Cluster[k] = Cluster[i];
              Cluster[i] = Cluster[nClusters];
              Cluster[nClusters] = clust;
              i--;
            }
          } else  {
            // ith cluster has lower Q-score for adding the segment
            // than previously achieved with kth cluster. Therefore,
            // remove cluster i from the system. The way the cluster
            // is removed does not affect serial numbers of other
            // clusters, terefore k does not change for further
            // workflow.
            nClusters--;
            clust      = Cluster[i];
            Cluster[i] = Cluster[nClusters];
            Cluster[nClusters] = clust;
            i--;
          }
        }
      }
    }
    i++;
  }

  if (k<0)  {
    if (nClusters>=nClAlloc)  {
      nClAlloc = nClusters + 1000;
      C = new PSegCluster[nClAlloc];
      for (i=0;i<nClusters;i++)
        C[i] = Cluster[i];
      for (i=nClusters;i<nClAlloc;i++)
        C[i] = NULL;
      if (Cluster)  delete[] Cluster;
      Cluster = C;
    }
    k = nClusters;
    nClusters++;
    clusterID++;
    if (!Cluster[k])  Cluster[k] = new SegCluster ( clusterID );
                else  Cluster[k]->Reset ( clusterID );
    if (checkAndAdd(Cluster[k],i1,i2,n,mc1,mc2))
      k = clusterID;
    else  {
      nClusters--;
      clusterID--;
      k = -1;
    }
  } else
    k = Cluster[k]->id;

  if (k>=0)  {
    j1 = i1;
    j2 = i2;
    for (i=0;i<n;i++)  {
      sid[j1][j2] = k;
      j1++;
      j2++;
    }
  }

}


gsmt::GSMT_CODE gsmt::Clusterer::makeSegClusters ( PStructure s1,
                                                   PStructure s2 )  {
SortClusters  QSC;
mmdb::rvector D1j1,D2j2;
mmdb::vect3   mc1,mc2;
int           i,k, i1,i2, j1,j2, n, nseg1,nseg2;
int           clusterID;
bool          B;

  if (Cluster)
    for (i=0;i<nClAlloc;i++)
      if (Cluster[i])
        Cluster[i]->Reset ( i+1 );
  nClusters = 0;

  //  1. Get mainchain coordinates

  s1->prepareStructure ( 0.0 );
  s2->prepareStructure ( 0.0 );

  s1->fetchData ( A1,natoms1,D1 );
  s2->fetchData ( A2,natoms2,D2 );

  if ((natoms1<minSegLen) || (natoms2<minSegLen))  {
    A1      = NULL;
    A2      = NULL;
    natoms1 = 0;
    natoms2 = 0;
    return GSMT_ShortStruct;
  }

  //  Estimate the size of minimal substructure to look at

  minMatch = mmdb::IMax ( int(minMatch1*natoms1),
                          int(minMatch2*natoms2) );
                          
  if ((minMatch>natoms1) || (minMatch>natoms2))
    return GSMT_NoMinimalMatch;

  //  Suggest parameters for cluster pruning

  pruneSegLen = mmdb::IMax ( minSegLen,
                     mmdb::mround(0.025*mmdb::IMax(natoms1,natoms2)) );
  pruneSegGap = mmdb::IMax ( 4*minSegLen,
                     mmdb::mround(0.050*mmdb::IMax(natoms1,natoms2)) );

  //  2. Allocate memory

  GetDMatMemory ( natoms1,natoms2 );

  //  3. Identify continuous segments and cluster them

  for (i1=0;i1<natoms1;i1++)
    for (i2=0;i2<natoms2;i2++)
      sid[i1][i2] = 0;

  nseg1     = natoms1 - minSegLen;
  nseg2     = natoms2 - minSegLen;
  minMatch -= minSegLen;
  clusterID = 0;

  for (i1=0;i1<nseg1;i1++)  {
      if (stop) {
          return GSMT_UserError;
      }
    for (i2=0;i2<nseg2;i2++)  {
      n = i1 - i2;
      if ((!sid[i1][i2]) && (nseg1-n>=minMatch) && (nseg2+n>=minMatch)) {
        // identify a potential segment
        j1 = i1+1;
        j2 = i2+1;
        B  = (fabs(D1[i1+minSegLen-1][i1]-D2[i2+minSegLen-1][i2])
               <segTol);
        n  = 0;
        while ((j1<natoms1) && (j2<natoms2) && B)  {
          D1j1 = &(D1[j1++][i1]);
          D2j2 = &(D2[j2++][i2]);
          for (k=n;(k>=0) && B;k--)
            B = (fabs(D1j1[k]-D2j2[k])<segTol);
          n++;
        }
        if (n>=minSegLen)  {
          calcMassCenters ( &(A1[i1]),&(A2[i2]),n,mc1,mc2 );
          // call to isRotoinversion is cheap but gives
          // some 10-15% speed-up
          if (!isRotoinversion(&(A1[i1]),&(A2[i2]),n,mc1,mc2))
            addSegment ( i1,i2,n,clusterID,mc1,mc2 );
        }
      }
    }
    pruneClusters ( i1 );
  }

  QSC.Sort ( Cluster,nClusters );

  return GSMT_Ok;

}


void  gsmt::Clusterer::read  ( mmdb::io::RFile f )  {
UNUSED_ARGUMENT(f);
}

void  gsmt::Clusterer::write ( mmdb::io::RFile f )  {
UNUSED_ARGUMENT(f);
}

