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
//    25.01.17   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  GSMT_Structure <implementation>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::Structure
//       ~~~~~~~~~
//
//  (C) E. Krissinel, 2008-2017
//
// =================================================================
//

#include <string.h>
#include <math.h>
#include <ctime>

#include "mmdb2/mmdb_tables.h"
#include "rvapi/rvapi_interface.h"

#include "gsmt_structure.h"

// =================================================================

gsmt::Structure::Structure() : mmdb::io::Stream()  {
  initClass();
}

gsmt::Structure::Structure ( mmdb::io::RPStream Object )
               : mmdb::io::Stream(Object)  {
  initClass();
}

gsmt::Structure::~Structure()  {
  freeMemory();
}

void  gsmt::Structure::initClass()  {
  refName[0] = char(0); // reference name for output
  M          = NULL;    // the structure
  A          = NULL;    // selected atoms
  D          = NULL;    // distance matrix
  xc         = 0.0;     // mass center x
  yc         = 0.0;     // mass center y
  zc         = 0.0;     // mass center z
  R          = 0.0;     // embedding radius
  natoms     = 0;       // number of selected atoms
  selection  = NULL;    // selection string
  filePath   = NULL;    // path to file read
  outFile    = NULL;    // path to output file
  outNote    = NULL;    // output note
  scopSel    = false;   // true if SCOP selection
  selDHnd    = 0;       // domain selection
  selAHnd    = 0;       // Ca-selection
  selRHnd    = 0;       // residue selection
  nd_alloc   = 0;
}

void  gsmt::Structure::freeMemory() {
  if (M)          delete M;
  if (selection)  delete[] selection;
  if (filePath)   delete[] filePath;
  if (outFile)    delete[] outFile;
  if (outNote)    delete[] outNote;
  mmdb::FreeMatrixMemory ( D,nd_alloc,0,0 );
  M         = NULL;  // the structure
  A         = NULL;  // selected atoms
  D         = NULL;  // distance matrix
  xc        = 0.0;   // mass center x
  yc        = 0.0;   // mass center y
  zc        = 0.0;   // mass center z
  R         = 0.0;   // embedding radius
  natoms    = 0;     // number of selected atoms
  selection = NULL;  // selection string
  filePath  = NULL;  // path to file read
  outFile   = NULL;  // path to output file
  outNote   = NULL;  // output note
  scopSel   = false; // true if SCOP selection
  selDHnd   = 0;     // domain selection
  selAHnd   = 0;     // Ca-selection
  selRHnd   = 0;     // residue selection
}

void  gsmt::Structure::selectCalphasSCOP ( mmdb::cpstr select )  {
// select is of the following format:
//    "*", "(all)"            - take all file
//    "-"                     - take chain without chain ID
//    "a:Ni-Mj,b:Kp-Lq,..."   - take chain a residue number N
//                              insertion code i to residue number M
//                              insertion code j plus chain b
//                              residue number K insertion code p to
//                              residue number L insertion code q and
//                              so on.
//    "a:,b:..."              - take whole chains a and b and so on
//    "a:,b:Kp-Lq,..."        - any combination of the above.
int  rc, mdl;

  mmdb::CreateCopy ( selection,select );
  scopSel = true;

  if (!M)  {
    selDHnd = 0;
    selAHnd = 0;
    selRHnd = 0;
    return;
  }

  if (!selDHnd)  selDHnd = M->NewSelection();
  if (!selAHnd)  selAHnd = M->NewSelection();
  if (!selRHnd)  selRHnd = M->NewSelection();

  mdl = M->GetFirstModelNum();
  rc  = M->SelectDomain ( selDHnd,select,mmdb::STYPE_ATOM,
                          mmdb::SKEY_NEW,mdl );

  if (!rc)  {

    M->Select ( selAHnd,mmdb::STYPE_ATOM,selDHnd,mmdb::SKEY_NEW );
    M->Select ( selAHnd,mmdb::STYPE_ATOM,mdl,"*",
                        mmdb::ANY_RES,"*",mmdb::ANY_RES,"*",
                        "*","[ CA ]","*","*",mmdb::SKEY_AND );

    deselectAltLocs();

    M->Select ( selRHnd,mmdb::STYPE_RESIDUE,selAHnd,mmdb::SKEY_NEW );

  } else  {
    M->DeleteAllSelections();
    selDHnd = 0;
    selAHnd = 0;
    selRHnd = 0;
  }

}

void  gsmt::Structure::selectSubStructure ( mmdb::cpstr CID )  {

  if ((!M) || (!selAHnd))  {
    selDHnd = 0;
    selAHnd = 0;
    selRHnd = 0;
    return;
  }

  if (!CID)  {
    // make original selection
    if (scopSel)  selectCalphasSCOP ( selection );
            else  selectCalphasCID  ( selection );
  } else  {
    M->Select ( selAHnd,mmdb::STYPE_ATOM   ,CID,mmdb::SKEY_AND );
    M->Select ( selRHnd,mmdb::STYPE_RESIDUE,CID,mmdb::SKEY_AND );
  }

}


void  gsmt::Structure::selectAtom ( mmdb::PAtom         atom,
                                    mmdb::SELECTION_KEY selKey )  {
  if (M && selAHnd)
    M->SelectAtom ( selAHnd,atom,selKey,false );                                    
}

void  gsmt::Structure::finishAtomSelection()  {
  if (M && selAHnd)  {
    M->MakeSelIndex ( selAHnd );
    M->Select ( selRHnd,mmdb::STYPE_RESIDUE,selAHnd,mmdb::SKEY_NEW );
    mmdb::FreeMatrixMemory   ( D,nd_alloc,0,0 );
    natoms = M->GetSelLength ( selAHnd );
    M->RemoveBricks();
  }
}


void  gsmt::Structure::selectCalphasCID ( mmdb::cpstr CID )  {
mmdb::pstr sel;
int        mdl;

  if (selection!=CID)
    mmdb::CreateCopy ( selection,CID );
  scopSel = false;

  if (!M)  {
    selDHnd = 0;
    selAHnd = 0;
    selRHnd = 0;
    return;
  }

  if (!selAHnd)  selAHnd = M->NewSelection();
  if (!selRHnd)  selRHnd = M->NewSelection();

  sel = NULL;
  mmdb::CreateCopy ( sel,CID );
  if (!strcmp(sel,"-all"))
    mmdb::CreateCopy ( sel,"*" );
  M->Select ( selAHnd,mmdb::STYPE_ATOM,sel,mmdb::SKEY_NEW );

  if (sel[0]=='/')  mdl = 0;
              else  mdl = M->GetFirstModelNum();
  M->Select ( selAHnd,mmdb::STYPE_ATOM,mdl,"*",
                      mmdb::ANY_RES,"*",mmdb::ANY_RES,"*",
                      "*","[ CA ]","*","*",mmdb::SKEY_AND );

  if (M->GetSelLength(selAHnd)<=0)  {
    M->DeleteAllSelections();
    selDHnd = 0;
    selAHnd = 0;
    selRHnd = 0;
  } else  {
    deselectAltLocs();
    M->Select ( selRHnd,mmdb::STYPE_RESIDUE,selAHnd,mmdb::SKEY_NEW );
  }

  if (sel)  delete[] sel;

}

void  gsmt::Structure::deselectAltLocs()  {
// deselects extra altlocs, if any found
mmdb::PPAtom  A;
mmdb::ChainID chID;
mmdb::ResName rName;
mmdb::InsCode iCode;
mmdb::AltLoc  aLoc;
int           natoms,i,j,k, seqNum;
bool          B;

  M->GetSelIndex ( selAHnd,A,natoms );
  i = 0;
  k = 0;
  while (i<natoms)  {
    if (A[i])  {
      seqNum = A[i]->GetSeqNum();
      strcpy ( chID ,A[i]->GetChainID() );
      strcpy ( rName,A[i]->GetResName() );
      strcpy ( iCode,A[i]->GetInsCode() );
      strcpy ( aLoc ,A[i]->altLoc       );
      B = true;
      j = i;
      i++;
      while ((i<natoms) && B)
        if (A[i])  {
          if ((A[i]->GetSeqNum()==seqNum)         &&
              (!strcmp(A[i]->GetInsCode(),iCode)) &&
              (!strcmp(A[i]->GetResName(),rName)) &&
              (!strcmp(A[i]->GetChainID(),chID )))  {
            if (A[i]->occupancy<A[j]->occupancy)  {
              M->SelectAtom ( selAHnd,A[i],mmdb::SKEY_CLR,false );
              k++;
            } else if (A[i]->occupancy>A[j]->occupancy)  {
              strcpy ( aLoc,A[i]->altLoc );
              M->SelectAtom ( selAHnd,A[j],mmdb::SKEY_CLR,false );
              k++;
              j = i;
            } else if (!aLoc[0])  {
              M->SelectAtom ( selAHnd,A[i],mmdb::SKEY_CLR,false );
              k++;
            } else if (!A[i]->altLoc[0])  {
              strcpy ( aLoc,A[i]->altLoc );
              M->SelectAtom ( selAHnd,A[j],mmdb::SKEY_CLR,false );
              k++;
              j = i;
            } else if (strcmp(aLoc,A[i]->altLoc)<=0)  {
              M->SelectAtom ( selAHnd,A[i],mmdb::SKEY_CLR,false );
              k++;
            } else  {
              strcpy ( aLoc,A[i]->altLoc );
              M->SelectAtom ( selAHnd,A[j],mmdb::SKEY_CLR,false );
              k++;
              j = i;
            }
            i++;
          } else
            B = false;
        } else
          i++;
    } else
      i++;
  }

  if (k)  M->MakeSelIndex ( selAHnd );

}


mmdb::ERROR_CODE gsmt::Structure::getStructure (
                                         mmdb::cpstr fname,
                                         mmdb::cpstr select,
                                         int         chainNo,
                                         bool        SCOPSelSyntax )  {
mmdb::ERROR_CODE rc;
bool             newRead;

  mmdb::FreeMatrixMemory ( D,nd_alloc,0,0 );
  
  if (!filePath)                     newRead = true;
  else if (!strcmp(fname,filePath))  newRead = false;
                               else  newRead = true;

  if (!M)  {
    M = new mmdb::Manager();
    newRead = true;
  } else
    M->DeleteAllSelections();

  selDHnd = 0;
  selAHnd = 0;
  selRHnd = 0;

  xc = 0.0;   // mass center x
  yc = 0.0;   // mass center y
  zc = 0.0;   // mass center z
  R  = 0.0;   // embedding radius

  if (newRead)  {

    M->SetFlag ( mmdb::MMDBF_IgnoreDuplSeqNum       |
                 mmdb::MMDBF_IgnoreNonCoorPDBErrors |
                 mmdb::MMDBF_IgnoreBlankLines       |
                 mmdb::MMDBF_IgnoreUnmatch          |
                 mmdb::MMDBF_IgnoreHash             |
                 mmdb::MMDBF_IgnoreRemarks          |
                 mmdb::MMDBF_DoNotProcessSpaceGroup );

    rc = M->ReadCoorFile ( fname );
    mmdb::CreateCopy ( filePath,fname );

  } else
    rc = mmdb::Error_Ok;

  if (rc==mmdb::Error_Ok)
    rc = analyseStructure ( M,select,chainNo,SCOPSelSyntax );

  return rc;

}

mmdb::ERROR_CODE gsmt::Structure::getStructure  (
                                       mmdb::PManager MMDB,
                                       mmdb::cpstr    select,
                                       int            chainNo,
                                       bool           SCOPSelSyntax )  {

  mmdb::FreeMatrixMemory ( D,nd_alloc,0,0 );
  
  if (M)  {
    delete M;
    M = NULL;
  }

  mmdb::CreateCopy ( filePath,"" );
  
  selDHnd = 0;
  selAHnd = 0;
  selRHnd = 0;

  xc = 0.0;   // mass center x
  yc = 0.0;   // mass center y
  zc = 0.0;   // mass center z
  R  = 0.0;   // embedding radius

  return analyseStructure ( MMDB,select,chainNo,SCOPSelSyntax );
                              
}


mmdb::ERROR_CODE gsmt::Structure::analyseStructure (
                                    mmdb::PManager MMDB,
                                    mmdb::cpstr    select,
                                    int            chainNo,
                                    bool           SCOPSelSyntax )  {
mmdb::PModel   model;
mmdb::PPChain  chain;
mmdb::PPAtom   A;
mmdb::ChainID  chID;
mmdb::realtype dx,dy,dz, d;
int            nc,i;

  M = MMDB;

  // Calculate secondary structure ONLY FOR FINAL OUTPUT!
  model = M->GetFirstDefinedModel();
  if (model)
    model->CalcSecStructure();

  if ((!select) && (chainNo>=0))  {
    model->GetChainTable ( chain,nc );
    if (chainNo<nc)  {
      strcpy ( chID,chain[chainNo]->GetChainID() );
      if (!chID[0])  strcpy ( chID,"-" );
               else  strcat ( chID,":" );
      selectCalphasSCOP ( chID );
    } else
      return mmdb::Error_GeneralError1;
  } else if (SCOPSelSyntax)
    selectCalphasSCOP ( select );
  else
    selectCalphasCID ( select );

  M->GetSelIndex ( selAHnd,A,natoms );

  if (natoms>0)  {

    for (i=0;i<natoms;i++)  {
      xc += A[i]->x;
      yc += A[i]->y;
      zc += A[i]->z;
    }

    xc /= natoms;
    yc /= natoms;
    zc /= natoms;
    for (i=0;i<natoms;i++)  {
      dx = A[i]->x - xc;
      dy = A[i]->y - yc;
      dz = A[i]->z - zc;
      d  = dx*dx + dy*dy + dz*dz;
      if (d>R)  R = d;
    }
    R = sqrt ( R );

  }

  return mmdb::Error_Ok;

}



mmdb::pstr  gsmt::Structure::getErrDesc ( mmdb::ERROR_CODE rc,
                                          mmdb::pstr & S )  {
char L[500],ibuf[100];
int  lcount;

  sprintf ( ibuf,"%i",rc );
  mmdb::CreateCopCat ( S," ***** ERROR #",ibuf," READ:\n\n ",
                       mmdb::GetErrorDescription(rc),"\n\n" );
  if (M)  {
    M->GetInputBuffer ( L,lcount );
    sprintf ( ibuf,"%i",lcount );
    if (lcount>=0)
      mmdb::CreateConcat ( S,"       LINE #",ibuf,":\n",L,"\n\n" );
    else if (lcount==-1)
      mmdb::CreateConcat ( S,"       CIF ITEM: ",L,"\n\n" );
  } else
    mmdb::CreateConcat ( S,"       no MMDB Manager allocated\n\n" );

  return S;

}


void gsmt::Structure::prepareStructure ( mmdb::realtype maxContact )  {

  if (!D)  {

    if ((!M) || (!selAHnd))  return;
    
    getCalphas ( A,natoms );
    
    mmdb::GetMatrixMemory ( D,natoms,natoms,0,0 );
    nd_alloc = natoms;
    for (int i=0;i<natoms;i++)  {
      for (int j=i+1;j<natoms;j++)  {
        D[i][j] = sqrt ( A[i]->GetDist2(A[j]) );
        D[j][i] = D[i][j];
      }
      D[i][i] = 0.0;
    }
  
    if (maxContact>0.0)
      M->MakeBricks ( A,natoms,1.25*maxContact );
    
  }


}


mmdb::PManager gsmt::Structure::getSelectedStructure (
                                        mmdb::SELECTION_TYPE type )  {
mmdb::PManager  MS;
mmdb::PModel    model;
mmdb::PPModel   models;
mmdb::PChain    chain;
mmdb::PPChain   chains;
mmdb::ChainID   chID,chIDi;
mmdb::PPResidue res;
mmdb::PPAtom    atoms;
int             nAtoms,nRes,nChains,nModels,iModel,mdlNo,i;
int             selHnd;

  if (natoms<=0)
    return NULL;

  MS = new mmdb::Manager();
  
  switch (type)  {
  
    case mmdb::STYPE_ATOM      :
    case mmdb::STYPE_RESIDUE   :
          getChainRes ( res,nRes );
          chID[0] = char(1);
          chID[1] = char(0);
          mdlNo   = -1;
          model   = NULL;
          chain   = NULL;
          for (i=0;i<nRes;i++)  {
            strcpy ( chIDi,res[i]->GetChainID() );
            iModel = res[i]->GetModelNum();
            if (strcmp(chID,chIDi) || (mdlNo!=iModel)) {
              chain = mmdb::newChain();
              chain->SetChainID ( chIDi );
              strcpy ( chID,chIDi );
              if ((!model) || (mdlNo!=iModel))  {
                if (model)
                  MS->AddModel ( model );
                model = mmdb::newModel();
                mdlNo = iModel;
              }
              model->AddChain ( chain );
              model->GetChain(model->GetNumberOfChains()-1)->SetChainID(chIDi);
            }
            chain->AddResidue ( res[i] );
          }
          MS->AddModel ( model );
          if (type==mmdb::STYPE_ATOM)  {
            selHnd = MS->NewSelection();
            MS->Select ( selHnd,mmdb::STYPE_ATOM,0,"*",
                                mmdb::ANY_RES,"*",mmdb::ANY_RES,"*",
                                "*","[ CA ]","*","*",mmdb::SKEY_NEW );
            MS->SelectAtoms ( selHnd,0,0,mmdb::SKEY_XOR );
            MS->GetSelIndex ( selHnd,atoms,nAtoms );
            for (i=0;i<nAtoms;i++)  {
              delete atoms[i];
              atoms[i] = NULL;
            }
            MS->DeleteSelection ( selHnd );
          }
        break;

    case mmdb::STYPE_CHAIN     :

          selHnd = M->NewSelection();
          M->Select ( selHnd,mmdb::STYPE_CHAIN,selAHnd,mmdb::SKEY_NEW );
          M->GetSelIndex ( selHnd,chains,nChains );
          mdlNo = -1;
          model = NULL;
          for (i=0;i<nChains;i++)  {
            iModel = chains[i]->GetModelNum();
            if ((!model) || (mdlNo!=iModel))  {
              if (model)
                MS->AddModel ( model );
              model = mmdb::newModel();
              mdlNo = iModel;
            }
            model->AddChain ( chains[i] );
            model->GetChain(i)->SetChainID ( chains[i]->GetChainID() );
          }
          MS->AddModel ( model );
          M->DeleteSelection ( selHnd );
        break;
    
    case mmdb::STYPE_MODEL     :
          selHnd = M->NewSelection();
          M->Select ( selHnd,mmdb::STYPE_MODEL,selAHnd,mmdb::SKEY_NEW );
          M->GetSelIndex ( selHnd,models,nModels );
          for (i=0;i<nModels;i++)
            MS->AddModel ( models[i] );
          M->DeleteSelection ( selHnd );
        break;
  
    default:
    case mmdb::STYPE_UNDEFINED :  
      MS->Copy ( M,mmdb::MMDBFCM_All );

  }
  
  return MS;
  
}


void gsmt::Structure::fetchData ( mmdb::PPAtom  & Atoms, int & nat,
                                  mmdb::rmatrix & DMat )  {
  Atoms = A;
  nat   = natoms;
  DMat  = D;
}


int  gsmt::Structure::getNCalphas()  {
  if (!M)  return -1;
  return M->GetSelLength ( selAHnd );
}

void  gsmt::Structure::getCalphas ( mmdb::PPAtom & Atoms, int & nat )  {
  if ((!M) || (!selAHnd))  {
    Atoms = NULL;
    nat   = 0;
  } else
    M->GetSelIndex ( selAHnd,Atoms,nat );
}


void  gsmt::Structure::getChainRes ( mmdb::PPResidue & Res,
                                     int & nRes )  {
  if (M && selRHnd)
    M->GetSelIndex ( selRHnd,Res,nRes );
  else  {
    Res  = NULL;
    nRes = 0;
  }
}


void gsmt::Structure::getCoordSequence ( mmdb::pstr & seq )  {
mmdb::PPResidue res;
int             nRes,i,j;

  getChainRes ( res,nRes );

  if (seq)
    delete[] seq;
  seq = new char[nRes+1];

  j = 0;
  for (i=0;i<nRes;i++)
    if (res[i])
      mmdb::Get1LetterCode ( res[i]->GetResName(),seq[j++] );

  seq[j] = char(0);

}


bool gsmt::Structure::isCrystInfo()  {
  if (M)
    return (M->CrystReady()!=mmdb::CRRDY_NoTransfMatrices);
  return false;
}


void gsmt::Structure::getCentroid  ( mmdb::realtype & x,
                                     mmdb::realtype & y,
                                     mmdb::realtype & z )  {
  x = xc;
  y = yc;
  z = zc;
}

bool gsmt::Structure::getCentroidF ( mmdb::realtype & xf,
                                     mmdb::realtype & yf,
                                     mmdb::realtype & zf )  {
  xf = -1.0;
  yf = -1.0;
  zf = -1.0;
  if (M->CrystReady()!=mmdb::CRRDY_NoTransfMatrices)  {
    if (M->Orth2Frac(xc,yc,zc,xf,yf,zf))
      return true;
  }
  return false;
}

mmdb::cpstr gsmt::Structure::getPDBTitle ( mmdb::pstr & title )  {
  if (M)  return M->GetStructureTitle ( title );
  mmdb::CreateCopy ( title,"" );
  return title;
}

void  gsmt::Structure::copy ( PStructure S )  {
// deep-copy from S to this
  
  freeMemory();
  
  M = new mmdb::Manager();
  M->Copy ( S->getMMDBManager(),
            mmdb::COPY_MASK(mmdb::MMDBFCM_Cryst | mmdb::MMDBFCM_Coord) );
  
  mmdb::CreateCopy ( selection,S->getSelString() );
  strcpy ( refName,S->refName );  // reference name for output
  
  if (S->isSCOPSelection())
        selectCalphasSCOP ( selection );
  else  selectCalphasCID  ( selection );
  
  R = S->getERadius();
  S->getCentroid ( xc,yc,zc );

  mmdb::CreateCopy ( filePath ,S->getFilePath() );  
  
}

void  gsmt::Structure::setOutFile ( mmdb::cpstr fname,
                                    mmdb::cpstr note )  {
  mmdb::CreateCopy ( outFile,fname );
  mmdb::CreateCopy ( outNote,note  );
}

void  gsmt::Structure::setRefName ( mmdb::cpstr rname )  {
  strcpy ( refName,rname );
}

void  gsmt::Structure::read  ( mmdb::io::RFile f )  {
  
  mmdb::FreeMatrixMemory ( D,nd_alloc,0,0 );
  nd_alloc = 0;

  if (!M)  M = new mmdb::Manager();
  
  M->ReadMMDBF ( f );
  
  f.CreateRead ( selection );
  f.CreateRead ( filePath  );
  f.ReadBool   ( &scopSel  );
  
  f.ReadReal   ( &xc );
  f.ReadReal   ( &yc );
  f.ReadReal   ( &zc );
  f.ReadReal   ( &R  );
  
  selDHnd = 0;
  selAHnd = M->NewSelection();
  selRHnd = M->NewSelection();
  M->SelectAtoms ( selAHnd,0,0,mmdb::SKEY_NEW );
  M->Select ( selRHnd,mmdb::STYPE_RESIDUE,selAHnd,mmdb::SKEY_NEW );
  
}


void  gsmt::Structure::write ( mmdb::io::RFile f )  {
mmdb::PManager M1;
mmdb::PModel   model;
mmdb::PChain   chain;
mmdb::PResidue res;
int            i;

  if (M)  {
  
    getCalphas ( A,natoms );
    
    if (A && (natoms>0))  {
      
      M1 = new mmdb::Manager();
      M1->SetEntryID ( M->GetEntryID() );
      M1->Copy ( M,mmdb::MMDBFCM_Title );
      
      chain = new mmdb::Chain();
      chain->SetChainID ( A[0]->GetChainID() );
      for (i=0;i<natoms;i++)  {
        res = new mmdb::Residue();
        res->SetResID ( A[i]->GetResName(),A[i]->GetSeqNum(),
                        A[i]->GetInsCode() );
        res->AddAtom  ( A[i] );
        chain->AddResidue ( res );
      }
      model = new mmdb::Model();
      model->AddChain ( chain );
      M1->AddModel ( model );
      
      M1->SetCompactBinary();    
      M1->WriteMMDBF ( f );
      
      f.CreateWrite ( selection );
      f.CreateWrite ( filePath  );
      f.WriteBool   ( &scopSel  );
        
      f.WriteReal   ( &xc );
      f.WriteReal   ( &yc );
      f.WriteReal   ( &zc );
      f.WriteReal   ( &R  );
    
      delete M1;
      
    }
  
  }
  
}

void gsmt::Structure::writeSelectedCoordinates ( mmdb::cpstr outFile )  {
mmdb::PManager mmdbMan = getSelectedStructure ( mmdb::STYPE_ATOM );

  if (mmdbMan)  {
    mmdbMan->WritePDBASCII ( outFile );
    delete mmdbMan;
  }

}


void printMat4 ( mmdb::cpstr title, mmdb::mat44 & T )  {
  printf ( "%s\n",title );
  printf ( " %12.5f %12.5f %12.5f    %12.5f\n"
           " %12.5f %12.5f %12.5f    %12.5f\n"
           " %12.5f %12.5f %12.5f    %12.5f\n",
           T[0][0],T[0][1],T[0][2],T[0][3],
           T[1][0],T[1][1],T[1][2],T[1][3],
           T[2][0],T[2][1],T[2][2],T[2][3] );
}


void gsmt::printStructureSummary ( PPStructure s,
                                   int         nStruct,
                                   bool        isOutput )  {
char L1[2000];
int  nc1,nc2,len1,len2,i,j;

  nc1 = 0;
  nc2 = 0;
  for (i=0;i<nStruct;i++)
    nc1 = mmdb::IMax ( nc1,strlen(s[i]->getFilePath()) +
                           strlen(s[i]->getSelString()) ) - 12;
  if (isOutput)
    for (i=0;i<nStruct;i++)
      nc2 = mmdb::IMax ( nc2,strlen(s[i]->getOutputFile()) +
                             strlen(s[i]->getOutputNote()) ) - 4;
  
  printf ( "\n ===== Structures\n\n" );
  strcpy ( L1,"     Ref.  |  Nres  | File (selection)" );
  len1 = strlen(L1);
  for (i=0;i<nc1;i++)
    L1[len1++] = ' ';

  len2 = len1;
  if (isOutput)  {
    L1[len1] = char(0);
    strcat ( L1,"| Output File" );
    len2 = strlen(L1);
    for (i=0;i<nc2;i++)
      L1[len2++] = ' ';
  }
  
  L1[len2] = char(0);
  printf ( "%s\n",L1 );

  strcpy ( L1,"   ========+========+" );
  for (i=strlen(L1);i<len1;i++)
    L1[i] = '=';
  if (isOutput)  {
    L1[len1] = '+';
    for (i=len1+1;i<len2;i++)
      L1[i] = '=';
  }
  L1[len2] = char(0);
  printf ( "%s\n",L1 );

  for (i=0;i<nStruct;i++)  {
    printf ( "    %6s | %5i  | %s (%s) ",
             s[i]->getRefName(),s[i]->getNCalphas(),
             s[i]->getFilePath(),s[i]->getSelString() );
    if (isOutput)  {
      for (j=strlen(L1);j<len1;j++)
        printf ( " " );
      printf ( "| %s (%s) ",
               s[i]->getOutputFile(),s[i]->getOutputNote() );
    }
    printf ( "\n" );
  }

/*
  strcpy ( L1,"   --------'--------'" );
  for (i=strlen(L1);i<len1;i++)
    L1[i] = '-';
  if (isOutput)  {
    L1[len1] = '+';
    for (i=len1+1;i<len2;i++)
      L1[i] = '-';
  }
  L1[len2] = char(0);
  printf ( "%s\n",L1 );
*/

  if (nStruct>1)
    printf ( "\n have been aligned and superposed.\n\n" );
  
}


void gsmt::makeStructureSummary_rvapi ( mmdb::cpstr tableId,
                                        PPStructure s,
                                        int         nStruct )  {
mmdb::pstr S = NULL;
mmdb::pstr L = NULL;
int        row;
 
  rvapi_put_horz_theader ( tableId,"N<sub>res</sub>",
                                   "Structure size in residues",0 );
  rvapi_put_horz_theader ( tableId,"File (selection)",
                                   "Structure name",1 );

  rvapi_put_vert_theader ( tableId,s[0]->getRefName(),
                                   "Fixed structure",0 );
  for (row=1;row<nStruct;row++)
    rvapi_put_vert_theader ( tableId,s[row]->getRefName(),
                                     "Moving structure",row );

  for (row=0;row<nStruct;row++)  {
    rvapi_put_table_int ( tableId,s[row]->getNCalphas(),row,0 );
    mmdb::CreateCopCat  ( S,mmdb::io::GetFName(s[row]->getFilePath()),
                          " (",s[row]->getSelString(),")" );
    mmdb::CreateCopCat  ( L,S,S,S,S );
    rvapi_make_hard_spaces ( L,S );
    rvapi_put_table_string ( tableId,L,row,1 );
  }
  
  if (S)  delete[] S;
  if (L)  delete[] L;
                                     
}


/*
void gsmt::writeStructureSummary ( mmdb::io::RFile f,
                                   PPStructure     s,
                                   int             nStruct,
                                   bool            isOutput )  {
char L1[2000];
int  nc1,nc2,len1,len2,i,j;

  nc1 = 0;
  nc2 = 0;
  for (i=0;i<nStruct;i++)
    nc1 = mmdb::IMax ( nc1,strlen(s[i]->getFilePath()) +
                           strlen(s[i]->getSelString()) ) - 12;
  if (isOutput)
    for (i=0;i<nStruct;i++)
      nc2 = mmdb::IMax ( nc2,strlen(s[i]->getOutputFile()) +
                             strlen(s[i]->getOutputNote()) ) - 8;
  
  f.Write ( "\n ===== Structures\n\n" );
  strcpy ( L1,"     Ref.  |  Nres  | File (selection)" );
  len1 = strlen(L1);
  for (i=0;i<nc1;i++)
    L1[len1++] = ' ';

  len2 = len1;
  if (isOutput)  {
    L1[len1] = char(0);
    strcat ( L1,"| Output File" );
    len2 = strlen(L1);
    for (i=0;i<nc2;i++)
      L1[len2++] = ' ';
  }
  
  L1[len2] = char(0);
  f.WriteLine ( L1 );

  strcpy ( L1,"   --------+--------+" );
  for (i=strlen(L1);i<len1;i++)
    L1[i] = '-';
  if (isOutput)  {
    L1[len1] = '+';
    for (i=len1+1;i<len2;i++)
      L1[i] = '-';
  }
  L1[len2] = char(0);
  f.WriteLine ( L1 );

  for (i=0;i<nStruct;i++)  {
    sprintf ( L1,"    %6s | %5i  | %s (%s) ",
                 s[i]->getRefName(),s[i]->getNCalphas(),
                 s[i]->getFilePath(),s[i]->getSelString() );
    f.Write ( L1 );
    if (isOutput)  {
      for (j=strlen(L1);j<len1;j++)
        f.Write ( " " );
      sprintf ( L1,"| %s (%s) ",
                s[i]->getOutputFile(),s[i]->getOutputNote() );
      f.Write ( L1 );
    }
    f.LF();
  }
  
  f.Write ( "\n have been aligned and superposed.\n\n" );
  
}
*/

void gsmt::writeStructureSummary_csv ( mmdb::io::RFile f,
                                       PPStructure s, int nStruct,
                                       bool isOutput )  {
char L1[2000];

  if (isOutput)
       f.WriteLine ( "\nSTRUCTURES\n\n"
                     "Ref., Nres, File, Selection, Output file, Note" );
  else f.WriteLine ( "\nSTRUCTURES\n\n"
                     "Ref., Nres, File, Selection" );

  for (int i=0;i<nStruct;i++)  {
    sprintf ( L1,"%6s, %5i, %s, %s",
                 s[i]->getRefName(),s[i]->getNCalphas(),
                 s[i]->getFilePath(),s[i]->getSelString() );
    f.Write ( L1 );
    if (isOutput)  {
      sprintf ( L1,", %s, %s",
                s[i]->getOutputFile(),s[i]->getOutputNote() );
      f.Write ( L1 );
    }
    f.LF();
  }
  
}


void gsmt::addStructureSummaryJSON ( gsmt::RJSON json,
                                     PPStructure s,
                                     int         nStruct,
                                     bool        isOutput )  {
gsmt::JSON *json1;

  for (int i=0;i<nStruct;i++)  {
    json1 = new gsmt::JSON();
    json1->addValue ( "refname"  ,s[i]->getRefName  () );
    json1->addValue ( "ncalphas" ,s[i]->getNCalphas () );
    json1->addValue ( "filepath" ,s[i]->getFilePath () );
    json1->addValue ( "selstring",s[i]->getSelString() );
    if (isOutput)  {
      json1->addValue ( "outfilename",s[i]->getOutputFile() );
      json1->addValue ( "outputnote" ,s[i]->getOutputNote() );
    }
    json.addJSON ( "structures",json1 );
  }
  
}
