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
//  **** Module  :  GSMT_SeqScanThread <implementation>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::SeqScanThread
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2008-2016
//
// =================================================================
//

#include <string.h>

#include "gsmt_seqscanthread.h"
#include "gsmt_defs.h"
#include "memio_.h"

// =================================================================

gsmt::SeqScanThread::SeqScanThread() : ThreadBase()  {
  A           = NULL;
  s1          = NULL;
  path0       = NULL;
  index       = NULL;
  alignMethod = mmdb::math::ALIGN_GLOBAL;
  minMatch1   = 0.0;
  minMatch2   = 0.0;
  trimSize    = 0.0;
}

gsmt::SeqScanThread::~SeqScanThread()  {
  freeMemory();
}

void gsmt::SeqScanThread::freeMemory()  {
  if (A)  delete A;
  H.deleteHits();
  if (path0)  delete[] path0;
  A     = NULL;
  path0 = NULL;
}

void gsmt::SeqScanThread::initAligner (
                                   mmdb::math::ALIGN_METHOD method,
                                   mmdb::realtype       gapPenalty,
                                   mmdb::realtype     spacePenalty,
                                   mmdb::realtype       scoreEqual,
                                   mmdb::realtype    scoreNotEqual,
                                   mmdb::realtype       min_match1,
                                   mmdb::realtype       min_match2,
                                   mmdb::realtype      trim_size )  {
  A = new mmdb::math::Alignment();
  A->SetAffineModel ( gapPenalty,spacePenalty  );
  A->SetScores      ( scoreEqual,scoreNotEqual );
  alignMethod = method;
  minMatch1   = min_match1;
  minMatch2   = min_match2;
  trimSize    = trim_size;
}

void gsmt::SeqScanThread::run()  {
mmdb::io::File       f;
#ifdef __use_memory_pool
mmdb::io::File       fmem;
gsmt::MemIO          memIO;
#endif
gsmt::PEntry         entry;
gsmt::PSubEntry      subEntry;
gsmt::PSequence      s2;
mmdb::pstr           fpath,fname,fname0,logmsg,title;
#ifdef __use_memory_pool
mmdb::pstr           memPool;
int                  poolSize;
#endif
char                 archFName[200];
char                 L[300];
mmdb::realtype       seqId,similarity,score;
int                  nres1,minNRes1,minMatch;
int                  Nalign,n1,n2,n,m,k,pack_no;

  H.deleteHits();
  H.dataKey = Hits::DATA_SequenceScan;
  
  nres1    = s1->getSeqLength();
  minNRes1 = int(minMatch1*nres1);
  
  s2 = new gsmt::Sequence();
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
      
      sprintf ( archFName,seq_pack_name_tempate,pack_no );
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
          
              minMatch = mmdb::IMax ( minNRes1,
                                      int(minMatch2*subEntry->size) );

              if ((minMatch<=nres1) && (minMatch<=subEntry->size))  {
          
                f.seek ( subEntry->offset_seq );

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

                A->Align ( s1->getSequence(),s2->getSequence(),
                           alignMethod );
                Nalign = A->GetNAlign();
                if (Nalign>=trimSize*minMatch)  {
                  H.checkHitsBuffer();
                  seqId      = A->GetSeqId ();
                  score      = A->GetScore ();
                  similarity = A->GetSimilarity();
                  H.setHit ( s2->getIDCode(),fname,
                             subEntry->id,s2->getTitle(),
                             A->GetAlignedS(),A->GetAlignedT(),
                             seqId,similarity,score,Nalign,
                             subEntry->size );
                  if (verbosity>1)  {
                    sprintf ( L," %i aligned, seqId=%5.3f sim=%5.3f "
                                "score=%5.3f\n",
                                Nalign,seqId,similarity,score );
                    mmdb::CreateConcat ( logmsg,L );
                    printf ( "%s",logmsg );
                  }
                } else if (verbosity>1)
                  sprintf ( L," --- trimmed (%i aligned, %i minimum)\n",
                              Nalign,(int)trimSize*minMatch );
    
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

