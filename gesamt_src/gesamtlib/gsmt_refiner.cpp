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
//  **** Module  :  GSMT_Refiner <interface>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::Refiner
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2008-2015
//
// =================================================================
//

#include <math.h>
#include <string.h>

#include "gsmt_refiner.h"
#include "gsmt_svd.h"
#include "gsmt_defs.h"


// =================================================================

gsmt::Refiner::Refiner() : Clusterer()  {
  initRefiner();
}

gsmt::Refiner::Refiner ( mmdb::io::RPStream Object ) : Clusterer(Object) {
  initRefiner();
}

gsmt::Refiner::~Refiner()  {
  freeMemory();
}

void  gsmt::Refiner::initRefiner()  {

  //  --------- Fixed parameters

  maxContact     = 6.0;   // maximum contact distance
  iterMax        = 30;    // limit for iterative refinement,
                          // default 30

  //  --------- Input parameters

  refineDepth    = 0.7;   // depth of going into cluster list at
                          // refinement [0..1], default 0.7
  deltaRT        = 2.0;   // threshold for comparison of RT
                          // matrices, angstrom
  maxClustRefine = 100;   // maximal number of clusters to refine


  M1  = NULL;             // MMDB manager for contact seeking

  //  --------- Structure's emedding sphere
  radius2 = 20.0;         // radius of embedding sphere

  // memory pools for iterative refinement
  xyz2       = NULL;      // working copy of structure #2
  c10        = NULL;      // best correspondence
  c20        = NULL;      //     vectors
  cdist      = NULL;      // contact distances to sort
  cix        = NULL;      // contact indexes to sort
  contact    = NULL;      // pool for atom contacts
  nContAlloc = 0;         // allocated length of atom contacts pool

}

void gsmt::Refiner::freeMemory()  {
  freeRefMemory();
}


void gsmt::Refiner::getRefMemory()  {

  xyz2 = new mmdb::vect3[natoms2];
  mmdb::GetVectorMemory ( c10  ,natoms1,0 );
  mmdb::GetVectorMemory ( c20  ,natoms2,0 );
  mmdb::GetVectorMemory ( cdist,natoms1,0 );
  mmdb::GetVectorMemory ( cix  ,natoms1,0 );

  nContAlloc = natoms1*natoms2;
  contact    = new mmdb::Contact[nContAlloc];

}


void gsmt::Refiner::freeRefMemory()  {
  if (xyz2)  {
    delete[] xyz2;
    xyz2 = NULL;
  }
  mmdb::FreeVectorMemory ( c10  ,0 );
  mmdb::FreeVectorMemory ( c20  ,0 );
  mmdb::FreeVectorMemory ( cdist,0 );
  mmdb::FreeVectorMemory ( cix  ,0 );
  if (contact)  {
    delete[] contact;
    contact = NULL;
  }
  nContAlloc = 0;
}


void gsmt::Refiner::printClusterData()  {
mmdb::io::File f;
char           S[300];
int            i;

  f.assign ( "clusters.dat" );
  f.rewrite();

  for (i=0;i<nClusters;i++)  {
    sprintf ( S," %5i  %5i",i+1,Cluster[i]->n );
    f.WriteLine ( S );
  }

}


void  gsmt::Refiner::Refine ( PStructure s1, PStructure s2 )  {
int  i,min_size;

//  printClusterData();

  freeRefMemory();

  M1      = s1->getMMDBManager();
  radius2 = s2->getRadius();

  getRefMemory();

  if (!M1->areBricks())
    M1->MakeBricks ( A1,natoms1,1.25*maxContact );

  if (nClusters>0)  {

    min_size = mmdb::mround ( refineDepth*Cluster[0]->n );

    for (i=0;(i<mmdb::IMin(maxClustRefine,nClusters)) && (Cluster[i]->n>=min_size);i++) {
        refineCluster(i);
        if (stop) {
            break;
        }
    }

  }

  freeRefMemory();


}

void  gsmt::Refiner::Superpose ( mmdb::ivector ac1, mmdb::mat44 & T,
                                 bool useSigma )  {
mmdb::vect3    vc1,vc2;
mmdb::realtype cx1,cy1,cz1, cx2,cy2,cz2, det, B, sigma2,weight;
int            i,j,k, i1,i2;

  //  1.  Calculate mass centers

  cx1 = 0.0;
  cy1 = 0.0;
  cz1 = 0.0;
  cx2 = 0.0;
  cy2 = 0.0;
  cz2 = 0.0;
  k   = 0;
  for (i1=0;i1<natoms1;i1++)  {
    i2 = ac1[i1];
    if (i2>=0)  {
      cx1 += A1[i1]->x;
      cy1 += A1[i1]->y;
      cz1 += A1[i1]->z;
      cx2 += A2[i2]->x;
      cy2 += A2[i2]->y;
      cz2 += A2[i2]->z;
      k++;
    }
  }
  cx1 /= k;
  cy1 /= k;
  cz1 /= k;
  cx2 /= k;
  cy2 /= k;
  cz2 /= k;

  //  2.  Calculate the correlation matrix

  for (i=0;i<3;i++)
    for (j=0;j<3;j++)
      svd_A[i][j] = 0.0;

  if (useSigma && (sigma>0.0))  {
    
    sigma2 = sigma*sigma;
    for (i1=0;i1<natoms1;i1++)  {
      i2 = ac1[i1];
      if (i2>=0)  {
        vc1[0] = A1[i1]->x - cx1;
        vc1[1] = A1[i1]->y - cy1;
        vc1[2] = A1[i1]->z - cz1;
        vc2[0] = A2[i2]->x - cx2;
        vc2[1] = A2[i2]->y - cy2;
        vc2[2] = A2[i2]->z - cz2;
        weight = mmdb::Exp ( -A1[i1]->GetDist2(A2[i2])/sigma2 );
        for (j=0;j<3;j++)
          for (k=0;k<3;k++)
            svd_A[j][k] += weight*vc1[k]*vc2[j];
      }
    }
    
    
  } else  {

    for (i1=0;i1<natoms1;i1++)  {
      i2 = ac1[i1];
      if (i2>=0)  {
        vc1[0] = A1[i1]->x - cx1;
        vc1[1] = A1[i1]->y - cy1;
        vc1[2] = A1[i1]->z - cz1;
        vc2[0] = A2[i2]->x - cx2;
        vc2[1] = A2[i2]->y - cy2;
        vc2[2] = A2[i2]->z - cz2;
        for (j=0;j<3;j++)
          for (k=0;k<3;k++)
            svd_A[j][k] += vc1[k]*vc2[j];
      }
    }
    
  }

  det = svd_A[0][0]*svd_A[1][1]*svd_A[2][2] +
        svd_A[0][1]*svd_A[1][2]*svd_A[2][0] +
        svd_A[1][0]*svd_A[2][1]*svd_A[0][2] -
        svd_A[0][2]*svd_A[1][1]*svd_A[2][0] -
        svd_A[0][0]*svd_A[1][2]*svd_A[2][1] -
        svd_A[2][2]*svd_A[0][1]*svd_A[1][0];


  //  3.  Calculate the transformation matrix
  //      (to be applied to 2nd structure)

  SVD3 ( svd_A,svd_U,svd_V,svd_W,svd_RV1,k );

  if (k)
    mmdb::Mat4Init ( T );
  else  {

    if (det<=0.0)  {
      k = -1;
      B = mmdb::MaxReal;
      for (i=0;i<3;i++)
        if (svd_W[i]<B)  {
          B = svd_W[i];
          k = i;
      }
      for (i=0;i<3;i++)
        svd_V[i][k] = -svd_V[i][k];
    }

    //  Calculate rotational part of T

    for (i=0;i<3;i++)
      for (j=0;j<3;j++)  {
        B = 0.0;
        for (k=0;k<3;k++)
          B += svd_U[i][k]*svd_V[j][k];
        T[j][i] = B;
      }

    //  Add translational part to T

    T[0][3] = cx1 - T[0][0]*cx2 - T[0][1]*cy2 - T[0][2]*cz2;
    T[1][3] = cy1 - T[1][0]*cx2 - T[1][1]*cy2 - T[1][2]*cz2;
    T[2][3] = cz1 - T[2][0]*cx2 - T[2][1]*cy2 - T[2][2]*cz2;

    T[3][0] = 0.0;  T[3][1] = 0.0;  T[3][2] = 0.0;  T[3][3] = 1.0;

  }

}


bool gsmt::Refiner::checkRTMatrix ( mmdb::mat44 & T, int clusterNo )  {
PSuperposition SD;
mmdb::realtype x0,y0,z0, x,y,z;
int            i;
bool           Ok;

  x0 = T[0][0]*radius2 + T[0][3];
  y0 = T[1][1]*radius2 + T[1][3];
  z0 = T[2][2]*radius2 + T[2][3];

  Ok = false;
  for (i=0;(i<clusterNo) && (!Ok);i++)  {
    SD = Cluster[i]->SD;
    if (SD)  {
      x = SD->T[0][0]*radius2 + SD->T[0][3];
      if (fabs(x-x0)<=deltaRT)  {
        y = SD->T[1][1]*radius2 + SD->T[1][3];
        if (fabs(y-y0)<=deltaRT)  {
          z = SD->T[2][2]*radius2 + SD->T[2][3];
          Ok = (fabs(z-z0)<=deltaRT);
        }
      }
    }
  }

  return Ok;

}


void gsmt::Refiner::refineCluster ( int clusterNo )  {
mmdb::ivector  c1,c2;
mmdb::mat44    T,T0;
mmdb::realtype R02, rmsd,rmsd0, Q,Q1,Q0, dist2, seqId, eps;
int            iter, i, i1,i2, ncont, alen,alen0, noImpCnt;
int            Done;

  alen = 0;
  R02  = QR0*QR0;       // square Q-score parameter
  eps  = mmdb::RMax(1.0e-12,100.0*mmdb::MachEps);

  iter     =  0;
  noImpCnt =  5;
  rmsd0    = -1.0;
  rmsd     =  2.0*QR0;
  alen0    =  0;
  Q0       = -1.0;
  Q        =  1.0;

  //  1.  Obtain the initial superposition matrix from the cluster

  Cluster[clusterNo]->FreeSuperposition();
  Cluster[clusterNo]->GetAlignment ( c10,natoms1,c20,natoms2 );
  Superpose ( c10,T,false );
  if (checkRTMatrix(T,clusterNo))  return;

   
  //  2.  Conduct iterative refinement

  do  {

    Done = 1;   // termination signal

    //  2.1  Apply transformation matrix

    for (i2=0;i2<natoms2;i2++)
      A2[i2]->TransformCopy ( T,xyz2[i2] );
    
    //  2.2  Make 3D alignment of the coordinates

    ncont = 0;
//    M1->SeekContacts ( A1,natoms1,A2,natoms2,maxContact,contact,ncont,
//                       mmdb::BRICK_ON_1 | mmdb::BRICK_READY );
    M1->SeekContacts ( xyz2,natoms2,maxContact,contact,ncont );
    DA.Align         ( natoms1,natoms2, contact,ncont, QR0,2.0*rmsd );
    DA.getAlignment  ( c1,c2 );

    //  2.3  Identify close 3D matches and calculate alignment scores
    //       and mass centers for new superposition

    //  2.3.1  Identify subset of matches with maximal Q-score

    ncont = 0;    // total number of corresponding atoms
    for (i1=0;i1<natoms1;i1++)  {
      i2 = c1[i1];
      if (i2>=0)  {
        cdist[ncont] = A1[i1]->GetDist2 ( xyz2[i2] );
        cix  [ncont] = i1;
        ncont++;
      }
    }

    //  2.3.2  Unmap atoms for optimizing the quality function

    if (ncont<=3)  {

      rmsd = 0.0;
      for (i=0;i<ncont;i++)
        rmsd += cdist[i];
      alen = ncont;
      Q    = alen*alen/(1.0+rmsd/(alen*R02))/(natoms1*natoms2);
      rmsd = sqrt(rmsd/alen);

    } else  {

      QSP.Sort ( cix,cdist,ncont );

      dist2 = 0.0; // square distance between the structures
                   // at current rotation
      Q  = -1.0;
      Q1 = 0.0;
      for (i=0;(i<ncont) && (Q1>=Q);i++)  {
        dist2 += cdist[i];
        i1     = i+1;
        Q1     = i1*i1/(1.0+dist2/(i1*R02));
        if (Q1>=Q)  {
          Q    = Q1;
          rmsd = dist2;
          alen = i1;
        }
      }

      //  Finalize scores

      Q   /= (natoms1*natoms2);
      rmsd = sqrt(rmsd/alen);

      //  Unmap unwanted contacts

      for (i=alen;i<ncont;i++)  {
        i1 = cix[i];
        i2 = c1[i1];
        c1[i1] = -1;
        c2[i2] = -1;
      }

    }

    //  2.3.3  Note score improvement

    if (Q-Q0>eps)  {
      alen0 = alen;
      rmsd0 = rmsd;
      Q0    = Q;
      for (i=0;i<natoms1;i++)
        c10[i] = c1[i];
      mmdb::Mat4Copy ( T,T0 );
      noImpCnt = mmdb::IMax ( 5,iter/2 );
    } else
      noImpCnt--;

    iter++;

    if (noImpCnt>0)  Done = 0;

    if (checkRTMatrix(T,clusterNo))  Done = -1;  // error

    if (!Done)  {
      if (iter>iterMax)  Done = 2;
      if (noImpCnt<=0)   Done = 1;
    }

    if (!Done)  {

      Done = 1;
      for (i2=0;(i2<natoms2) && Done;i2++)
        if (c2[i2]!=c20[i2])  Done = 0;

      for (i2=0;i2<natoms2;i2++)
        c20[i2] = c2[i2];

      if (Done)  {
        noImpCnt = mmdb::IMin ( noImpCnt,0 );
        Done     = 0;
      }

      if (!Done)
        Superpose ( c1,T,true );

    }

  } while (!Done);

  if (Done>=0)  {

    for (i2=0;i2<natoms2;i2++)
      c20[i2] = -1;

    seqId = 0.0;
    for (i1=0;i1<natoms1;i1++)  {
      i2 = c10[i1];
      if (i2>=0)  {
        c20  [i2] = i1;
        cdist[i1] = A1[i1]->GetDist2(A2[i2],T0);
        if (!strcmp(A1[i1]->GetResName(),A2[i2]->GetResName()))
          seqId += 1.0;
      } else
        cdist[i1] = -1.0;
    }

    Cluster[clusterNo]->SetSuperposition ( T0,rmsd0,seqId/alen0,Q0,
                                           alen0,c10,cdist,natoms1,
                                           c20,natoms2 );

  }

}


void gsmt::Refiner::read  ( mmdb::io::RFile f )  {
  Clusterer::read ( f );
}

void gsmt::Refiner::write ( mmdb::io::RFile f )  {
  Clusterer::write ( f );
}

