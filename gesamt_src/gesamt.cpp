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
//  **** Module  :  Gesamt <implementation>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :
//       ~~~~~~~~~
//
//  GESAMT: General and Efficient Structural Alignment of
//          Macromoecular Targets
//
//  (C) E. Krissinel, 2008-2017
//
// =================================================================
//

#include <string.h>

#include "gesamt_input.h"
#include "gesamt_usage.h"
#include "gesamtlib/gsmt_archive.h"
#include "gesamt_pairwise.h"
#include "gesamt_multiple.h"
#include "gesamt_sheaf.h"
#include "gesamt_domains.h"
#include "gesamt_utils.h"
#include "gesamtlib/gsmt_model.h"
#include "rvapi/rvapi_interface.h"

#ifdef compile_for_ccp4

#include "ccp4/ccp4_parser.h"
#include "ccp4/ccp4_general.h"
#include "ccp4/ccp4_program.h"

using namespace CCP4;

#endif

// =================================================================

void maintainArchive ( gsmt::RInput Input )  {
gsmt::PArchive Archive;
  Archive = new gsmt::Archive();
  Archive->setVerbosity ( Input.verbosity );
  Archive->prepare      ( Input.pdbDir,Input.archDir,Input.npacks,
                          Input.outFile,Input.compressArch,
                          Input.taskCode==gsmt::TASK_UpdateArchive );
  delete Archive;
}

void PDBScan ( gsmt::RInput Input )  {
gsmt::PArchive Archive;
  Archive = new gsmt::Archive();
  Archive->setPerformanceLevel     ( Input.mode       );
  Archive->setSimilarityThresholds ( Input.minMatch1,
                                     Input.minMatch2  );
  Archive->setTrimFactors          ( Input.trimQ,
                                     Input.trimSize   );
  Archive->setQR0                  ( Input.QR0        );
  Archive->setSigma                ( Input.sigma      );
  Archive->setVerbosity            ( Input.verbosity  );
  Archive->setNThreads             ( Input.nthreads   );
  Archive->scanPDB ( Input.fstruct[0],Input.sel[0],
                     Input.pdbDir,Input.outFile,Input.csvFile,
                     Input.scopSel[0] );
  delete Archive;
}


void archiveStructScan ( gsmt::RInput Input )  {
gsmt::PArchive Archive;
mmdb::pstr     rvapiMeta = NULL;
mmdb::pstr     p;
int            rvapiReport;

  rvapiReport = initRVAPIDomOutput ( Input );
  if (rvapiReport==2)
    mmdb::CreateCopy ( rvapiMeta,rvapi_get_meta() );
    
  //printf ( " rvapireport=%i,  rvapiMeta='%s'",rvapiReport,rvapiMeta );

  Archive = new gsmt::Archive();
  if ((rvapiReport==2) && rvapiMeta)  {
    p = strchr ( rvapiMeta,';' );
    if (p)  {
      *p = char(0);
      Archive->setRVAPIProgressWidgets ( rvapiMeta,&(p[1]) );
    }
  }
  Archive->setPerformanceLevel     ( Input.mode       );
  Archive->setSimilarityThresholds ( Input.minMatch1,
                                     Input.minMatch2  );
  Archive->setTrimFactors          ( Input.trimQ,
                                     Input.trimSize   );
  Archive->setQR0                  ( Input.QR0        );
  Archive->setSigma                ( Input.sigma      );
  Archive->setVerbosity            ( Input.verbosity  );
  Archive->setNThreads             ( Input.nthreads   );
  Archive->readInclusionList       ( Input.inclFile   );
  Archive->readExclusionList       ( Input.exclFile   );
  Archive->scanStruct ( Input.fstruct[0],Input.sel[0],
                        Input.archDir,Input.outFile,Input.csvFile,
                        Input.jsonOutFile,Input.scopSel[0] );
  delete Archive;

  if (rvapiReport==2)  {
    rvapi_put_meta        ( rvapiMeta       );
    rvapi_store_document2 ( Input.rvapiRDoc );
    rvapi_keep_polling    ( true            );
  }
  
  if (rvapiMeta)
    delete[] rvapiMeta;

}


void archiveSeqScan ( gsmt::RInput Input )  {
gsmt::PArchive Archive;
  Archive = new gsmt::Archive();
  Archive->setSimilarityThresholds ( Input.minMatch1,
                                     Input.minMatch2  );
  Archive->setTrimFactors          ( Input.trimQ,
                                     Input.trimSize   );
  Archive->setVerbosity            ( Input.verbosity  );
  Archive->setNThreads             ( Input.nthreads   );
  Archive->readInclusionList       ( Input.inclFile   );
  Archive->readExclusionList       ( Input.exclFile   );
  Archive->scanSeq ( Input.inpSeqFile,Input.archDir,Input.outFile,
                     Input.csvFile );
   delete Archive;
}

void makeModel ( gsmt::RInput Input )  {
gsmt::PModel  Model;

  Model = new gsmt::Model();
  Model->setVerbosity   ( Input.verbosity   );
  Model->setNThreads    ( Input.nthreads    );
  Model->setArchiveDir  ( Input.archDir     );
  Model->setSubSeqLen   ( Input.subSeqLen   );
  Model->setMinSeqMatch ( Input.minSeqMatch );
  Model->setOutputDir   ( Input.outFile     );

//  Model->readInclusionList       ( Input.inclFile  );
//  Model->readExclusionList       ( Input.exclFile  );

  Model->makeModel ( Input.inpSeqFile );

  delete Model;

}

int main ( int argc, char ** argv, char ** env )  {
UNUSED_ARGUMENT(env);
gsmt::Input       Input;
#ifdef compile_for_ccp4
mmdb::pstr        ccp4msg = NULL;
#endif
gsmt::INPUT_CODE  IC;
int               rc = 0;

  printf (
   "\n"
   " GESAMT: General Efficient Structural Alignment of Macromolecular Targets\n"
   " ------------------------------------------------------------------------\n"
   " Version " GESAMT_Version " of " GESAMT_Date ", built with MMDB v.%i.%i.%i\n",
   mmdb::MAJOR_VERSION,mmdb::MINOR_VERSION,mmdb::MICRO_VERSION );

#ifdef compile_for_ccp4

  ccp4ProgramName ( "Gesamt" );
  ccp4_banner();

  printf ( "$TEXT:Reference: $$Please cite$$\n"
    "E. Krissinel (2012). Enhanced fold recognition using efficient\n"
    "short fragment clustering. J. Mol. Biochem. 1(2) 76-85.\n"
    "$$\n" );

  printf ( "<!--SUMMARY_BEGIN-->\n" );

#endif

  if (argc<=1)  {
    printInstructions ( argv[0] );
#ifdef compile_for_ccp4
    printf ( "<!--SUMMARY_END-->\n" );
    ccperror ( 1,"No input" );
#endif
    return 1;
  }

  if ((!strcmp(argv[1],"-?")) || (!strcasecmp(argv[1],"-help")) ||
      (!strcasecmp(argv[1],"--help")))  {
    printInstructions ( argv[0] );
#ifdef compile_for_ccp4
    printf ( "<!--SUMMARY_END-->\n" );
    ccperror ( 2,"Wrong input" );
#endif
    return 2;
  }

  if (!strcmp(argv[1],"--print-opt"))  {
    printViewerOptTemplate ( argv[0] );
    return 0;
  }


  if (argc<3)  {
    printInstructions ( argv[0] );
#ifdef compile_for_ccp4
    printf ( "<!--SUMMARY_END-->\n" );
    ccperror ( 3,"Wrong input" );
#endif
    return 3;
  }
  
  IC = Input.parseCommandLine ( argc,argv );
  if (IC==gsmt::INPUT_Ok)  {

    switch (Input.taskCode)  {
      
      case gsmt::TASK_PairwiseAlignment: alignToTarget     ( Input ); break;
      case gsmt::TASK_MultipleAlignment: multipleAlignment ( Input ); break;
      case gsmt::TASK_SheafAlignment   : sheafAlignment    ( Input ); break;
      case gsmt::TASK_Domains          : analyseDomains    ( Input ); break;
      case gsmt::TASK_PDBScan          : PDBScan           ( Input ); break;
      case gsmt::TASK_MakeArchive      : maintainArchive   ( Input ); break;
      case gsmt::TASK_UpdateArchive    : maintainArchive   ( Input ); break;
      case gsmt::TASK_ArchiveStructScan: archiveStructScan ( Input ); break;
      case gsmt::TASK_ArchiveSeqScan   : archiveSeqScan    ( Input ); break;
      case gsmt::TASK_MakeModel        : makeModel         ( Input ); break;

      default: printf ( " *** unknown task code (%i)\n",Input.taskCode );

    }

    rc = 0;
#ifdef compile_for_ccp4
    mmdb::CreateCopy ( ccp4msg,"Normal termination" );
#else
  printf (
    "\n"
    " Please cite:\n"
    " E. Krissinel (2012) Enhanced fold recognition using efficient\n"
    " short fragment clustering. J. Mol. Biochem., 1(2) 76-85.\n" );
#endif
        
  } else  {
    
    printf ( "\n\n"
             " *** INPUT ERROR (%i): %s\n\n"
             "     Run %s -help for full list of options.\n\n",
             (int)IC,Input.errMessage(IC),argv[0] );
#ifdef compile_for_ccp4
    printf ( "<!--SUMMARY_END-->\n" );
    mmdb::CreateCopy ( ccp4msg,Input.errMessage(IC) );
#endif
    rc = 4 + (int)IC;
        
  }
  
#ifdef compile_for_ccp4
  ccperror ( rc,ccp4msg );
  if (ccp4msg)  delete[] ccp4msg;
#endif

  return rc;

}

