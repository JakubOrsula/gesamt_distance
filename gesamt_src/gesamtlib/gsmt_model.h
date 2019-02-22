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
//    16.02.16   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  GSMT_Model <interface>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::Model
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2008-2016
//
// =================================================================
//

#ifndef __GSMT_Model__
#define __GSMT_Model__

#include "gsmt_archive.h"
#include "gsmt_defs.h"

// =================================================================

namespace gsmt {

  DefineClass(Model);

  class Model  {

    public :

      Model();
      virtual ~Model();
      
      inline void setVerbosity   ( int v )  { verbosity = v; }
      inline void setNThreads    ( int n )  { nthreads  = n; }
      inline void setSubSeqLen   ( int l )  { subSeqLen = l; }
      inline void setMinSeqMatch ( mmdb::realtype minSM )
                                      { minSeqMatch = minSM; }
      
      void setArchiveDir ( mmdb::cpstr archiveDir );
      void setOutputDir  ( mmdb::cpstr outputDir  );
      
      GSMT_CODE makeModel ( mmdb::cpstr seqFile );

    protected :
      PArchive       archive;     //!< gesamt archive
      mmdb::pstr     archDir;     //!< path to archive directory
      mmdb::pstr     outDir;      //!< path to output directory + "/"
      mmdb::realtype minSeqMatch; //!< minimal acceptable sequence match
      mmdb::realtype Qthresh;     //!< Threshold Q-score for sheafing
      int            subSeqLen;   //!< subsequence length
      int            nthreads;    //!< number of threads to use
      int            verbosity;   //!< verbosity level

      int            nSheafs;     //!< number of sequence sheafs
      int            sheafLen;    //!< common sheaf length
      PPStructure    asmS;

      void initModel ();
      void freeMemory();
      
      GSMT_CODE makeSeqSheafs ( mmdb::cpstr     seqFile,
                                int        fragmentSize,
                                mmdb::realtype minMatch );      
      GSMT_CODE alignSeqSheafs();
      GSMT_CODE assembleSheafs();

      void asmInitMemory();
      void asmGetMemory ();
      void asmFreeMemory();
      
      PStructure asmMakeStructure ( int structNo );
      
    private:
      int nStructAlloc;

  };

}

#endif
