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
//    04.02.17   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  GSMT_MAligner <implementation>
//       ~~~~~~~~~
//  **** Project :  GESAMT - multiple structure aligner
//       ~~~~~~~~~
//  **** Classes :  gsmt::MAligner
//       ~~~~~~~~~
//
//  (C) E. Krissinel, 2008-2017
//
// =================================================================
//

#include <time.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>

#include "mmdb2/mmdb_math_linalg.h"
#include "mmdb2/mmdb_tables.h"
#include "mmdb2/mmdb_utils.h"
#include "rvapi/rvapi_interface.h"

#include "gsmt_maligner.h"
#include "gsmt_svd.h"
#include "gsmt_mamap.h"


// =================================================================

gsmt::MAligner::MAligner() : Base()  {
  initClass();
}

gsmt::MAligner::MAligner ( mmdb::io::RPStream Object )
              : Base(Object) {
  initClass();
}

gsmt::MAligner::~MAligner()  {
  freeMemory();
}

void gsmt::MAligner::initClass()  {

  minIter     =   3;  // minimal number of iterations
  maxIter     =  30;  // maximal number of iterations
  maxHollowIt =   3;  // maximal allowed number of consequtive
                      // iterations without quality improvement

  S           = NULL;
  SPS         = NULL;
  v1          = NULL;
  nStruct     = 0;

  Map         = NULL;
  map_nrows   = 0;

  Nalign        = 0;
  rmsd_achieved = -1.0;  // achieved RMSD
  Q_achieved    = -1.0;  // achieved Q

  minNres     = 0;
  maxNres     = 0;     // structure size range
  xc          = NULL;  // consensus X-coordinates
  yc          = NULL;  // consensus Y-coordinates
  zc          = NULL;  // consensus Z-coordinates
  mx_rmsd     = NULL;  // matrix of inter-structure rmsds
  mx_Qscore   = NULL;  // matrix of inter-structure Q-scores
  mx_seqId    = NULL;  // matrix of inter-structure seq identities

  nStructAlloc = 0;

}

void gsmt::MAligner::freeMemory()  {

  mmdb::FreeVectorMemory ( v1,1 );

  freeSuperpositions();
  freeMap           ();

  mmdb::FreeVectorMemory ( xc  ,0 );
  mmdb::FreeVectorMemory ( yc  ,0 );
  mmdb::FreeVectorMemory ( zc  ,0 );
  //mmdb::FreeVectorMemory ( disp,0 );

  mmdb::FreeMatrixMemory ( mx_rmsd  ,nStructAlloc,0,0 );
  mmdb::FreeMatrixMemory ( mx_Qscore,nStructAlloc,0,0 );
  mmdb::FreeMatrixMemory ( mx_seqId ,nStructAlloc,0,0 );
  nStructAlloc = 0;

}

void gsmt::MAligner::freeSuperpositions()  {
int i,j;
  if (SPS)  {
    for (i=0;i<nStruct-1;i++)
      if (SPS[i])  {
        for (j=i+1;j<nStruct;j++)
          if (SPS[i][j])  delete SPS[i][j];
        SPS[i] += i+1;
        delete[] SPS[i];
      }
    delete[] SPS;
  }
  SPS = NULL;
}

gsmt::GSMT_CODE gsmt::MAligner::Align ( PPMAStructure structures,
                                        int           nStructures,
                                        bool          measure_cpu )  {
mmdb::realtype clapse1,clapse2;
clock_t        ct,ct1;
int            i;
GSMT_CODE      rc;

  clapse1 = 0.0;
  clapse2 = 0.0;
  if (measure_cpu) ct = clock();
              else ct = 0;

  freeMemory();

  S       = structures;
  nStruct = nStructures;

  for (i=0;i<nStruct;i++)
    S[i]->serNo = i;

  //  1. Make all cross-superpositions
  
  if (nthreads<=1)
        rc = makeCrossSuperpositions1();
  else  rc = makeCrossSuperpositions ();

  if (measure_cpu)  {
    ct1     = clock();
    clapse1 = mmdb::realtype(ct1-ct)/CLOCKS_PER_SEC;
    ct      = ct1;
  }

  if (rc==GSMT_Ok)  {
    
    mmdb::GetVectorMemory ( v1,nStruct,1 );
  
    //  2. Obtain first approximation to optimal superposition
    rc = makeFirstGuess();

    if (rc==GSMT_Ok)  {
    
      //  3. Iteratively improve C-alpha alignments
      optimizeAlignments();
    
      //  4. Restore original order of structures
      sortStructures();    

      //  5. Calculate consensus scores
      calcConsensusScores();
    
    }

    deselectCalphas();
    
  }

  if (measure_cpu && (verbosity>=0))  {

    ct1     = clock();
    clapse2 = mmdb::realtype(ct1-ct)/CLOCKS_PER_SEC;
    ct      = ct1;

    printf ( "\n"
         " CPU stage 1 (cross-alignments):  %8.5f secs\n"
         " CPU stage 2 (refinement):        %8.5f secs\n\n",
         clapse1,clapse2 );

  }
  
  return rc;

}


struct ma_thread_arg  {
  
  gsmt::PAligner        A;
  gsmt::PMAStructure    S1;
  gsmt::PMAStructure    S2;
  gsmt::PPSuperposition SPS;
  bool                  ok;
  
  void init ( gsmt::PERFORMANCE_CODE performance,
              mmdb::realtype         minMatch1,
              mmdb::realtype         minMatch2,
              mmdb::realtype         QR0,
              mmdb::realtype         sigma )  {
    A = new gsmt::Aligner();
    A->setPerformanceLevel     ( performance         );
    A->setSimilarityThresholds ( minMatch1,minMatch2 );
    A->setQR0                  ( QR0                 );
    A->setSigma                ( sigma               );
  }
  
  void align()  {
  gsmt::PSuperposition SD;
  int                  matchNo;
    A->Align        ( S1,S2,false );
    A->getBestMatch ( SD,matchNo  );
    if (SD)  {
      *SPS = new gsmt::Superposition ( SD );
      ok   = true;
    } else  {
      *SPS = NULL;
      ok   = false;
    }
  }
  
  void release()  {
    if (A)  delete A;
  }
 
};

//#include <unistd.h>

void *ma_thread ( void *arg )  {
  ((ma_thread_arg*)arg)->align();
//  sleep(1);
  return NULL;
}

gsmt::GSMT_CODE gsmt::MAligner::makeCrossSuperpositions()  {
ma_thread_arg *targ;
pthread_t     *tid;
pthread_attr_t attr;
int            i,j,k,n;
GSMT_CODE      rc;

  targ = new ma_thread_arg[nthreads];
  tid  = new pthread_t    [nthreads];
  pthread_attr_init ( &attr );
  pthread_attr_setdetachstate ( &attr,PTHREAD_CREATE_JOINABLE );
  
  for (i=0;i<nthreads;i++)
    targ[i].init ( performance,minMatch1,minMatch2,QR0,sigma );

  for (i=0;i<nStruct;i++)
    S[i]->prepareStructure ( maxContact );
  
  SPS = new PPSuperposition[nStruct-1];
  for (i=0;i<nStruct-1;i++)
    SPS[i] = NULL;
  
  n  = 0;
  rc = GSMT_Ok;
  for (i=0;(i<nStruct-1) && (rc==GSMT_Ok);i++)  {
    SPS[i]  = new PSuperposition[nStruct-i-1];
    SPS[i] -= (i+1);
    for (j=i+1;j<nStruct;j++)
      if (rc==GSMT_Ok)  {
        targ[n].S1  = S[i];
        targ[n].S2  = S[j];
        targ[n].SPS = &(SPS[i][j]);
        if (n<nthreads-1)  {
          pthread_create ( &(tid[n]),&attr,&ma_thread,&(targ[n]) );
          n++;
        } else  {
          ma_thread ( &(targ[n]) );
          if (!targ[n].ok)  rc = GSMT_NoMinimalMatch;
          for (k=0;k<n;k++)  {
            pthread_join ( tid[k],NULL );
            if (!targ[k].ok)  rc = GSMT_NoMinimalMatch;
          }
          n = 0;
        }
      } else
        SPS[i][j] = NULL;
  }

  if  (rc==GSMT_Ok)
    for (k=0;k<n;k++)  {
      pthread_join ( tid[k],NULL );
      if (!targ[k].ok)  rc = GSMT_NoMinimalMatch;
    }
  
  pthread_attr_destroy ( &attr );
    
  for (i=0;i<nthreads;i++)
    targ[i].release();
  
  delete[] targ;
  delete[] tid;
  
  if (rc==GSMT_Ok)
    for (i=0;i<nStruct;i++)
      S[i]->prepareMAStructure();
  
  return rc;

}

gsmt::GSMT_CODE gsmt::MAligner::makeCrossSuperpositions1()  {
PSuperposition SD;
PAligner       A;
int            i,j,matchNo;
GSMT_CODE     rc;

  A = new Aligner();
  A->setPerformanceLevel     ( performance         );
  A->setSimilarityThresholds ( minMatch1,minMatch2 );
  A->setQR0                  ( QR0                 );
  A->setSigma                ( sigma               );
  
  SPS = new PPSuperposition[nStruct-1];
  for (i=0;i<nStruct-1;i++)
    SPS[i] = NULL;
  
  rc = GSMT_Ok;
  for (i=0;(i<nStruct-1) && (rc==GSMT_Ok);i++)  {
    SPS[i]  = new PSuperposition[nStruct-i-1];
    SPS[i] -= (i+1);
    for (j=i+1;j<nStruct;j++)
      if (rc==GSMT_Ok)  {
        A->Align        ( S[i],S[j],false );
        A->getBestMatch ( SD,matchNo );
        if (SD)
          SPS[i][j] = new Superposition ( SD );
        else  {
          SPS[i][j] = NULL;
          rc        = GSMT_NoMinimalMatch;
        }
      } else
        SPS[i][j] = NULL;
  }

  if (rc==GSMT_Ok)
    for (i=0;i<nStruct;i++)
      S[i]->prepareMAStructure();

  delete A;
  
  return rc;

}

gsmt::GSMT_CODE gsmt::MAligner::makeFirstGuess()  {
//   Using the results of multiple cross superpositions, this function
// chooses a structure which makes the highest quality superposition
// with all other structures. Other structures are then oriented
// to the chosen one.
PMAStructure   MAS;
mmdb::realtype Q,Qmax;
int            i,i1,j,k,m;
GSMT_CODE     rc;

  rc = GSMT_Ok;

  //  1. Find structure showing the highest sum of Q-scores

  Qmax = -mmdb::MaxReal;
  k    = -1;
  for (i=0;i<nStruct;i++)  {
    i1 = S[i]->serNo;
    Q  = 0.0;
    for (j=0;j<i1;j++)
      Q += SPS[j][i1]->Q;
    for (j=i1+1;j<nStruct;j++)
      Q += SPS[i1][j]->Q;
    if (Q>Qmax)  {
      Qmax = Q;
      k    = i;
    }
    S[i]->Qsum = Q;
  }

  if (k<0)  k = 0;  // simply choose 1st structure if fails to choose

  //  2. Sort structures by decreasing their multiple Q-scores

  if (k>0)  {
    MAS = S[k];  S[k] = S[0];  S[0] = MAS;
  }
  for (i=1;i<nStruct-1;i++)
    for (j=i+1;j<nStruct;j++)
      if (S[j]->Qsum>S[i]->Qsum)  {
        MAS = S[j];  S[j] = S[i];  S[i] = MAS;
      }

  //  3. Make 3D alignments to the central (1st) structure

  i1 = S[0]->serNo;
  mmdb::Mat4Init ( S[0]->RT0 );
  for (i=1;i<nStruct;i++)  {
    j = S[i]->serNo;
    if (i1<j)  {
      mmdb::Mat4Copy ( SPS[i1][j]->T,S[i]->RT0 );
      for (k=0;k<S[i]->nres;k++)  {
        S[i]->c0[k] = SPS[i1][j]->c2[k];
        S[i]->c [k] = SPS[i1][j]->c2[k];
      }
    } else if (j<i1)  {
      mmdb::Mat4Inverse ( SPS[j][i1]->T,S[i]->RT0 );
      for (k=0;k<S[i]->nres;k++)  {
        S[i]->c0[k] = SPS[j][i1]->c1[k];
        S[i]->c [k] = SPS[j][i1]->c1[k];
      }
    }
  }

  //  4. Make a first-guess C-alpha alignment: leave only
  //     mappings that are common for all structures

  for (i=0;i<S[0]->nres;i++)
    S[0]->c0[i] = 0;  // use this as a counter of mapped atoms
  
  //     count atoms mapped on each atom of 1st structure
  for (i=1;i<nStruct;i++)
    for (j=0;j<S[i]->nres;j++)  {
      m = S[i]->c0[j];  // i:j is mapped on 0:m
      if (m>=0)  S[0]->c0[m]++;  // count atoms mapped on this one
    }

  Nalign = 0;
  for (i=0;i<S[0]->nres;i++)  {
    if (S[0]->c0[i]<nStruct-1)  {
      S[0]->c0[i] = -1;  // unmap this atom as not all structures
                         // have atoms mapped on it
    } else  {
      S[0]->c0[i] = i;   // map on itself for consensus calcs
      Nalign++;
    }
    S[0]->c[i] = S[0]->c0[i];
  }

  for (i=1;i<nStruct;i++)  {
    for (j=0;j<S[i]->nres;j++)  {
      m = S[i]->c0[j];
      if (m>=0)  {
        if (S[0]->c0[m]<0)
          S[i]->c0[j] = -1;
      }
      S[i]->c[j] = S[i]->c0[j];
    }
  }
  
  return rc;

}


void  gsmt::MAligner::freeMap()  {
int i;
  if (Map)  {
    for (i=0;i<map_nrows;i++)
      Map[i].Dispose();
    delete[] Map;
    Map = NULL;
  }
  map_nrows = 0;
}

void  gsmt::MAligner::allocateMap()  {
int i;
  freeMap();
  map_nrows = S[0]->nres;
  Map = new MAMap[map_nrows];
  for (i=0;i<map_nrows;i++)
    Map[i].Init ( nStruct-1 );
}


bool gsmt::MAligner::evaluateMapping ( mmdb::PMContact C )  {
//  Calculates optimal mapping for the contact, but does not
//  do the actual mapping
mmdb::PAtom    a0;
mmdb::realtype d,dmin;
int            i0,i,j,j0;

  i0 = C->contactID;
  j0 = -1;
  a0 = S[0]->Calpha[i0];  // the reference atom
  if (!a0)  return false;

  // now simply choose, from all atoms contacting the reference one,
  // those still available, fitting by chainline and closest
  for (i=0;i<C->nStruct;i++)  {
    dmin = mmdb::MaxReal;
    j0   = -1;
    for (j=0;j<C->nAtoms[i];j++)
      if ((S[i+1]->c[C->id[i][j]]<0) &&
          (!S[i+1]->isMC(C->id[i][j],S[0],i0)))  {
        d = a0->GetDist2 ( C->atom[i][j] );
        if (d<dmin)  {
          dmin = d;
          j0   = j;
        }
      }
    if (j0>=0)  Map[i0].map[i+1] = C->id[i][j0];
          else  break;
  }
  if (j0<0)  return false;

  Map[i0].xc = a0->x;
  Map[i0].yc = a0->y;
  Map[i0].zc = a0->z;
  for (i=1;i<nStruct;i++)  {
    Map[i0].xc += S[i]->Calpha[Map[i0].map[i]]->x;
    Map[i0].yc += S[i]->Calpha[Map[i0].map[i]]->y;
    Map[i0].zc += S[i]->Calpha[Map[i0].map[i]]->z;
  }
  Map[i0].xc /= nStruct;
  Map[i0].yc /= nStruct;
  Map[i0].zc /= nStruct;

  Map[i0].rmsd = a0->GetDist2 ( Map[i0].xc,Map[i0].yc,Map[i0].zc );
  for (i=1;i<nStruct;i++)
    Map[i0].rmsd += S[i]->Calpha[Map[i0].map[i]]->GetDist2 (
                                    Map[i0].xc,Map[i0].yc,Map[i0].zc );
  Map[i0].rmsd /= nStruct;
  
  return true;

}



void gsmt::MAligner::correspondContacts ( mmdb::realtype contDist )  {
mmdb::PPAtom *   AIndex;
mmdb::PPMContact contact;
mmdb::realtype   dmin;
int              i,j,k,mpos, nconts;

  //  1. Prepare array of atom indices for contact seeking

  AIndex = new mmdb::PPAtom[nStruct];
  for (i=0;i<nStruct;i++)  {
    AIndex[i] = new mmdb::PAtom[S[i]->nres];
    for (j=0;j<S[i]->nres;j++)
      if (S[i]->c[j]<0)
           AIndex[i][j] = S[i]->Calpha[j];
      else AIndex[i][j] = NULL;
    if (i>0)  v1[i] = S[i]->nres;
  }

  //  2. Find multiple contacts
  
  contact = NULL;
  S[0]->getMMDBManager()->SeekContacts ( AIndex[0],S[0]->nres,
                                         &(AIndex[1]),&(v1[1]),
                                         nStruct-1,0.0,contDist,
                                         contact,0 );

  //  3. Remove incomlete contacts
  
  nconts = 0;
  for (i=0;i<S[0]->nres;i++)
    if (contact[i])  {
      k = 1;
      if (contact[i]->nAtoms)
        for (j=0;j<contact[i]->nStruct;j++)
          if (contact[i]->nAtoms[j]>0)  k++;
      if (k>=nStruct)  {
        if (i>nconts)  {
          contact[nconts] = contact[i];
          contact[i]      = NULL;
        }
        nconts++;
      } else  {
        delete contact[i];
        contact[i] = NULL;
      }
    }


  //  4. Cover contacts one-by-one in order of increasing
  //     the optimal mapping rmsd

  do  {

    //  4.1 As any new contact affects the optimal mapping,
    //      calculate the shortest contact each time before
    //      the mapping
    dmin = mmdb::MaxReal;
    k    = -1;
    for (i=0;i<nconts;i++)
      if (contact[i])  {
        if (evaluateMapping(contact[i]))  {
          if (Map[contact[i]->contactID].rmsd<dmin)  {
            dmin = Map[contact[i]->contactID].rmsd;
            k = i;
          }
        } else  {
          mpos = contact[i]->contactID;
          for (j=1;j<nStruct;j++)
            Map[mpos].map[j] = -1;
          delete contact[i];
          contact[i] = NULL;
        }
      }

    if (k>=0)  {
      mpos = contact[k]->contactID;
      S[0]->c[mpos] = mpos;
      for (i=1;i<nStruct;i++)
        S[i]->c[Map[mpos].map[i]] = mpos;
      delete contact[k];
      contact[k] = NULL;
    }

  } while (k>=0);

  DeleteMContacts ( contact,S[0]->nres );

  for (i=0;i<nStruct;i++)
    if (AIndex[i])  delete[] AIndex[i];
  delete[] AIndex;

}


void gsmt::MAligner::calcRMSD ( int mappos )  {
int i;

  Map[mappos].xc = S[0]->Calpha[mappos]->x;
  Map[mappos].yc = S[0]->Calpha[mappos]->y;
  Map[mappos].zc = S[0]->Calpha[mappos]->z;
  for (i=1;i<nStruct;i++)  {
    Map[mappos].xc += S[i]->Calpha[Map[mappos].map[i]]->x;
    Map[mappos].yc += S[i]->Calpha[Map[mappos].map[i]]->y;
    Map[mappos].zc += S[i]->Calpha[Map[mappos].map[i]]->z;
  }
  Map[mappos].xc /= nStruct;
  Map[mappos].yc /= nStruct;
  Map[mappos].zc /= nStruct;

  Map[mappos].rmsd = S[0]->Calpha[mappos]->GetDist2 ( 
                        Map[mappos].xc,Map[mappos].yc,Map[mappos].zc );
  for (i=1;i<nStruct;i++)
    Map[mappos].rmsd += S[i]->Calpha[Map[mappos].map[i]]->GetDist2 (
                        Map[mappos].xc,Map[mappos].yc,Map[mappos].zc );
  Map[mappos].rmsd /= nStruct;

  
}


mmdb::realtype  gsmt::MAligner::MatchQuality  ( int            Nalgn,
                                                int            N1,
                                                int            N2,
                                                mmdb::realtype dist2 ) {
mmdb::realtype  NormN,Na2,NormR;
  NormN = N1*N2;
  if (NormN<=0.0) return 0.0;
  Na2   = Nalgn*Nalgn;
  NormR = 4.0*dist2/(Nalgn*QR0*QR0);  // 4.0 makes it compatible with 
  return  Na2/((1.0+NormR)*NormN);    //     pairwise Q-score
}

mmdb::realtype  gsmt::MAligner::matchQuality2 ( int            Nalgn,
                                                mmdb::realtype dist2 ) {
mmdb::realtype  NormN,Na2,NormR;
  NormN = minNres*maxNres;
  if (NormN<=0.0) return 0.0;
  Na2   = Nalgn*Nalgn;
  NormR = 4.0*dist2/(Nalgn*QR0*QR0);  // 4.0 makes it compatible with
  return  Na2/((1.0+NormR)*NormN);    //     pairwise Q-score
}


void  gsmt::MAligner::calcConsensus()  {
int i,j,k;
  for (i=0;i<maxNres;i++)  {
    xc[i] = 0.0;
    yc[i] = 0.0;
    zc[i] = 0.0;
  }
  for (i=0;i<nStruct;i++)
    for (j=0;j<S[i]->nres;j++)  {
      k = S[i]->c[j];
      if (k>=0)  {
        xc[k] += S[i]->Calpha[j]->x;
        yc[k] += S[i]->Calpha[j]->y;
        zc[k] += S[i]->Calpha[j]->z;
      }
    }
  for (i=0;i<maxNres;i++)  {
    xc[i] /= nStruct;
    yc[i] /= nStruct;
    zc[i] /= nStruct;
  }
}


int  gsmt::MAligner::calcRotation ( mmdb::mat44 & R )  {
//   Given the correlation matrix A, this function calculates rotation
// matrix R, optimally excluding the rotoinversion if occured
mmdb::realtype det,B;
int            i,j,k;

  det = svdA[0][0]*svdA[1][1]*svdA[2][2] +
        svdA[0][1]*svdA[1][2]*svdA[2][0] +
        svdA[1][0]*svdA[2][1]*svdA[0][2] -
        svdA[0][2]*svdA[1][1]*svdA[2][0] -
        svdA[0][0]*svdA[1][2]*svdA[2][1] -
        svdA[2][2]*svdA[0][1]*svdA[1][0];

  SVD3 ( svdA,svdZ,svdV,svdW,svdRV,i );

//  mmdb::math::SVD ( 3,3,3,svdA,svdZ,svdV,svdW,svdRV,true,true,i );

  if (i!=0) {
    for (i=0;i<4;i++)  {
      for (j=0;j<4;j++)
        R[i][j] = 0.0;
      R[i][i] = 1.0;
    }
    return 1;
  }

  if (det<=0.0)  {
    k = 0;
    B = mmdb::MaxReal;
    for (j=0;j<3;j++)
      if (svdW[j]<B)  {
        B = svdW[j];
        k = j;
      }
    for (j=0;j<3;j++)
      svdV[j][k] = -svdV[j][k];
  }

  for (j=0;j<3;j++)
    for (k=0;k<3;k++)  {
      B = 0.0;
      for (i=0;i<3;i++)
        B += svdZ[j][i]*svdV[k][i];
      R[j][k] = B;
    }

  R[3][0] = 0.0;
  R[3][1] = 0.0;
  R[3][2] = 0.0;
  R[3][3] = 1.0;

  return 0;

}


void  gsmt::MAligner::optimizeAlignments()  {
SortMappings    sortMappings;
mmdb::rvector   rmsd0;
mmdb::ivector   ci;
mmdb::realtype  rmsd,Qscore,r1,Q1;
int             i,j,k, nc, nalgn,nalgn1, rc, iter;
int             nobetter_cnt;
bool            done;

  rc = GSMT_Ok;

  mmdb::FreeVectorMemory ( xc  ,0 );
  mmdb::FreeVectorMemory ( yc  ,0 );
  mmdb::FreeVectorMemory ( zc  ,0 );
  //mmdb::FreeVectorMemory ( disp,0 );

  minNres = mmdb::MaxInt4;
  maxNres = mmdb::MinInt4;
  for (i=0;i<nStruct;i++)  {
    S[i]->saveCoordinates();
    if (S[i]->nres<minNres)  minNres = S[i]->nres;
    if (S[i]->nres>maxNres)  maxNres = S[i]->nres;
    mmdb::Mat4Copy ( S[i]->RT0,S[i]->RT );
  }

  mmdb::GetVectorMemory ( xc,maxNres,0 );
  mmdb::GetVectorMemory ( yc,maxNres,0 );
  mmdb::GetVectorMemory ( zc,maxNres,0 );
  mmdb::GetVectorMemory ( ci   ,S[0]->nres,0 );
  mmdb::GetVectorMemory ( rmsd0,S[0]->nres,0 );
  //mmdb::GetVectorMemory ( disp ,S[0]->nres,0 );

  allocateMap();

  Nalign        = 0;
  rmsd_achieved = mmdb::MaxReal;
  Q_achieved    = -1.0;

  iter         = 0;
  nobetter_cnt = 0;
  do  {

    //  1. Bring structures to the best mutual positions
    for (i=0;i<nStruct;i++)  {
      S[i]->transform();
      for (j=0;j<S[i]->nres;j++)  {
        S[i]->c[j] = S[i]->c0[j];
        if (S[i]->c[j]>=0)
             S[i]->unmap1[j] = UNMAP_NO;
        else S[i]->unmap1[j] = UNMAP_YES;
      }
    }

    //  2. Set initial mappings

    for (i=0;i<S[0]->nres;i++)
      for (j=1;j<nStruct;j++)
        Map[i].map[j] = -1;

    // 0:i is mapped onto j:SMAMap[i].map[j]
    for (i=1;i<nStruct;i++)
      for (j=0;j<S[i]->nres;j++)  {
        k = S[i]->c[j];
        if (k>=0)  Map[k].map[i] = j;
      }

    //  3. Try to expand the mappings

    correspondContacts ( maxContact );

    //    RecoverGaps();

    nalgn = 0;
    rmsd  = 0.0;
    nc    = 0;
    for (i=0;i<S[0]->nres;i++)
      if (S[0]->c[i]>=0)  {
        k = 0;
        for (j=1;(j<nStruct) && (k>=0);j++)
          k = Map[i].map[j];
        if (k>=0)  {
          nalgn++;
          calcRMSD ( i );
          rmsd += Map[i].rmsd;
          if (S[0]->unmap1[i]!=UNMAP_NO)
            ci[nc++] = i;
        } else  {
          S[0]->c[i] = -1;
          for (j=1;j<nStruct;j++)
            if (Map[i].map[j]>=0)  {
              S[j]->c[Map[i].map[j]] = -1;
              Map[i].map[j] = -1;
            }
        }
      }

    Qscore = matchQuality2 ( nalgn,rmsd );

    if (nc>0)  {
      // unmap atoms for increasing the Q-score
      sortMappings.Sort ( ci,nc,Map );
      nalgn1 = nalgn;
      r1     = rmsd;
      k      = -1;
      for (i=0;i<nc;i++)  {
        r1 -= Map[ci[i]].rmsd;
        nalgn1--;
        Q1  = matchQuality2 ( nalgn1,r1 );
        if (Q1>Qscore)  {
          Qscore = Q1;
          rmsd   = r1;
          nalgn  = nalgn1;
          k      = i;
        }
      }
      for (i=0;i<=k;i++)  {
        S[0]->c[ci[i]] = -1;
        for (j=1;j<nStruct;j++)
          S[j]->c[Map[ci[i]].map[j]] = -1;
      }
    }

    //  4. Calculate consensus coordinates

    if (nalgn>0)  rmsd /= nalgn;

    if (Qscore>Q_achieved)  {
      Q_achieved    = Qscore;
      rmsd_achieved = rmsd;  // square root is taken once before return
      Nalign        = nalgn;
      for (i=0;i<nStruct;i++)
        for (j=0;j<S[i]->nres;j++)
          S[i]->c0[j] = S[i]->c[j];
      for (i=0;i<S[0]->nres;i++)
        rmsd0[i] = Map[i].rmsd;
      for (i=0;i<nStruct;i++)
        mmdb::Mat4Copy ( S[i]->RT,S[i]->RT0 );
      nobetter_cnt = 0;
    } else
      nobetter_cnt++;

    done = (nalgn==0) || (iter>maxIter) ||
           ((iter>minIter) && (nobetter_cnt>maxHollowIt));

    if (!done)  {

//      calcConsensus();
      /*
      for (i=0;i<map_nrows;i++)  {
        xc[i] = Map[i].xc;
        yc[i] = Map[i].yc;
        zc[i] = Map[i].zc;
      }
      */
      
      //  5. Optimize superposition

      for (i=0;(i<nStruct) && (rc==GSMT_Ok);i++)  {
        S[i]->calcCorrelationMatrix ( svdA,Map,sigma*sigma );
        if (calcRotation(S[i]->RT))  rc = GSMT_SVDFail;
                               else  S[i]->calcTranslation();
      }

      done = (rc!=GSMT_Ok);

    }

    iter++;
//      if (ProgressFunc)  (*ProgressFunc)(ProgFuncData,2,iter);

    for (i=0;i<nStruct;i++)
      S[i]->restoreCoordinates();

  } while (!done);

  for (i=0;i<nStruct;i++)  {
    mmdb::Mat4Copy ( S[i]->RT0,S[i]->RT );
    for (j=0;j<S[i]->nres;j++)
      S[i]->c[j] = S[i]->c0[j];
  }

  for (i=0;i<S[0]->nres;i++)  {
    Map[i].rmsd = rmsd0[i];
    for (j=1;j<nStruct;j++)
      Map[i].map[j] = -1;
  }
  for (i=1;i<nStruct;i++)
    for (j=0;j<S[i]->nres;j++)  {
      k = S[i]->c0[j];
      if (k>=0)  Map[k].map[i] = j;
    }

  mmdb::FreeVectorMemory ( ci   ,0 );
  mmdb::FreeVectorMemory ( rmsd0,0 );

  rmsd_achieved = sqrt(rmsd_achieved);

}


void  gsmt::MAligner::sortStructures()  {
//  sorts structures in the original order
PMAStructure MAS;
mmdb::mat44  RT;
int          i,j,k,m;

  k = 0;
  j = S[0]->serNo;
  for (i=1;i<nStruct;i++)
    if (S[i]->serNo<j)  {
      k = i;
      j = S[i]->serNo;
    }

  if (k>0)  {

    //  All alignments are mapped as i->0.
    //  Make inverse mapping 0->k
    for (i=0;i<S[0]->nres;i++)  {
      S[0]->c0[i] = -1;  // unmap 1st structure completely
      S[0]->c [i] = -1;
    }

    for (i=0;i<S[k]->nres;i++)  {
      j = S[k]->c0[i];    // k:i is mapped to 0:j
      if (j>=0)  {
        S[0]->c0[j] = i;  // now 0:j is mapped to k:i
        S[0]->c [j] = i;
      }
    }

    //  Remap all other alignments as i->0->k
    for (i=1;i<nStruct;i++)
      if (i!=k)  {
        for (j=0;j<S[i]->nres;j++)  {
          m = S[i]->c0[j];    // i:j is mapped on 0:m
          if (m>=0)  {
            m = S[0]->c0[m];
            if (m>=0)  S[i]->c0[j] = m;
                 else  S[i]->c0[j] = -1;
          }
          S[i]->c[j] = S[i]->c0[j];
        }
      }

  }

  //  Sort structures by serial numbers

  if (k>0)  {
    MAS = S[k];  S[k] = S[0];  S[0] = MAS;
  }
  for (i=1;i<nStruct-1;i++)
    for (j=i+1;j<nStruct;j++)
      if (S[j]->serNo<S[i]->serNo)  {
        MAS = S[j];  S[j] = S[i];  S[i] = MAS;
      }

  //  Map 0th structure onto itself
  for (i=0;i<S[0]->nres;i++)
    if (S[0]->c0[i]>=0)  {
      S[0]->c0[i] = i;
      S[0]->c [i] = i;
    }

  //  Set mappings
  if (Map)  {
    freeMap    ();
    allocateMap();
    for (i=0;i<S[0]->nres;i++)
      for (j=1;j<nStruct;j++)
        Map[i].map[j] = -1;
    for (i=1;i<nStruct;i++)
      for (j=0;j<S[i]->nres;j++)  {
        k = S[i]->c[j];
        if (k>=0)  Map[k].map[i] = j;
      }
  }
  
  // Orient all structures relatively 1st one
  for (i=nStruct-1;i>=0;i--)  {
    mmdb::Mat4Copy ( S[i]->RT0,RT );
    //  Calculates A=B^{-1}*C
    mmdb::Mat4Div1 ( S[i]->RT0,S[0]->RT0,RT );
    mmdb::Mat4Copy ( S[i]->RT,RT );
    mmdb::Mat4Div1 ( S[i]->RT,S[0]->RT,RT );
  }

}


void  gsmt::MAligner::calcConsensusScores()  {
mmdb::ivector  ix;
mmdb::ovector  sc;
mmdb::realtype B,nid;
int            i,j,k,m;

  mmdb::FreeMatrixMemory ( mx_rmsd  ,nStructAlloc,0,0 );
  mmdb::FreeMatrixMemory ( mx_Qscore,nStructAlloc,0,0 );
  mmdb::FreeMatrixMemory ( mx_seqId ,nStructAlloc,0,0 );

  mmdb::GetMatrixMemory ( mx_rmsd  ,nStruct,nStruct,0,0 );
  mmdb::GetMatrixMemory ( mx_Qscore,nStruct,nStruct,0,0 );
  mmdb::GetMatrixMemory ( mx_seqId ,nStruct,nStruct,0,0 );
  nStructAlloc = nStruct;

  mmdb::GetVectorMemory ( ix,maxNres,0 );
  mmdb::GetVectorMemory ( sc,maxNres,0 );

  for (i=0;i<maxNres;i++)
    sc[i] = false;

  //  Bring structures to the best mutual positions
  for (i=0;i<nStruct;i++)
    S[i]->transform();

  //  Calculate consensus coordinates
  calcConsensus();

  //  Calculate relative and consensus-related scores
  for (i=0;i<nStruct;i++)  {
    B = 0.0;
    for (j=0;j<S[i]->nres;j++)  {
      k = S[i]->c0[j];
      if (k>=0)  {
        ix[k] = j;
        sc[k] = true;
        B    += S[i]->Calpha[j]->GetDist2 ( xc[k],yc[k],zc[k] );
      }
    }
    mx_rmsd  [i][i] = sqrt(B/Nalign);
    mx_Qscore[i][i] = MatchQuality ( Nalign,S[i]->nres,Nalign,B );
    mx_seqId [i][i] = 1.0;
    for (m=i+1;m<nStruct;m++)  {
      B   = 0.0;
      nid = 0.0;
      for (j=0;j<S[m]->nres;j++)  {
        k = S[m]->c0[j];
        if (k>=0)  {
          B += S[m]->Calpha[j]->GetDist2 ( S[i]->Calpha[ix[k]] );
          if (!strcmp(S[m]->Calpha[j]->GetResName(),
                      S[i]->Calpha[ix[k]]->GetResName()))  nid += 1.0;
        }
      }
      mx_rmsd  [i][m] = sqrt(B/Nalign);
      mx_Qscore[i][m] = MatchQuality ( Nalign,S[i]->nres,S[m]->nres,B );
      mx_seqId [i][m] = nid/mmdb::realtype(Nalign);
      mx_rmsd  [m][i] = mx_rmsd  [i][m];
      mx_Qscore[m][i] = mx_Qscore[i][m];
      mx_seqId [m][i] = mx_seqId [i][m];
    }
  }

  //  Mark unoccupied consensus positions
  for (i=0;i<maxNres;i++)
    if (!sc[i])  {
      xc[i] = -mmdb::MaxReal;
      yc[i] = -mmdb::MaxReal;
      zc[i] = -mmdb::MaxReal;
    }

  //  Restore original coordinates
  for (i=0;i<nStruct;i++)
    S[i]->restoreCoordinates();

  mmdb::FreeVectorMemory ( sc,0 );
  mmdb::FreeVectorMemory ( ix,0 );

}


void gsmt::MAligner::deselectCalphas()  {}


void gsmt::MAligner::getAlignScores ( int            & n_align,
                                      mmdb::realtype & rmsd,
                                      mmdb::realtype & Qscore )  {
  n_align = Nalign;        // number of multuply-aligned rows
  rmsd    = rmsd_achieved; // achieved RMSD
  Qscore  = Q_achieved;    // achieved Q
}

void gsmt::MAligner::getConsensusScores ( mmdb::rvector & cons_x,
                                          mmdb::rvector & cons_y,
                                          mmdb::rvector & cons_z,
                                          int           & cons_len,
                                          mmdb::rmatrix & m_rmsd,
                                          mmdb::rmatrix & m_Qscore,
                                          mmdb::rmatrix & m_seqId )  {
//  The function does not allocate the vectors, it simply
// returns pointers to the internal fields:
// cons_x,y,z[0..conslen-1], m_rmsd,Qscore[0..nStruct][0..nStruct]
  cons_x   = xc;
  cons_y   = yc;
  cons_z   = zc;
  cons_len = maxNres;
  m_rmsd   = mx_rmsd;
  m_Qscore = mx_Qscore;
  m_seqId  = mx_seqId;
}

void gsmt::MAligner::takeConsensusScores ( mmdb::rmatrix & m_rmsd,
                                           mmdb::rmatrix & m_Qscore,
                                           mmdb::rmatrix & m_seqId )  {
  m_rmsd    = mx_rmsd;
  m_Qscore  = mx_Qscore;
  m_seqId   = mx_seqId;
  mx_rmsd   = NULL;
  mx_Qscore = NULL;
  mx_seqId  = NULL;
}


namespace gsmt  {

  void ExpandMAOut ( PPMAOutput & MAOut, int nrows1,
                     int ncols, int & nrows0 )  {
  PPMAOutput M;
  int        i,j;
    M = new PMAOutput[nrows1];
    for (i=0;i<nrows0;i++)
      if (MAOut[i])  {
        M[i] = new MAOutput[ncols];
        for (j=0;j<ncols;j++)
          M[i][j].Copy ( MAOut[i][j] );
      } else
        M[i] = NULL;
    for (i=nrows0;i<nrows1;i++)
      M[i] = NULL;
    FreeMSOutput ( MAOut,nrows0 );
    MAOut  = M;
    nrows0 = nrows1;
  }

}

void gsmt::MAligner::getMAOutput ( PPMAOutput & MAOut,
                                   int & nrows, int & ncols )  {
mmdb::ivector  ic1,ic2;
int            i,j,k,m,ic,n0,k0;
bool           done;

  FreeMSOutput ( MAOut,nrows );

  ncols = nStruct;
  if (nStruct<=0)  return;

//  SelectCalphas();

  n0 = -1;
  for (i=0;i<nStruct;i++)
    if (S[i]->nres>n0)
      n0 = S[i]->nres;

  if (n0<=0)  {
//    DeselectCalphas();
    return;
  }

  n0 = 2*n0+1;
  MAOut = new PMAOutput[n0];
  for (i=0;i<n0;i++)
    MAOut[i] = NULL;

  mmdb::GetVectorMemory ( ic1,nStruct,0 );
  mmdb::GetVectorMemory ( ic2,nStruct,0 );
  for (i=0;i<nStruct;i++)  {
    ic1[i] = 0;  // structure cursors
    ic2[i] = 0;  // structure cursors
  }

  nrows = 0;

  do  {

    done = false;

    // Align last C-alphas in the gap to the begining of the block.
    // Non-aligned C-alphas are skipped in all structures, and
    // cursors ic2 are set to the first aligned row, which starts
    // a block of aligned residues.
    k  = 0; // k counts the maximal number of non-aligned residues
            // between the bloacks.
    ic = 0; // ic counts the number of structures for which cursors
            // have run out of range
    for (i=0;i<nStruct;i++)  {
      j = ic1[i];
      while (j<S[i]->nres)
        if (S[i]->c[j]<0)  j++;
                     else  break;
      if (j>=S[i]->nres)  ic++;
      m = j-ic1[i];
      if (m>k)  k = m;
      ic2[i] = j;  // cursor set on first aligned C-alpha in the block
    }
//    if (ic>=nStruct)  done = true;

    if (ic>0)  done = true;  // quit if any cursor is out of range

    if (!done)  {

      if (k>0)  {
        m      = nrows;
        nrows += k;
        if (nrows>n0)
          ExpandMAOut ( MAOut,nrows*3/2,ncols,n0 );
        for (i=m;i<nrows;i++)  {
          MAOut[i] = new MAOutput[nStruct];
          for (j=0;j<nStruct;j++)
            MAOut[i][j].Init();
        }
        for (i=0;i<nStruct;i++)  {
          j = ic2[i]-1;  // last non-aligned C-alpha before the block
          k = nrows-1;   // and its position in the aligned table
          while (j>=ic1[i])  {
            MAOut[k][i].Fill ( S[i]->Calpha[j],false );
            k--;
            j--;
          }
        }
      }

      // align C-alphas in the block
      k = mmdb::MaxInt4;
      ic = 0;
      for (i=0;i<nStruct;i++)  {
        ic1[i] = ic2[i];
        j      = ic1[i];
        while (j<S[i]->nres)
          if (S[i]->c[j]>=0)  j++;
                        else  break;
        if (j>=S[i]->nres)  ic++;
        m = j-ic1[i];
        if ((m>=0) && (m<k))  k = m;
      }
      // quit only if all cursors ran to the end
      if (ic>=nStruct)  done = true;
//      if (ic>0)  done = true;
      if (k>0)  {
        m = nrows+k;
        if (m>n0)
          ExpandMAOut ( MAOut,m*3/2,ncols,n0 );
        for (i=nrows;i<m;i++)  {
          MAOut[i] = new MAOutput[nStruct];
          for (j=0;j<nStruct;j++)
            MAOut[i][j].Init();
        }
        for (i=0;i<nStruct;i++)  {
          j = ic1[i];
          ic2[i] = mmdb::IMin(S[i]->nres,j+k);
          m = nrows;
          for (j=ic1[i];j<ic2[i];j++)  {
            MAOut[m][i].Fill ( S[i]->Calpha[j],true );
            k0 = S[i]->c0[j];
            if (k0>=0)
              MAOut[m][i].rmsd = S[i]->Calpha[j]->GetDist2 ( S[i]->RT0,
                                                xc[k0],yc[k0],zc[k0] );
            m++;
          }
          ic1[i] = ic2[i];
        }
        nrows += k;
      }

    }

    // align C-alphas in the following gap to the end of the block
    k = 0;
    ic = 0;
    for (i=0;i<nStruct;i++)  {
      j = ic1[i];
      while (j<S[i]->nres)
        if (S[i]->c[j]<0)  j++;
                     else  break;
      if (j>=S[i]->nres)  ic++;
                    else  j = mmdb::mround((j+ic1[i]+0.25)/2.0);
      m = j-ic1[i];
      if (m>k)  k = m;
      ic2[i] = j;  // cursor set on between the aligned blocks
    }
//    if (ic>=nStruct)  done = true;
    if (ic>0)  done = true;   // quit if any cursor is out of range
    if (k>0)  {
      m      = nrows;
      nrows += k;
      if (nrows>n0)
        ExpandMAOut ( MAOut,nrows*3/2,ncols,n0 );
      for (i=m;i<nrows;i++)  {
        MAOut[i] = new MAOutput[nStruct];
        for (j=0;j<nStruct;j++)
          MAOut[i][j].Init();
      }
      for (i=0;i<nStruct;i++)  {
        k = m;
        for (j=ic1[i];j<ic2[i];j++)  {
          MAOut[k][i].Fill ( S[i]->Calpha[j],false );
          k++;
        }
        ic1[i] = ic2[i];
      }
    }

  } while (!done);

  mmdb::FreeVectorMemory ( ic1,0 );
  mmdb::FreeVectorMemory ( ic2,0 );

//  DeselectCalphas();

}


void gsmt::MAligner::writeMultAlign ( mmdb::io::RFile f )  {
PPMAOutput     MAOut;
char           L[100],SS[4];
mmdb::realtype disp;
int            nrows,ncols, i,j;

  MAOut = NULL;
  nrows = 0;
  ncols = 0;
  getMAOutput ( MAOut,nrows,ncols );

  f.Write ( "  Disp. " );
  for (i=0;i<nStruct;i++)  {
    f.Write ( "| |" );
    sprintf ( L,"    %4s    ",S[i]->getRefName() );
    f.Write ( L );
  }
  f.LF();
  f.Write ( " -------" );
  for (i=0;i<nStruct;i++)  {
    f.Write ( "+-+" );
    f.Write ( "------------" );
  }
  f.LF();

  for (i=0;i<nrows;i++)  {
    if (MAOut[i][0].aligned)  {
      disp = MAOut[i][0].rmsd;
      for (j=1;j<ncols;j++)
        disp += MAOut[i][j].rmsd;
      sprintf ( L," %6.3f ",disp/ncols );
    } else
      strcpy  ( L,"        " );
    f.Write ( L );
    for (j=0;j<ncols;j++)  {
      if (MAOut[i][j].aligned)  f.Write ( "|*|" );
                          else  f.Write ( "| |" );
      if (MAOut[i][j].name[0])  {
        if (MAOut[i][j].sseType==mmdb::SSE_Helix)
             strcpy ( SS,"H|" );
        else if (MAOut[i][j].sseType==mmdb::SSE_Strand)
             strcpy ( SS,"S|" );
        else strcpy ( SS,"  " );
        sprintf ( L,"%2s%1s:%3s%4i%1s",SS,MAOut[i][j].chID,
                  MAOut[i][j].name,MAOut[i][j].seqNum,
                  MAOut[i][j].insCode );
      } else
        strcpy ( L,"            " );
      f.Write ( L );
    }
    f.LF();
  }

  f.Write ( " -------" );
  for (i=0;i<nStruct;i++)  {
    f.Write ( "'-'" );
    f.Write ( "------------" );
  }
  f.LF();

  FreeMSOutput ( MAOut,nrows );

}


void gsmt::MAligner::makeMultAlignTable_rvapi ( mmdb::cpstr tableId )  {
PPMAOutput     MAOut;
char           L[100],SS[1000];
mmdb::realtype disp;
int            nrows,ncols, i,j,k;

  MAOut = NULL;
  nrows = 0;
  ncols = 0;
  getMAOutput ( MAOut,nrows,ncols );

  rvapi_put_horz_theader ( tableId,"Disp.","Dispersion (&Aring;)",0 );
  k = 1;
  for (i=0;i<nStruct;i++)  {
    rvapi_put_horz_theader ( tableId,"&nbsp;",
                             "Alignment sign: = for aligned residues, "
                             "empty string for not aligned",k++ );
    sprintf ( L,"%4s",S[i]->getRefName() );
    sprintf ( SS,"Structure #%i",i+1 );
    rvapi_put_horz_theader ( tableId,L,SS,k++ );
  }

  for (i=0;i<nrows;i++)  {
    sprintf ( L,"%i",i+1 );
    rvapi_put_vert_theader ( tableId,L,"",i );
    if (MAOut[i][0].aligned)  {
      disp = MAOut[i][0].rmsd;
      for (j=1;j<ncols;j++)
        disp += MAOut[i][j].rmsd;
      sprintf ( L,"%.4f",disp/ncols );
    } else
      strcpy  ( L,"&nbsp;" );
    k = 0;
    rvapi_put_table_string ( tableId,L,i,k++ );
    for (j=0;j<ncols;j++)  {
      if (MAOut[i][j].aligned)
            rvapi_put_table_string ( tableId,"=",i,k++ );
      else  rvapi_put_table_string ( tableId,"&nbsp;",i,k++ );
      if (MAOut[i][j].name[0])  {
        if (MAOut[i][j].sseType==mmdb::SSE_Helix)
             strcpy ( SS,"H|" );
        else if (MAOut[i][j].sseType==mmdb::SSE_Strand)
             strcpy ( SS,"S|" );
        else strcpy ( SS,"  " );
        sprintf ( L,"%2s%1s:%3s%4i%1s",SS,MAOut[i][j].chID,
                  MAOut[i][j].name,MAOut[i][j].seqNum,
                  MAOut[i][j].insCode );
        rvapi_make_hard_spaces ( SS,L );
      } else
        strcpy ( SS,"&nbsp;" );
      rvapi_put_table_string ( tableId,SS,i,k++ );
    }
  }

  FreeMSOutput ( MAOut,nrows );

}



void gsmt::MAligner::makeMultAlignGraph_rvapi ( mmdb::cpstr graphId )  {
// Puts alignment distance profile in graph with given graph id
PPMAOutput     MAOut;
mmdb::realtype disp;
int            nrows,ncols, i,j;

  MAOut = NULL;
  nrows = 0;
  ncols = 0;
  getMAOutput ( MAOut,nrows,ncols );

  rvapi_add_graph_data    ( "mal_data",graphId,"Dispersion profile" );
  rvapi_add_graph_dataset ( "pos","mal_data",graphId,
                            "Alignment position","Alignment position" );
  rvapi_add_graph_dataset ( "disp","mal_data",graphId,
                            "Dispersion (&Aring;)","Dispersion" );

  for (i=0;i<nrows;i++)
    if (MAOut[i][0].aligned)  {
      disp = MAOut[i][0].rmsd;
      for (j=1;j<ncols;j++)
        disp += MAOut[i][j].rmsd;
      rvapi_add_graph_int  ( "pos" ,"mal_data",graphId,i );
      rvapi_add_graph_real ( "disp","mal_data",graphId,disp,"%g" );
    }

  rvapi_add_graph_plot ( "mal_dist_plot",graphId,
                         "Dispersion profile of alignment",
                         "Alignment position",
                         "R.m.s.d."
                       );

  rvapi_add_plot_line ( "mal_dist_plot","mal_data",graphId,"pos","disp" );
  
  rvapi_set_line_options ( "disp","mal_dist_plot","mal_data",graphId,
                           RVAPI_COLOR_DarkBlue,RVAPI_LINE_Off,
                           RVAPI_MARKER_filledCircle,2.5,true );
  rvapi_set_plot_legend  ( "mal_dist_plot",graphId,
                           RVAPI_LEGEND_LOC_N,
                           RVAPI_LEGEND_PLACE_Inside );
 
}


void gsmt::MAligner::writeMultAlign_csv ( mmdb::io::RFile f )  {
PPMAOutput     MAOut;
char           L[100],SS[4];
mmdb::realtype disp;
int            nrows,ncols, i,j;

  MAOut = NULL;
  nrows = 0;
  ncols = 0;
  getMAOutput ( MAOut,nrows,ncols );

  f.Write ( "Disp.[A]" );
  for (i=0;i<nStruct;i++)  {
    f.Write ( ", " );
    f.Write ( S[i]->getRefName() );
  }
  f.LF();

  for (i=0;i<nrows;i++)  {
    if (MAOut[i][0].aligned)  {
      disp = MAOut[i][0].rmsd;
      for (j=1;j<ncols;j++)
        disp += MAOut[i][j].rmsd;
      sprintf ( L," %8.5f ",disp/ncols );
      f.Write ( L );
    }
    for (j=0;j<ncols;j++)  {
      f.Write ( ", " );
      if (MAOut[i][j].name[0])  {
        if (MAOut[i][j].sseType==mmdb::SSE_Helix)
             strcpy ( SS,"H " );
        else if (MAOut[i][j].sseType==mmdb::SSE_Strand)
             strcpy ( SS,"S " );
        else strcpy ( SS,"  " );
        sprintf ( L,"%2s%1s:%3s%4i%1s",SS,MAOut[i][j].chID,
                  MAOut[i][j].name,MAOut[i][j].seqNum,
                  MAOut[i][j].insCode );
        f.Write ( L );
      }
    }
    f.LF();
  }

  FreeMSOutput ( MAOut,nrows );

}

void gsmt::calcSeqAlignment ( PPMAOutput  MAOut,
                              int         nrows,
                              int         ncols,
                              mmdb::psvector & S  )  {
int i,j,k;

  mmdb::GetVectorMemory ( S,ncols,0 );

  k = nrows + (nrows/80+2)*2;
  for (j=0;j<ncols;j++)
    S[j] = new char[k];

  k = 0;
  for (i=0;i<nrows;i++)  {
    for (j=0;j<ncols;j++)  {
      if (MAOut[i][j].name[0])
            mmdb::Get1LetterCode ( MAOut[i][j].name,S[j][k] );
      else  S[j][k] = '-';
      if (!MAOut[i][j].aligned)
        S[j][k] = char(tolower(int(S[j][k])));
    }
    k++;
  }

  for (j=0;j<ncols;j++)
    S[j][k] = char(0);

}


void gsmt::MAligner::writeSeqAlignment ( mmdb::io::RFile f,
                                         mmdb::psvector  fnames,
                                         mmdb::psvector  sel )  {
PPMAOutput     MAOut;
mmdb::psvector S;
int            nrows,ncols,j;

  MAOut = NULL;
  nrows = 0;
  ncols = 0;
  getMAOutput ( MAOut,nrows,ncols );
  
  calcSeqAlignment ( MAOut,nrows,ncols,S  );

/*

  mmdb::GetVectorMemory ( S,ncols,0 );

  k = nrows + (nrows/80+2)*2;
  for (j=0;j<ncols;j++)
    S[j] = new char[k];

  k = 0;
  for (i=0;i<nrows;i++)  {
    for (j=0;j<ncols;j++)  {
      if (MAOut[i][j].name[0])
            mmdb::Get1LetterCode ( MAOut[i][j].name,S[j][k] );
      else  S[j][k] = '-';
      if (!MAOut[i][j].aligned)
        S[j][k] = char(tolower(int(S[j][k])));
    }
    k++;
  }
*/

  for (j=0;j<ncols;j++)  {
//    S[j][k] = char(0);
    f.Write ( ">"    );
    f.Write ( mmdb::io::GetFName(fnames[j],mmdb::io::syskey_all) );
    f.Write ( "("    );
    f.Write ( sel[j] );
    f.Write ( ")"    );
    f.LF();
    f.WriteLine ( S[j] );
    f.LF();
    delete[] S[j];
  }

  mmdb::FreeVectorMemory ( S,0 );

  FreeMSOutput ( MAOut,nrows );

}

void gsmt::MAligner::read  ( mmdb::io::RFile f )  {
  mmdb::io::Stream::read ( f );
}

void gsmt::MAligner::write ( mmdb::io::RFile f )  {
  mmdb::io::Stream::write ( f );
}

