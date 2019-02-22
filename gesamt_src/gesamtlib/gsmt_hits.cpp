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
//  **** Module  :  GSMT_Hits <implementation>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::Hit
//       ~~~~~~~~~  gsmt::Hits
//
//  (C) E. Krissinel 2008-2016
//
// =================================================================
//

#include <string.h>

#include "mmdb2/mmdb_io_file.h"
#include "gsmt_hits.h"


// =================================================================

gsmt::Hit::Hit()  {
   pdbCode[0] = char(0);
   fileName   = NULL;
   pdbTitle   = NULL;
   qSeq       = NULL;     // aligned query sequence
   tSeq       = NULL;     // aligned target sequence
   dist       = NULL;     // C-alpha distances at superposition
   mmdb::Mat4Init ( RT ); // rotation-translation matrix
   chainID[0] = char(0);
   rmsd       = -1.0;     // best superposition r.m.s.d.
   Q          = -1.0;     // Q-score
   seqId      = -1.0;     // sequence identity
   Nalgn      = -1;       // alignment length
   nRes       = -1;
   sel        = false;    // selection attribute
}

gsmt::Hit::~Hit()  {
  if (fileName)
    delete[] fileName;
  if (pdbTitle)
    delete[] pdbTitle;
  if (qSeq)
    delete[] qSeq;
  if (tSeq)
    delete[] tSeq;
  mmdb::FreeVectorMemory ( dist,0 );
}

int gsmt::Hit::getWriteSize()  {
  return 3*sizeof(mmdb::realUniBin) +
           sizeof(mmdb::ChainID)    +
           sizeof(mmdb::IDCode)     +
         3*sizeof(mmdb::intUniBin)  + 5002;
}

void gsmt::Hit::mem_write ( mmdb::pstr  S, int & l )  {
int i,j;
int version=1;
  mmdb::mem_write ( version ,S,l );
  mmdb::mem_write ( fileName,S,l );
  mmdb::mem_write ( pdbCode ,sizeof(pdbCode),S,l );
  mmdb::mem_write ( chainID ,sizeof(chainID),S,l );
  mmdb::mem_write ( rmsd    ,S,l );
  mmdb::mem_write ( Q       ,S,l );
  mmdb::mem_write ( seqId   ,S,l );
  mmdb::mem_write ( pdbTitle,S,l );
  mmdb::mem_write ( qSeq    ,S,l );
  mmdb::mem_write ( tSeq    ,S,l );
  if (qSeq)  {
    j = strlen(qSeq);
    for (i=0;i<j;i++)
      mmdb::mem_write ( dist[i],S,l );
  }
  for (i=0;i<3;i++)
    for (j=0;j<4;j++)
      mmdb::mem_write ( RT[i][j],S,l );
  mmdb::mem_write ( Nalgn   ,S,l );
  mmdb::mem_write ( nRes    ,S,l );
  mmdb::mem_write ( sel     ,S,l );
}

void gsmt::Hit::mem_read  ( mmdb::cpstr S, int & l )  {
int i,j,version;

  mmdb::FreeVectorMemory ( dist,0 );

  mmdb::mem_read ( version ,S,l );
  mmdb::mem_read ( fileName,S,l );
  mmdb::mem_read ( pdbCode ,sizeof(pdbCode),S,l );
  mmdb::mem_read ( chainID ,sizeof(chainID),S,l );
  mmdb::mem_read ( rmsd    ,S,l );
  mmdb::mem_read ( Q       ,S,l );
  mmdb::mem_read ( seqId   ,S,l );
  mmdb::mem_read ( pdbTitle,S,l );
  mmdb::mem_read ( qSeq    ,S,l );
  mmdb::mem_read ( tSeq    ,S,l );
  if (qSeq)  {
    j = strlen(qSeq);
    if (j>0)  {
      mmdb::GetVectorMemory ( dist,j,0 );
      for (i=0;i<j;i++)
        mmdb::mem_read ( dist[i],S,l );
    }
  }
  for (i=0;i<3;i++)
    for (j=0;j<4;j++)
      mmdb::mem_read ( RT[i][j],S,l );
  mmdb::mem_read ( Nalgn   ,S,l );
  mmdb::mem_read ( nRes    ,S,l );
  mmdb::mem_read ( sel     ,S,l );
}

void gsmt::Hit::csv_title ( mmdb::pstr S, mmdb::cpstr sep )  {
  sprintf ( S,"PDB Code%sChain ID%sNres%sQ-score%srmsd%sseq.Id%sNalign%sTitle",
              sep,sep,sep,sep,sep,sep,sep );
}

void gsmt::Hit::write_csv ( mmdb::pstr S, mmdb::cpstr sep )  {
  sprintf ( S,"'%s'%s%s%s%i%s%.6g%s%.6g%s%.6g%s%i%s'%s'",
              pdbCode,sep,chainID,sep,nRes,sep,Q,sep,rmsd,sep,
              seqId,sep,Nalgn,sep,pdbTitle ); 
}


// =================================================================

gsmt::Hits::Hits() : mmdb::QuickSort()  {
  hits       = NULL;
  nHits      = 0;
  nHitsAlloc = 0;
  dataKey    = DATA_StructureScan;
}

gsmt::Hits::~Hits()  {
  deleteHits();
}

void gsmt::Hits::deleteHits()  {
int i;

  if (hits)  {
    for (i=0;i<nHitsAlloc;i++)
      if (hits[i])  delete hits[i];
    delete[] hits;
    hits = NULL;
  }

  nHits      = 0;
  nHitsAlloc = 0;
    
}

void gsmt::Hits::checkHitsBuffer()  {
PPHit hits1;
int   i,n;

  if (nHits>=nHitsAlloc)  {
    n = nHitsAlloc + 10000;
    hits1 = new PHit[n];
    for (i=0;i<nHitsAlloc;i++)
      hits1[i] = hits[i];
    for (i=nHitsAlloc;i<n;i++)
      hits1[i] = NULL;
    if (hits)  delete[] hits;
    hits = hits1;
    nHitsAlloc = n;
  }

  if (!hits[nHits])
    hits[nHits] = new Hit();

}

void gsmt::Hits::setHit ( mmdb::IDCode         pdbId,
                          mmdb::cpstr          fname,
                          mmdb::ChainID        chainId,
                          mmdb::cpstr          title,
                          gsmt::PSuperposition SD,
                          gsmt::PAligner       Aligner,
                          int                  nAtoms )  {

  strcpy ( hits[nHits]->pdbCode,pdbId );
  mmdb::CreateCopy ( hits[nHits]->fileName,fname );
  mmdb::CreateCopy ( hits[nHits]->pdbTitle,title );
  strcpy ( hits[nHits]->chainID,chainId );
  hits[nHits]->rmsd  = SD->rmsd;
  hits[nHits]->Q     = SD->Q;
  hits[nHits]->seqId = SD->seqId;
  hits[nHits]->Nalgn = SD->Nalgn;
  hits[nHits]->nRes  = nAtoms;
  hits[nHits]->sel   = false;
  Aligner->getSeqAlignment  ( hits[nHits]->qSeq,hits[nHits]->tSeq,
                              &(hits[nHits]->dist),SD );
  mmdb::Mat4Copy ( SD->T,hits[nHits]->RT );

  nHits++;

}

void gsmt::Hits::setHit ( mmdb::cpstr    pdbId,
                          mmdb::cpstr    fname,
                          mmdb::ChainID  chainId,
                          mmdb::cpstr    title,
                          mmdb::cpstr    qAlignSeq,
                          mmdb::cpstr    tAlignSeq,
                          mmdb::realtype seqId,
                          mmdb::realtype similarity,
                          mmdb::realtype score,
                          int            Nalign,
                          int            nRes )  {

  strcpy ( hits[nHits]->pdbCode,pdbId );
  mmdb::CreateCopy ( hits[nHits]->fileName,fname );
  mmdb::CreateCopy ( hits[nHits]->pdbTitle,title );
  strcpy ( hits[nHits]->chainID,chainId );
  mmdb::CreateCopy ( hits[nHits]->qSeq,qAlignSeq );
  mmdb::CreateCopy ( hits[nHits]->tSeq,tAlignSeq );
  hits[nHits]->rmsd  = similarity;
  hits[nHits]->Q     = score;
  hits[nHits]->seqId = seqId;
  hits[nHits]->Nalgn = Nalign;
  hits[nHits]->nRes  = nRes;
  hits[nHits]->sel   = false;
  nHits++;

}

void gsmt::Hits::setHit ( RHit h )  {
int i,k;

  checkHitsBuffer();

  mmdb::FreeVectorMemory ( hits[nHits]->dist,0 );
  strcpy ( hits[nHits]->pdbCode,h.pdbCode );
  mmdb::CreateCopy ( hits[nHits]->fileName,h.fileName );
  mmdb::CreateCopy ( hits[nHits]->pdbTitle,h.pdbTitle );
  mmdb::CreateCopy ( hits[nHits]->qSeq,h.qSeq );
  mmdb::CreateCopy ( hits[nHits]->tSeq,h.tSeq );
  if (h.qSeq)  {
    k = strlen(h.qSeq);
    if (k>0)  {
      mmdb::GetVectorMemory ( hits[nHits]->dist,k,0 );
      for (i=0;i<k;i++)
        hits[nHits]->dist[i] = h.dist[i];
    } 
  }
  mmdb::Mat4Copy ( h.RT,hits[nHits]->RT );
  strcpy ( hits[nHits]->chainID,h.chainID );
  hits[nHits]->rmsd  = h.rmsd;
  hits[nHits]->Q     = h.Q;
  hits[nHits]->seqId = h.seqId;
  hits[nHits]->Nalgn = h.Nalgn;
  hits[nHits]->nRes  = h.nRes;
  hits[nHits]->sel   = h.sel;
  nHits++;

}

void gsmt::Hits::mergeHits ( RHits H )  {
PPHit hits1;
int   i,n;
  
  if (nHits+H.nHits>=nHitsAlloc)  {
    n = nHits + H.nHits;
    hits1 = new PHit[n];
    for (i=0;i<nHits;i++)
      hits1[i] = hits[i];
    if (hits)  delete[] hits;
    hits       = hits1;
    nHitsAlloc = n;
  }

  for (i=0;i<H.nHits;i++)  {
    hits  [nHits++] = H.hits[i];
    H.hits[i]       = NULL;
  }
  H.nHits = 0;
  
}


int gsmt::Hits::Compare ( int i, int j )  {
// sort by decreasing Q
int  cmp;

  cmp = 0;

  if (dataKey==DATA_StructureScan)  {
  
    if (((PPHit)data)[i]->Q<((PPHit)data)[j]->Q) return  1;
    if (((PPHit)data)[i]->Q>((PPHit)data)[j]->Q) return -1;
  
    if (((PPHit)data)[i]->rmsd>((PPHit)data)[j]->rmsd) return  1;
    if (((PPHit)data)[i]->rmsd<((PPHit)data)[j]->rmsd) return -1;
  
  } else  {
  
    // Here rmsd stands for similarity [0..1]
    if (((PPHit)data)[i]->rmsd>((PPHit)data)[j]->rmsd) return -1;
    if (((PPHit)data)[i]->rmsd<((PPHit)data)[j]->rmsd) return  1;
  
    if (((PPHit)data)[i]->Nalgn<((PPHit)data)[j]->Nalgn) return  1;
    if (((PPHit)data)[i]->Nalgn>((PPHit)data)[j]->Nalgn) return -1;
  
    if (((PPHit)data)[i]->seqId<((PPHit)data)[j]->seqId) return  1;
    if (((PPHit)data)[i]->seqId>((PPHit)data)[j]->seqId) return -1;
  
  }
  
  cmp = strcmp ( ((PPHit)data)[i]->chainID,
                 ((PPHit)data)[j]->chainID );
  
  if (!cmp)  
    cmp = strcmp ( ((PPHit)data)[i]->fileName,
                   ((PPHit)data)[j]->fileName );

  return cmp;

}

void gsmt::Hits::Swap ( int i, int j )  {
PHit h;
  h = ((PPHit)data)[i];
  ((PPHit)data)[i] = ((PPHit)data)[j];
  ((PPHit)data)[j] = h;
}

void gsmt::Hits::sortHits()  {
  mmdb::QuickSort::Sort ( (void*)hits,nHits );
}

void gsmt::Hits::writeHits ( mmdb::cpstr fName )  {
  if (dataKey==DATA_StructureScan)  writeStructureHits ( fName );
                              else  writeSequenceHits  ( fName );
}

void gsmt::Hits::writeHits_csv ( mmdb::cpstr fName )  {
  if (dataKey==DATA_StructureScan)  writeStructureHits_csv ( fName );
                              else  writeSequenceHits_csv  ( fName );
}

void gsmt::Hits::writeHits_json ( PStructure s, mmdb::cpstr fName )  {
  if (dataKey==DATA_StructureScan)  writeStructureHits_json ( s,fName );
                              else  writeSequenceHits_json  ( s,fName );
}

void gsmt::Hits::writeStructureHits ( mmdb::cpstr fName )  {
mmdb::io::File f;
char           S[200];
int            i;

  f.assign ( fName,true );
  if (!f.rewrite())  {
    printf ( "\n *** can't open file '%s' for writing\n",fName );
    return;
  }

  f.WriteLine (
     "#  Hit   PDB  Chain  Q-score  r.m.s.d     Seq.  Nalign  nRes    File" );
  f.WriteLine (
     "#  No.   code   Id                         Id.                  name" );

  for (i=0;i<nHits;i++)  {
    sprintf ( S," %5i   %4s   %1s   %8.4f %8.4f   %6.4f  %5i  %5i   %s",
             i+1,hits[i]->pdbCode,hits[i]->chainID,
             hits[i]->Q,hits[i]->rmsd,hits[i]->seqId,
             hits[i]->Nalgn,hits[i]->nRes,hits[i]->fileName );
    f.WriteLine ( S );
  }

  f.shut();

}


void gsmt::Hits::writeSequenceHits ( mmdb::cpstr fName )  {
mmdb::io::File f;
char           S[200];
int            i;

  f.assign ( fName,true );
  if (!f.rewrite())  {
    printf ( "\n *** can't open file '%s' for writing\n",fName );
    return;
  }

  f.WriteLine (
     "#  Hit   PDB  Chain   Seq.  Similarity  Score   Nalign  nRes    File" );
  f.WriteLine (
     "#  No.   code   Id     Id.                                      name" );

  for (i=0;i<nHits;i++)  {
    sprintf ( S," %5i   %4s   %1s   %6.4f %8.4f %10.4f  %5i  %5i   %s",
             i+1,hits[i]->pdbCode,hits[i]->chainID,
             hits[i]->seqId,hits[i]->rmsd,hits[i]->Q,
             hits[i]->Nalgn,hits[i]->nRes,hits[i]->fileName );
    f.WriteLine ( S );
  }

  f.shut();

}


void gsmt::Hits::writeStructureHits_csv ( mmdb::cpstr fName )  {
mmdb::io::File f;
mmdb::pstr     sid;
char           S[2000];
int            i,j,n;

  f.assign ( fName,true );
  if (!f.rewrite())  {
    printf ( "\n *** can't open file '%s' for writing\n",fName );
    return;
  }

  for (i=0;i<nHits;i++)  {
    if (hits[i]->pdbCode[0])  sid = hits[i]->pdbCode;
                        else  sid = hits[i]->fileName;
    sprintf ( S," === HIT #, %i\n"
                " [Target]\n"
                "  Chain:, %s:%s\n"
                "  Length:, %i\n"
                "  Title:, \"%s\"\n"
                " [Scores]\n"
                "  Q-score:, %.4f\n"
                "  Rmsd:, %.4f\n"
                "  Seq Id:, %.4f\n"
                "  N_align:, %i",
                i+1,sid,hits[i]->chainID,hits[i]->nRes,
                hits[i]->pdbTitle,hits[i]->Q,hits[i]->rmsd,
                hits[i]->seqId,hits[i]->Nalgn );
    f.WriteLine ( S );
    sprintf ( S," [RT matrix]\n"
                " %9.6f, %9.6f, %9.6f, %.6f\n"
                " %9.6f, %9.6f, %9.6f, %.6f\n"
                " %9.6f, %9.6f, %9.6f, %.6f",
                hits[i]->RT[0][0],hits[i]->RT[0][1],
                                  hits[i]->RT[0][2],hits[i]->RT[0][3],
                hits[i]->RT[1][0],hits[i]->RT[1][1],
                                  hits[i]->RT[1][2],hits[i]->RT[1][3],
                hits[i]->RT[2][0],hits[i]->RT[2][1],
                                  hits[i]->RT[2][2],hits[i]->RT[2][3]
              );
    f.WriteLine ( S );
    if (hits[i]->qSeq)  {
      f.WriteLine ( " [Alignment]\n"
                    " Query, Target, Distance" );
      n = strlen(hits[i]->qSeq);
      for (j=0;j<n;j++)  {
        sprintf ( S," %c, %c, %.4f",
                    hits[i]->qSeq[j],hits[i]->tSeq[j],hits[i]->dist[j] );
        f.WriteLine ( S );
      }
    }
    f.LF();
  }

  f.shut();

}

void gsmt::Hits::writeStructureHits_json ( PStructure  s,
                                           mmdb::cpstr fName )  {
JSON *json = new JSON();
JSON *json1[7];
JSON *json2;
char  S[100];  
int   i,c;

  for (i=0;i<7;i++)
    json1[i] = new JSON();

  c = 0;
  json1[c++]->addValue ( "title","PDB code"   );
  json1[c++]->addValue ( "title","Size"       );
  json1[c++]->addValue ( "title","Q-score"    );
  json1[c++]->addValue ( "title","R.m.s.d."   );
  json1[c++]->addValue ( "title","Nalign"     );
  json1[c++]->addValue ( "title","Seq. Id"    );
  json1[c  ]->addValue ( "title","PDB Title"  );

  c = 0;
  json1[c++]->addValue ( "tooltip","PDB code"   );
  json1[c++]->addValue ( "tooltip","Size of PDB structure" );
  json1[c++]->addValue ( "tooltip","Q-score"    );
  json1[c++]->addValue ( "tooltip","R.m.s.d. between C-alpha atoms in "
                                   "best structure superposition" );
  json1[c++]->addValue ( "tooltip","Alignment length (number of aligned "
                                   "residues"   );
  json1[c++]->addValue ( "tooltip","Sequence identity" );
  json1[c  ]->addValue ( "tooltip","PDB structure title" );
  
  for (i=0;i<nHits;i++)  {
    c = 0;
    sprintf ( S,"%s:%s",hits[i]->pdbCode,hits[i]->chainID );
    json1[c++]->addValue ( "value",S                      );
    json1[c++]->addValue ( "value",hits[i]->nRes          );
    json1[c++]->addValue ( "value",hits[i]->Q    ,"%.4f"  );
    json1[c++]->addValue ( "value",hits[i]->rmsd ,"%.4f"  );
    json1[c++]->addValue ( "value",hits[i]->Nalgn         );
    json1[c++]->addValue ( "value",hits[i]->seqId,"%.4f"  );
    if (hits[i]->pdbTitle)
          json1[c]->addValue ( "value",hits[i]->pdbTitle  );
    else  json1[c]->addValue ( "value","No title" );
  }
  
  json2 = new JSON();
  json2->addValue ( "file"     ,mmdb::io::GetFName(s->getFilePath()) );
  json2->addValue ( "selection",s->getSelString() );
  json2->addValue ( "size"     ,s->getNCalphas () );

  json->addJSON ( "query",json2 );

  for (i=0;i<7;i++)
    json->addJSON ( "columns",json1[i] );
  
  json->write ( fName,0 );
  
  delete json;

}

void gsmt::Hits::writeSequenceHits_csv ( mmdb::cpstr fName )  {
mmdb::io::File f;
mmdb::pstr     sid;
char           S[2000];
int            i;

  f.assign ( fName,true );
  if (!f.rewrite())  {
    printf ( "\n *** can't open file '%s' for writing\n",fName );
    return;
  }

  for (i=0;i<nHits;i++)  {
    if (hits[i]->pdbCode[0])  sid = hits[i]->pdbCode;
                        else  sid = hits[i]->fileName;
    sprintf ( S," === HIT #, %i\n"
                " [Target]\n"
                "  Chain:, %s:%s\n"
                "  Length:, %i\n"
                "  Title:, \"%s\"\n"
                " [Scores]\n"
                "  Seq Id:, %.4f\n"
                "  Similarity:, %.4f\n"
                "  Score:, %.4f\n"
                "  N_align:, %i",
                i+1,sid,hits[i]->chainID,hits[i]->nRes,
                hits[i]->pdbTitle,hits[i]->seqId,hits[i]->rmsd,
                hits[i]->Q,hits[i]->Nalgn );
    f.WriteLine ( S );
    if (hits[i]->qSeq)  {
      sprintf ( S," [Alignment]\n"
                  " Query:,  %s\n"
                  " Target:, %s",
                  hits[i]->qSeq,hits[i]->tSeq );
      f.WriteLine ( S );
    }
    f.LF();
  }

  f.shut();

}

void gsmt::Hits::writeSequenceHits_json ( PStructure  s,
                                          mmdb::cpstr fName )  {
}

void gsmt::Hits::takeHits ( PPHit & hit_array, int & n_hits )  {
// Removes pointer as is; releasing memory is responsibility
// of calling process. Both array and its elements should be
// deallocated.
  hit_array  = hits;
  n_hits     = nHits;
  hits       = NULL;
  nHits      = 0;
  nHitsAlloc = 0;
}
