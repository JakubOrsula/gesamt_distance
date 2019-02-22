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
//    14.06.16   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  GSMT_Hits <implementation>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::SubEntry
//       ~~~~~~~~~  gsmt::Entry
//                  gsmt::Index
//
//  (C) E. Krissinel 2008-2016
//
// =================================================================
//

#include <string.h>

#if defined(__DEC) || defined(_WIN32)
#define USE_DIRENT_H 1
#endif

#if USE_DIRENT_H
 #include <dirent.h>
 #ifdef __MINGW32__
   #include <sys/stat.h>
 #endif
#else
 #include <sys/dir.h>
#endif

#include "gsmt_index.h"
#include "gesamtlib/shiftsort_.h"

// =================================================================

void  gsmt::SubEntry::init()  {
  id[0]         = char(0);
  offset_struct = -1;
  offset_seq    = -1;
  size          = 0;
  selected      = true;
}

void  gsmt::SubEntry::copy ( PSubEntry source )  {
  strcpy ( id,source->id );
  offset_struct = source->offset_struct;
  offset_seq    = source->offset_seq;
  size          = source->size;
  selected      = source->selected;
}

void  gsmt::SubEntry::write ( mmdb::io::RFile f )  {
  f.WriteTerLine ( id,false       );
  f.WriteLong    ( &offset_struct );
  f.WriteLong    ( &offset_seq    );
  f.WriteShort   ( &size          );
}

void  gsmt::SubEntry::read ( mmdb::io::RFile f )  {
  f.ReadTerLine ( id,false       );
  f.ReadLong    ( &offset_struct );
  f.ReadLong    ( &offset_seq    );
  f.ReadShort   ( &size          );
  selected = true;
}


// =================================================================
    
gsmt::Entry::Entry()  {
  id[0]       = char(0);
  fname       = NULL;
  packNo      = -1;
  nSubEntries = 0;
  subEntries  = NULL;
}

gsmt::Entry::~Entry()  {
  clear();
}
    
void gsmt::Entry::clear()  {
  
  if (fname)  delete[] fname;
  
  if (subEntries)  {
    for (int i=0;i<nSubEntries;i++)
      if (subEntries[i])  delete subEntries[i];
    delete[] subEntries;
    subEntries = NULL;
  }
  
  id[0]       = char(0);
  fname       = NULL;
  packNo      = -1;
  nSubEntries = 0;

}

void gsmt::Entry::addSubEntry ( PSubEntry subEntry )  {
PPSubEntry subEnt = subEntries;
  subEntries = new PSubEntry[nSubEntries+1];
  for (int i=0;i<nSubEntries;i++)
    subEntries[i] = subEnt[i];
  if (subEnt)  delete[] subEnt;
  subEntries[nSubEntries++] = subEntry;
}

void gsmt::Entry::write ( mmdb::io::RFile f )  {
  f.WriteTerLine ( id,false     );
  f.CreateWrite  ( fname        );
  f.WriteShort   ( &packNo      );
  f.WriteShort   ( &nSubEntries );
  for (int i=0;i<nSubEntries;i++ )
    subEntries[i]->write ( f );
}

void gsmt::Entry::read ( mmdb::io::RFile f )  {
  clear();
  f.ReadTerLine ( id,false     );
  f.CreateRead  ( fname        );
  f.ReadShort   ( &packNo      );
  f.ReadShort   ( &nSubEntries );
  subEntries = new PSubEntry[nSubEntries];
  for (int i=0;i<nSubEntries;i++ )  {
    subEntries[i] = new SubEntry();
    subEntries[i]->read ( f );
  }
}

// =================================================================

/*
namespace gsmt  {
  
  DefineClass(SortEntries);
  
  class SortEntries : public mmdb::QuickSort  {

    public:
      SortEntries ();
      ~SortEntries();

      void sortByIds    ( PPEntry entries, int nEntries );
      void sortByFNames ( PPEntry entries, int nEntries );
      void sortByFPaths ( PPEntry entries, int nEntries );
      void sortByPackNo ( PPEntry entries, int nEntries );
      void sortByOffset ( PPEntry entries, int nEntries );
 
    protected:
      PPEntry entry;
      int     sortMode;
      virtual int  Compare ( int i, int j );
      virtual void Swap    ( int i, int j );

  };

}

gsmt::SortEntries::SortEntries() : mmdb::QuickSort()  {
  sortMode = 1;
}

gsmt::SortEntries::~SortEntries()  {}

void gsmt::SortEntries::sortByIds ( PPEntry entries, int nEntries )  {
  entry    = entries;
  sortMode = 1;
  if (nEntries>1)
    Sort ( (void*)entries,nEntries );
}

void gsmt::SortEntries::sortByFNames ( PPEntry entries, int nEntries ) {
  entry    = entries;
  sortMode = 2;
  if (nEntries>1)
    Sort ( (void*)entries,nEntries );
}

void gsmt::SortEntries::sortByFPaths ( PPEntry entries, int nEntries ) {
  entry    = entries;
  sortMode = 3;
  if (nEntries>1)
    Sort ( (void*)entries,nEntries );
}

void gsmt::SortEntries::sortByPackNo ( PPEntry entries, int nEntries ) {
  entry    = entries;
  sortMode = 4; 
  if (nEntries>1)
    Sort ( (void*)entries,nEntries );
}

void gsmt::SortEntries::sortByOffset ( PPEntry entries, int nEntries ) {
  entry    = entries;
  sortMode = 5;
  if (nEntries>1)
    Sort ( (void*)entries,nEntries );
}
 
int gsmt::SortEntries::Compare ( int i, int j )  {
PEntry ei,ej;
long   li,lj;
int    cmp;

  ei = entry[i];
  ej = entry[j];
  
  switch (sortMode)  {

    default:
    case 1:  // sort by increasing ids
        cmp = strcmp ( ej->id,ei->id );
      break;

    case 2:  // sort by increasing file names
        if (ej->fname && ei->fname)
          cmp = strcmp ( mmdb::io::GetFName(ei->fname,mmdb::io::syskey_all),
                         mmdb::io::GetFName(ej->fname,mmdb::io::syskey_all) );
        else if (ej->fname)
          cmp = -1;
        else if (ei->fname)
          cmp =  1;
        else
          cmp = 0;
      break;

    case 3:  // sort by increasing file paths
        if (ej->fname && ei->fname)
          cmp = strcmp ( ei->fname,ej->fname );
        else if (ej->fname)
          cmp = -1;
        else if (ei->fname)
          cmp =  1;
        else
          cmp = 0;
      break;

    case 4:  // sort by pack number
        if (ei->packNo<ej->packNo)       cmp = -1;
        else if (ei->packNo>ej->packNo)  cmp =  1;
                                   else  cmp =  0;
      break;
      
    case 5:
        if (ei->nSubEntries>0)  li = ei->subEntries[0]->offset;
                          else  li = mmdb::MaxInt4;
        if (ej->nSubEntries>0)  lj = ej->subEntries[0]->offset;
                          else  lj = mmdb::MaxInt4;
        if (li<lj)       cmp = -1;
        else if (li>lj)  cmp =  1;
                   else  cmp =  0;
      
  }

  return cmp;

}

void gsmt::SortEntries::Swap ( int i, int j )  {
PEntry ent;
  ent      = entry[i];
  entry[i] = entry[j];
  entry[j] = ent;
}

*/

namespace gsmt  {
  
  DefineClass(SortEntries);
  
  class SortEntries : public ShiftSort  {

    public:
      SortEntries ();
      ~SortEntries();

      void sortByIds    ( PPEntry entries, int nEntries );
      void sortByFNames ( PPEntry entries, int nEntries );
      void sortByFPaths ( PPEntry entries, int nEntries );
      void sortByPackNo ( PPEntry entries, int nEntries );
      void sortByOffset ( PPEntry entries, int nEntries );
 
    protected:
      int  sortMode;
      virtual int Compare ( void * di, void * dj );

  };

}

gsmt::SortEntries::SortEntries() : ShiftSort()  {
  sortMode = 1;
}

gsmt::SortEntries::~SortEntries()  {}

void gsmt::SortEntries::sortByIds ( PPEntry entries, int nEntries )  {
  sortMode = 1;
  if (nEntries>1)
    Sort ( (void**)entries,nEntries );
}

void gsmt::SortEntries::sortByFNames ( PPEntry entries, int nEntries ) {
  sortMode = 2;
  if (nEntries>1)
    Sort ( (void**)entries,nEntries );
}

void gsmt::SortEntries::sortByFPaths ( PPEntry entries, int nEntries ) {
  sortMode = 3;
  if (nEntries>1)
    Sort ( (void**)entries,nEntries );
}

void gsmt::SortEntries::sortByPackNo ( PPEntry entries, int nEntries ) {
  sortMode = 4; 
  if (nEntries>1)
    Sort ( (void**)entries,nEntries );
}

void gsmt::SortEntries::sortByOffset ( PPEntry entries, int nEntries ) {
  sortMode = 5;
  if (nEntries>1)
    Sort ( (void**)entries,nEntries );
}
 
int gsmt::SortEntries::Compare ( void * di, void * dj )  {
PEntry ei,ej;
long   li,lj;
int    cmp;

  ei = PEntry(di);
  ej = PEntry(dj);
  
  switch (sortMode)  {

    default:
    case 1:  // sort by increasing ids
        cmp = strcmp ( ej->id,ei->id );
      break;

    case 2:  // sort by increasing file names
        if (ej->fname && ei->fname)
          cmp = strcmp ( mmdb::io::GetFName(ei->fname,mmdb::io::syskey_all),
                         mmdb::io::GetFName(ej->fname,mmdb::io::syskey_all) );
        else if (ej->fname)
          cmp = -1;
        else if (ei->fname)
          cmp =  1;
        else
          cmp = 0;
      break;

    case 3:  // sort by increasing file paths
        if (ej->fname && ei->fname)
          cmp = strcmp ( ei->fname,ej->fname );
        else if (ej->fname)
          cmp = -1;
        else if (ei->fname)
          cmp =  1;
        else
          cmp = 0;
      break;

    case 4:  // sort by pack number
        if (ei->packNo<ej->packNo)       cmp = -1;
        else if (ei->packNo>ej->packNo)  cmp =  1;
                                   else  cmp =  0;
      break;
      
    case 5:
        if (ei->nSubEntries>0)  li = ei->subEntries[0]->offset_struct;
                          else  li = mmdb::MaxInt4;
        if (ej->nSubEntries>0)  lj = ej->subEntries[0]->offset_struct;
                          else  lj = mmdb::MaxInt4;
        if (li<lj)       cmp = -1;
        else if (li>lj)  cmp =  1;
                   else  cmp =  0;
      
  }

  return cmp;

}


// =================================================================

gsmt::Index::Index()  {
  entries           = NULL;
  nEntries          = 0;
  pack_index        = NULL;
  pack_size         = NULL;
  pack_struct_fsize = NULL;
  pack_seq_fsize    = NULL;
  nPacks            = 0;
  nPackedEntries    = 0;
  nPackedSubEntries = 0;
  path0             = NULL;
  nEntriesAlloc     = 0;
  compressed        = false;
  prepared          = false;
  sortState         = UNSORTED;
}

gsmt::Index::~Index()  {
  clear();
}

void gsmt::Index::clear()  {
  if (entries)  {
    for (int i=0;i<nEntries;i++)
      if (entries[i])  delete entries[i];
    delete[] entries;
    entries = NULL;
  }
  nEntries          = 0;
  mmdb::FreeVectorMemory ( pack_index,0 );
  mmdb::FreeVectorMemory ( pack_size ,0 );
  mmdb::FreeVectorMemory ( pack_struct_fsize,0 );
  mmdb::FreeVectorMemory ( pack_seq_fsize   ,0 );
  nPacks            = 0;
  nPackedEntries    = 0;
  nPackedSubEntries = 0;
  if (path0)  {
    delete[] path0;
    path0 = NULL;
  }
  nEntriesAlloc = 0;
  prepared      = false;
}

void gsmt::Index::makePackIndex()  {
int  npack;

  sortByPackNo();
  
  nPackedEntries    = 0;
  nPackedSubEntries = 0;
  
  npack = -mmdb::MaxInt;
  for (int i=0;i<nEntries;i++)
    if (entries[i]->packNo>=0)  {
      if (entries[i]->packNo>npack)  {
        npack = entries[i]->packNo;
        pack_index[npack] = i;
        pack_size [npack] = 0; 
      }
      pack_size[npack]++;
      if (entries[i]->nSubEntries>0)  {
        nPackedEntries++;
        nPackedSubEntries += entries[i]->nSubEntries;
      }
    }
  
}

void gsmt::Index::addEntry ( PEntry entry )  {
PPEntry ent;

  if (nEntries>=nEntriesAlloc)  {
    nEntriesAlloc += 20000;
    ent     = entries;
    entries = new PEntry[nEntriesAlloc];
    for (int i=0;i<nEntries;i++)
      entries[i] = ent[i];
    if (ent)  delete[] ent;
  }
  
  entries[nEntries++] = entry;
  
}

void gsmt::Index::sortByIds()  {
SortEntries sortEntries;
  sortEntries.sortByIds ( entries,nEntries );
  sortState = SORTED_BY_Ids;
}

void gsmt::Index::sortByFNames()  {
SortEntries sortEntries;
  sortEntries.sortByFNames ( entries,nEntries );
  sortState = SORTED_BY_FNames;
}

void gsmt::Index::sortByFPaths()  {
SortEntries sortEntries;
  sortEntries.sortByFPaths ( entries,nEntries );
  sortState = SORTED_BY_FPaths;
}

void gsmt::Index::sortByPackNo()  {
SortEntries sortEntries;
int         i0,i1;

  sortEntries.sortByPackNo ( entries,nEntries );
  i0 = 0;
  for (i1=1;i1<nEntries;i1++)
    if (entries[i1]->packNo!=entries[i0]->packNo)  {
      sortEntries.sortByOffset ( &(entries[i0]),i1-i0 );
      i0 = i1;
    }

  if (i0<nEntries-1)
    sortEntries.sortByOffset ( &(entries[i0]),nEntries-i0 );

  sortState = SORTED_BY_PackNo;

}


void gsmt::Index::readDir ( mmdb::cpstr dirPath )  {
  clear();
  mmdb::CreateCopy ( path0,dirPath );
  read_dir ( "" );
}


void gsmt::Index::read_dir ( mmdb::cpstr path )  {
// All paths to directories should not have trailing separator
// (slash or back slash). Entries returned in 'list' contain file
// paths relative to path0 and starting with the separator, so that
// the full path may be obtained by concatenation of 'path0' and
// entry from the list.
DIR           *dir;
#if USE_DIRENT_H
dirent        *dp;
 #ifdef __MINGW32__
  struct stat s;
 #endif
#else
direct        *dp;
#endif
PEntry         ent;
mmdb::cpstr    fext;
mmdb::pstr     path1;
#ifdef __MINGW32__
  mmdb::pstr   crpath = NULL;
#endif
int            n;

  path1 = NULL;
  if (path[0])  {
    n = strlen(path0) - 1;
    if ((path[0]=='/') || (path[0]=='\\'))  fext = &(path[1]);
                                      else  fext = path;
    if ((path0[n]=='/') || (path0[n]=='\\'))
          mmdb::CreateCopCat ( path1,path0,fext );
    else  mmdb::CreateCopCat ( path1,path0,mmdb::io::_dir_sep,fext );
    dir = opendir ( path1 );
#ifdef __MINGW32__
    mmdb::CreateCopy( crpath,path1 );
#endif
  } else  {
    dir = opendir ( path0 );
#ifdef __MINGW32__
    mmdb::CreateCopy( crpath,path0 );
#endif
  }

#ifdef __MINGW32__
  n = strlen(crpath) - 1;
  if ((crpath[n]=='/') || (crpath[n]=='\\'))
    crpath[n] = char(0);
#endif

  if (dir)  {
  
    dp = readdir ( dir );
    while (dp)  {

      if (dp->d_name[0]!='.')  {
#ifdef __MINGW32__
        mmdb::CreateCopCat ( path1,crpath,mmdb::io::_dir_sep,dp->d_name );
        stat ( path1,&s );
        if ((s.st_mode & S_IFMT) == S_IFDIR) {
#else
        if (dp->d_type==DT_DIR)  {
#endif          
          mmdb::CreateCopCat ( path1,path,mmdb::io::_dir_sep,dp->d_name );
          read_dir ( path1 );
        
        } else {

          fext = mmdb::io::GetFExt(dp->d_name);
          if ((!strcmp(fext,".pdb"))    ||
              (!strcmp(fext,".ent"))    ||
              (!strcmp(fext,".cif"))
#ifndef __MINGW32__
                                        ||
              (!strcmp(fext,".pdb.gz")) ||
              (!strcmp(fext,".ent.gz")) ||
              (!strcmp(fext,".cif.gz"))
#endif
             )  {
            ent = new Entry();
            mmdb::CreateCopCat ( ent->fname,path,mmdb::io::_dir_sep,
                                 dp->d_name );
            addEntry ( ent );
          }
        
        }  
      
      }

      dp = readdir ( dir );
    
    }

    closedir ( dir );
    
  }
  
  if (path1)  delete[] path1;

  
}

void gsmt::Index::initPacks ( int n_packs )  {
int n;

  mmdb::FreeVectorMemory ( pack_index       ,0 );
  mmdb::FreeVectorMemory ( pack_size        ,0 );
  mmdb::FreeVectorMemory ( pack_struct_fsize,0 );
  mmdb::FreeVectorMemory ( pack_seq_fsize   ,0 );

  nPacks = n_packs;
  mmdb::GetVectorMemory ( pack_index       ,nPacks,0 );
  mmdb::GetVectorMemory ( pack_size        ,nPacks,0 );
  mmdb::GetVectorMemory ( pack_struct_fsize,nPacks,0 );
  mmdb::GetVectorMemory ( pack_seq_fsize   ,nPacks,0 );
  
  for (n=0;n<nPacks;n++)  {
    pack_index       [n] = -1;
    pack_size        [n] =  0;
    pack_struct_fsize[n] =  0;
    pack_seq_fsize   [n] =  0;
  }

}

int gsmt::Index::mergeIndex ( RIndex index0 )  {
//  assume both indexes sorted by file name
int i0,nNewEntries;

  nNewEntries = 0;
  i0          = 0;
  for (int i=0;i<nEntries;i++)
    if (i0>=index0.nEntries)  {
      entries[i]->packNo = -1;
      nNewEntries++;
    } else if (strcmp(entries[i]->fname,index0.entries[i0]->fname))  {
      entries[i]->packNo = -1;
      nNewEntries++;
    } else  {
      delete entries[i];
      entries[i] = index0.entries[i0];
      index0.entries[i0] = NULL;
      i0++;
    }

  initPacks ( index0.nPacks );

  for (int i=0;i<nPacks;i++)  {
    pack_struct_fsize[i] = index0.pack_struct_fsize[i];
    pack_seq_fsize   [i] = index0.pack_seq_fsize   [i];
    pack_size        [i] = index0.pack_size        [i];
    pack_index       [i] = index0.pack_index       [i];
  }
  
  nPackedEntries    = index0.nPackedEntries;
  nPackedSubEntries = index0.nPackedSubEntries;
  
  return nNewEntries;
    
}

gsmt::PEntry gsmt::Index::getEntry ( const mmdb::IDCode pdbCode )  {
PEntry entry;
int    i,i1,i2,cmp;

  entry = NULL;

  if (sortState!=SORTED_BY_Ids)  {

    for (i=0;(i<nEntries) && (!entry);i++)
      if (!strcasecmp(entries[i]->id,pdbCode))
        entry = entries[i];

  } else  {

    i1 = 0;
    i2 = nEntries-1;
    while ((i2-i1>1) && (!entry))  {
      i = (i1+i2)/2;
      cmp = strcasecmp(pdbCode,entries[i]->id);
      if (cmp>0)       i2 = i;
      else if (cmp<0)  i1 = i;
                 else  entry = entries[i];
    }
    if ((!entries) && (i2>i1))  {
      if (!strcasecmp(pdbCode,entries[i1]->id))
        entry = entries[i1];
      else if (!strcasecmp(pdbCode,entries[i2]->id))
        entry = entries[i2];
    }

  }

  return entry;

}

void gsmt::Index::markInclusions ( mmdb::IDCode * list )  {
  markEntries ( list,true );
}

void gsmt::Index::markExclusions ( mmdb::IDCode * list )  {
  markEntries ( list,false );
}

void gsmt::Index::markEntries ( mmdb::IDCode * list, bool mark ) {
PEntry       entry;
PSubEntry    subEntry;
char         c;
SORT_STATE   sort_state;
int          i,j;

  if (!list)  return;

  for (i=0;i<nEntries;i++)  {
    entry = entries[i];
    if (entry)
      for (j=0;j<entry->nSubEntries;j++)  {
        subEntry = entry->subEntries[j];
        if (subEntry)
          subEntry->selected = !mark;
      }
  }
  
  sort_state = sortState;
  if (sort_state!=SORTED_BY_Ids)
    sortByIds();

  i = 0;
  while (list[i][0])  {
    c = list[i][4];
    list[i][4] = char(0);
    entry = getEntry ( list[i] );
    list[i][4] = c;
    if (entry)  {
      if (list[i][5]=='*')  {
        for (j=0;j<entry->nSubEntries;j++)
          entry->subEntries[j]->selected = mark;
      } else  {
        for (j=0;j<entry->nSubEntries;j++)
          if (!strcmp(entry->subEntries[j]->id,&(list[i][5])))  {
            entry->subEntries[j]->selected = mark;
            break;
          }
      }
    }
    i++;
  }


  if (sortState!=sort_state)
    switch (sort_state)  {
      case SORTED_BY_FNames:  sortByFNames();  break;
      case SORTED_BY_FPaths:  sortByFPaths();  break;
      case SORTED_BY_PackNo:  sortByPackNo();  break;
      default: ;
    }

}


void gsmt::Index::write ( mmdb::io::RFile f )  {
int n;
int version = 2;
  f.WriteInt  ( &version           );
  f.WriteInt  ( &nEntries          );
  f.WriteInt  ( &nPacks            );
  f.WriteInt  ( &nPackedEntries    );
  f.WriteInt  ( &nPackedSubEntries );
  f.WriteBool ( &compressed        );
  f.WriteBool ( &prepared          );
  for (int i=0;i<nEntries;i++)
    entries[i]->write ( f );
  for (int i=0;i<nPacks;i++)  {
    f.WriteInt  ( &(pack_index       [i]) );
    f.WriteInt  ( &(pack_size        [i]) );
    f.WriteLong ( &(pack_struct_fsize[i]) );
    f.WriteLong ( &(pack_seq_fsize   [i]) );
  }
  n = sortState;
  f.WriteInt ( &n );

/*
  mmdb::io::File ff;
  ff.assign ( "/Users/eugene/Projects/Gesamt/qtgesamt/entries-written.txt",true,false );
  ff.rewrite();
  char S[1000];
  for (int i=0;i<nEntries;i++)  {
    long l = -1;
    if (entries[i]->nSubEntries>0)
      l = entries[i]->subEntries[0]->offset;
    sprintf ( S," %5i %s %s  %i %i  %li",i,entries[i]->id,entries[i]->fname,entries[i]->packNo,entries[i]->nSubEntries,l );
    ff.WriteLine ( S );
  }
  ff.shut();  
*/

}

void gsmt::Index::read ( mmdb::io::RFile f )  {
int version,n;
  clear();
  f.ReadInt  ( &version           );
  f.ReadInt  ( &nEntries          );
  f.ReadInt  ( &nPacks            );
  f.ReadInt  ( &nPackedEntries    );
  f.ReadInt  ( &nPackedSubEntries );
  f.ReadBool ( &compressed        );
  f.ReadBool ( &prepared          );
  nEntriesAlloc = nEntries;
  entries = new PEntry[nEntriesAlloc];
  for (int i=0;i<nEntries;i++)  {
    entries[i] = new Entry();
    entries[i]->read ( f );
  }
  if (nPacks>0)  {
    mmdb::GetVectorMemory ( pack_index       ,nPacks,0 );
    mmdb::GetVectorMemory ( pack_size        ,nPacks,0 );
    mmdb::GetVectorMemory ( pack_struct_fsize,nPacks,0 );
    mmdb::GetVectorMemory ( pack_seq_fsize   ,nPacks,0 );
    for (int i=0;i<nPacks;i++)  {
      f.ReadInt  ( &(pack_index       [i]) );
      f.ReadInt  ( &(pack_size        [i]) );
      f.ReadLong ( &(pack_struct_fsize[i]) );
      f.ReadLong ( &(pack_seq_fsize   [i]) );
    }
  }
  if (version>1)  {
    f.ReadInt ( &n );
    sortState = SORT_STATE(n);
  } else
    sortState = SORTED_BY_PackNo;

  /*
  mmdb::io::File ff;
  ff.assign ( "/Users/eugene/Projects/Gesamt/qtgesamt/entries.txt",true,false );
  ff.rewrite();
  char S[1000];
  for (int i=0;i<nEntries;i++)  {
    sprintf ( S," %5i %s %s  %i %i",i,entries[i]->id,entries[i]->fname,entries[i]->packNo,entries[i]->nSubEntries );
    ff.WriteLine ( S );
  }
  ff.shut();  
  */

}

bool gsmt::Index::store ( mmdb::cpstr fname )  {
mmdb::io::File f;

  sortByPackNo();

  f.assign ( fname,false,true );
  if (f.rewrite())  {
    write ( f );
    f.shut();
    return true;
  }
  
  return false;
  
}

bool gsmt::Index::restore ( mmdb::cpstr fname )  {
mmdb::io::File f;

  f.assign ( fname,false,true );
  if (f.reset())  {
    read  ( f );
    f.shut();
    return true;
  }
  
  return false;
  
}

bool gsmt::Index::readSummary ( mmdb::cpstr fname )  {
mmdb::io::File f;
int      version;

  f.assign ( fname,false,true );
  if (f.reset())  {
    clear();
    f.ReadInt  ( &version           );
    f.ReadInt  ( &nEntries          );
    f.ReadInt  ( &nPacks            );
    f.ReadInt  ( &nPackedEntries    );
    f.ReadInt  ( &nPackedSubEntries );
    f.ReadBool ( &compressed        );
    f.ReadBool ( &prepared          );
    f.shut();
    return true;
  }
  
  return false;

}
