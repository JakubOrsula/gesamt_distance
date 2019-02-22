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
//    06.05.16   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  Gesamt <implementation>
//       ~~~~~~~~~
//  **** Project :  GESAMT Sheaf Alignment Driver
//       ~~~~~~~~~
//  **** Classes :
//       ~~~~~~~~~
//
//  GESAMT: General and Efficient Structural Alignment of
//          Macromoecular Targets
//
//  (C) E. Krissinel, 2008-2016
//
// =================================================================
//

#include <string.h>
#include "gesamtlib/gsmt_sheaf.h"
#include "gesamtlib/gsmt_utils.h"
#include "gesamtlib/gsmt_defs.h"

#include "gesamt_sheaf.h"
#include "gesamt_pairwise.h"

// =================================================================

void writeLogFile ( gsmt::PSheafData sheafData, gsmt::RInput Input,
                    bool isOutput )  {
UNUSED_ARGUMENT(Input);
mmdb::io::File  f;

  printStructureSummary ( sheafData->S,sheafData->nStruct,isOutput );

  if (sheafData->nStruct<=1)  {

    printf ( "\n"
             " Single-structure sheaf: no scoring, no alignment\n" );

  } else  {

    printf ( "\n"
      " ===== Scores achieved:\n"
      "\n"
       "   quality Q:  %-7.4f (normalised to [0...1])\n"
       "     r.m.s.d:  %-7.4f (A)\n"
       "      Nalign:  %-6i  (residues)\n",
       sheafData->Q,sheafData->rmsd,sheafData->Nalign );
  
    f.assign ( "stdout" );
    f.rewrite();

//#ifdef _ccp4_
//    f.Write ( "$$\n\n" );
//    f.Write ( "$TEXT:Residue alignment: $$ $$\n\n" );
//#else

    f.Write ( "\n\n"
              " ===== Residue alignment:\n\n" );
//#endif
    sheafData->writeResAlign   ( f );

//#ifdef _ccp4_
//  f.Write ( "$$\n" );
//#endif
    f.shut();

  }

}

/*
void writeCSVFile ( gsmt::PPMAStructure M, gsmt::RInput Input,
                    gsmt::PMAligner MAligner, bool isOutput )  {
mmdb::io::File  f;
char            L1[2000];
mmdb::rmatrix   m_rmsd,m_Qscore,m_seqId;
mmdb::rvector   cons_x,cons_y,cons_z;
mmdb::realtype  rmsd,Qscore;
int             n_align,cons_len;

  f.assign ( Input.csvFile,true,false );
  f.rewrite();

  printStructureSummary_csv ( f,(gsmt::PPStructure)M,
                              Input.nStruct,isOutput );

  f.WriteLine ( "\n"
                "SUPERPOSITION MATRICES (ORTHOGONAL)" );

  for (int i=0;i<Input.nStruct;i++)  {

    sprintf ( L1,"\n"
                 "STRUCTURE, %s, %s, %s\n\n"
                 "Rx, Ry, Rz, T\n"
                 "%14.7f, %14.7f, %14.7f, %14.7f\n"
                 "%14.7f, %14.7f, %14.7f, %14.7f\n"
                 "%14.7f, %14.7f, %14.7f, %14.7f\n",
                 M[i]->getRefName(),Input.fstruct[i],Input.sel[i],
                 M[i]->RT0[0][0],M[i]->RT0[0][1],M[i]->RT0[0][2],
                                                 M[i]->RT0[0][3],
                 M[i]->RT0[1][0],M[i]->RT0[1][1],M[i]->RT0[1][2],
                                                 M[i]->RT0[1][3],
                 M[i]->RT0[2][0],M[i]->RT0[2][1],M[i]->RT0[2][2],
                                                 M[i]->RT0[2][3] );
    f.Write ( L1 );
//    for (int j=0;j<Input.nStruct;j++)  {
//      writeFracAnalysis_csv ( L1,M[i]->RT0,M[i]->getRefName(),
//                                 M[j]->getMMDBManager() );
//      f.Write ( L1 );
//    }
    
  }


  f.LF();

  MAligner->getAlignScores ( n_align,rmsd,Qscore );

  sprintf ( L1,"\n"
    "SCORES ACHIEVED\n"
    "\n"
     "quality Q, %-7.4f, (normalised to [0...1])\n"
     "r.m.s.d, %-7.4f, (A)\n"
     "Nalign, %-6i,(residues)\n",
     Qscore,rmsd,n_align );
  f.Write ( L1 );

  MAligner->getConsensusScores ( cons_x,cons_y,cons_z,cons_len,
                                 m_rmsd,m_Qscore,m_seqId );

  print_matrix_csv ( f,
     "Pairwise Q-scores (consensus Q-score on diagonal):",
     m_Qscore,"%5.3f",M,Input.nStruct );
  print_matrix_csv ( f,
     "Pairwise r.m.s.d. (consensus r.m.s.d. on diagonal):",
     m_rmsd,"%5.3f",M,Input.nStruct );
  print_matrix_csv ( f,
     "Pairwise seq. Id:",m_seqId,
     "%5.3f",M,Input.nStruct );

  f.Write ( "\n\n"
            "RESIDUE ALIGNMENT\n\n" );
  MAligner->writeMultGSMT_csv ( f );

  f.shut();

}
*/

void sheafAlignment ( gsmt::RInput Input )  {
//mmdb::io::File     f;
gsmt::PSheaf       sheafAligner;
gsmt::PSheafData   sheafData;
gsmt::PPStructure  M;
mmdb::pstr         S,fext;
mmdb::rmatrix      rmsdX;
mmdb::ERROR_CODE   rc;
char               rname[20];
int                alrc,i,j,nSheafs;
bool               isOutput;

  M = new gsmt::PStructure[Input.nStruct];
  for (int i=0;i<Input.nStruct;i++)
    M[i] = NULL;

  fext = NULL;
  S    = NULL;
  printf (
    "\n ===========================================================\n" );

  rc = mmdb::Error_NoError;
  for (i=0;(i<Input.nStruct) && (rc==mmdb::Error_NoError);i++)  {

    printf ( " ... reading file '%s', selection '%s':\n",
             Input.fstruct[i],Input.sel[i] );
    M[i] = new gsmt::Structure();
    sprintf ( rname,"S%03i",i+1 );
    M[i]->setRefName ( rname );
    rc = M[i]->getStructure ( Input.fstruct[i],Input.sel[i],-1,
                              Input.scopSel[i] );
    if (rc==mmdb::Error_NoError)
      printf ( "      %5i atoms selected\n",M[i]->getNCalphas() );
    else  {
      if (rc>0)
        printf ( "      %5i atoms selected with warning (rc=%i)\n",
                 M[i]->getNCalphas(),rc );
      else
        printf ( " *error* (rc=%i)\n",rc );
      M[i]->getErrDesc ( rc,S );
      printf ( "     %s\n",S );
    }

  }

  if (rc==mmdb::Error_NoError)  {

    printf ( "\n Parameter Q-score:                %.3f angstroms\n",
             Input.QR0 );
    if (Input.sigma<=0.0)
         printf ( " Weighted superposition is not used\n" );
    else printf ( " Sigma for weighted superposition: %.3f angstroms\n",
                  Input.sigma );
    printf ( " Number of threads used:           %i\n\n",Input.nthreads );

    sheafAligner = new gsmt::Sheaf();
    sheafAligner->setSheafMode  ( Input.sheafMode );
    sheafAligner->setQR0        ( Input.QR0       );
    sheafAligner->setQThreshold ( Input.Qthresh   );
    sheafAligner->setNThreads   ( Input.nthreads  );
    sheafAligner->setVerbosity  ( Input.verbosity );

    alrc = sheafAligner->Align ( M,Input.nStruct,true );
    if (alrc==gsmt::GSMT_Ok)  {

#ifdef _ccp4_
      printf ( "$TEXT:Alignment results: $$ $$\n" );
#endif
      nSheafs = sheafAligner->getNofSheafs();

      if (Input.sheafMode & gsmt::SHEAF_X)  {
      
        mmdb::GetMatrixMemory ( rmsdX,Input.nStruct,Input.nStruct,0,0 );

        for (i=0;i<Input.nStruct;i++)
          for (j=0;j<Input.nStruct;j++)
            rmsdX[i][j] = 0.0;

        for (i=0;i<nSheafs;i++)  {
          sheafData = sheafAligner->getSheafData(i);
          if (sheafData)  {
            rmsdX[sheafData->sId[0]][sheafData->sId[1]] = sheafData->rmsd;
            rmsdX[sheafData->sId[1]][sheafData->sId[0]] = sheafData->rmsd;
          }
        }

        printf ( "\n ===== CROSS-RMSDs\n\n" );

        for (i=0;i<Input.nStruct;i++)  {
          printf ( " %4i| %s:%s |",i+1,Input.fstruct[i],Input.sel[i] );
          for (j=0;j<Input.nStruct;j++)
            printf ( "  %12.8f",rmsdX[i][j] );
          printf ( "\n" );
        }

        mmdb::FreeMatrixMemory ( rmsdX,Input.nStruct,0,0 );

      } else  {

        printf ( "\n Total sheafs made: %i\n\n",nSheafs );
  
        for (i=0;i<nSheafs;i++)  {
          sheafData = sheafAligner->getSheafData(i);
          if (sheafData)  {
            printf ( "\n ##### SHEAF No. %i\n",i+1 );
            if (S)  delete[] S;
            if (Input.outFile)  {
              S = new char[2*strlen(Input.outFile)];
              strcpy ( S,Input.outFile );
              sprintf ( rname,"%03i",i+1 );
              mmdb::CreateCopCat ( fext,rname,mmdb::io::GetFExt(S) );
              mmdb::io::ChangeExt ( S,fext );
            } else
              S = NULL;
            isOutput = writeSuperposed ( sheafData->S,sheafData->T,
                                         sheafData->nStruct,
                                         S,Input.out_units,
                                         Input.out_multi );
            writeLogFile ( sheafData,Input,isOutput );
          }
        }
  
  /*      
  
  
        if (Input.outSeqFile)  {
          f.assign ( Input.outSeqFile,true,false );
          f.rewrite();
          MAligner->writeSeqAlignment ( f,Input.fstruct,Input.sel );
          f.shut();
        }
  
  //      if (Input.csvFile)
  //        writeCSVFile ( M,Input,MAligner,isOutput );
  */
      }

    } else  {
      printf (
        "\n\n ALIGNMENT ERROR %i\n"
        " ===========================================================\n",
        alrc );
    }

    delete sheafAligner;

  } else  {
    printf (
      "\n\n STOP DUE TO READ ERRORS\n"
      " --- check input file format\n\n"
      " ===========================================================\n"
           );
  }

  for (int i=0;i<Input.nStruct;i++)
    if (M[i])  delete M[i];
  delete[] M;

  if (S)     delete[] S;
  if (fext)  delete[] fext;

}
