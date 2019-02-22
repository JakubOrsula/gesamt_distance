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
//    14.06.16   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  GSMT_PDBScanThread <implementation>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::PDBScanThread
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2008-2016
//
// =================================================================
//

#include <string.h>

#include "gsmt_pdbscanthread.h"

// =================================================================

gsmt::PDBScanThread::PDBScanThread() : ThreadBase()  {
  A        = NULL;
  s1       = NULL;
  index    = NULL;
  path0    = NULL;
  trimQ    = 0.0;
  trimSize = 0.0;
}

gsmt::PDBScanThread::~PDBScanThread()  {
  freeMemory();
}

void gsmt::PDBScanThread::freeMemory()  {
  if (A)  delete A;
  H.deleteHits();
  if (path0)  delete[] path0;
  A     = NULL;
  path0 = NULL;
}

void gsmt::PDBScanThread::initAligner (
                                  gsmt::PERFORMANCE_CODE performance,
                                  mmdb::realtype         minMatch1,
                                  mmdb::realtype         minMatch2,
                                  mmdb::realtype         trim_Q,
                                  mmdb::realtype         trim_size,
                                  mmdb::realtype         QR0,
                                  mmdb::realtype         sigma )  {
  A = new gsmt::Aligner();
  A->setPerformanceLevel     ( performance         );
  A->setSimilarityThresholds ( minMatch1,minMatch2 );
  A->setQR0                  ( QR0                 );
  A->setSigma                ( sigma               );
  trimQ    = trim_Q;
  trimSize = trim_size;
}

void gsmt::PDBScanThread::run()  {
gsmt::PSuperposition SD;
gsmt::PStructure     s2;
mmdb::PPAtom         atom;
mmdb::pstr           path,fn;
mmdb::pstr           logmsg,title;
char                 L[300];
int                  chainNo,natoms1,natoms2,minNAtoms1,minMatch;
int                  n,k,m,matchNo;
mmdb::ERROR_CODE     rc;

  H.deleteHits();
  H.dataKey = Hits::DATA_StructureScan;

  rc = mmdb::Error_NoError;
  s2 = new gsmt::Structure();
  SD = NULL;
  
  natoms1    = s1->getNCalphas();
  minNAtoms1 = mmdb::mround ( A->getMinMatch1()*natoms1 );
  
  n_jobs = index->nEntries;

  logmsg = NULL;
  title  = NULL;
  
  path    = NULL;
  fn      = NULL;
  n       = threadNo;
  job_cnt = 0;
  while ((n<n_jobs) && (*keepRunning))  {
    
    mmdb::CreateCopCat ( path,path0,index->entries[n]->fname );
    mmdb::CreateCopy   ( fn,mmdb::io::GetFName(path,mmdb::io::syskey_all) );
    mmdb::CreateCopy   ( logmsg,"" );

    chainNo = 0;
    rc      = mmdb::Error_NoError;
    k       = 0;
    while ((!rc) && (*keepRunning))  {
      
      rc = s2->getStructure ( path,NULL,chainNo,true );
      
      if (rc!=mmdb::Error_GeneralError1)  {
      
        if (rc>0)  {
          
          printf ( "\n *** File '%s' chain number %i error #%i"
                   " on read:\n %s\n\n",
                   path,chainNo,rc,mmdb::GetErrorDescription(rc) );
          rc = mmdb::Error_NoError;
        
        } else if (rc<=-10000)  {
          
          printf ( "\n *** File '%s' chain number %i error #%i"
                   " on read:\n %s\n\n",
                   path,chainNo,rc,mmdb::GetErrorDescription(rc) );
        
        } else  {
          
          s2->getCalphas ( atom,natoms2 );
          rc = mmdb::Error_NoError;

          if (verbosity>1)  {
            if (k<=0)  {
              sprintf ( L," %6i. %s:",n,fn );
            } else  {
              m = (int)strlen(fn)+10;
              for (int i=0;i<m;i++)
                L[i] = ' ';
              L[m] = char(0);
            }
            mmdb::CreateConcat ( logmsg,L );
            k++;
          }

          if (natoms2<=0)  {
            if (verbosity>1)
              mmdb::CreateConcat ( logmsg,
                                   "? not a protein chain, skipped\n" );
          } else  {
          
            if (verbosity>1)  {
              sprintf ( L,"%s  %5i residues:",
                          atom[0]->GetChainID(),natoms2 );
              mmdb::CreateConcat ( logmsg,L );
            }
                      
            if (natoms2>=seg_length_default)  {

              minMatch = mmdb::IMax ( minNAtoms1,
                            mmdb::mround(A->getMinMatch2()*natoms2) );
              
              if ((minMatch<=natoms1) && (minMatch<=natoms2))  {
              
                A->Align        ( s1,s2,false );
                A->getBestMatch ( SD,matchNo  );
      
                if (SD)  {
                  if (SD->Q>=trimQ)  {
                    if (SD->Nalgn>=trimSize*minMatch)  {
                      H.checkHitsBuffer();
                      H.setHit ( s2->getMMDBManager()->GetEntryID(),
                                 fn,atom[0]->GetChainID(),
                                 s2->getPDBTitle(title),SD,A,natoms2 );
                      if (verbosity>1)
                        sprintf ( L," %i aligned, rmsd=%5.3f Q=%5.3f\n",
                                    SD->Nalgn,SD->rmsd,SD->Q );
                    } else if (verbosity>1)
                        sprintf ( L," --- trimmed (%i aligned, %i minimum)\n",
                                    SD->Nalgn,(int)trimSize*minMatch );
                  } else if (verbosity>1)
                      sprintf ( L," --- trimmed (Q=%.4f, %.4f minimum)\n",
                                  SD->Q,trimQ );
                } else if (verbosity>1)
                  sprintf ( L," => *** failed *** \n" );
      
                if (verbosity>1)
                  mmdb::CreateConcat ( logmsg,L );

              } else if (verbosity>1)  {
                sprintf ( L," => filtered out by size (%i residues) \n",
                            natoms2 );
                mmdb::CreateConcat ( logmsg,L );
              }
            } else
              mmdb::CreateConcat ( logmsg,
                                   " too short a chain, skipped\n" );
          }
          
        }
  
        chainNo++;
        
      }      

    }
    
    if (verbosity>1)  {
      printf ( "%s",logmsg );
      mmdb::CreateCopy ( logmsg,"" );
    }
    
    job_cnt++;
    if ((threadNo==0) && pbar)
      displayProgress();

    n += nthreads;

  }

  delete s2;
  if (title)  delete[] title;
  if (logmsg) delete[] logmsg;
  if (path)   delete[] path;
  if (fn)     delete[] fn;
  
  finished = true;
  
}

