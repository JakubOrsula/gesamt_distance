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
//    06.02.17   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  GSMT_Archive <interface>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::ProgressBar
//       ~~~~~~~~~  gsmt::Archive
//
//  (C) E. Krissinel 2008-2017
//
// =================================================================
//

#ifndef __GSMT_Archive__
#define __GSMT_Archive__

#include <time.h>
#include <pthread.h>

#include "gsmt_aligner.h"
#include "gsmt_threadbase.h"
#include "gsmt_index.h"
#include "gsmt_hits.h"
#include "gsmt_subseqscanthread.h"

// =================================================================

namespace gsmt  {

  DefineClass(Archive);

  class Archive : public Base  {

    public :

      Archive ();
      Archive ( mmdb::io::RPStream Object );
      ~Archive();
      
      int readExclusionList ( mmdb::cpstr filePath );
      int readInclusionList ( mmdb::cpstr filePath );

      GSMT_CODE scanPDB ( mmdb::cpstr fQuery,
                          mmdb::cpstr selQuery,
                          mmdb::cpstr pdbDir,
                          mmdb::cpstr outFile,
                          mmdb::cpstr csvFile,
                          bool        SCOPSelSyntax
                        );

      GSMT_CODE prepare ( mmdb::cpstr pdbDir,
                          mmdb::cpstr archiveDir,
                          int         n_packs,
                          mmdb::cpstr outFile,
                          bool        compressed,
                          bool        update
                        );

      GSMT_CODE scanStruct (
                          mmdb::cpstr fQuery,
                          mmdb::cpstr selQuery,
                          mmdb::cpstr archiveDir,
                          mmdb::cpstr outFile,
                          mmdb::cpstr csvFile,
                          mmdb::cpstr jsonFile,
                          bool        SCOPSelSyntax
                        );

      GSMT_CODE scanSeq ( mmdb::cpstr fQuery,
                          mmdb::cpstr archiveDir,
                          mmdb::cpstr outFile,
                          mmdb::cpstr csvFile
                        );

      GSMT_CODE scanSubSeq ( mmdb::cpstr    fQuery,
                             mmdb::cpstr    archiveDir,
                             int            subSeqLen,
                             mmdb::realtype minSeqMatch,
                             mmdb::cpstr    outFile
                           );
                   
      inline void stop() { keepRunning = false; }
      
      inline int  getNofNewEntries () { return nNewEntries;  }
      inline int  getNofDoneEntries() { return nEntriesDone; }

      void setRVAPIProgressWidgets ( mmdb::cpstr progressBarId,
                                     mmdb::cpstr etaId );

      mmdb::realtype getProgress  ();
      mmdb::cpstr    getTimeString();
        
      PHits  takeHits();
      inline PSeqSheafs getSeqSheafs()  { return seqSheafs; }

      static mmdb::io::PFile getStructFile ( mmdb::cpstr archiveDir,
                                             int  packNo );
      static PStructure readStructure ( mmdb::io::PFile structFile,
                                        long struct_offset );

    protected :
      PHits           H;
      mmdb::IDCode   *inclChains;
      mmdb::IDCode   *exclChains;
      PSeqSheafs      seqSheafs;
      ProgressBar     progressBar;
      pthread_mutex_t lock;
      int             nNewEntries;
      int             nEntriesDone;
      bool            keepRunning;

      //  Initialization and memory management
      void initArchive();
      void freeMemory ();
      
      int  readList ( mmdb::IDCode *& list, int & alloc_len,
                      mmdb::cpstr filePath );

      void write_log ( mmdb::cpstr S, mmdb::io::RFile f, int verb );
      
    private:
      int  incl_alloc,excl_alloc;

  };

  extern mmdb::cpstr getArchiveIndexName();
  extern void copyDirPath ( mmdb::cpstr dirPath, mmdb::pstr & path );
  
}

#endif
