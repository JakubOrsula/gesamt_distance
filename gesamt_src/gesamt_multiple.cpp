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
//    24.07.15   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  Gesamt <implementation>
//       ~~~~~~~~~
//  **** Project :  GESAMT Multiple Alignment Driver
//       ~~~~~~~~~
//  **** Classes :
//       ~~~~~~~~~
//
//  GESAMT: General and Efficient Structural Alignment of
//          Macromoecular Targets
//
//  (C) E. Krissinel, 2008-2015
//
// =================================================================
//

#include <string.h>
#include "gesamtlib/gsmt_maligner.h"
#include "gesamtlib/gsmt_utils.h"
#include "gesamtlib/gsmt_defs.h"
#include "rvapi/rvapi_interface.h"

#include "gesamt_multiple.h"
#include "gesamt_pairwise.h"
#include "gesamt_utils.h"

// =================================================================


void writeLogFile ( gsmt::PPMAStructure M, gsmt::RInput Input,
                    gsmt::PMAligner MAligner, bool isOutput )  {
mmdb::io::File  f;
mmdb::rmatrix   m_rmsd,m_Qscore,m_seqId;
mmdb::rvector   cons_x,cons_y,cons_z;
mmdb::realtype  rmsd,Qscore;
int             nc,n_align,cons_len;

  nc = 0;
  for (int i=0;i<Input.nStruct;i++)
    nc = mmdb::IMax ( nc,strlen(Input.fstruct[i])+strlen(Input.sel[i]) );

  printStructureSummary ( (gsmt::PPStructure)M,
                          Input.nStruct,isOutput );
  
  printf ( "\n"
           " ===== Superposition matrices (orthogonal):\n" );

  for (int i=0;i<Input.nStruct;i++)  {

    printf ( "\n"
              "   ____________________________" );
    for (int j=0;j<nc;j++)  printf ( "_" );
    printf ( "\n"
             "   (o) For structure %s [%s(%s)]:\n\n"
             "        Rx         Ry         Rz           T\n"
             " %10.3f %10.3f %10.3f   %10.3f\n"
             " %10.3f %10.3f %10.3f   %10.3f\n"
             " %10.3f %10.3f %10.3f   %10.3f\n",
             M[i]->getRefName(),Input.fstruct[i],Input.sel[i],
             M[i]->RT0[0][0],M[i]->RT0[0][1],M[i]->RT0[0][2],
                                                 M[i]->RT0[0][3],
             M[i]->RT0[1][0],M[i]->RT0[1][1],M[i]->RT0[1][2],
                                                 M[i]->RT0[1][3],
             M[i]->RT0[2][0],M[i]->RT0[2][1],M[i]->RT0[2][2],
                                                 M[i]->RT0[2][3] );
    /*
    for (int j=0;j<Input.nStruct;j++)
       printFracAnalysis ( M[i]->RT0,M[i]->getRefName(),
                           M[j]->getMMDBManager() );
    */
  }

  printf ( "\n"
            " ===== Superposition matrices (fractional):\n" );

  MAligner->getAlignScores ( n_align,rmsd,Qscore );

  printf ( "\n"
    " ===== Scores achieved:\n"
    "\n"
     "   quality Q:  %-7.4f (normalised to [0...1])\n"
     "     r.m.s.d:  %-7.4f (A)\n"
     "      Nalign:  %-6i  (residues)\n",
     Qscore,rmsd,n_align );

  MAligner->getConsensusScores ( cons_x,cons_y,cons_z,cons_len,
                                 m_rmsd,m_Qscore,m_seqId );

  print_matrix (
     "(o) pairwise Q-scores (consensus Q-score on diagonal):",
     m_Qscore,"%5.3f",M,Input.nStruct );
  print_matrix (
     "(o) pairwise r.m.s.d. (consensus r.m.s.d. on diagonal):",
     m_rmsd,"%5.3f",M,Input.nStruct );
  print_matrix (
     "(o) pairwise seq. Id:",m_seqId,
     "%5.3f",M,Input.nStruct );

#ifdef _ccp4_
  printf ( "$$\n\n" );
  printf ( "$TEXT:Residue alignment: $$ $$\n\n" );
#else

  printf ( "\n\n"
            " ===== Residue alignment:\n\n" );
#endif

  f.assign ( "stdout" );
  f.rewrite();
  MAligner->writeMultAlign   ( f );
  f.shut();

#ifdef _ccp4_
  printf ( "$$\n" );
#endif

}


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

  writeStructureSummary_csv ( f,(gsmt::PPStructure)M,
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
    /*
    for (int j=0;j<Input.nStruct;j++)  {
      writeFracAnalysis_csv ( L1,M[i]->RT0,M[i]->getRefName(),
                                 M[j]->getMMDBManager() );
      f.Write ( L1 );
    }
    */
    
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

  write_matrix_csv ( f,
     "Pairwise Q-scores (consensus Q-score on diagonal):",
     m_Qscore,"%5.3f",M,Input.nStruct );
  write_matrix_csv ( f,
     "Pairwise r.m.s.d. (consensus r.m.s.d. on diagonal):",
     m_rmsd,"%5.3f",M,Input.nStruct );
  write_matrix_csv ( f,
     "Pairwise seq. Id:",m_seqId,
     "%5.3f",M,Input.nStruct );

  f.Write ( "\n\n"
            "RESIDUE ALIGNMENT\n\n" );
  MAligner->writeMultAlign_csv ( f );

  f.shut();

}


void makeRVAPIMultAlOutput ( gsmt::PMAligner     MAligner,
                             gsmt::PPMAStructure M,
                             int                 nStruct,
                             mmdb::cpstr         rvapi_report_id )  {
mmdb::rmatrix   m_rmsd,m_Qscore,m_seqId;
mmdb::rvector   cons_x,cons_y,cons_z;
mmdb::realtype  rmsd,Qscore;
mmdb::pstr      structTableId   = NULL;
mmdb::pstr      summaryTableId  = NULL;
mmdb::pstr      resAlignSecId   = NULL;
mmdb::pstr      consSecId       = NULL;
mmdb::pstr      resAlignTableId = NULL;
mmdb::pstr      graphId         = NULL;
mmdb::pstr      S               = NULL;
char            L1[1000];
int             n_align,cons_len;
int             srow;
int             row = 0;

  // -----------------------------------------------------------------

  mmdb::CreateCopCat ( structTableId  ,rvapi_struct_tbl_id   ,rvapi_report_id );
  mmdb::CreateCopCat ( summaryTableId ,rvapi_summary_tbl_id  ,rvapi_report_id );
  mmdb::CreateCopCat ( resAlignSecId  ,rvapi_resalign_sec_id ,rvapi_report_id );
  mmdb::CreateCopCat ( consSecId      ,rvapi_consensus_sec_id,rvapi_report_id );
  mmdb::CreateCopCat ( resAlignTableId,rvapi_resalign_tbl_id ,rvapi_report_id );
  mmdb::CreateCopCat ( graphId        ,rvapi_res_graph_id    ,rvapi_report_id );

  // -----------------------------------------------------------------

  rvapi_add_table ( structTableId,"Structure Summary",
                    rvapi_report_id,row,0,1,1, 0 );
  rvapi_set_text  ( " ",rvapi_report_id,row,1,1,1 );
  rvapi_set_text  ( " ",rvapi_report_id,row,2,1,1 );
  row++;

  makeStructureSummary_rvapi ( structTableId,(gsmt::PPStructure)M,
                               nStruct );

  // -----------------------------------------------------------------

  rvapi_set_text  ( "&nbsp;",rvapi_report_id,row++,0,1,3 );
  rvapi_add_table ( summaryTableId,"Alignment Summary",
                    rvapi_report_id,row++,0,1,2, 0 );
                    
  rvapi_put_vert_theader ( summaryTableId,"Q-score","Q-score", 0 );
  rvapi_put_vert_theader ( summaryTableId,"RMSD (&Aring;)",
               "Root Mean Square Deviation between C-alpha atoms",1 );
  rvapi_put_vert_theader ( summaryTableId,"Aligned residues",
               "Number of aligned residues",2 );
  
  MAligner->getAlignScores ( n_align,rmsd,Qscore );

  rvapi_put_table_real ( summaryTableId,Qscore,"%.4f",0,0 );  
  rvapi_put_table_real ( summaryTableId,rmsd,  "%.4f",1,0 );
  rvapi_put_table_int  ( summaryTableId,n_align,2,0 );
  
  sprintf ( L1,"{ 'qscore':%.4f, 'rmsd':%.4f, 'nalign':%i, 'seqid':%.4f }",
               Qscore,rmsd,n_align,0.0 );
  rvapi_put_meta ( L1 );

  // -----------------------------------------------------------------

  MAligner->getConsensusScores ( cons_x,cons_y,cons_z,cons_len,
                                 m_rmsd,m_Qscore,m_seqId );

  rvapi_set_text  ( "&nbsp;",rvapi_report_id,row++,0,1,1 );
  rvapi_add_section ( consSecId,"Pairwise score matrices",
                      rvapi_report_id,row++,0,1,3, false );
  srow = 0;
                      
  makeScoreMatrix_rvapi (
                mmdb::CreateCopCat(S,"pairwise_q_tbl",rvapi_report_id),
                consSecId,srow++,0,1,1, 1,
                "Q-scores (consensus scores on diagonal)",
                m_Qscore,"%.4g",M,nStruct );

  rvapi_set_text  ( "&nbsp;",consSecId,srow++,0,1,1 );

  makeScoreMatrix_rvapi (
                mmdb::CreateCopCat(S,"pairwise_rmsd_tbl",rvapi_report_id),
                consSecId,srow++,0,1,1, 1,
                "R.m.s.d. (consensus values on diagonal)",
                m_rmsd,"%.4g",M,nStruct );

  rvapi_set_text  ( "&nbsp;",consSecId,srow++,0,1,1 );

  makeScoreMatrix_rvapi (
                mmdb::CreateCopCat(S,"pairwise_sec_tbl",rvapi_report_id),
                consSecId,srow,0,1,1, 1,
                "Sequence Identity",
                m_seqId,"%.4g",M,nStruct );

  // -----------------------------------------------------------------

  rvapi_add_section ( resAlignSecId,"Residue alignment",
                      rvapi_report_id,row++,0,1,3, false );
  srow = 0;

  // -----------------------------------------------------------------

  rvapi_add_graph      ( graphId,resAlignSecId,srow++,0,1,1 );
  rvapi_set_graph_size ( graphId,700,400 );

  MAligner->makeMultAlignGraph_rvapi ( graphId );

  rvapi_set_text  ( "<hr/>",resAlignSecId,srow++,0,1,1 );

  // -----------------------------------------------------------------

  rvapi_add_table ( resAlignTableId,"Rigid-body residue alignment",
                    resAlignSecId,srow,0,1,1, -1 );

  MAligner->makeMultAlignTable_rvapi ( resAlignTableId );

  delete[] structTableId  ;
  delete[] summaryTableId ;
  delete[] resAlignSecId  ;
  delete[] consSecId      ;
  delete[] graphId        ;
  delete[] resAlignTableId;
  delete[] S;

}


void multipleAlignment ( gsmt::RInput Input )  {
mmdb::io::File       f;
gsmt::PMAligner      MAligner;
gsmt::PPMAStructure  M;
mmdb::mat44         *T;
mmdb::pstr           rvapiMeta,rvapi_report_id;
mmdb::ERROR_CODE     rc;
int                  alrc,i;
int                  rvapiReport;
bool                 isOutput;

  rvapiMeta = NULL;
  rvapi_report_id = NULL;

  rvapiReport = initRVAPIDomOutput ( Input );
  if (rvapiReport==2)  {
    mmdb::CreateCopy ( rvapiMeta,"" );
    mmdb::CreateCopy ( rvapi_report_id,rvapi_get_meta() );
  } else
    mmdb::CreateCopy ( rvapi_report_id,rvapi_tab_id );

  rc = readStructures ( M,Input );

  printInputParameters ( Input );

  if (rc==mmdb::Error_NoError)  {

    MAligner = new gsmt::MAligner();
    MAligner->setPerformanceLevel     ( Input.mode     );
    MAligner->setSimilarityThresholds ( 0.0,0.0        );
    MAligner->setQR0                  ( Input.QR0      );
    MAligner->setSigma                ( Input.sigma    );
    MAligner->setNThreads             ( Input.nthreads );

    alrc = MAligner->Align ( M,Input.nStruct,true );
    if (alrc==gsmt::GSMT_Ok)  {

#ifdef _ccp4_
      printf ( "$TEXT:Alignment results: $$ $$\n" );
#endif
      
      T = new mmdb::mat44[Input.nStruct];
      for (i=0;i<Input.nStruct;i++)
        mmdb::Mat4Copy ( M[i]->RT0,T[i] );

      isOutput = writeSuperposed ( (gsmt::PPStructure)M,T,
                                   Input.nStruct,Input.outFile,
                                   Input.out_units,Input.out_multi );
      delete[] T;

      writeLogFile ( M,Input,MAligner,isOutput );

      if (Input.outSeqFile)  {
        f.assign ( Input.outSeqFile,true,false );
        f.rewrite();
        MAligner->writeSeqAlignment ( f,Input.fstruct,Input.sel );
        f.shut();
      }

      if (Input.csvFile)
        writeCSVFile ( M,Input,MAligner,isOutput );
        
      if (rvapiReport)
        makeRVAPIMultAlOutput ( MAligner,M,Input.nStruct,rvapi_report_id );

    } else  {
      printf (
        "\n\n ALIGNMENT ERROR %i\n"
        " ===========================================================\n",
        alrc );
    }

    delete MAligner;

    for (int i=0;i<Input.nStruct;i++)
      if (M[i])  delete M[i];
    delete[] M;

  } else  {
    printf (
      "\n\n STOP DUE TO READ ERRORS\n"
      " --- check input file format\n\n"
      " ===========================================================\n"
           );
  }


  if (rvapiReport==2)  {
    //rvapi_put_meta        ( rvapiMeta       );
    rvapi_store_document2 ( Input.rvapiRDoc );
    rvapi_keep_polling    ( true            );
  }
  
  if (rvapiMeta)
    delete[] rvapiMeta;

  if (rvapi_report_id)
    delete[] rvapi_report_id;

}

