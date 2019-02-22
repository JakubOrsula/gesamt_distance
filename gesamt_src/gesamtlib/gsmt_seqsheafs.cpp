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
//  **** Module  :  GSMT_SeqSheafs <implementation>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::SeqSheafItem
//       ~~~~~~~~~  gsmt::SeqSheaf
//                  gsmt::seqSheafs
//
//  (C) E. Krissinel 2008-2016
//
// =================================================================
//

#include "gsmt_seqsheafs.h"
#include "gsmt_archive.h"

// =================================================================

gsmt::SeqSheafItem::SeqSheafItem()  {
  score    = 0.0;
  id[0]    = char(0);
  startPos = -1;
  packNo   = -1;
  subEntry.init();
}

void gsmt::SeqSheafItem::writeSummaryTitle ( mmdb::io::RFile f )  {
  f.WriteLine ( " PDB Id   Start  Score" );  
}

void gsmt::SeqSheafItem::writeSummary ( mmdb::io::RFile f )  {
char S[200];
  sprintf ( S,"  %s:%s %4i %8.4f",id,subEntry.id,startPos,score );
  f.WriteLine ( S );
}

gsmt::PStructure gsmt::SeqSheafItem::getStructure (
                                        mmdb::io::PFile structFile,
                                        int sheafLen )  {
PStructure      s;
mmdb::PPResidue r;
char            CID[100];
int             nr,n2;

  s = Archive::readStructure ( structFile,subEntry.offset_struct );
  if (s)  {
    s->getChainRes ( r,nr );
    n2 = startPos+sheafLen-1;
    if (nr>n2)  {
      sprintf ( CID,"%i.%s-%i.%s",
                    r[startPos]->GetSeqNum(),r[startPos]->GetInsCode(),
                    r[n2]->GetSeqNum(),r[n2]->GetInsCode() );
      s->selectSubStructure ( CID );
      sprintf ( CID,"%s:%s:%04i",id,subEntry.id,startPos );
      s->setRefName ( CID );
    } else  {
      delete s;
      s = NULL;
    }
  }
  
  return s;

}


// =================================================================

gsmt::SeqSheaf::SeqSheaf() : ShiftSort()  {
  items         = NULL;  // all subsequence matches
  S             = NULL;  // all structures of subsequences
  nItems        = 0;     // number of subsequence matcjes
  nMaxItems     = 100;   // maximum bumber of sequence matches
  sheafLen      = 0;     // length of subsequences
  structSheafs  = NULL;  // structure sheafs
  nStructSheafs = 0;     // number of structure sheafs 
}

gsmt::SeqSheaf::~SeqSheaf()  {
  freeMemory();
}

void gsmt::SeqSheaf::freeSeqData()  {
  if (items)  {
    for (int i=0;i<nItems;i++)
      if (items[i])  delete items[i];
    delete[] items;
    items = NULL;
  }
  nItems = 0;
}

void gsmt::SeqSheaf::freeStructData()  {
  if (S)  {
    for (int i=0;i<nItems;i++)
      if (S[i])  delete S[i];
    delete[] S;
    S = NULL;
  }
}

void gsmt::SeqSheaf::freeSheafData()  {
  if (structSheafs)  {
    for (int i=0;i<nStructSheafs;i++)
      if (structSheafs[i])  delete structSheafs[i];
    delete[] structSheafs;
    structSheafs = NULL;
  }
  nStructSheafs = 0;
}

void gsmt::SeqSheaf::freeMemory()  {
  freeStructData();
  freeSeqData   ();
  freeSheafData ();
} 
 
void gsmt::SeqSheaf::addItem ( PSeqSheafItem item )  {

  if (!items)  {
    items = new PSeqSheafItem[nMaxItems];
    for (int i=0;i<nMaxItems;i++)
      items[i] = NULL;
    nItems = 0;
  }

  if (nItems>=nMaxItems)  {
    mmdb::realtype score0 = items[0]->score;
    int            n0     = 0;
    for (int i=1;i<nItems;i++)
      if (items[i]->score<score0)  {
        score0 = items[i]->score;
        n0     = i;
      }
    if (item->score>score0)  {
      delete items[n0];
      items[n0] = item;
    }
  } else
    items[nItems++] = item;

}

void gsmt::SeqSheaf::merge ( PSeqSheaf source )  {
PPSeqSheafItem items1;
int            n,i,j;

  n = nItems + source->nItems;
  items1 = new PSeqSheafItem[n];
  for (i=0;i<nItems;i++)
    items1[i] = items[i];
  items = items1;

  j = 0;
  for (i=nItems;i<n;i++)  {
    items[i] = source->items[j];
    source->items[j] = NULL;
    j++;
  }
  
  nItems = n;

}

int gsmt::SeqSheaf::Compare ( void * di, void * dj )  {

  switch (sKey)  {

    case SORT_ITEMS_Score: // sort items by decreasing score size
      if (PSeqSheafItem(di)->score<PSeqSheafItem(dj)->score) return  1;
      if (PSeqSheafItem(di)->score>PSeqSheafItem(dj)->score) return -1;

    case SORT_ITEMS_File : // sort items by increasing pack number and
                           // offset within the pack
      if (PSeqSheafItem(di)->packNo<PSeqSheafItem(dj)->packNo) return -1;
      if (PSeqSheafItem(di)->packNo>PSeqSheafItem(dj)->packNo) return  1;
    
      if (PSeqSheafItem(di)->subEntry.offset_struct <
          PSeqSheafItem(dj)->subEntry.offset_struct) return -1;
      if (PSeqSheafItem(di)->subEntry.offset_struct >
          PSeqSheafItem(dj)->subEntry.offset_struct) return  1;

      return 0;

    case SORT_SHEAFS_SeqId: // sort sheafs by decreasing average seq.id
      if (PSheafData(di)->seqId<PSheafData(dj)->seqId) return  1;
      if (PSheafData(di)->seqId>PSheafData(dj)->seqId) return -1;

    case SORT_SHEAFS_Size : // sort sheafs by decreasing sheaf size
      if (PSheafData(di)->nStruct<PSheafData(dj)->nStruct) return  1;
      if (PSheafData(di)->nStruct>PSheafData(dj)->nStruct) return -1;

      return 0;

   default : return 0;

  }

}


void gsmt::SeqSheaf::sort ( SORT_KEY sortKey )  {
  sKey = sortKey;
  switch (sortKey)  {
    case SORT_ITEMS_Score :
    case SORT_ITEMS_File  : if (items)
                              ShiftSort::Sort ( (void**)items,nItems );
                         break;
    case SORT_SHEAFS_SeqId:
    case SORT_SHEAFS_Size : if (structSheafs)
                              ShiftSort::Sort ( (void**)structSheafs,
                                                nStructSheafs );
                         break;
    default : ;
  }
}

void gsmt::SeqSheaf::truncate ( int maxItems )  {

  if (maxItems>0)
    nMaxItems = maxItems;

  sort ( SORT_ITEMS_Score );
  
  if (nItems>nMaxItems)  {
    for (int i=nMaxItems;i<nItems;i++)
      if (items[i])  {
        delete items[i];
        items[i] = NULL;
      }
    nItems = nMaxItems;
  }
    
}


void gsmt::SeqSheaf::writeSummary ( mmdb::io::RFile f )  {
  gsmt::SeqSheafItem::writeSummaryTitle ( f );
  for (int i=0;i<nItems;i++)
    items[i]->writeSummary ( f );
}


void gsmt::SeqSheaf::calcSeqScores()  {
// works when sheafs are calculated

  for (int i=0;i<nStructSheafs;i++)  {
    structSheafs[i]->seqId = 0.0;
    for (int j=0;j<structSheafs[i]->nStruct;j++)
      structSheafs[i]->seqId += items[structSheafs[i]->sId[j]]->score;
    structSheafs[i]->seqId /= structSheafs[i]->nStruct;
  }

}


gsmt::PPStructure gsmt::SeqSheaf::getStructureSheaf (
                                             mmdb::cpstr archiveDir )  {
mmdb::io::PFile f;
mmdb::ovector   mask;
short           packNo;

  if (nItems<=0)
    return NULL;

  freeStructData();

  sort ( SORT_ITEMS_Score );

  S = new PStructure[nItems];
  mmdb::GetVectorMemory ( mask,nItems,0 );
  for (int i=0;i<nItems;i++)
    mask[i] = false;

  f  = NULL;
  do {
    packNo = -1;
    for (int i=0;(i<nItems) && (packNo<0);i++)
      if (!mask[i])
        packNo = items[i]->packNo;
    if (packNo>=0)  {
      if (f)  {
        f->shut();
        delete f;
      }
      f = Archive::getStructFile ( archiveDir,packNo );
      for (int i=0;i<nItems;i++)
        if ((!mask[i]) && (items[i]->packNo==packNo))  {
          S[i]    = items[i]->getStructure ( f,sheafLen );
          mask[i] = true;
        }
    }
  } while (packNo>=0);

  return S;

}

// =================================================================

gsmt::SeqSheafs::SeqSheafs()  {
  sheafs   = NULL;
  nSheafs  = 0;
  sheafLen = 0;
}

gsmt::SeqSheafs::SeqSheafs ( int n_sheafs, int sheaf_len )  {
  sheafs  = NULL;
  nSheafs = 0;
  init ( n_sheafs,sheaf_len );
}

gsmt::SeqSheafs::~SeqSheafs()  {
  freeMemory();
} 

void gsmt::SeqSheafs::freeMemory()  {
  if (sheafs)  {
    for (int i=0;i<nSheafs;i++)
      if (sheafs[i])  delete sheafs[i];
    delete[] sheafs;
    sheafs = NULL;
  }
  nSheafs = 0;
}

void gsmt::SeqSheafs::init ( int n_sheafs, int sheaf_len )  {
  freeMemory();
  nSheafs  = n_sheafs;
  sheafLen = sheaf_len;
  sheafs   = new PSeqSheaf[nSheafs];
  for (int i=0;i<nSheafs;i++)  {
    sheafs[i] = new SeqSheaf();
    sheafs[i]->sheafLen = sheafLen;
  }
}

void gsmt::SeqSheafs::merge ( PSeqSheafs source )  {
  for (int i=0;i<nSheafs;i++)
    sheafs[i]->merge ( source->sheafs[i] );
}

void gsmt::SeqSheafs::truncateSheafs ( int maxItems )  {
  for (int i=0;i<nSheafs;i++)
    sheafs[i]->truncate ( maxItems );
}

void gsmt::SeqSheafs::sortSheafs ( SeqSheaf::SORT_KEY sortKey )  {
  for (int i=0;i<nSheafs;i++)
    sheafs[i]->sort ( sortKey );
}

void gsmt::SeqSheafs::writeSummary ( mmdb::cpstr fpath )  {
mmdb::io::File f;
char           S[200];

  f.assign ( fpath,true );
  if (!f.rewrite())  {
    printf ( "\n *** can't open file '%s' for writing\n",fpath );
    return;
  }

  for (int i=0;i<nSheafs;i++)  {
    sprintf ( S,"\n\n"
" ===================================================================\n"
" SHEAF #%04i\n\n",i );
    f.Write ( S );
    sheafs[i]->writeSummary ( f );
  }

  f.shut();

}
