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
//    04.02.17   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  Gesamt <implementation>
//       ~~~~~~~~~
//  **** Project :  GESAMT General Gesamt Utilities
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
#include "gesamtlib/gsmt_maligner.h"
#include "gesamtlib/gsmt_utils.h"
#include "gesamtlib/gsmt_defs.h"
#include "rvapi/rvapi_interface.h"

#include "gesamt_utils.h"
#include "gesamt_multiple.h"
#include "gesamt_pairwise.h"

// =================================================================


mmdb::ERROR_CODE __readStructures ( gsmt::PPStructure & M,
                                    bool                forMA,
                                    gsmt::RInput        Input )  {
mmdb::pstr        S;
char              rname[20];
mmdb::ERROR_CODE  rc;
int               i,nc;

  M = new gsmt::PStructure[Input.nStruct];
  for (int i=0;i<Input.nStruct;i++)
    M[i] = NULL;

  S = NULL;
  printf (
    "\n ===========================================================\n" );

  nc = 0;
  rc = mmdb::Error_NoError;
  for (i=0;(i<Input.nStruct) && (rc==mmdb::Error_NoError);i++)  {

    printf ( " ... reading file '%s', selection '%s':\n",
             Input.fstruct[i],Input.sel[i] );
    if (forMA)  M[i] = new gsmt::MAStructure();
          else  M[i] = new gsmt::Structure();
    sprintf ( rname,"S%03i",i+1 );
    M[i]->setRefName ( rname );
    nc = mmdb::IMax ( nc,strlen(Input.fstruct[i])+strlen(Input.sel[i]) );
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
  
  if (rc!=mmdb::Error_NoError)  {
    for (i=0;i<Input.nStruct;i++)
      if (M[i])  delete M[i];
    delete[] M;
    M = NULL;
  }
  
  return rc;

}


mmdb::ERROR_CODE readStructures ( gsmt::PPStructure & M,
                                  gsmt::RInput        Input )  {   

  return __readStructures ( M,false,Input );

}


mmdb::ERROR_CODE readStructures ( gsmt::PPMAStructure & M,
                                  gsmt::RInput          Input )  {
mmdb::ERROR_CODE rc;

  rc = __readStructures ( (gsmt::PPStructure&)M,true,Input );

  if ((rc==mmdb::Error_NoError) && M)  {
    for (int i=0;i<Input.nStruct;i++)
      M[i]->serNo = i;  
  }
  
  return rc;

}


void print_matrix ( mmdb::cpstr name, mmdb::rmatrix m,
                    mmdb::cpstr fmt,  gsmt::PPStructure M,
                    int nStructures )  {
char S[200];
int  i,j,n,k;

  printf ( "\n   " );
  for (i=0;i<(int)strlen(name);i++)
    printf ( "_" );
  printf ( "\n"
           "   %s\n\n",name );

  k = strlen(M[0]->getRefName());
  n = k + 2;
  for (i=0;i<nStructures;i++)
    for (j=0;j<nStructures;j++)  {
      sprintf ( S,fmt,m[i][j] );
      n = mmdb::IMax ( n,strlen(S) );
    }

  printf ( "      " );
  for (i=0;i<k;i++)
    printf ( " " );
  n -= k;
  for (i=0;i<nStructures;i++)  {
    for (j=0;j<n;j++)
      printf ( " " );
    printf ( " %s ",M[i]->getRefName() );
  }
  printf ( "\n" );

  printf ( "      " );
  for (i=0;i<k-1;i++)
    printf ( " " );
  printf ( "." );
  for (i=0;i<nStructures;i++)  {
    for (j=0;j<n+k+2;j++)
      printf ( "-" );
  }
  printf ( "\n" );

  for (i=0;i<nStructures;i++)  {
    printf ( "     %s| ",M[i]->getRefName() );
    for (j=0;j<nStructures;j++)  {
      sprintf ( S,fmt,m[i][j] );
      printf ( " %s",S );
      for (k=0;k<n;k++)
        printf ( " " );
    }
    printf ( "\n" );
  }

}

void print_matrix ( mmdb::cpstr name, mmdb::rmatrix m,
                    mmdb::cpstr fmt,  gsmt::PPMAStructure M,
                    int nStructures )  {
  print_matrix ( name,m,fmt,(gsmt::PPStructure)M,nStructures );
}

void write_matrix ( mmdb::io::RFile f,
                    mmdb::cpstr name, mmdb::rmatrix m,
                    mmdb::cpstr fmt,  gsmt::PPStructure M,
                    int nStructures )  {
char S[2000];
int  i,j,n,k;

  f.Write ( "\n   " );
  for (i=0;i<(int)strlen(name);i++)
    f.Write ( "_" );
  sprintf ( S,"\n"
              "   %s\n\n",name );
  f.Write ( S );

  k = strlen(M[0]->getRefName());
  n = k + 2;
  for (i=0;i<nStructures;i++)
    for (j=0;j<nStructures;j++)  {
      sprintf ( S,fmt,m[i][j] );
      n = mmdb::IMax ( n,strlen(S) );
    }

  f.Write ( "      " );
  for (i=0;i<k;i++)
    f.Write ( " " );
  n -= k;
  for (i=0;i<nStructures;i++)  {
    for (j=0;j<n;j++)
      f.Write ( " " );
    sprintf ( S," %s ",M[i]->getRefName() );
    f.Write ( S );
  }
  f.LF();

  f.Write ( "      " );
  for (i=0;i<k-1;i++)
    f.Write ( " " );
  f.Write ( "." );
  for (i=0;i<nStructures;i++)  {
    for (j=0;j<n+k+2;j++)
      f.Write ( "-" );
  }
  f.LF();

  for (i=0;i<nStructures;i++)  {
    sprintf ( S,"     %s| ",M[i]->getRefName() );
    f.Write ( S );
    for (j=0;j<nStructures;j++)  {
      f.Write ( " " );
      sprintf ( S,fmt,m[i][j] );
      f.Write ( S );
      for (k=0;k<n;k++)
        f.Write ( " " );
    }
    f.LF();
  }

}

void write_matrix ( mmdb::io::RFile f,
                    mmdb::cpstr name, mmdb::rmatrix m,
                    mmdb::cpstr fmt,  gsmt::PPMAStructure M,
                    int nStructures )  {
  write_matrix ( f,name,m,fmt,(gsmt::PPStructure)M,nStructures );
}


void write_matrix_csv ( mmdb::io::RFile f,
                        mmdb::cpstr name, mmdb::rmatrix m,
                        mmdb::cpstr fmt,  gsmt::PPStructure M,
                        int nStructures )  {
char S[100];
int  i,j;

  f.LF();
  f.WriteLine ( name );
  f.LF();

  for (i=0;i<nStructures;i++)  {
    f.Write ( ", " );
    f.Write ( M[i]->getRefName() );
  }
  f.LF();

  for (i=0;i<nStructures;i++)  {
    f. Write ( M[i]->getRefName() );
    for (j=0;j<nStructures;j++)  {
      f.Write ( ", " );
      sprintf ( S,fmt,m[i][j] );
      f.Write ( S );
    }
    f.LF();
  }

}

void write_matrix_csv ( mmdb::io::RFile f,
                        mmdb::cpstr name, mmdb::rmatrix m,
                        mmdb::cpstr fmt,  gsmt::PPMAStructure M,
                        int nStructures )  {
  write_matrix_csv ( f,name,m,fmt,(gsmt::PPStructure)M,nStructures );
}


void printInputParameters ( gsmt::RInput Input )  {

  printf ( "\n Parameter Q-score:                %.3f angstroms\n",
           Input.QR0 );
  if (Input.sigma<=0.0)
       printf ( " Weighted superposition is not used\n" );
  else printf ( " Sigma for weighted superposition: %.3f angstroms\n",
                Input.sigma );
  printf ( " Number of threads used:           %i\n\n",Input.nthreads );

}


void printHorzLine ( char C, int len )  {
  printf ( "\n " );
  for (int i=1;i<len;i++)
    printf ( "%c",C );
  printf ( "\n" );
}


void printSuperpositionData2 ( mmdb::cpstr          title,
                               gsmt::PSuperposition SD,
                               char sep, int printWidth,
                               int qSize, int tSize )  {

  printHorzLine ( sep,printWidth );

  printf (
    "\n%s\n"
    "\n"
    " Q-score          : %-10.3f\n"
    " RMSD             : %-10.3f\n"
    " Aligned residues : %i\n"
    " Sequence Id:     : %-10.3f\n",
    title,
    SD->Q,SD->rmsd,SD->Nalgn,SD->seqId
  );
  
  if (qSize>=0)
    printf ( " Size of FIXED structure : %i\n",qSize );
  if (tSize>=0)
    printf ( " Size of MOVING structure: %i\n",tSize );
  
  printf (
    "\n"
    " Transformation matrix for FIXED structure is identity.\n"
    "\n"
    " Transformation matrix for MOVING structure:\n"
    "\n"
    "          Rx           Ry           Rz             T\n"
    " %12.5f %12.5f %12.5f   %12.5f\n"
    " %12.5f %12.5f %12.5f   %12.5f\n"
    " %12.5f %12.5f %12.5f   %12.5f\n",
    SD->T[0][0],SD->T[0][1],SD->T[0][2],SD->T[0][3],
    SD->T[1][0],SD->T[1][1],SD->T[1][2],SD->T[1][3],
    SD->T[2][0],SD->T[2][1],SD->T[2][2],SD->T[2][3]
  );

}


void printSuperpositionData2 ( mmdb::cpstr    title,
                               gsmt::PDomData DD,
                               char sep, int printWidth )  {
gsmt::PMSuperposition SD;

  SD = DD->SD[0];

  printHorzLine ( sep,printWidth );

  printf (
    "\n%s\n"
    "\n"
    "                  | Rigid-Body | Combined Domains\n"
    "                  |  Alignment |    Alignment\n"
    " =================+============+=================\n"
    " Q-score          | %-10.4f |  %-10.4f\n"
    " RMSD             | %-10.4f |  %-10.4f\n"
    " Aligned residues | %-10i |  %-10i\n"
    " Sequence Id      | %-10.4f |  %-10.4f\n"
    " -----------------+------------'-----------------\n"
    " Size of FIXED    | %i\n"
    " Size of MOVING   | %i\n",
    title,
    SD->Q,     DD->Q,
    SD->rmsd,  DD->rmsd,
    SD->Nalgn, DD->Nalign,
    SD->seqId, DD->seqId,
    DD->sSize[0],
    DD->sSize[1]
  );
  
  printf (
    "\n"
    " Transformation matrix for FIXED structure is identity.\n"
    "\n"
    " Transformation matrix for MOVING structure:\n"
    "\n"
    "          Rx           Ry           Rz             T\n"
    " %12.5f %12.5f %12.5f   %12.5f\n"
    " %12.5f %12.5f %12.5f   %12.5f\n"
    " %12.5f %12.5f %12.5f   %12.5f\n",
    SD->T[1][0][0],SD->T[1][0][1],SD->T[1][0][2],SD->T[1][0][3],
    SD->T[1][1][0],SD->T[1][1][1],SD->T[1][1][2],SD->T[1][1][3],
    SD->T[1][2][0],SD->T[1][2][1],SD->T[1][2][2],SD->T[1][2][3]
  );

}


void printSuperpositionData2_rvapi ( mmdb::cpstr    title,
                                     gsmt::PDomData DD )  {
gsmt::PMSuperposition SD;

  SD = DD->SD[0];


  printf (
    "\n%s\n"
    "\n"
    "                  | Rigid-Body | Combined Domains\n"
    "                  |  Alignment |    Alignment\n"
    " =================+============+=================\n"
    " Q-score          | %-10.4f |  %-10.4f\n"
    " RMSD             | %-10.4f |  %-10.4f\n"
    " Aligned residues | %-10i |  %-10i\n"
    " Sequence Id      | %-10.4f |  %-10.4f\n"
    " -----------------+------------'-----------------\n"
    " Size of FIXED    | %i\n"
    " Size of MOVING   | %i\n",
    title,
    SD->Q,     DD->Q,
    SD->rmsd,  DD->rmsd,
    SD->Nalgn, DD->Nalign,
    SD->seqId, DD->seqId,
    DD->sSize[0],
    DD->sSize[1]
  );
  
  printf (
    "\n"
    " Transformation matrix for FIXED structure is identity.\n"
    "\n"
    " Transformation matrix for MOVING structure:\n"
    "\n"
    "          Rx           Ry           Rz             T\n"
    " %12.5f %12.5f %12.5f   %12.5f\n"
    " %12.5f %12.5f %12.5f   %12.5f\n"
    " %12.5f %12.5f %12.5f   %12.5f\n",
    SD->T[1][0][0],SD->T[1][0][1],SD->T[1][0][2],SD->T[1][0][3],
    SD->T[1][1][0],SD->T[1][1][1],SD->T[1][1][2],SD->T[1][1][3],
    SD->T[1][2][0],SD->T[1][2][1],SD->T[1][2][2],SD->T[1][2][3]
  );

}


void makeFileName ( mmdb::pstr & newFName,
                    mmdb::cpstr  oldFName,
                    mmdb::cpstr  preSuffix,
                    int          mod )  {
char newSuffix[200];
mmdb::pstr p;

  mmdb::CreateCopy ( newFName,oldFName );
  
  if (!oldFName)  return;

  p = mmdb::LastOccurence ( newFName,'.');

  if (mod<0)  {
    strcpy ( newSuffix,preSuffix );
    if (p)
      strcat ( newSuffix,p );
  } else if (p)
    sprintf ( newSuffix,"%s_%03i%s",preSuffix,mod,p );
  else
    sprintf ( newSuffix,"%s_%03i",preSuffix,mod );

  if (p)  *p = char(0);
  mmdb::CreateConcat ( newFName,".",newSuffix );

}



int initRVAPIDomOutput ( gsmt::RInput Input )  {

  if (Input.rvapiRDir)  {
    rvapi_init_document ( "gesamt_report",Input.rvapiRDir,
                          "GESAMT - Pairwise Alignment",
                          RVAPI_MODE_Html,
                          RVAPI_LAYOUT_Tabs,
                          Input.rvapiJSUri,
                          NULL,NULL,NULL,NULL );
    rvapi_add_tab ( rvapi_tab_id,rvapi_tab_name,true );
    rvapi_set_text  ( "<h2>GESAMT Pairwise Alignment</h2>",
                      rvapi_tab_id,0,0,1,1 );
    return 1;
  } else if (Input.rvapiRDoc)  {
    rvapi_restore_document2 ( Input.rvapiRDoc );
    return 2;
  }
  
  return 0;

}


void makeScoreMatrix_rvapi ( mmdb::cpstr  widgetId,
                             mmdb::cpstr    gridId,
                             int               row,
                             int               col,
                             int           rowSpan,
                             int           colSpan,
                             int         openState,
                             mmdb::cpstr      name,
                             mmdb::rmatrix       m,
                             mmdb::cpstr       fmt,
                             gsmt::PPMAStructure M,
                             int           nStruct )  {
char S[200];
int  i,j;

  rvapi_add_table ( widgetId,name,gridId,row,col,rowSpan,colSpan,
                    openState );
                    
  for (i=0;i<nStruct;i++)  {
    sprintf ( S,"Structure #%i",i+1 );
    rvapi_put_horz_theader ( widgetId,M[i]->getRefName(),S,i );
  }

  for (i=0;i<nStruct;i++)  {
    sprintf ( S,"Structure #%i",i+1 );
    rvapi_put_vert_theader ( widgetId,M[i]->getRefName(),S,i );
    for (j=0;j<nStruct;j++)
      rvapi_put_table_real ( widgetId,m[i][j],fmt,i,j );
  }
}
