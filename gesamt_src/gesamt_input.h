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
//    14.05.16   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  GSMT_Input <interface>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::Input
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2008-2016
//
// =================================================================
//

#ifndef __GSMT_Input__
#define __GSMT_Input__

#include "mmdb2/mmdb_mattype.h"
#include "gesamtlib/gsmt_viewer.h"
#include "gesamtlib/gsmt_sheaf.h"
#include "gesamtlib/gsmt_defs.h"

// =================================================================

namespace gsmt  {

  DefineClass(Input);

  enum TASK_CODE  {
    TASK_None = 0,
    TASK_PairwiseAlignment,
    TASK_MultipleAlignment,
    TASK_SheafAlignment,
    TASK_Domains,
    TASK_PDBScan,
    TASK_MakeArchive,
    TASK_UpdateArchive,
    TASK_ArchiveStructScan,
    TASK_ArchiveSeqScan,
    TASK_MakeModel
  };

  enum INPUT_CODE  {
    INPUT_Ok = 0,
    INPUT_CantOpenInputList,
    INPUT_InputListError,
    INPUT_WrongCommandLine,
    INPUT_UnknownKeyword,
    INPUT_WrongR0Value,
    INPUT_WrongR0Format,
    INPUT_WrongQThreshValue,
    INPUT_WrongQThreshFormat,
    INPUT_WrongSigmaValue,
    INPUT_WrongSigmaFormat,
    INPUT_WrongMin1Value,
    INPUT_WrongMin1Format,
    INPUT_WrongMin2Value,
    INPUT_WrongMin2Format,
    INPUT_WrongTrimQValue,
    INPUT_WrongTrimQFormat,
    INPUT_WrongTrimSizeValue,
    INPUT_WrongTrimSizeFormat,
    INPUT_WrongNThreads,
    INPUT_WrongNPacks,
    INPUT_NoOutputFile,
    INPUT_NoOutputDirectory
  };

  class Input  {

    public:
      TASK_CODE        taskCode;    //!< task code
      mmdb::psvector   fstruct;     //!< structure file names
      mmdb::psvector   sel;         //!< selection strings per structure
      mmdb::ovector    scopSel;     //!< scop/mmdb selection switches
      int              nStruct;     //!< number of structures
      mmdb::pstr       outFile;     //!< output file
      mmdb::pstr       jsonOutFile; //!< json-formatted output file
      mmdb::pstr       rvapiRDir;   //!< rvapi report directory
      mmdb::pstr       rvapiJSUri;  //!< rvapi JS Uri
      mmdb::pstr       rvapiRDoc;   //!< rvapi report document
      mmdb::pstr       outSeqFile;  //!< output sequence file
      mmdb::pstr       inpSeqFile;  //!< intput sequence file
      mmdb::pstr       csvFile;     //!< output CSV file
      mmdb::pstr       pdbDir;      //!< PDB directory to screen
      mmdb::pstr       archDir;     //!< archive directory to screen
      mmdb::pstr       inclFile;    //!< file with inclusive entries
      mmdb::pstr       exclFile;    //!< file with exclusive entries
      mmdb::realtype   QR0;         //!< parameter of Q-score
      mmdb::realtype   Qthresh;     //!< Q-threshold for sheafs
      mmdb::realtype   sigma;       //!< superposition sigma
      mmdb::realtype   minMatch1;   //!< minimal match fraction #1 (query) 
      mmdb::realtype   minMatch2;   //!< minimal match fraction #1 (target)
      mmdb::realtype   trimSize;    //!< size-based trim factor for scan results
      mmdb::realtype   trimQ;       //!< Q-based trim factor for scan results
      PERFORMANCE_CODE mode;        //!< efficiency mode
      SHEAF_MODE       sheafMode;   //!< sheaf mode
      int              subSeqLen;   //!< subseq-ce length for model making
      mmdb::realtype   minSeqMatch; //!< min seq. score for model making
      int              nthreads;    //!< number of threads
      int              npacks;      //!< number of packs in archive
      int              verbosity;   //!< verbosity level
      bool             compressArch; //! making compressed gesamt archive
      int              out_multi;   //!< output multi-files/single-file
      mmdb::SELECTION_TYPE out_units; //!< output selected mmdb units
      PViewer          viewer;      //!< viewer handler
      int              viewerData;  //!< viewer data modifier
      bool             jsonCompact; //!< key to make compact json output

      Input ();
      ~Input();

      INPUT_CODE parseCommandLine ( int argc, char ** argv );
      
      mmdb::cpstr errMessage ( INPUT_CODE icode );
      
    protected:
      INPUT_CODE IC;

      void init      ();
      void freeMemory();
      void addFName  ( mmdb::cpstr fname );
      mmdb::realtype getReal ( mmdb::cpstr    argv,
                               mmdb::cpstr    key,
                               mmdb::realtype minValid,
                               mmdb::realtype maxValid,
                               INPUT_CODE     wrongFormat,
                               INPUT_CODE     wrongValue );

  };

}

extern int getNofSystemThreads();

#endif

