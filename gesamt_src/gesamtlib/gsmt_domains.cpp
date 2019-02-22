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
//    26.01.17   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  GSMT_Domains <interface>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::Domains
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2008-2017
//
// =================================================================
//

#include <string.h>
#include <stdlib.h>

#include "rvapi/rvapi_interface.h"

#include "gsmt_domains.h"
#include "gsmt_output.h"
#include "gsmt_utils.h"

// =================================================================

gsmt::Domains::Domains() : Aligner()  {
  initDomains();
}

gsmt::Domains::Domains ( mmdb::io::RPStream Object ) : Aligner(Object) {
  initDomains();
}

gsmt::Domains::~Domains()  {
  freeMemory();
}

void gsmt::Domains::initDomains()  {
  
  outDir      = NULL;   // path to output directory + "/"
  mmdb::CreateCopy ( outDir,"./gesamt_domains/" );
  
  minDomainSize = 20;   // allowed size in residues
  maxTrimSize   = 3;    // allowed group size in residues
  maxGapSize    = 1;    // allowed gap size in residues

  R0factor      = 0.9;  // R0 decrease factor <1
  minR0         = 0.05; // minimum R0 to try
  
  DSD           = NULL; // domain alignments
  nDomains      = 0;    // number of identified domains
  domData       = NULL; // final domain definitions

  asn1          = NULL; // serial atom numbers in 1st structure
  asn2          = NULL; // serial atom numbers in 2nd structure
  natoms01      = 0;    // number of atoms in full 1st structure
  natoms02      = 0;    // number of atoms in full 2nd structure

}


void gsmt::Domains::freeMemory()  {

  if (outDir)  {
    delete[] outDir;
    outDir = NULL;
  }

  freeDomains();

}


void gsmt::Domains::freeDomains()  {

  if (DSD)  {
    for (int i=0;i<nDomains;i++)
      if (DSD[i])
        delete DSD[i];
    delete[] DSD;
    DSD = NULL;
  }
  nDomains = 0;

  if (domData)  {
    delete domData;
    domData = NULL;
  }

  mmdb::FreeVectorMemory ( asn1,0 );
  mmdb::FreeVectorMemory ( asn1,0 );
  natoms01 = 0;
  natoms02 = 0;

}


void gsmt::Domains::setOutputDir ( mmdb::cpstr outputDir )  {
int n = strlen(outputDir)-1;
  if (outputDir[n]!='/')  {
    if (outputDir[n]=='\\')
         mmdb::CreateCopy   ( outDir,outputDir );
    else mmdb::CreateCopCat ( outDir,outputDir,mmdb::io::_dir_sep );
  } else
    mmdb::CreateCopy ( outDir,outputDir );
}


gsmt::GSMT_CODE gsmt::Domains::findDomains ( PStructure s1,
                                             PStructure s2 )  {
mmdb::PPAtom atoms;
PStructure   S[2];
int          i,natoms;
GSMT_CODE    rc;

  mkdirp ( outDir );

  freeDomains();
  
  do  {
    rc = findDomain ( s1,s2 );
  } while (rc==GSMT_Ok);
  
  if ((rc==GSMT_NoDomainFound) ||
      ((nDomains>0) && (rc==GSMT_NoAlignmentAchieved)))
    rc = GSMT_Ok;

  if (!domData)
    domData = new DomData();

  S[0] = s1;
  S[1] = s2;
  domData->setData ( S,DSD,this,2,nDomains );

  if (asn1)  {
    s1->selectSubStructure(NULL);
    s1->getCalphas ( atoms,natoms );
    for (i=0;i<natoms;i++)
      atoms[i]->serNum = asn1[i];
  }
  if (asn2)  {
    s2->selectSubStructure(NULL);
    s2->getCalphas ( atoms,natoms );
    for (i=0;i<natoms;i++)
      atoms[i]->serNum = asn2[i];
  }

  return rc;

}


void gsmt::Domains::trimAlignment ( mmdb::ivector c1,
                                    mmdb::ivector c2 )  {
int  i1,i2,j1,j2,k1,k2, n1,n2,  g1,n,g2;
int  s11,s12, s21,s22, s;
bool done;

  do  {
  
    done = true;
    
    i1 = 0;               // counts atoms in 1st structure
    i2 = 0;               // counts atoms in 2nd structure 
    g1 = maxGapSize + 1;  // fictitious l.h.s gap
    while ((i1<natoms1) && (c1[i1]<0))  // skip all gap in the beginning
      i1++;

    if (i1<natoms1)  {  // group in 1str structure found 

      i2 = c1[i1];      // start of the group in 2nd structure

      while ((i1<natoms1) && (i2<natoms2))  {
      
        j1  = i1;  // beginning of group in 1st structure
        k1  = i1;
        s11 = A1[i1]->serNum;
        s12 = s11;
        while (i1<natoms1)  {
          s = A1[i1]->serNum;
          if (s-s12>maxGapSize+1)
            break;
          else if (c1[i1]>=0)  {
            s12 = s;
            k1  = i1;
          }
          i1++;
        }
        n1  = s12 - s11 + 1;   // size of group in 1st structure
      
        j2  = i2;  // beginning of group in 2nd structure
        k2  = i2;
        s21 = A2[i2]->serNum;
        s22 = s21;
        while (i2<natoms2)  {
          s = A2[i2]->serNum;
          if (s-s22>maxGapSize+1)
            break;
          else if (c2[i2]>=0)  {
            s22 = s;
            k2  = i2;
          }
          i2++;
        }
        n2 = s21 - s22 + 1;   // size of group in 2nd structure
      
        i1 = k1 + 1;
        i2 = k2 + 1;

        if (n1>=n2)  {
          i2 = c1[i1-1] + 1;  // end of common group in 2nd structure
          n  = n1;            // common group size
        } else  {
          i1 = c2[i2-1] + 1;  // end of common group in 1st structure
          n  = n2;            // common group size
        }

        s11 = s12 + 1;
        k1  = i1;  // beginning of r.h.s gap in 1st group
        while ((i1<natoms1) && (c1[i1]<0))  // count over the r.h.s. gap
          i1++;
        s21 = s22 + 1;
        k2  = i2;  // beginning of r.h.s gap in 2nd group
        while ((i2<natoms2) && (c2[i2]<0))  // count over the r.h.s. gap
          i2++;
          
        if ((i1>=natoms1) || (i2>=natoms2))
              g2 += maxGapSize + 1;  // fictitious r.h.s. gap
        else  g2  = mmdb::IMin ( A1[i1]->serNum-s11,A2[i2]->serNum-s21 );
                                     // common r.h.s. gap size
          
        if ((g1>maxGapSize) && (n<=maxTrimSize) && (g2>maxGapSize))  {
          // trim (remove) the group
          
          while (j1<k1)
            c1[j1++] = -1;
          while (j2<k2)
            c2[j2++] = -1;
    
          g1 += n + g2;  // new l.h.s. gap size

          done = false;  // anothre iteration is required
    
        } else
         g1 = g2;  // r.h.s. gap becomes l.h.s. gap for next iteration

      }

    } 

  } while (!done);

}


void gsmt::Domains::addDomain ( PSuperposition & SD, bool copy )  {
PPSuperposition DSD1;

  DSD1 = new PSuperposition[nDomains+1];
  for (int i=0;i<nDomains;i++)
    DSD1[i] = DSD[i];
  
  if (DSD)
    delete[] DSD;
  DSD = DSD1;

  if (copy)  {
    DSD[nDomains] = new Superposition();
    DSD[nDomains++]->CopyFrom ( SD );
  } else  {
    DSD[nDomains++] = SD;
    SD = NULL;
  }

}

/*
#define  _fast_
#undef   _fast_

#ifdef _fast_

gsmt::GSMT_CODE gsmt::Domains::findDomain ( PStructure s1,
                                            PStructure s2 )  {
PSuperposition  SD;
mmdb::ivector   c1,c2;
mmdb::realtype  rmsd,rmsd0,rmsd1,Q,Q0,dist2,d,eps;
//mmdb::realtype  median,median0;
mmdb::realtype  skew,skew0,skew1,rsigma;
mmdb::mat44     T,T0;
int             matchNo,i,j,i1,i2,j1,j2,ncont,alen,alen0,noImpCnt;
int             iter,Done;
GSMT_CODE       rc;

  if ((s1->getNCalphas()<minDomainSize) ||
      (s2->getNCalphas()<minDomainSize))
    return gsmt::GSMT_NoDomainFound;

  setSimilarityThresholds ( 0.0,0.0 );

  eps = mmdb::RMax(1.0e-12,100.0*mmdb::MachEps);

  // 1. Produce an ordinary alignment

  rc = Align ( s1,s2,false );

  if (rc!=GSMT_Ok)
    return rc;

  getBestMatch ( SD,matchNo );

  if (!SD)
    return gsmt::GSMT_NoAlignmentAchieved;
    
  if (SD->Nalgn<minDomainSize)
    return gsmt::GSMT_NoDomainFound;

  if (nDomains==0)  {
  
    addDomain ( SD,true );  // 0th "domain" is full alignment

    mmdb::GetVectorMemory ( asn1,natoms1,0 );
    mmdb::GetVectorMemory ( asn2,natoms2,0 );

    for (i1=0;i1<natoms1;i1++)  {
      asn1[i1] = A1[i1]->serNum;
      A1  [i1]->serNum = i1;
    }

    for (i2=0;i2<natoms2;i2++)  {
      asn2[i2] = A2[i2]->serNum;
      A2  [i2]->serNum = i2;
    }

    natoms01 = natoms1;
    natoms02 = natoms2;

  }

  // 2. Select domain by decreasing the Q-score's R0 parameter in
  //    simlutaed annealing fashion.

  getRefMemory();

  // 2.1 Restore best superposition state

  mmdb::Mat4Copy ( SD->T,T );
  for (i1=0;i1<natoms1;i1++)  c10[i1] = SD->c1[i1];
  for (i2=0;i2<natoms2;i2++)  c20[i2] = SD->c2[i2];

  // 2.2 Start the "simulated annealing" loop

  Q0       = -1.0;
  rmsd0    = -1.0;
  skew0    =  mmdb::MaxReal;
  rmsd     =  2.0*SD->rmsd;
  alen0    =  0;
  alen     =  0;
  Q        =  0.0;

  SD     = NULL;

  noImpCnt =  5;
  iter     =  0;

  do  {  // refinement loop

    Done = 1;   // termination signal

    //  2.2.1  Apply transformation matrix

    for (i2=0;i2<natoms2;i2++)
      A2[i2]->TransformCopy ( T,xyz2[i2] );
  
    //  2.2.2  Make 3D alignment of the coordinates

    ncont = 0;
    M1->SeekContacts ( xyz2,natoms2,maxContact,contact,ncont );
    DA.Align         ( natoms1,natoms2, contact,ncont, QR0,2.0*rmsd );
    DA.getAlignment  ( c1,c2 );
  
    //  2.2.3  Identify close 3D matches and calculate alignment scores
    //         and mass centers for new superposition

    //  2.2.3.1  Identify subset of matches with maximal Q-score

    ncont = 0;    // total number of corresponding atoms
    for (i1=0;i1<natoms1;i1++)  {
      i2 = c1[i1];
      if (i2>=0)  {
        cdist[ncont] = A1[i1]->GetDist2 ( xyz2[i2] );
        cix  [ncont] = i1;
        ncont++;
      }
    }
  
    //  2.2.3.2  Unmap atoms for optimizing the quality function

    if (ncont<1)  {

      rmsd   = 0.0;
      Q      = 0.0;
      skew   = 0.0;

    } else if (ncont<=3)  {

      rmsd = 0.0;
      for (i=0;i<ncont;i++)
        rmsd += cdist[i];
      alen   = ncont;
      Q      = alen*alen/(1.0+rmsd/(alen*QR0*QR0))/(natoms1*natoms2);
      rmsd   = sqrt(rmsd/alen);
      skew   = 0.0;  // not measurable

    } else  {

      QSP.Sort ( cix,cdist,ncont );

      dist2 = 0.0; // square distance between the structures
                   // at current rotation

      skew   = mmdb::MaxReal;
      rmsd   = 0.0;

      for (i=0;i<ncont;i++)  {

        dist2 += cdist[i];

        if (i>3)  {
          i1     = i+1;
          rmsd1  = sqrt(dist2/i1);
          skew1  = 0.0;
          rsigma = 0.0;
          for (j=0;j<=i;j++)  {
            d       = rmsd1 - sqrt(cdist[j]);
            skew1  += d*d*d;
            rsigma += d*d;        
          }
          rsigma = sqrt(rsigma);
          if (rsigma<eps)
            skew1  = mmdb::MaxReal;
          else  {
            skew1 /= rsigma*rsigma*rsigma;
            if (fabs(skew1)<=fabs(skew))  {
              skew = skew1;
              rmsd = rmsd1;
              alen = i1;
            }
          }
        }

      }
      
      //  Unmap unwanted contacts

      for (i=alen;i<ncont;i++)  {
        i1 = cix[i];
        i2 = c1[i1];
        c1[i1] = -1;
        c2[i2] = -1;
      }
      
      trimAlignment ( c1,c2 );

      //  Finalize scores
      
      rmsd = 0.0;
      i1   = 0;
      for (i=0;i<alen;i++)
        if (c1[cix[i]]>=0)  {
          rmsd += cdist[i];
          i1++;
        }

      rmsd  /= i1;
      Q      = i1*i1/(natoms1*natoms2*(1.0+rmsd/(QR0*QR0)));
      rmsd   = sqrt(rmsd);

      skew   = 0.0;
      rsigma = 0.0;
      for (i=0;i<alen;i++)
        if (c1[cix[i]]>=0)  {
          d       = rmsd - sqrt(cdist[i]);
          skew   += d*d*d;
          rsigma += d*d;
        }
      rsigma = sqrt(rsigma);
      if (rsigma<eps)
           skew  = mmdb::MaxReal;
      else skew /= rsigma*rsigma*rsigma;

      alen  = i1;

    }

    //  2.2.3.3  Note score improvement

//    if (Q-Q0>eps)  {
    if (fabs(skew)<=fabs(skew0))  {
      alen0   = alen;
      rmsd0   = rmsd;
      Q0      = Q;
      skew0   = skew;
      for (i=0;i<natoms1;i++)
        c10[i] = c1[i];
      mmdb::Mat4Copy ( T,T0 );
      noImpCnt = mmdb::IMax ( 5,iter/2 );
    } else
      noImpCnt--;

    iter++;

    if (noImpCnt>0)  Done = 0;

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
  
  if ((Done>=0) && (alen0>=minDomainSize))  {

    if (!SD)  SD = new Superposition();
    SD->SetSuperposition ( T0,rmsd0,0.0,Q0,alen0,c10,cdist,natoms1,
                           c20,natoms2 );
  
//printf ( " alen0=%i  skew0=%10.8g  Q0=%10.5f\n",alen0,skew0,Q0 );  
  
    c1 = SD->c1;
    c2 = SD->c2;
    mmdb::FreeVectorMemory ( SD->dist1,0 );
    mmdb::GetVectorMemory  ( SD->c1   ,natoms01,0 );
    mmdb::GetVectorMemory  ( SD->dist1,natoms01,0 );
    mmdb::GetVectorMemory  ( SD->c2   ,natoms02,0 );
    
    for (i1=0;i1<natoms01;i1++)  {
      SD->c1   [i1] = -1;
      SD->dist1[i1] = -1.0;
    }
    for (i2=0;i2<natoms02;i2++)
      SD->c2[i2] = -1;

    for (i2=0;i2<natoms2;i2++)
      c2[i2] = -1;

    SD->seqId = 0.0;
    
    for (i1=0;i1<natoms1;i1++)  {
      i2 = c1[i1];
      if (i2>=0)  {
        c2[i2]        = i1;
        j1            = A1[i1]->serNum;
        j2            = A2[i2]->serNum;
        SD->c2[j2]    = j1;
        SD->c1[j1]    = j2;
        SD->dist1[j1] = sqrt ( A1[i1]->GetDist2(A2[i2],SD->T) );
        if (!strcmp(A1[i1]->GetResName(),A2[i2]->GetResName()))
          SD->seqId += 1.0;
        s1->selectAtom ( A1[i1],mmdb::SKEY_CLR );
        s2->selectAtom ( A2[i2],mmdb::SKEY_CLR );
      }
    }
    SD->seqId /= SD->Nalgn;
    
    SD->natoms1 = natoms01;
    SD->natoms2 = natoms02;
    
    s1->finishAtomSelection();
    s2->finishAtomSelection();

    addDomain ( SD,false );

  } else
    rc = GSMT_NoDomainFound;

  return rc;

}

#else
*/


gsmt::GSMT_CODE gsmt::Domains::findDomain ( PStructure s1,
                                            PStructure s2 )  {
PSuperposition  SD;
mmdb::ivector   c1,c2;
mmdb::realtype  R0,rmsd,rmsd0,Q,Q0,Q1,dist2,eps;
mmdb::realtype  median,median0, skew,skew0,skew1,rsigma;
mmdb::mat44     T,T0;
int             matchNo,nR0,i,i1,i2,j1,j2,ncont,alen,alen0,noImpCnt;
int             iter,Done;
bool            zcross;
GSMT_CODE       rc;

  if ((s1->getNCalphas()<minDomainSize) ||
      (s2->getNCalphas()<minDomainSize))
    return gsmt::GSMT_NoDomainFound;

  setSimilarityThresholds ( 0.0,0.0 );

  eps = mmdb::RMax(1.0e-12,100.0*mmdb::MachEps);

  // 1. Produce an ordinary alignment

  rc = Align ( s1,s2,false );

  if (rc!=GSMT_Ok)
    return rc;

  getBestMatch ( SD,matchNo );

  if (!SD)
    return gsmt::GSMT_NoAlignmentAchieved;
    
  if (nDomains==0)  {
  
    addDomain ( SD,true );  // 0th "domain" is full alignment

    mmdb::GetVectorMemory ( asn1,natoms1,0 );
    mmdb::GetVectorMemory ( asn2,natoms2,0 );

    for (i1=0;i1<natoms1;i1++)  {
      asn1[i1] = A1[i1]->serNum;
      A1  [i1]->serNum = i1;
    }

    for (i2=0;i2<natoms2;i2++)  {
      asn2[i2] = A2[i2]->serNum;
      A2  [i2]->serNum = i2;
    }

    natoms01 = natoms1;
    natoms02 = natoms2;

  }

  if (SD->Nalgn<minDomainSize)
    return gsmt::GSMT_NoDomainFound;

  rmsd = SD->rmsd;

  // 2. Select domain by decreasing the Q-score's R0 parameter in
  //    simlutaed annealing fashion.

  getRefMemory();

  // 2.1 Restore best superposition state

  mmdb::Mat4Copy ( SD->T,T );
  for (i1=0;i1<natoms1;i1++)  c10[i1] = SD->c1[i1];
  for (i2=0;i2<natoms2;i2++)  c20[i2] = SD->c2[i2];

  // 2.2 Start the "simulated annealing" loop

  R0  = QR0;  // the original Q-score's R0 parameter not to be changed

  if (verbosity>1)  {
    printf ( "    ##    QR0     Q-score    r.m.s.d.     Nalign\n\n" );
    printf ( " %5i  %6.2f  %10.6f %10.6f  %5i %6.2f %10.6f\n",
             0,R0,SD->Q,SD->rmsd,SD->Nalgn,0.0,0.0 );
  }
  
  skew1  = mmdb::MaxReal;
  SD     = NULL;
  zcross = false;

  nR0    = 0;
  while (R0>=minR0)  {
  
    nR0++;
    R0      *=  R0factor; // decrease the "simouated annealing" parameter
    Q0       = -1.0;
    rmsd0    = -1.0;
    alen0    =  0;
    median0  = -1.0;
    skew0    =  0.0;
    alen     = 0;
    Q        = 0.0;

    noImpCnt =  5;
    iter     =  0;

    do  {  // refinement loop for given value of QR0
    
      Done = 1;   // termination signal

      //  2.2.1  Apply transformation matrix

      for (i2=0;i2<natoms2;i2++)
        A2[i2]->TransformCopy ( T,xyz2[i2] );
    
      //  2.2.2  Make 3D alignment of the coordinates

      ncont = 0;
      M1->SeekContacts ( xyz2,natoms2,maxContact,contact,ncont );
      DA.Align         ( natoms1,natoms2, contact,ncont, R0,2.0*rmsd );
      DA.getAlignment  ( c1,c2 );
    
      //  2.2.3  Identify close 3D matches and calculate alignment scores
      //         and mass centers for new superposition

      //  2.2.3.1  Identify subset of matches with maximal Q-score

      ncont = 0;    // total number of corresponding atoms
      for (i1=0;i1<natoms1;i1++)  {
        i2 = c1[i1];
        if (i2>=0)  {
          cdist[ncont] = A1[i1]->GetDist2 ( xyz2[i2] );
          cix  [ncont] = i1;
          ncont++;
        }
      }
    
      //  2.2.3.2  Unmap atoms for optimizing the quality function

      if (ncont<1)  {

        rmsd   = 0.0;
        Q      = 0.0;
        median = 0.0;
        skew   = 0.0;

      } else if (ncont<=3)  {

        rmsd = 0.0;
        for (i=0;i<ncont;i++)
          rmsd += cdist[i];
        alen   = ncont;
        Q      = alen*alen/(1.0+rmsd/(alen*R0*R0))/(natoms1*natoms2);
        rmsd   = sqrt(rmsd/alen);
        median = sqrt(cdist[ncont/2]);
        skew   = 0.0;  // not measurable

      } else  {

        QSP.Sort ( cix,cdist,ncont );

        dist2 = 0.0; // square distance between the structures
                     // at current rotation
        Q  = -1.0;
        Q1 =  0.0;
        for (i=0;(i<ncont) && (Q1>=Q);i++)  {
          dist2 += cdist[i];
          i1     = i+1;
          Q1     = i1*i1/(1.0+dist2/(i1*R0*R0));
          if (Q1>=Q)  {
            Q    = Q1;
            rmsd = dist2;
            alen = i1;
          }
        }

        //  Unmap unwanted contacts

        for (i=alen;i<ncont;i++)  {
          i1 = cix[i];
          i2 = c1[i1];
          c1[i1] = -1;
          c2[i2] = -1;
        }
        
        trimAlignment ( c1,c2 );

        //  Finalize scores
        
        rmsd = 0.0;
        i1   = 0;
        for (i=0;i<alen;i++)
          if (c1[cix[i]]>=0)  {
            rmsd += cdist[i];
            i1++;
          }

        rmsd /= i1;
        Q     = i1*i1/(natoms1*natoms2*(1.0+rmsd/(R0*R0)));
        rmsd  = sqrt(rmsd);
          
        i2     = i1/2;
        skew   = 0.0;
        rsigma = 0.0;
        for (i=0;i<alen;i++)
          if (c1[cix[i]]>=0)  {
            dist2   = rmsd - sqrt(cdist[i]);
            skew   += dist2*dist2*dist2;
            rsigma += dist2*dist2;
            i2--;
            if (i2==0)
              median = sqrt(cdist[i]);
          }
        rsigma = sqrt(rsigma);
        if (rsigma<eps)
             skew  = mmdb::MaxReal;
        else skew /= rsigma*rsigma*rsigma;

        alen  = i1;

      }

      //  2.2.3.3  Note score improvement

      if (Q-Q0>eps)  {
        alen0   = alen;
        rmsd0   = rmsd;
        Q0      = Q;
        median0 = median;
        skew0   = skew;
        for (i=0;i<natoms1;i++)
          c10[i] = c1[i];
        mmdb::Mat4Copy ( T,T0 );
        noImpCnt = mmdb::IMax ( 5,iter/2 );
      } else
        noImpCnt--;

      iter++;

      if (noImpCnt>0)  Done = 0;

//    if (checkRTMatrix(T,clusterNo))  Done = -1;  // error

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
    
    if ((Done>=0) && (alen0>=minDomainSize) &&
        (((fabs(skew0)<fabs(skew1)) && (!zcross)) ||
         (skew0*skew1<=0.0)))  {
      if (!SD)  SD = new Superposition();
      SD->SetSuperposition ( T0,rmsd0,0.0,Q0,alen0,c10,cdist,natoms1,
                             c20,natoms2 );
      if (skew0*skew1<=0.0)
        zcross = true;
      skew1 = skew0;
    }
  
    if (verbosity>1)
      printf ( " %5i  %6.2f  %10.6f %10.6f  %5i %6.2f %10.6f\n",
               nR0,R0,Q0,rmsd0,alen0,median0,skew0 );
    
  }
  
  if (SD)  {

    c1 = SD->c1;
    c2 = SD->c2;
    mmdb::FreeVectorMemory ( SD->dist1,0 );
    mmdb::GetVectorMemory  ( SD->c1   ,natoms01,0 );
    mmdb::GetVectorMemory  ( SD->dist1,natoms01,0 );
    mmdb::GetVectorMemory  ( SD->c2   ,natoms02,0 );
    
    for (i1=0;i1<natoms01;i1++)  {
      SD->c1   [i1] = -1;
      SD->dist1[i1] = -1.0;
    }
    for (i2=0;i2<natoms02;i2++)
      SD->c2[i2] = -1;

    for (i2=0;i2<natoms2;i2++)
      c2[i2] = -1;

    SD->seqId = 0.0;
    
    for (i1=0;i1<natoms1;i1++)  {
      i2 = c1[i1];
      if (i2>=0)  {
        c2[i2]        = i1;
        j1            = A1[i1]->serNum;
        j2            = A2[i2]->serNum;
        SD->c2[j2]    = j1;
        SD->c1[j1]    = j2;
        SD->dist1[j1] = sqrt ( A1[i1]->GetDist2(A2[i2],SD->T) );
        if (!strcmp(A1[i1]->GetResName(),A2[i2]->GetResName()))
          SD->seqId += 1.0;
        s1->selectAtom ( A1[i1],mmdb::SKEY_CLR );
        s2->selectAtom ( A2[i2],mmdb::SKEY_CLR );
      }
    }
    SD->seqId /= SD->Nalgn;
    
    SD->natoms1 = natoms01;
    SD->natoms2 = natoms02;
    
    s1->finishAtomSelection();
    s2->finishAtomSelection();

//printf ( " alen0=%i  skew0=%10.8g  Q0=%10.5f\n",SD->Nalgn,skew0,SD->Q );  

    addDomain ( SD,false );

  } else
    rc = GSMT_NoDomainFound;

  return rc;

}

//#endif


void gsmt::Domains::R0scan ( PStructure s1, PStructure s2 )  {
PSuperposition SD;
mmdb::realtype saveQR0,dR0,Q0,r1;
int            matchNo, nSteps, n1;

  setSimilarityThresholds ( 0.0,0.0 );
  saveQR0 = QR0;

  printf ( "    ##    QR0     Q-score    r.m.s.d.      "
           "Drmsd     Nalign  Dnalign    Qeff\n\n" );

  nSteps = 200;
  dR0    = QR0/nSteps;
  r1     = 0.0;
  n1     = 0;
  for (int i=0;i<nSteps;i++)  {
    setQR0 ( saveQR0-i*dR0 );
    Align        ( s1,s2,false );
    getBestMatch ( SD,matchNo  );
    if (SD)  {
      Q0 = SD->rmsd/saveQR0;
      Q0 = s1->getNCalphas()*s2->getNCalphas()*(1.0+Q0*Q0);
      Q0 = SD->Nalgn*SD->Nalgn/Q0;
      Q0 = 1.0/(1.0+Q0*Q0);
      printf ( " %5i  %6.2f  %10.6f %10.6f  %12.8f  %5i  %6i  %10.6f\n",
               i+1,saveQR0-i*dR0,SD->Q,SD->rmsd,fabs(SD->rmsd-r1),
               SD->Nalgn,abs(SD->Nalgn-n1),Q0 );
      r1 = SD->rmsd;
      n1 = SD->Nalgn;
    }
  }
  
  QR0 = saveQR0;

}


void gsmt::Domains::resetStructureData ( PStructure s1, PStructure s2 ) {
  s1->selectSubStructure ( NULL );
  s2->selectSubStructure ( NULL );
  s1->getCalphas ( A1,natoms1 );
  s2->getCalphas ( A2,natoms2 );
}

void gsmt::Domains::writeDomAlignTable ( PStructure s1, PStructure s2,
                                         mmdb::io::RFile f )  {
mmdb::imatrix c;
mmdb::rvector var;
XAlignText    CXA;
PXTAlign      XTA;
int           nr,j;

  domData->getCombinedAlignment ( c,var );

  s1->selectSubStructure ( NULL );
  s2->selectSubStructure ( NULL );
  
  s1->getCalphas ( A1,natoms1 );
  s2->getCalphas ( A2,natoms2 );

  for (j=0;j<natoms1;j++)
    A1[j]->serNum = domData->ddef[0][j];
  for (j=0;j<natoms2;j++)
    A2[j]->serNum = domData->ddef[1][j];

  CXA.Align ( A1,c[0],domData->sSize[0],
              A2,c[1],domData->sSize[1],
              var,nr );
  f.LF();
  f.WriteLine ( ".-------------.--.------------.--.-------------." );
  f.WriteLine ( "|    FIXED    |DD|  Dist.(A)  |DD|   MOVING    |" );
  f.WriteLine ( "|-------------+--+------------+--+-------------|" );
  XTA = CXA.getTextRows();
  for (j=0;j<nr;j++)
    XTA[j].print ( f,2 );
  f.WriteLine ( "`-------------'--'------------'--'-------------'" );
  f.LF();
  f.WriteLine ( " Notations:" );
  f.WriteLine ( " DD    domain number" );
  f.WriteLine ( " S/H   residue belongs to a strand/helix" );
  f.WriteLine ( " +/-/. hydrophylic/hydrophobic/neutral residue" );
  f.WriteLine ( " **    identical residues matched: similarity 5" );
  f.WriteLine ( " ++    similarity 4" );
  f.WriteLine ( " ==    similarity 3" );
  f.WriteLine ( " --    similarity 2" );
  f.WriteLine ( " ::    similarity 1" );
  f.WriteLine ( " ..    dissimilar residues: similarity 0" );

  for (j=0;j<natoms1;j++)  A1[j]->serNum = asn1[j];
  for (j=0;j<natoms2;j++)  A2[j]->serNum = asn2[j];
  
  mmdb::FreeMatrixMemory ( c,domData->nStructures,0,0 );
  mmdb::FreeVectorMemory ( var,0 );

}


void gsmt::Domains::makeDomAlignTable_rvapi ( mmdb::cpstr tableId,
                                              PStructure  s1,
                                              PStructure  s2 )  {
// Puts pairwise residue alignment in the pre-existing table with
// given table Id
mmdb::imatrix c;
mmdb::rvector var;
XAlignText    CXA;
PXTAlign      XTA;
int           nr,j;

  domData->getCombinedAlignment ( c,var );

  s1->selectSubStructure ( NULL );
  s2->selectSubStructure ( NULL );
  
  s1->getCalphas ( A1,natoms1 );
  s2->getCalphas ( A2,natoms2 );

  for (j=0;j<natoms1;j++)
    A1[j]->serNum = domData->ddef[0][j];
  for (j=0;j<natoms2;j++)
    A2[j]->serNum = domData->ddef[1][j];

  CXA.Align ( A1,c[0],domData->sSize[0],
              A2,c[1],domData->sSize[1],
              var,nr );
  XTA = CXA.getTextRows();
  
  rvapi_put_horz_theader ( tableId,"FIXED","Residues of fixed molecule",
                           0 );
  rvapi_put_horz_theader ( tableId,"D","Fixed molecule domain id",
                           1 );
  rvapi_put_horz_theader ( tableId,"Dist. (&Aring;)",
                           "DIstance between aligned C-alpha atoms",2 );
  rvapi_put_horz_theader ( tableId,"D","Moving molecule domain id",
                           3 );
  rvapi_put_horz_theader ( tableId,"MOVING",
                           "Residues of moving (rotated-translated) "
                           "molecule",4 );
  for (j=0;j<nr;j++)
    XTA[j].put_rvapi ( tableId,j,true );

  for (j=0;j<natoms1;j++)  A1[j]->serNum = asn1[j];
  for (j=0;j<natoms2;j++)  A2[j]->serNum = asn2[j];
  
  mmdb::FreeMatrixMemory ( c,domData->nStructures,0,0 );
  mmdb::FreeVectorMemory ( var,0 );
  
}
