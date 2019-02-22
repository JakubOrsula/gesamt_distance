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
//  **** Module  :  GSMT_StructScanThread <implementation>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::StructScanThread
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2008-2016
//
// =================================================================
//

#include <string.h>

#include "gsmt_structscanthread.h"
#include "memio_.h"

// =================================================================

gsmt::StructScanThread::StructScanThread() : PDBScanThread()  {
  packNo = 0;
}

gsmt::StructScanThread::~StructScanThread()  {}


void gsmt::StructScanThread::run()  {
mmdb::io::File       f;
#ifdef __use_memory_pool
mmdb::io::File       fmem;
gsmt::MemIO          memIO;
#endif
gsmt::PEntry         entry;
gsmt::PSubEntry      subEntry;
gsmt::PSuperposition SD;
gsmt::PStructure     s2;
mmdb::pstr           fpath,fname,fname0,logmsg,title;
#ifdef __use_memory_pool
mmdb::pstr           memPool;
int                  poolSize;
#endif
char                 archFName[200];
char                 L[300];
int                  natoms1,minNAtoms1,minMatch;
int                  matchNo,n1,n2,n,m,k,pack_no;

  H.deleteHits();
  H.dataKey = Hits::DATA_StructureScan;
  SD = NULL;
  
  natoms1    = s1->getNCalphas();
  minNAtoms1 = int(A->getMinMatch1()*natoms1);
  
  s2 = new gsmt::Structure();
  n_jobs = index->nPackedSubEntries;
 
  logmsg = NULL;
  fname  = NULL;
  fname0 = NULL;
  fpath  = NULL;
  title  = NULL;
  mmdb::CreateCopy ( fname0,"" );
  
  pack_no = packNo;

  while ((pack_no<index->nPacks) && (*keepRunning))  {

    if (index->pack_index[pack_no]>=0)  {
      
      sprintf ( archFName,struct_pack_name_tempate,pack_no );
      mmdb::CreateCopCat ( fpath,path0,mmdb::io::_dir_sep,archFName );
      f.assign ( fpath,false,true,mmdb::io::GZM_NONE );
      if (!f.reset())  {
        if (verbosity>=0)
          printf ( "\n *** cannot open pack file\n"
                   "       %s\n"
                   "       for reading\n\n",fpath );
      } else  {
      
        n1 = index->pack_index[pack_no];
        if (pack_no==index->nPacks-1)
          n2 = index->nEntries;
        else  {
          n2 = index->pack_index[pack_no+1];
          if (n2<0)
            n2 = index->nEntries;
        }
      
        for (n=n1;(n<n2) && (*keepRunning);n++)  {
        
          entry = index->entries[n];
        
          for (m=0;m<entry->nSubEntries;m++)  {
          
            subEntry = entry->subEntries[m];
            
            if (subEntry->selected)  {
          
              mmdb::CreateCopy ( fname,
                mmdb::io::GetFName(entry->fname,mmdb::io::syskey_all) );

              if (verbosity>1)  {
                if (strcmp(fname,fname0))  {
                  sprintf ( L," %03i/%05i. %s:",pack_no,n-n1,fname );
                  mmdb::CreateCopy ( fname0,fname );
                } else  {
                  k = strlen(fname) + 13;
                  for (int i=0;i<k;i++)
                    L[i] = ' ';
                  L[k] = char(0);
                }
                mmdb::CreateCopy ( logmsg,L );
                sprintf ( L,"%s  %5i residues:",
                            subEntry->id,subEntry->size );
                mmdb::CreateConcat ( logmsg,L );
              }
          
              minMatch = mmdb::IMax ( minNAtoms1,
                                int(A->getMinMatch2()*subEntry->size) );

              if ((minMatch<=natoms1) && (minMatch<=subEntry->size))  {
          
                f.seek ( subEntry->offset_struct );

#ifdef __use_memory_pool
                memIO.read ( f );
                memIO.get_buffer ( &memPool,&poolSize );
                fmem.assign ( poolSize,0,memPool );
                fmem.reset();
                s2->read ( fmem );
                fmem.shut();
#else
                s2->read ( f );
#endif

                A->Align        ( s1,s2,false );
                A->getBestMatch ( SD,matchNo  );
    
                if (SD)  {
                  if (SD->Q>=trimQ)  {
                    if (SD->Nalgn>=trimSize*minMatch)  {
                      H.checkHitsBuffer();
                      H.setHit ( s2->getMMDBManager()->GetEntryID(),fname,
                                 subEntry->id,s2->getPDBTitle(title),
                                 SD,A,subEntry->size );
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
              
                if (verbosity>1)  {
                  mmdb::CreateConcat ( logmsg,L );
                  printf ( "%s",logmsg );
                }
    
              } else if (verbosity>1)
                printf ( "%s => filtered out by size (%i residues) \n",
                         logmsg,subEntry->size );
          
            }
            
            job_cnt++;
            if (pbar && (threadNo==0))
              displayProgress();

          }

        }

        f.shut();

      }

    }

    pack_no += nthreads;

  }

  delete s2;

  if (fpath)  delete[] fpath;
  if (fname)  delete[] fname;
  if (fname0) delete[] fname0;
  if (logmsg) delete[] logmsg;
  if (title)  delete[] title;

  finished = true;

}

