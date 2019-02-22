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

#include <string.h>

#include "gsmt_model.h"
#include "gsmt_sheaf.h"
#include "gsmt_utils.h"

// =================================================================

gsmt::Model::Model()  {
  initModel();
}

gsmt::Model::~Model()  {
  freeMemory();
}

void gsmt::Model::initModel()  {
  
  archive     = NULL; // gesamt archive
  archDir     = NULL; // path to archive directory
  outDir      = NULL; // path to output directory + "/"
  mmdb::CreateCopy ( outDir,"./gesamt_model/" );
  minSeqMatch = -1.0; // minimal acceptable sequence match (-1: explore)
  Qthresh     = -0.5; // threshold Q-score for sheafing (-1: explore)
  subSeqLen   = -1;   // subsequence length (-1: choose or explore)
  nthreads    = 1;    // number of threads to use
  verbosity   = 0;    // verbosity level

  nSheafs     = 0;    // number of sequence sheafs
  sheafLen    = 0;    // common length of sequence sheafs
  
  asmInitMemory();

}


void gsmt::Model::freeMemory()  {
  if (archDir)  {
    delete[] archDir;
    archDir = NULL;
  }
  if (outDir)  {
    delete[] outDir;
    outDir = NULL;
  }
  if (archive)  {
    delete archive;
    archive = NULL;
  }
  nSheafs  = 0;
  sheafLen = 0;
  asmFreeMemory();
}

void gsmt::Model::setArchiveDir ( mmdb::cpstr archiveDir )  {
  mmdb::CreateCopy ( archDir,archiveDir );
}

void gsmt::Model::setOutputDir ( mmdb::cpstr outputDir )  {
int n = strlen(outputDir)-1;
  if (outputDir[n]!='/')  {
    if (outputDir[n]=='\\')
         mmdb::CreateCopy   ( outDir,outputDir );
    else mmdb::CreateCopCat ( outDir,outputDir,mmdb::io::_dir_sep );
  } else
    mmdb::CreateCopy ( outDir,outputDir );
}


gsmt::GSMT_CODE gsmt::Model::makeModel ( mmdb::cpstr seqFile )  {
GSMT_CODE   rc;

  mkdirp ( outDir );

  //  Collect sheafs of subsequances
  rc = makeSeqSheafs ( seqFile,subSeqLen,minSeqMatch );
  if (rc!=GSMT_Ok)
    return rc;
    
  //  Make multiple alignments in sheafs
  rc = alignSeqSheafs();
  if (rc!=GSMT_Ok)
    return rc;

  //  Assemble sheafs
  rc = assembleSheafs();
  if (rc!=GSMT_Ok)
    return rc;
   
  return rc;

}

gsmt::GSMT_CODE gsmt::Model::makeSeqSheafs ( mmdb::cpstr     seqFile,
                                             int        fragmentSize,
                                             mmdb::realtype minMatch ) {
mmdb::pstr outFile;
GSMT_CODE  rc;

  if (!archive)
    archive = new gsmt::Archive();
  archive->setVerbosity ( verbosity );
  archive->setNThreads  ( nthreads  );

  outFile = NULL;
  mmdb::CreateCopCat ( outFile,outDir,"subseqscan.txt" );

  rc = archive->scanSubSeq ( seqFile,archDir,fragmentSize,minMatch,
                             outFile );

  if (outFile)
    delete[] outFile;

  return rc;

}

gsmt::GSMT_CODE gsmt::Model::alignSeqSheafs()  {
mmdb::io::File f;
PSeqSheafs     seqSheafs;
PPSeqSheaf     seqSheaf;
PSheaf         sheafAligner;
ProgressBar    progressBar;
mmdb::pstr     outFile;
mmdb::pstr     L;
char           msg[1000];
GSMT_CODE      rc = GSMT_Ok;

  seqSheafs = archive->getSeqSheafs();
  if (!seqSheafs)
    return GSMT_NoSeqSheafs;

  seqSheaf = seqSheafs->sheafs;
  nSheafs  = seqSheafs->nSheafs;
  sheafLen = seqSheafs->sheafLen;

  if (verbosity>0)
    printf (
      "\n"
      " ... total %i substructure subsets will be multiply aligned\n"
      " ... substructure length: %i residues\n\n",
      nSheafs,sheafLen );

  progressBar.reset      ( pbar_len     );
  progressBar.setVisible ( verbosity==1 );

  sheafAligner = new Sheaf();

  sheafAligner->setSheafMode  ( SHEAF_Auto );
//  sheafAligner->setQR0        ( Input.QR0       );
  sheafAligner->setQThreshold ( Qthresh    );
  sheafAligner->setNThreads   ( nthreads   );
  sheafAligner->setVerbosity  ( verbosity  );

  L       = NULL;
  outFile = NULL;
  mmdb::CreateCopCat ( outFile,outDir,"sheafalign.txt" );
  f.assign ( outFile,true,false );
  f.rewrite();

  for (int i=0;i<nSheafs;i++)  {

    seqSheaf[i]->getStructureSheaf ( archDir );
    sprintf ( msg,"\n ======= Sequence sheaf #%i, total %i items\n\n",
                  i,seqSheaf[i]->nItems );
    f.Write ( msg );

    for (int j=0;j<seqSheaf[i]->nItems;j++)  {
      seqSheaf[i]->S[j]->getCoordSequence ( L );
      sprintf ( msg," %5i. %10.4f  %s  %s\n",j+1,
                    seqSheaf[i]->items[j]->score,
                    seqSheaf[i]->S[j]->getRefName(),L );
      f.Write ( msg );
    }

    rc = sheafAligner->Align ( seqSheaf[i]->S,seqSheaf[i]->nItems,
                               false );

    if (rc==GSMT_Ok)  {

      sheafAligner->takeSheafData ( seqSheaf[i]->structSheafs,
                                    seqSheaf[i]->nStructSheafs );
      seqSheaf[i]->calcSeqScores();
      seqSheaf[i]->sort ( SeqSheaf::SORT_SHEAFS_SeqId );

      sprintf ( msg,"\n"
                    " ... Total sheafs made: %i\n",
                    seqSheaf[i]->nStructSheafs );
      f.Write ( msg );

      for (int j=0;j<seqSheaf[i]->nStructSheafs;j++)
        if (seqSheaf[i]->structSheafs[j])  {
          sprintf ( msg,"\n ... Sheaf No. %i\n",j+1 );
          f.Write ( msg );
          if (seqSheaf[i]->structSheafs[j]->nStruct<=1)
            f.Write ( "\n"
                " Single-structure sheaf: no scoring, no alignment\n" );
          else  {
            sprintf ( msg,
                "        size:  %-6i (structures)\n"
                "     seq. Id:  %.4f (normalised to [0..1])\n"
                "   quality Q:  %.4f (normalised to [0..1])\n"
                "     r.m.s.d:  %.4f (A)\n"
                "      Nalign:  %-6i (residues)\n",
                seqSheaf[i]->structSheafs[j]->nStruct,
                seqSheaf[i]->structSheafs[j]->seqId,
                seqSheaf[i]->structSheafs[j]->Q,
                seqSheaf[i]->structSheafs[j]->rmsd,
                seqSheaf[i]->structSheafs[j]->Nalign );
            f.Write ( msg );
          }
        }
      
    } else  {
      sprintf ( msg,"\n ... Sheaf Alignment failed rc=%i\n",rc );
      f.Write ( msg );    
    }

    progressBar.setProgress ( i+1,nSheafs );

  }

  progressBar.setProgress ( nSheafs,0 );

  f.shut();
  if (L)        delete[] L;
  if (outFile)  delete[] outFile;
  
  if (sheafAligner)  delete sheafAligner;

  return rc;

}


gsmt::GSMT_CODE gsmt::Model::assembleSheafs()  {
GSMT_CODE  rc;

  rc = GSMT_Ok;
  asmGetMemory();
  
  // Make partial substructures for further sheafing
  for (int i=0;i<nSheafs;i++)
    asmS[i] = asmMakeStructure ( i );

  return rc;

}

void gsmt::Model::asmInitMemory()  {
  asmS         = NULL;
  nStructAlloc = 0;
}

void gsmt::Model::asmFreeMemory()  {
  if (asmS)  {
    for (int i=0;i<nStructAlloc;i++)
      if (asmS[i])  delete asmS[i];
    delete[] asmS;
    asmS = NULL;
  }
  nStructAlloc = 0;
}

void gsmt::Model::asmGetMemory()  {

  asmFreeMemory();

  asmS = new PStructure[nSheafs];

}

gsmt::PStructure gsmt::Model::asmMakeStructure ( int structNo )  {
PSeqSheaf      seqSheaf;
PSheafData     structSheaf;
PStructure     S;
mmdb::PManager M;
mmdb::PModel   model;
mmdb::PChain   chain;
mmdb::PResidue res;
mmdb::PAtom    atom;
mmdb::realtype occ;
int            structSize,k;

  if (structNo<0)         return NULL;
  if (structNo>=nSheafs)  return NULL;

  seqSheaf = archive->getSeqSheafs()->sheafs[structNo];
  if (!seqSheaf)                   return NULL;
  if (seqSheaf->nStructSheafs<=0)  return NULL;

  structSheaf = seqSheaf->structSheafs[0];
  if (!structSheaf)  return NULL;

  structSize = nSheafs + sheafLen - 1;
  
  chain = new mmdb::Chain();
  chain->SetChainID ( "A" );
  for (int i=0;i<structSize;i++)  {
    atom = new mmdb::Atom();
    atom->SetAtomName    ( " CA " );
    atom->SetElementName ( " C"   );
    if ((structNo<=i) && (i<structNo+sheafLen))  {
      k = i-structNo;
      if (structSheaf->mask[k])  occ = 1.0;
                           else  occ = 0.5;
      atom->SetCoordinates ( structSheaf->cx[k],structSheaf->cy[k],
                             structSheaf->cz[k],occ,
                             structSheaf->var2[k] );
    } else
      atom->SetCoordinates ( 0.0,0.0,0.0, 0.0, 99.0 );
    res  = new mmdb::Residue();
    res->SetResID ( "ALA",i+1,"" );
    res->AddAtom ( atom );
    chain->AddResidue ( res );
  }
  
  model = new mmdb::Model();
  model->AddChain ( chain );
  M = new mmdb::Manager();
  M->AddModel ( model );

/*
char L[1000];
sprintf ( L,"%sstructure_%03i.pdb",outDir,structNo );
M->WritePDBASCII ( L );
*/

  S = new Structure();
  S->getStructure ( M,NULL,0,false );

  return S;

}

