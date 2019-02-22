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
//    06.02.16   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  GSMT_Sheaf <interface>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::Sheaf
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2008-2016
//
// =================================================================
//

#include <math.h>
#include <string.h>
#include <time.h>

#include "mmdb2/mmdb_atom.h"

#include "gsmt_sheaf.h"
#include "gsmt_superpose.h"

// =================================================================

gsmt::Sheaf::Sheaf()  {
  initSheaf();
}

gsmt::Sheaf::~Sheaf()  {
  freeMemory();
}

void gsmt::Sheaf::initSheaf()  {

  mode         = SHEAF_Auto; // sheaf mode
  R0           = 3.0;        // Q-score parameter
  Qthresh      = -1.0;       // threshold Q for sheafs (when >0)
  useOcc       = false;      // flag to use atom occupancy
  nthreads     = 1;          // number of threads to use
  verbosity    = 0;          // verbosity level
  iterMax      = 300;        // limit for iterative refinement

  S            = NULL;       // copy pointer on structure vector
  nAtoms       = 0;          // number of atoms (sheaf length)
  nStruct      = 0;          // number of structures
  sheafData    = NULL;       // sheafs
  allocNSheafs = 0;          // number of allocated sheafs
  nSheafs      = 0;          // number of sheafs found

  // working arrays
  allocNAtoms  = 0;          // length of working atom arrays
  var2         = NULL;       // square coordinate variance
  cx           = NULL;       // consensus x
  cy           = NULL;       // consensus y
  cz           = NULL;       // consensus z
  ix           = NULL;       // atom indices
  mask         = NULL;       // atom mask
  mask0        = NULL;       // temporary atom mask

  allocNStruct = 0;          // length of working structure arrays
  smask        = NULL;       // structure mask
  atm          = NULL;       // atom data
  RT           = NULL;       // rotation-translation matrices
  ax           = NULL;       // temporary atom x-coordinates
  ay           = NULL;       // temporary atom y-coordinates
  az           = NULL;       // temporary atom z-coordinates

}


void gsmt::Sheaf::freeMemory()  {

  mmdb::FreeVectorMemory ( var2 ,0 );
  mmdb::FreeVectorMemory ( cx   ,0 );
  mmdb::FreeVectorMemory ( cy   ,0 );
  mmdb::FreeVectorMemory ( cz   ,0 );
  mmdb::FreeVectorMemory ( ix   ,0 );
  mmdb::FreeVectorMemory ( mask ,0 );
  mmdb::FreeVectorMemory ( mask0,0 );
  allocNAtoms  = 0;
  
  mmdb::FreeVectorMemory ( smask,0 );
  if (atm)  {
    delete[] atm;
    atm = NULL;
  }
  if (RT)  {
    delete[] RT;
    RT = NULL;
  }
  mmdb::FreeVectorMemory ( ax,0 );
  mmdb::FreeVectorMemory ( ay,0 );
  mmdb::FreeVectorMemory ( az,0 );
  allocNStruct = 0;

  if (sheafData)  {
    for (int i=0;i<allocNSheafs;i++)
      if (sheafData[i])
        delete sheafData[i];
    delete[] sheafData;
    sheafData = NULL;
  }
  allocNSheafs = 0;
  nSheafs      = 0;
  
}


void gsmt::Sheaf::makeMemoryAllocations()  {

  freeMemory();

  allocNAtoms = nAtoms;
  mmdb::GetVectorMemory ( var2 ,allocNAtoms,0 );
  mmdb::GetVectorMemory ( cx   ,allocNAtoms,0 );
  mmdb::GetVectorMemory ( cy   ,allocNAtoms,0 );
  mmdb::GetVectorMemory ( cz   ,allocNAtoms,0 );
  mmdb::GetVectorMemory ( ix   ,allocNAtoms,0 );
  mmdb::GetVectorMemory ( mask ,allocNAtoms,0 );
  mmdb::GetVectorMemory ( mask0,allocNAtoms,0 );

  allocNStruct = nStruct;
  mmdb::GetVectorMemory ( smask,allocNStruct,0 );
  atm = new mmdb::PPAtom[allocNStruct];
  RT  = new mmdb::mat44[allocNStruct];
  mmdb::GetVectorMemory ( ax,allocNStruct,0 );
  mmdb::GetVectorMemory ( ay,allocNStruct,0 );
  mmdb::GetVectorMemory ( az,allocNStruct,0 );

  allocNSheafs = nStruct*(nStruct-1)/2;
  nSheafs      = allocNSheafs;
  sheafData = new PSheafData[allocNSheafs];
  for (int i=0;i<allocNSheafs;i++)
    sheafData[i] = new SheafData(2,allocNAtoms);

}


gsmt::GSMT_CODE gsmt::Sheaf::Align ( PPStructure structures,
                                     int         nStructures,
                                     bool        measure_cpu )  {
PSheafData     sdata,sdata0,sdata1;
char           sign[10];
mmdb::realtype clapse1,clapse2, Qmin,Qmax, Qsheaf, Q0,Q1;
clock_t        ct,ct1;
int            i,j;
GSMT_CODE      rc;
int            optKey;
bool           done;

  S       = structures;
  nStruct = nStructures;
  
  if (nStruct<2)
    return GSMT_NoStructures;
    
  nAtoms = S[0]->getNCalphas();
  for (i=1;i<nStruct;i++)
    nAtoms = mmdb::IMin(nAtoms,S[i]->getNCalphas()); 

  clapse1 = 0.0;
  clapse2 = 0.0;
  if (measure_cpu) ct = clock();
              else ct = 0;

  makeMemoryAllocations();
  rc = GSMT_Ok;
  
  if (nStruct==2)  {

    sheafData[0]->sId[0] = 0;
    sheafData[0]->sId[1] = 1;
    sheafData[0]->S  [0] = S[0];
    sheafData[0]->S  [1] = S[1];
    rc = sheaf2 ( sheafData[0] );
    nSheafs = 1;

  } else  {
  
    // Calculate all-to-all pairwise alignments, resulting in
    // a set of 2-structure sheafs, ordered by decreasing Q-score.
    // These sheafs will be used as seeds for growing clusters. 
    rc = makeCrossAlignments();

    if (measure_cpu)  {
      ct1     = clock();
      clapse1 = mmdb::realtype(ct1-ct)/CLOCKS_PER_SEC;
      ct      = ct1;
    }

    if ((rc==GSMT_Ok) && ((mode & SHEAF_X)==0x0)) {
        
      sdata  = new SheafData(0,0);
      sdata0 = new SheafData(0,0);
    
      for (i=0;i<nStruct;i++)
        smask[i] = true;

      Qmax = sheafData[0]->Q;
      Qmin = sheafData[nSheafs-1]->Q;
      
      if (verbosity>=2)
        printf ( "\n\n ===== Making sheafs\n\n"
          " Qmax    = %.5f\n"
          " Qmin    = %.5f\n"
          " Qthresh = %.5f\n"
          "\n"
          " Sheaf  Qsheaf  Size  Structure  Q-score  r.m.s.d.  Nalign\n",
          Qmax,Qmin,Qthresh );

      // this loop goes over all sheaf seeds ranged by decreasing Q-score
      for (i=0;(i<nSheafs) && (rc==GSMT_Ok);i++)
        if (sheafData[i])  {

          for (j=0;j<sheafData[i]->nStruct;j++)
            smask[sheafData[i]->sId[j]] = false;
          
          do {

            if (mode & SHEAF_Chains)  {
              Qsheaf = 0.0;
              optKey = 1;
            } else if (Qthresh>=0)  {
              Qsheaf = Qthresh;
              optKey = 2;
            } else  {
            /*
              Q0 = mmdb::RMin(Qmax,sheafData[i]->Q);
//              Qsheaf = Qmin + (1.0+sqrt(5.0))*(Q0-Qmin)/(3.0+sqrt(5.0));
              Qsheaf = Qmin + 2.0*(Q0-Qmin)/(3.0+sqrt(5.0));
//              Qsheaf = Qmin + (Q0-Qmin)/2.0;
            */
              Qsheaf = sheafData[i]->nStruct;
              Qsheaf = Qsheaf*Qsheaf*sheafData[i]->Q;
              optKey = 3;
            }

            Q0   = Qsheaf;
            done = true;
            for (j=0;(j<nStruct) && (rc==GSMT_Ok);j++)
              if (smask[j])  {
                sdata->copyFrom ( sheafData[i] );
                rc = addStructure ( sdata,j );
                if (rc==GSMT_Ok)  {
                  if (optKey<3)
                    Q1 = sdata->Q;
                  else  {
                    Q1 = sdata->nStruct;
                    Q1 = Q1*Q1*sdata->Q;
                  }
                  if (Q1>Q0)  {
                    Q0 = Q1;
                    // swap pointers to avoid extra copying
                    sdata1 = sdata0;
                    sdata0 = sdata;
                    sdata  = sdata1;
                    done   = false;
                  }
                }
              }

            if (!done)  {
              // sheaf has grown
              // swap pointers to avoid extra copying
              sdata1       = sheafData[i];
              sheafData[i] = sdata0;
              sdata0       = sdata1;
              // mask out the new structure in the sheaf
              smask[sheafData[i]->sId[sheafData[i]->nStruct-1]] = false;
            }

            if (verbosity>=2)  {
              sdata1 = sheafData[i];
              if (!done)  strcpy ( sign,"+" );
                    else  strcpy ( sign,"*" );
              printf ( " %3i %9.4f %4i %7s%1s %11.4f %8.4f %6i\n",
                       i+1,Qsheaf, sdata1->nStruct,
                       S[sdata1->sId[sdata1->nStruct-1]]->getRefName(),
                       sign,sdata1->Q,sdata1->rmsd,sdata1->Nalign );
            }

            
          } while ((!done) && (rc==GSMT_Ok));
          
          // remove consumed seeds 
          for (j=i+1;j<nSheafs;j++)
            if (sheafData[i]->hasStructure(sheafData[j]))  {
              delete sheafData[j];
              sheafData[j] = NULL;
            }
            
        }

      // trim the sheaf list
      j = 0;
      for (i=0;i<nSheafs;i++)
        if (sheafData[i])  {
          if (i>j)  {
            sheafData[j] = sheafData[i];
            sheafData[i] = NULL;
          }
          j++;
        }

      // make single-chain sheafs if needed
      for (i=0;i<nStruct;i++)
        if (smask[i])  {
          if (!sheafData[j])
                sheafData[j] = new SheafData(0,0);
          else  sheafData[j]->clear();
          sheafData[j]->addStructure ( S[i],i );
          sheafData[j]->Q      = 0.0;
          sheafData[j]->rmsd   = 0.0;
          sheafData[j]->Nalign = 0;
          sheafData[j]->nAtoms = nAtoms;
          if (verbosity>=2)  {
            sdata1 = sheafData[j];
            printf ( " %3i %9.4f %4i %7s* %11.4f %8.4f %6i\n",
                      j+1,Qsheaf, sdata1->nStruct,
                      S[sdata1->sId[sdata1->nStruct-1]]->getRefName(),
                      sdata1->Q,sdata1->rmsd,sdata1->Nalign );
          } 
          j++;
        }

      nSheafs = j;
      
      // sort sheafs
      for (i=0;i<nSheafs;i++)
        for (j=i+1;j<nSheafs;j++)
          if (sheafData[j]->Q>sheafData[i]->Q)  {
            sdata1       = sheafData[i];
            sheafData[i] = sheafData[j];
            sheafData[j] = sdata1;
          }

      delete sdata;
      delete sdata0;
     
    }

  }

  if (measure_cpu)  {

    ct1     = clock();
    clapse2 = mmdb::realtype(ct1-ct)/CLOCKS_PER_SEC;

    if (verbosity>=0)
      printf ( "\n"
         " CPU stage 1 (cross-alignments):  %8.5f secs\n"
         " CPU stage 2 (making sheafs):     %8.5f secs\n\n",
         clapse1,clapse2 );

  }

  return rc;

}


gsmt::PSheafData gsmt::Sheaf::getSheafData ( int index )  {
  if (!sheafData)  return NULL;
  if ((index<0) || (index>=allocNSheafs))  return NULL;
  return sheafData[index];
}

void gsmt::Sheaf::takeSheafData ( PPSheafData & sdata, int & nsdata )  {
  sdata     = sheafData;
  nsdata    = nSheafs;
  sheafData = NULL;
  nSheafs   = 0;
}


gsmt::GSMT_CODE gsmt::Sheaf::sheaf2 ( PSheafData sdata )  {
// special case of 2 structures
mmdb::PPAtom   a1,a2;
mmdb::realtype d2,Q1,R02,rmsd, Q, x,y,z;
mmdb::mat44    T1;
SUP_RC         supRC;
GSMT_CODE      rc;
int            i,i1,iter,nat1,nat2,nat,alen, done;

  sdata->S[0]->getCalphas ( a1,i );
  sdata->S[1]->getCalphas ( a2,i );

  if (useOcc)  {
    nat1 = -1;
    nat2 = -1;
    for (i=0;i<nAtoms;i++)  {
      sdata->mask[i] = (a1[i]->occupancy>0.0) && (a2[i]->occupancy>0.0);
      if (sdata->mask[i])  {
        if (nat1<0)  nat1 = i;
        nat2 = i;
      }
    }
    nat2++;
  } else  {
    nat1 = 0;
    nat2 = nAtoms;
    for (i=0;i<nAtoms;i++)
      sdata->mask[i] = true;
  }
  
  nat = nat2-nat1;
  if (nat<3)  {
    sdata->Q      = -1.0;
    sdata->Nalign =  0;
    sdata->rmsd   = -4.0;
    return GSMT_NoMinimalMatch;
  }

  iter = 0;

  if (mode & SHEAF_Atoms)  {

    mmdb::Mat4Init ( sdata->T[0] );

    switch (superpose(a1,a2,sdata->mask,nat,sdata->T[1]))  {

      case SUP_Ok :  rmsd = 0.0;
                     alen = 0;
                     for (i=nat1;i<nat2;i++)
                       if (sdata->mask[i])  {
                         alen++;
                         rmsd += a1[i]->GetDist2 ( a2[i],sdata->T[1] );
                       }
                     rmsd /= alen;

                     sdata->Nalign = alen;
                     sdata->Q      = 1.0/(1.0+rmsd/(R0*R0));
                     sdata->rmsd   = rmsd;

                     rc = GSMT_Ok;

                  break;

      case SUP_Fail : rc = GSMT_SVDFail;
                      sdata->Q      = -1.0;
                      sdata->Nalign =  0;
                      sdata->rmsd   = -1.0;
                  break;

      case SUP_Empty: rc = GSMT_NoMinimalMatch;
                      sdata->Q      = -1.0;
                      sdata->Nalign =  0;
                      sdata->rmsd   = -2.0;
                  break;

      default:        rc = GSMT_UnknownSupError;
                      sdata->Q      = -1.0;
                      sdata->Nalign =  0;
                      sdata->rmsd   = -3.0;

    }

  } else  {
  
    for (i=0;i<nat;i++)
      mask[i] = sdata->mask[i];
    
    mmdb::Mat4Init ( sdata->T[0] );
    mmdb::Mat4Init ( sdata->T[1] );
  
    sdata->Q      = -1.0;
    sdata->rmsd   = -1.0;
    sdata->Nalign = 0;
  
    R02  = R0*R0;
    done = 0;
    rc   = GSMT_Ok;
    
    while ((!done) && (rc==GSMT_Ok) && (iter<iterMax))  {
    
      iter++;
  
      for (i=0;i<nat;i++)
        mask0[i] = mask[i];
  
      supRC = superpose ( a1,a2,mask,nat,T1 );
      
      if (supRC==SUP_Ok)  {
      
        for (i=0;i<nat;i++)  {
          mask[i] = true;
          ix  [i] = i;
          var2[i] = a1[i]->GetDist2 ( a2[i],T1 ); 
        }
    
        QSP.Sort ( ix,var2,nat );
    
        d2   = 0.0; // square distance between the structures
                    //    at current rotation
        rmsd = 0.0;
        alen = 0;
        Q    = -1.0;
        Q1   = 0.0;
        for (i=0;(i<nat) && (Q1>=Q);i++)  {
          d2 += var2[i];
          i1  = i+1;
          Q1  = i1*i1/(1.0+d2/(i1*R02));
          if (Q1>=Q)  {
            Q    = Q1;
            rmsd = d2;
            alen = i1;
          }
        }
    
        //  Finalize scores
    
        Q   /= (nat*nat);
        rmsd = sqrt(rmsd/alen);
    
        //  Unmap unwanted contacts
    
        for (i=alen;i<nat;i++)
          mask[ix[i]] = false;
    
        done = 1;
        for (i=0;(i<nat) && done;i++)
          if (mask[i]!=mask0[i])
            done = 0;
    
        if (Q>sdata->Q)  {
          sdata->Q      = Q;
          sdata->rmsd   = rmsd;
          sdata->Nalign = alen;
          for (i=0;i<nat;i++)
            sdata->mask[i] = mask[i];
          mmdb::Mat4Copy ( T1,sdata->T[1] );
        }
        
      } else if (supRC==SUP_Fail)
        rc = GSMT_SVDFail;

      else if (supRC==SUP_Empty)
        rc = GSMT_NoMinimalMatch;
      
      else rc = GSMT_UnknownSupError;
 
    }

  }

  sdata->rmsd = sqrt(sdata->rmsd);
  for (i=0;i<nat;i++)  {
    a2[i]->TransformCopy ( sdata->T[1],x,y,z );
    sdata->cx[i] = 0.5*(a1[i]->x+x);
    sdata->cy[i] = 0.5*(a1[i]->y+y);
    sdata->cz[i] = 0.5*(a1[i]->z+z);
    x -= a1[i]->x;
    y -= a1[i]->y;
    z -= a1[i]->z;
    sdata->var2[i] = 0.25*(x*x + y*y + z*z);
  }
  
  if ((iter>=iterMax) && (rc==GSMT_Ok))
    rc = GSMT_IterLimit;

  return rc;
            
}

gsmt::GSMT_CODE gsmt::Sheaf::makeCrossAlignments() {
SortSheafData SSD;
int           k;
GSMT_CODE    rc,rc1;

  rc = GSMT_NoStructures;
  k  = 0;
  
  for (int i=0;i<nStruct;i++)
    for (int j=i+1;j<nStruct;j++)  {
      sheafData[k]->sId[0] = i;
      sheafData[k]->sId[1] = j;
      sheafData[k]->S  [0] = S[i];
      sheafData[k]->S  [1] = S[j];
      rc1 = sheaf2 ( sheafData[k] );
      if (rc1!=GSMT_Ok)
           sheafData[k]->Q = -1.0;
      else rc = GSMT_Ok;
      k++;
    }
  
  if (k>1)  {
  
    SSD.Sort ( sheafData,k );

    if (verbosity>=2)  {
      printf ( " ===== Initial cross-alignments\n\n"
               "  Sheaf  Structures  Q-score  r.m.s.d.  Nalign\n");
      for (int i=0;i<k;i++)
        printf ( " %5i  %6s %4s   %6.4f   %6.4f   %4i\n",
                 i+1,S[sheafData[i]->sId[0]]->getRefName(),
                 S[sheafData[i]->sId[1]]->getRefName(),
                 sheafData[i]->Q,sheafData[i]->rmsd,
                 sheafData[i]->Nalign );
    }

  }
    
  return rc;

}

gsmt::GSMT_CODE gsmt::Sheaf::addStructure ( PSheafData sdata,
                                            int        structNo )  {
mmdb::realtype  d2,Q1,R02,rmsd, Q;
int             i,j,nat,nst,iter,iter1,i1,alen, done;
SUP_RC          supRC;
GSMT_CODE       rc;

  sdata->addStructure ( S[structNo],structNo );
  nst = sdata->nStruct;
  for (j=0;j<nst;j++)  {
    sdata->S[j]->getCalphas ( atm[j],nat );
    mmdb::Mat4Copy ( sdata->T[j],RT[j] );
  }
  nat = sdata->nAtoms;
  
  for (i=0;i<nat;i++)  {
    mask[i] = sdata->mask[i];
    cx[i]   = sdata->cx[i];
    cy[i]   = sdata->cy[i];
    cz[i]   = sdata->cz[i];
  }

  sdata->Q = -1.0;
  
  R02   = R0*R0; //*nst/2.0;
  done  = 0;
  rc    = GSMT_Ok;
  iter  = 0;
  iter1 = 0;
    
  while ((!done) && (rc==GSMT_Ok) && (iter<iterMax))  {
    
    iter++;
  
    for (i=0;i<nat;i++)
      mask0[i] = mask[i];
  
    if (iter<=1)
      supRC = superpose ( cx,cy,cz,atm[nst-1],mask,nat,RT[nst-1] );
    else
      for (j=0;(j<nst) && (supRC==SUP_Ok);j++)
        supRC = superpose ( cx,cy,cz,atm[j],mask,nat,RT[j] );
      
    if (supRC==SUP_Ok)  {

      for (i=0;i<nat;i++)  {
        mask[i] = true;
        ix  [i] = i;
        cx  [i] = 0.0;
        cy  [i] = 0.0;
        cz  [i] = 0.0;
        for (j=0;j<nst;j++)  {
          ax[j] = atm[j][i]->x;
          ay[j] = atm[j][i]->y;
          az[j] = atm[j][i]->z;
          atm[j][i]->Transform ( RT[j] );
          cx[i] += atm[j][i]->x;
          cy[i] += atm[j][i]->y;
          cz[i] += atm[j][i]->z;
        }
        cx[i]  /= nst;
        cy[i]  /= nst;
        cz[i]  /= nst;
        var2[i] = 0.0;
        for (j=0;j<nst;j++)  {
          var2[i] += atm[j][i]->GetDist2 ( cx[i],cy[i],cz[i] );
          atm[j][i]->x = ax[j];
          atm[j][i]->y = ay[j];
          atm[j][i]->z = az[j];
        }
        var2[i] *= 2.0/nst; 
      }

      if (mode & SHEAF_Atoms)  {
      
        rmsd = 0.0;
        for (i=0;i<nat;i++)
          rmsd += var2[i];
          
        alen  = nat;
        rmsd /= nat;
        Q     = 1.0/(1.0+rmsd/R02);

        if (Q>sdata->Q)  done = 0;
                   else  done = 1;

      } else  {

        QSP.Sort ( ix,var2,nat );
      
        d2   = 0.0; // square distance between the structures
                    //    at current rotation
        rmsd = 0.0;
        alen = 0;
        Q    = -1.0;
        Q1   = 0.0;
        for (i=0;(i<nat) && (Q1>=Q);i++)  {
          d2 += var2[i];
          i1  = i+1;
          Q1  = i1*i1/(1.0+d2/(i1*R02));
          if (Q1>=Q)  {
            Q    = Q1;
            rmsd = d2;
            alen = i1;
          }
        }

        //  Finalize scores
      
        Q   /= (nat*nat);
        rmsd = sqrt(rmsd/alen);
  
        //  Unmap unwanted contacts
      
        for (i=alen;i<nat;i++)
          mask[ix[i]] = false;
      
        done = 1;
        for (i=0;(i<nat) && done;i++)
          if (mask[i]!=mask0[i])
            done = 0;

      }    

      if (Q>sdata->Q)  {
        sdata->Q      = Q;
        sdata->rmsd   = rmsd;
        sdata->Nalign = alen;
        for (i=0;i<nat;i++)  {
          sdata->mask[i] = mask[i];
          sdata->cx  [i] = cx  [i];
          sdata->cy  [i] = cy  [i];
          sdata->cz  [i] = cz  [i];
          sdata->var2[ix[i]] = var2[i];
        }
        for (j=0;j<nst;j++)
          mmdb::Mat4Copy ( RT[j],sdata->T[j] );
        iter1 = 0;
      } else  {
        iter1++;
        if (iter1>10)
          done = 1;
      }

    } else if (supRC==SUP_Fail)  rc = GSMT_SVDFail;
    else if (supRC==SUP_Empty)   rc = GSMT_NoMinimalMatch;
                          else   rc = GSMT_UnknownSupError;
 
  }

  sdata->rmsd = sqrt(sdata->rmsd);
  
  if ((iter>=iterMax) && (rc==GSMT_Ok))
    rc = GSMT_IterLimit;

  return rc;

}

