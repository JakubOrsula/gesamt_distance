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
//  **** Project :  GESAMT Multiple Alignment Driver
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
#include "gesamtlib/gsmt_domains.h"
#include "gesamtlib/gsmt_utils.h"
#include "gesamtlib/gsmt_defs.h"
#include "rvapi/rvapi_interface.h"

#include "gesamt_domains.h"
#include "gesamt_utils.h"

// =================================================================


void makeRVAPIDomOutput ( gsmt::PPStructure structs,
                          gsmt::PDomains    D,
                          mmdb::cpstr       rvapi_report_id )  {
gsmt::PDomData        domData;
gsmt::PMSuperposition MSD;
gsmt::PSuperposition  SD;
char                  L1[1000],L2[100];
mmdb::pstr            structTableId  = NULL;
mmdb::pstr            summaryTableId = NULL;
mmdb::pstr            resAlignSecId  = NULL;
mmdb::pstr            dom0TableId    = NULL;
mmdb::pstr            domsTableId    = NULL;
mmdb::pstr            graphId        = NULL;
mmdb::pstr            S    = NULL;
int                   row  = 0;
int                   srow = 0;
int                   i;

  // -----------------------------------------------------------------

  mmdb::CreateCopCat ( structTableId ,rvapi_struct_tbl_id  ,rvapi_report_id );
  mmdb::CreateCopCat ( summaryTableId,rvapi_summary_tbl_id ,rvapi_report_id );
  mmdb::CreateCopCat ( resAlignSecId ,rvapi_resalign_sec_id,rvapi_report_id );
  mmdb::CreateCopCat ( dom0TableId   ,rvapi_domain0_tbl_id ,rvapi_report_id );
  mmdb::CreateCopCat ( domsTableId   ,rvapi_domains_tbl_id ,rvapi_report_id );
  mmdb::CreateCopCat ( graphId       ,rvapi_res_graph_id   ,rvapi_report_id );

  // -----------------------------------------------------------------

  domData = D->getDomains();

  // -----------------------------------------------------------------

  rvapi_add_table ( structTableId,"Structure Summary",
                    rvapi_report_id,row,0,1,1, 0 );
  rvapi_set_text  ( " ",rvapi_report_id,row,1,1,1 );
  rvapi_set_text  ( " ",rvapi_report_id,row,2,1,1 );
  row++;

  makeStructureSummary_rvapi ( structTableId,structs,2 );

  // -----------------------------------------------------------------

  rvapi_set_text  ( "&nbsp;",rvapi_report_id,row++,0,1,3 );
  rvapi_add_table ( summaryTableId,"Alignment Summary",
                    rvapi_report_id,row++,0,1,2, 0 );
                    
  MSD = domData->SD[0];

  rvapi_put_horz_theader ( summaryTableId,
                           "Rigid-Body<br>Alignment",
                           "Full rigid-body alignment data",
                           0 );
  rvapi_put_horz_theader ( summaryTableId,
                           "Combined Domains<br>Alignment",
                           "Full per-domain alignment data",
                           1 );

  rvapi_put_vert_theader   ( summaryTableId,"Q-score","Q-score", 0 );
  rvapi_put_vert_theader   ( summaryTableId,"RMSD (&Aring;)",
               "Root Mean Square Deviation between C-alpha atoms",1 );
  rvapi_put_vert_theader   ( summaryTableId,"Aligned residues",
               "Number of aligned residues",2 );
  rvapi_put_vert_theader   ( summaryTableId,"Sequence Id",
         "Sequence identity calculated from structural alignment",3 );
  
  rvapi_put_table_real ( summaryTableId,MSD->Q    ,"%.4f",0,0 );
  rvapi_put_table_real ( summaryTableId,domData->Q,"%.4f",0,1 );
  
  rvapi_put_table_real ( summaryTableId,MSD->rmsd,    "%.4f",1,0 );
  rvapi_put_table_real ( summaryTableId,domData->rmsd,"%.4f",1,1 );

  rvapi_put_table_int  ( summaryTableId,MSD->Nalgn,2,0 );
  rvapi_put_table_int  ( summaryTableId,domData->Nalign,2,1 );
  
  rvapi_put_table_real ( summaryTableId,MSD->seqId,     "%.4f",3,0 );
  rvapi_put_table_real ( summaryTableId,domData->seqId,"%.4f",3,1 );
  
  sprintf ( L1,"{ 'qscore':%.4f, 'rmsd':%.4f, 'nalign':%i, 'seqid':%.4f }",
               MSD->Q,MSD->rmsd,MSD->Nalgn,MSD->seqId );
  rvapi_put_meta ( L1 );
  
  for (i=1;i<domData->nDomains;i++)  {
    sprintf ( L1,"Domain %i",i );
    sprintf ( L2,"Domain #%i alignment data",i );
    rvapi_put_horz_theader ( summaryTableId,L1,L2,i+1 );
    rvapi_put_table_real ( summaryTableId,domData->SD[i]->Q,"%.4f",0,i+1 );
    rvapi_put_table_real ( summaryTableId,domData->SD[i]->rmsd,"%.4f",1,i+1 );
    rvapi_put_table_int  ( summaryTableId,domData->SD[i]->Nalgn,2,i+1 );
    rvapi_put_table_real ( summaryTableId,domData->SD[i]->seqId,"%.4f",3,i+1 );
  }

  // -----------------------------------------------------------------

  rvapi_set_text    ( "&nbsp;",rvapi_report_id,row++,0,1,1 );
  rvapi_add_section ( resAlignSecId,"Residue alignment",
                      rvapi_report_id,row++,0,1,3, false );

  // -----------------------------------------------------------------

  rvapi_add_graph ( graphId,resAlignSecId,srow++,0,1,2 );
  rvapi_set_graph_size ( graphId,700,400 );

  SD = new gsmt::Superposition();
  MSD->CopyTo ( SD );

  D->resetStructureData   ( structs[0],structs[1] );
  D->makeAlignGraph_rvapi ( graphId,SD );

  rvapi_set_text  ( "<hr/>",resAlignSecId,srow++,0,1,2 );

  // -----------------------------------------------------------------

  rvapi_add_table ( dom0TableId,"Rigid-body residue alignment",
                    resAlignSecId,srow,0,1,1, -1 );
  D->makeAlignTable_rvapi ( dom0TableId,SD );

  // -----------------------------------------------------------------

  rvapi_add_table ( domsTableId,"Per-domain residue alignment",
                    resAlignSecId,srow,1,1,1, -1 );

  D->makeDomAlignTable_rvapi ( domsTableId,structs[0],structs[1] );

  rvapi_set_cell_stretch ( resAlignSecId,45,0,srow,0 );
  rvapi_set_cell_stretch ( resAlignSecId,55,0,srow,1 );

  // -----------------------------------------------------------------

  delete[] structTableId ;
  delete[] summaryTableId;
  delete[] resAlignSecId ;
  delete[] dom0TableId   ;
  delete[] domsTableId   ;
  delete[] graphId       ;

  if (S)  delete[] S;
  if (SD) delete  SD;
 
}


void analyseDomains ( gsmt::RInput Input )  {
gsmt::PDomains        D;
gsmt::PPStructure     M;
gsmt::PSuperposition  SD;
gsmt::PStructure      structs[2];
gsmt::PDomData        domData;
mmdb::io::File        f;
mmdb::pstr            outFName;
mmdb::pstr            rvapiMeta,rvapi_report_id;
char                  S[1000];
bool                  isOutput;
int                   i,printWidth;
mmdb::ERROR_CODE      rc;
gsmt::GSMT_CODE       gsmt_rc;
int                   rvapiReport;

  printWidth = 80;

  SD        = NULL;
  outFName  = NULL;
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

    M[0]->setRefName ( "FIXED"  );
    M[1]->setRefName ( "MOVING" );
    structs[0] = M[0];
    structs[1] = M[1];
  
    printStructureSummary ( structs,2,false );

    D = new gsmt::Domains();
    D->setPerformanceLevel ( Input.mode      );
    D->setQR0              ( Input.QR0       );
    D->setSigma            ( Input.sigma     );
    D->setNThreads         ( Input.nthreads  );
    D->setVerbosity        ( Input.verbosity );

    gsmt_rc = D->findDomains ( M[0],M[1] );

    if (gsmt_rc==gsmt::GSMT_Ok)  {

      domData = D->getDomains();

      makeFileName ( outFName,Input.outFile,"full",-1 );
      isOutput = gsmt::writeSuperposed ( structs,domData->SD[0]->T,2,
                                         outFName,
                                         Input.out_units,
                                         Input.out_multi );
      if (rvapiReport==2)
        mmdb::CreateConcat ( rvapiMeta,outFName,"\n" );

      printSuperpositionData2 ( " FULL SUPERPOSITION\n"
                                " ~~~~~~~~~~~~~~~~~~",
                                domData,'=',printWidth );

      printf ( "\n"
               " RESIDUE ALIGNMENT\n"
               " -----------------\n"  );

      SD = new gsmt::Superposition();
      domData->SD[0]->CopyTo ( SD );
      f.assign ( "stdout" );
      f.rewrite();
      D->resetStructureData ( M[0],M[1] );
      D->writeAlignTable ( f,SD );
      f.shut();

      printf ( "\n" );
      printHorzLine ( '=',printWidth );

      printf ( "\n ... total %i domains found\n",domData->nDomains-1 );

      for (i=1;i<domData->nDomains;i++)  {

        domData->SD[i]->CopyTo ( SD );

        sprintf ( S," DOMAIN %i",i );
        printSuperpositionData2 ( S,SD,'-',printWidth,
                                  domData->SD[i]->dSize[0],
                                  domData->SD[i]->dSize[1] );

        makeFileName ( outFName,Input.outFile,"domain",i );
        isOutput = gsmt::writeSuperposed ( structs,domData->SD[i]->T,2,
                                           outFName,
                                           Input.out_units,
                                           Input.out_multi );
        if (rvapiReport==2)
          mmdb::CreateConcat ( rvapiMeta,outFName,"\n" );

        if (Input.viewer && (i==Input.viewerData))
          Input.viewer->View ( M[0],M[1],SD,NULL,NULL );

      }

      printHorzLine ( '=',printWidth );
      printf ( "\n"
               " RESIDUE ALIGNMENT IN DOMAINS\n"
               " ----------------------------\n"  );

      f.assign ( "stdout" );
      f.rewrite();
      D->writeDomAlignTable ( M[0],M[1],f );
      f.shut();

      if (rvapiReport)
        makeRVAPIDomOutput ( M,D,rvapi_report_id );

      if (Input.viewer && (Input.viewerData==0))  {
        if (!SD)
          SD = new gsmt::Superposition();
        domData->SD[0]->CopyTo ( SD );
        Input.viewer->View ( M[0],M[1],SD,NULL,NULL );
      }
    
    } else
      printf ( " ***** alignment failed (%i)\n",rc );
    
    delete D;

    for (int i=0;i<Input.nStruct;i++)
      if (M[i])  delete M[i];
    delete[] M;

    printHorzLine ( '=',printWidth );

  } else  {
    printf (
      "\n\n STOP DUE TO READ ERRORS\n"
      " --- check input file format\n\n"
      " ===========================================================\n"
           );
  }

  if (outFName) delete[] outFName;
  if (SD)       delete   SD;

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

