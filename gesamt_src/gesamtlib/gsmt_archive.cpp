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
//  **** Module  :  GSMT_Archive <implementation>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::ProgressBar
//       ~~~~~~~~~  gsmt::Archive
//
//  (C) E. Krissinel 2008-2017
//
// =================================================================
//

#include <string.h>
#include <unistd.h>

#include "mmdb2/mmdb_io_file.h"

#include "gsmt_archive.h"
#include "gsmt_pdbscanthread.h"
#include "gsmt_structscanthread.h"
#include "gsmt_seqscanthread.h"
#include "gsmt_sequence.h"
#include "gsmt_utils.h"
#include "memio_.h"

// =================================================================

mmdb::cpstr gsmt::getArchiveIndexName()  {
  return index_file_name;
}

gsmt::Archive::Archive() : Base() {
  initArchive();
}

gsmt::Archive::Archive ( mmdb::io::RPStream Object )
             : Base(Object)  {
  initArchive();
}

gsmt::Archive::~Archive()  {
  freeMemory();
  if (inclChains)  delete[] inclChains;
  if (exclChains)  delete[] exclChains;
  pthread_mutex_destroy ( &lock );
}

void gsmt::Archive::initArchive()  {
  H            = NULL;
  inclChains   = NULL;
  exclChains   = NULL;
  seqSheafs    = NULL;
  incl_alloc   = 0;
  excl_alloc   = 0;
  nNewEntries  = 0;
  nEntriesDone = 0;
  pthread_mutex_init ( &lock,NULL );
}

void gsmt::Archive::freeMemory()  {
  if (H)  {
    delete H;
    H = NULL;
  }
  if (seqSheafs)  {
    delete seqSheafs;
    seqSheafs = NULL;
  }
}

int gsmt::Archive::readExclusionList ( mmdb::cpstr filePath )  {
  return readList ( exclChains,excl_alloc,filePath );
}

int gsmt::Archive::readInclusionList ( mmdb::cpstr filePath )  {
  return readList ( inclChains,incl_alloc,filePath );
}

int gsmt::Archive::readList ( mmdb::IDCode *& list, int & alloc_len,
                              mmdb::cpstr filePath )  {
mmdb::io::File  f;
mmdb::IDCode   *list1;
char            L[501];
int             n,fsize,new_len,i;

  if (!filePath)
    return 0;

  f.assign ( filePath,true,false );
  if (!f.reset(true))  {
    if (list)
      list[0][0] = char(0);
    return -1;
  }

  fsize = f.FileLength();
  n     = 0;
  while (!f.FileEnd())  {
    f.ReadLine ( L,500 );
    mmdb::CutSpaces ( L,mmdb::SCUTKEY_BEGEND );
    if (L[0] && (L[0]!='#'))  {
      if (n>=alloc_len-1)  {
        if (alloc_len<=0)  new_len = fsize/8 + 32;
                     else  new_len = n + 1000;
        list1 = new mmdb::IDCode[new_len];
        for (i=0;i<alloc_len;i++)
          strcpy ( list1[i],list[i] );
        for (i=alloc_len;i<new_len;i++)
          list1[i][0] = char(0);
        if (list)
          delete[] list;
        list = list1;
        alloc_len = new_len;
      }
      strcpy ( list[n++],L );
    }
  }
  
  return 0;

}

void gsmt::Archive::setRVAPIProgressWidgets ( mmdb::cpstr progressBarId,
                                              mmdb::cpstr etaId )  {
  progressBar.setRVAPIProgressWidgets ( progressBarId,etaId );
}

mmdb::realtype gsmt::Archive::getProgress()  {
mmdb::realtype p; 
  pthread_mutex_lock ( &lock );
  p = progressBar.getProgress();
  pthread_mutex_unlock ( &lock );
  return p;
}

mmdb::cpstr gsmt::Archive::getTimeString()  {
mmdb::cpstr t;
  pthread_mutex_lock ( &lock );
  t = progressBar.getTimeString();
  pthread_mutex_unlock ( &lock );
  return t;
}


// -------------------------------------------------------------------

void gsmt::copyDirPath ( mmdb::cpstr dirPath, mmdb::pstr & path )  {
  mmdb::CreateCopy ( path,dirPath );
  int n = strlen(path) - 1;
  if ((path[n]=='/') || (path[n]=='\\'))
      path[n] = char(0);
}

void gsmt::Archive::write_log ( mmdb::cpstr S, mmdb::io::RFile f,
                                int verb )  {
  if (verbosity>=0)  {
    if (verb==2)
      printf ( "%s",S );
    if (f.isOpen())
      f.Write ( S );
  }
}


void schedulePacks ( mmdb::ivector pack_len,
                     mmdb::ivector pack_len0,
                     int n_entries, int n_packs )  {
int average_size,n,m,pmax,pmin,imin;

  n = n_entries;
  for (int i=0;i<n_packs;i++)
    pack_len[i] = 0;
  
  if (pack_len0)  {
    
    pmax = -1;
    for (int i=0;i<n_packs;i++)
      if (pack_len0[i]>pmax)
        pmax = pack_len0[i];
    
    do  {
      
      pmin = mmdb::MaxInt;
      imin = -1;
      for (int i=0;i<n_packs;i++)  {
        m = pack_len[i] + pack_len0[i];
        if (m<pmin)  {
          pmin = m;
          imin = i;
        }
      }
      
      if (pmax>pmin)  {
        m = mmdb::IMin ( n,pmax-pmin );
        pack_len[imin] += m;
        n -= m;        
      }
    
    } while ((n>0) && (pmax>pmin));
    
  }
  
  if (n<=n_packs)  {
    
    for (int i=0;i<n;i++)
      pack_len[i]++;
  
  } else  {
  
    switch (n_packs)  {
      case 1:  average_size = n;   break;
      case 2:  average_size = n/2; break;
      default: average_size = n/n_packs;
               m = n - average_size;
               if (m>average_size/10+1)
                 average_size = n/(n_packs-1);
    }
    
    for (int i=0;i<n_packs;i++)  {
      if (i==n_packs-1)
        pack_len[i] += n;
      else  {
        pack_len[i] += average_size;
        n -= average_size;
      }
    }
    
  }
  
}


//  1SNN

gsmt::GSMT_CODE gsmt::Archive::prepare ( mmdb::cpstr pdbDir,
                                         mmdb::cpstr archiveDir,
                                         int         n_packs,
                                         mmdb::cpstr outFile,
                                         bool        compressed,
                                         bool        update
                                       )  {
Index            index0;
Index            index;
PSubEntry        subEntry;
PStructure       structure;
PSequence        sequence;
mmdb::PPAtom     atom;
mmdb::io::File   fstruct;
mmdb::io::File   fseq;
mmdb::io::File   fout;
mmdb::io::File   fmem;
MemIO            memIO;
mmdb::pstr       pdbPath,archPath,fpath,fn;
mmdb::ivector    pack_len;
char             archFName[1000];
char             msg[1000];
int              n,nentry,npack,npacks,packNo;
int              chainNo,k,m,nAtoms;
int              nc,packSerNo;
int              ihour,imin,isec;
mmdb::word       memPoolSize,poolSize;
mmdb::pstr       memPool;
mmdb::ERROR_CODE rc;
GSMT_CODE        GSMT_rc;
time_t           t0 = time(NULL);
bool             fopen;
  
  nNewEntries = -1;  // signal "calculating number of entries"
  
  GSMT_rc  = GSMT_Ok;
  
  memPoolSize = 1000000;
  poolSize    = 0;

  keepRunning = true;
  
  sequence    = NULL;

  fn    = NULL;
  fpath = NULL;

  if (outFile)  {
    fout.assign ( outFile,true,false );
    if (!fout.rewrite())  {
      printf ( " \n\n"
        " *** error: cannot open log file\n"
        "     %s\n"
        " for writing.\n",outFile );
      return GSMT_CantWriteLog;
    }
  }

  pdbPath  = NULL;
  archPath = NULL;
  copyDirPath ( pdbDir    ,pdbPath  );
  copyDirPath ( archiveDir,archPath );
  
  if (update)  {

    sprintf ( msg,
      "\n"
      " Updating GESAMT archive at  %s\n"
      "        from PDB archive at  %s\n",
      archiveDir,pdbDir
    );
    write_log ( msg,fout,2 );
    
    mmdb::CreateCopCat ( fpath,archPath,mmdb::io::_dir_sep,
                         index_file_name );
    if (!index0.restore(fpath))  {
      write_log ( "\n *** error: cannot open GESAMT archive for "
                  "update, so stop.\n",fout,2 );
      if (pdbPath)  delete[] pdbPath;
      if (archPath) delete[] archPath;
      return GSMT_CantReadIndex;
    }
    index0.sortByFPaths();
    npacks = index0.nPacks;
  
  } else  {

    index.compressed = compressed;
    
    if (compressed)
          sprintf ( msg,
            "\n"
            " Making compressed GESAMT archive at  %s\n"
            "                 from PDB archive at  %s\n",
            archiveDir,pdbDir
          );
    else
          sprintf ( msg,
            "\n"
            " Making uncompressed GESAMT archive at  %s\n"
            "                   from PDB archive at  %s\n",
            archiveDir,pdbDir
          );
    write_log ( msg,fout,2 );
    npacks = n_packs;

  }

  index.readDir ( pdbDir );

  if (index.nEntries<=0)  {
    write_log ( "\n"
                " *** error: no structure files found in PDB directory "
                "-- nothing to do.\n",fout,2 );
    fout.shut();
    if (fpath)    delete[] fpath;
    if (pdbPath)  delete[] pdbPath;
    if (archPath) delete[] archPath;
    return GSMT_NoPDBStructures;
  }

  index.sortByFPaths();

  if (update)  {
    nNewEntries = index.mergeIndex ( index0 );
    index.compressed = index0.compressed;
    if (nNewEntries<=0)  {
      write_log ( "\n ... no new entries found -- nothing to do.\n",
                  fout,2 );
      if (pdbPath)  delete[] pdbPath;
      if (archPath) delete[] archPath;
      return GSMT_NoNewStructures;
    } else  {
      sprintf ( msg,"\n ... %i new entries will be examined and added "
                "to GESAMT archive\n",nNewEntries );
      write_log ( msg,fout,2 );
    }
  } else  {
    nNewEntries = index.nEntries;
    sprintf ( msg,
      "\n"
      " ... total %i entries will be examined and arranged in %i packs\n"
      " ... pack size: %i entries\n",
      index.nEntries,npacks,index.nEntries/npacks );
    write_log ( msg,fout,2 );
    index.nPackedEntries    = 0;
    index.nPackedSubEntries = 0;
    index.initPacks ( npacks );
  }

  pack_len = NULL;
  mmdb::GetVectorMemory ( pack_len,npacks,0 );
  schedulePacks ( pack_len,index0.pack_size,nNewEntries,npacks );
  index0.clear();
  
  if (outFile)
       sprintf ( msg," ... log file: %s\n\n",outFile );
  else strcpy  ( msg," ... no log file will be written (use -o key)\n\n" );
  write_log ( msg,fout,2 );

  progressBar.reset      ( pbar_len     );
  progressBar.setVisible ( verbosity==1 );
  
  if (index.compressed)  memIO.setCompressionLevel ( 1 );
                   else  memIO.setCompressionLevel ( 0 );

  structure = new Structure();
  sequence  = new Sequence ();
  n      = 0;
  nentry = 0;
  
  for (packNo=0;(packNo<npacks) && (n<index.nEntries)
                                && keepRunning;packNo++)
    if (pack_len[packNo]>0)  {  // === Start new pack

      sprintf   ( msg,"\n ----- making pack %03i, entry #%i\n\n",
                      packNo,n );
      write_log ( msg,fout,verbosity );

      // open structure pack file
      sprintf ( archFName,struct_pack_name_tempate,packNo );
      mmdb::CreateCopCat ( fpath,archPath,mmdb::io::_dir_sep,archFName );
      fstruct.assign  ( fpath,false,true,mmdb::io::GZM_NONE );
      if (update && (index.pack_struct_fsize[packNo]>0))  {
        fstruct.truncate ( index.pack_struct_fsize[packNo] );
        fopen = fstruct.append();
      } else
        fopen = fstruct.rewrite();
      if (!fopen)  {
        sprintf ( msg,"\n\n"
          " *** error: cannot open pack file\n"
          "     %s\n"
          " for writing.\n",fpath );
        write_log ( msg,fout,2 );
        GSMT_rc = GSMT_CantWritePackage;
        break;
      }

      // open sequence pack file
      sprintf ( archFName,seq_pack_name_tempate,packNo );
      mmdb::CreateCopCat ( fpath,archPath,mmdb::io::_dir_sep,archFName );
      fseq.assign  ( fpath,false,true,mmdb::io::GZM_NONE );
      if (update && (index.pack_seq_fsize[packNo]>0))  {
        fseq.truncate ( index.pack_seq_fsize[packNo] );
        fopen = fseq.append();
      } else
        fopen = fseq.rewrite();
      if (!fopen)  {
        sprintf ( msg,"\n\n"
          " *** error: cannot open pack file\n"
          "     %s\n"
          " for writing.\n",fpath );
        write_log ( msg,fout,2 );
        GSMT_rc = GSMT_CantWritePackage;
        break;
      }

      packSerNo = 0;

      for (npack=0;(npack<pack_len[packNo]) && (n<index.nEntries)
                                            && keepRunning;npack++)  {
        while ((n<index.nEntries) && (index.entries[n]->packNo>=0))
          n++;
      
        if (n<index.nEntries)  {

          // There are entries,, which are not suitable for gesamt
          // (rna/dna chains and too short chains). Their data is not
          // actually written in the pack, although a non-negative pack
          // number is set in their index records. Non-negative packNo
          // means that the entry has been processed, and should not
          // be taken as a new one at repeat updates. Suitable entries
          // have a non-zero number of subentries (chains) in their
          // index records, while this number is zero for unsuitable ones.
          index.entries[n]->packNo = packNo;

          mmdb::CreateCopCat ( fpath,pdbPath,index.entries[n]->fname );
          mmdb::CreateCopy   ( fn,mmdb::io::GetFName(fpath,mmdb::io::syskey_all) );
        
          sprintf   ( msg," %6i. %s:",nentry,fn );
          write_log ( msg,fout,verbosity );
        
          chainNo = 0;
          rc      = mmdb::Error_NoError;
          k       = 0;
          nc      = 0;
          while (!rc)  {
    
            rc = structure->getStructure ( fpath,NULL,chainNo,true );
          
            if (rc!=mmdb::Error_GeneralError1)  {
          
              if (rc>0)  {
              
                sprintf ( msg,"\n *** File '%s' chain number %i error #%i"
                              " on read:\n %s\n\n",
                              fpath,chainNo,rc,
                              mmdb::GetErrorDescription(rc) );
                write_log ( msg,fout,2 );
                rc = mmdb::Error_NoError;
            
              } else if (rc<=-10000)  {
              
                sprintf ( msg,"\n *** File '%s' chain number %i error #%i"
                              " on read:\n %s\n\n",
                              fpath,chainNo,rc,
                              mmdb::GetErrorDescription(rc) );
                write_log ( msg,fout,2 );
            
              } else  {
              
                structure->getCalphas ( atom,nAtoms );
                rc = mmdb::Error_NoError;
    
                if (k>0)  {
                  m = (int)strlen(fn)+10;
                  for (int i=0;i<m;i++)
                    msg[i] = ' ';
                  msg[m] = char(0);
                  write_log ( msg,fout,verbosity );
                } else
                  strcpy ( index.entries[n]->id,
                           structure->getMMDBManager()->GetEntryID() );
                k++;
    
                if (nAtoms<=0)
                  write_log ( "? not a protein chain, skipped\n",fout,
                              verbosity );
                else  {
              
                  sprintf ( msg,"%s  %5i residues:",
                                atom[0]->GetChainID(),nAtoms );
                  write_log ( msg,fout,verbosity );
                          
                  if (nAtoms>=seg_length_default)  {
                
                    subEntry = new SubEntry();
                    strcpy ( subEntry->id,atom[0]->GetChainID() );
                    subEntry->offset_struct = fstruct.Position();
                    subEntry->offset_seq    = fseq   .Position();
                    subEntry->size          = nAtoms;
                    index.entries[n]->addSubEntry ( subEntry );

                    // writing structure into pack using memory buffers
                    // with or without compression
                    sequence->setSequence ( structure );

#ifdef __use_memory_pool
                    if (poolSize>memPoolSize)
                      memPoolSize = poolSize;

                    fmem.assign ( memPoolSize,100000,NULL );
                    fmem.rewrite();
                    structure->write  ( fmem );
                    fmem.takeFilePool ( memPool,poolSize );
                    fmem.shut();
                    memIO.reset();
                    memIO.write_buffer ( memPool,poolSize );
                    memIO.write ( fstruct );
                    if (memPool)  delete[] memPool;

                    fmem.assign ( memPoolSize,1000,NULL );
                    fmem.rewrite();
                    sequence->write  ( fmem );
                    fmem.takeFilePool ( memPool,poolSize );
                    fmem.shut();
                    memIO.reset();
                    memIO.write_buffer ( memPool,poolSize );
                    memIO.write ( fseq );
                    if (memPool)  delete[] memPool;
#else
                    structure->write ( fstruct );
                    sequence ->write ( fseq );
#endif
                    
                    sprintf ( msg," packed in pack %03i/%05i\n",
                                  packNo,packSerNo++ );
                    write_log ( msg,fout,verbosity );
                    nc++;
                
                  } else
                    write_log ( " too short a chain, skipped\n",fout,
                                verbosity );
                }
              
              }
      
              chainNo++;
            
            }
    
          }
        
          nentry++;
          progressBar.setProgress ( nentry,nNewEntries );
        
        }
      
      }

      fstruct.shut();
      fstruct.reset();
      index.pack_struct_fsize[packNo] = fstruct.FileLength();
      fstruct.shut();

      fseq.shut();
      fseq.reset();
      index.pack_seq_fsize[packNo] = fseq.FileLength();
      fseq.shut();

    }
  

  if (keepRunning)  {
  
    progressBar.setProgress ( nNewEntries,0 );
    
    if (nentry!=nNewEntries)  {
      sprintf ( msg,"\n\n"
        " *** error: mismatched entry numbers %i/%i\n",nentry,nNewEntries );
      write_log ( msg,fout,2 );
      GSMT_rc = GSMT_NumbersMismatch;
    }
    
    index.makePackIndex();
    index.prepared = true;
    mmdb::CreateCopCat ( fpath,archPath,mmdb::io::_dir_sep,index_file_name );
    if (!index.store(fpath))  {
      sprintf ( msg,"\n\n"
        " *** error: cannot open file\n"
        "     %s\n"
        " for writing.\n",fpath );
      write_log ( msg,fout,2 );
      GSMT_rc = GSMT_CantWriteIndex;
    }
    
    getHours  ( time(NULL)-t0,ihour,imin,isec );
    sprintf   ( msg,
      "\n"
      " ... total %i entries found in PDB directory\n"
      " ... total %i suitable entries with %i chains packed\n"
      " ... total time:  %02i:%02i.%02i\n",
      index.nEntries,index.nPackedEntries,index.nPackedSubEntries,
      ihour,imin,isec );
    write_log ( msg,fout,2 );
  
  } else  {
    write_log ( " +++ execution terminated by user\n",fout,2 );
    GSMT_rc = GSMT_Terminated;
  }

  if (archPath) delete[] archPath;
  if (pdbPath)  delete[] pdbPath;
  if (fpath)    delete[] fpath;
  if (fn)       delete[] fn;
  
  mmdb::FreeVectorMemory ( pack_len,0 );
  
  delete structure;
  delete sequence;
  
  fout.shut();

  return GSMT_rc;
  
}


mmdb::io::PFile gsmt::Archive::getStructFile ( mmdb::cpstr archiveDir,
                                               int  packNo )  {
mmdb::io::PFile f;
mmdb::pstr      archPath,fpath;
char            archFName[200];

  fpath    = NULL;
  archPath = NULL;
  copyDirPath ( archiveDir,archPath );

  sprintf ( archFName,struct_pack_name_tempate,packNo );
  mmdb::CreateCopCat ( fpath,archPath,mmdb::io::_dir_sep,archFName );
  f = new mmdb::io::File();
  f->assign ( fpath,false,true,mmdb::io::GZM_NONE );
  if (!f->reset())  {
    delete f;
    f = NULL;
  }
  
  if (fpath)     delete[] fpath;
  if (archPath)  delete[] archPath;
  
  return f;
                                                 
}

gsmt::PStructure gsmt::Archive::readStructure (
                                          mmdb::io::PFile structFile,
                                          long struct_offset )  {
PStructure     s;
#ifdef __use_memory_pool
mmdb::io::File fmem;
gsmt::MemIO    memIO;
mmdb::pstr     memPool;
int            poolSize;
#endif

  structFile->seek ( struct_offset );
  s = new Structure();

#ifdef __use_memory_pool
  memIO.read       ( *structFile );
  memIO.get_buffer ( &memPool,&poolSize );
  fmem.assign      ( poolSize,0,memPool );
  fmem.reset();
  s->read ( fmem );
  fmem.shut();
#else
  s->read ( *structFile );
#endif

  return s;

}


gsmt::GSMT_CODE gsmt::Archive::scanStruct (
                                       mmdb::cpstr fQuery,
                                       mmdb::cpstr selQuery,
                                       mmdb::cpstr archiveDir,
                                       mmdb::cpstr outFile,
                                       mmdb::cpstr csvFile,
                                       mmdb::cpstr jsonFile,
                                       bool        SCOPSelSyntax )  {
PPStructScanThread threads;
Index              index;
PStructure         s1;
mmdb::pstr         archPath,fpath;

  keepRunning = true;
  
  if (!H)  H = new Hits();
     else  H->deleteHits();
  H->dataKey = Hits::DATA_StructureScan;
  
  // Prepare query structure

  s1 = getQueryStructure ( fQuery,selQuery,SCOPSelSyntax );
  if (!s1)
    return GSMT_CantReadQuery;

  // Open archive
  
  archPath = NULL;
  copyDirPath ( archiveDir,archPath );
  
  fpath = NULL;
  mmdb::CreateCopCat ( fpath,archPath,mmdb::io::_dir_sep,
                             index_file_name );
  
  if (!index.restore(fpath))  {
    if (verbosity>=0)
      printf ( " \n\n"
        " *** error: cannot open GESAMT archive at\n"
        "     %s\n"
        "     -- cannot proceed.\n",archPath );
    delete s1;
    return GSMT_CantReadIndex;
  }

  index.markInclusions ( inclChains );
  index.markExclusions ( exclChains );

  if (verbosity>=0)
    printf ( "\n"
           " ... open gesamt archive of %i packs\n"
           " ... total %i entries with %i chains will be scanned\n",
           index.nPacks,index.nPackedEntries,index.nPackedSubEntries );

  // Prepare thread instances
  
  threads = new PStructScanThread[nthreads];
  for (int n=0;n<nthreads;n++)  {
    threads[n] = new StructScanThread();
    threads[n]->s1     =  s1;
    threads[n]->path0  =  NULL;
    threads[n]->index  = &index;
    threads[n]->packNo =  n;
    threads[n]->initAligner ( performance,minMatch1,minMatch2,
                              trimQ,trimSize,QR0,sigma );
    mmdb::CreateCopy ( threads[n]->path0,archPath );
  }

  run_threads ( PPThreadBase(threads),nthreads,&lock,&keepRunning,
                &progressBar,verbosity );

  H->mergeHits  ( threads[0]->H );
  for (int n=1;n<nthreads;n++)
    H->mergeHits  ( threads[n]->H  );

  if (keepRunning)  {  // Sort hots and write them out
    H->sortHits();
    if (outFile)  {
      H->writeHits ( outFile );
      if (verbosity>=0)
        printf ( " ... summary of %i hits written in file %s\n",
                 H->nHits,outFile );
    } else if (verbosity>=0)  {  // correct!
      H->writeHits ( "hits.txt" );
      printf ( " ... summary of %i hits written in file hits.txt\n",
               H->nHits );
    }
    if (csvFile)  {
      H->writeHits_csv ( csvFile );
      if (verbosity>=0)
        printf ( " ... details of %i hits written in file %s\n",
                 H->nHits,csvFile );
    }
    if (jsonFile)  {
      H->writeHits_json ( s1,jsonFile );
      if (verbosity>=0)
        printf ( " ... details of %i hits written in file %s\n",
                 H->nHits,jsonFile );
    }
  }

  for (int n=0;n<nthreads;n++)
    delete threads[n];
  delete[] threads;

  if (archPath) delete[] archPath;
  if (fpath)    delete[] fpath;
  
  delete s1;  

  if (!keepRunning)  {
    if (verbosity>=0)
      printf ( " +++ execution terminated by user\n" );
    return GSMT_Terminated;
  } else
    return GSMT_Ok;

}


gsmt::GSMT_CODE gsmt::Archive::scanSeq ( mmdb::cpstr fQuery,
                                         mmdb::cpstr archiveDir,
                                         mmdb::cpstr outFile,
                                         mmdb::cpstr csvFile
                                       )  {
PPSeqScanThread threads;
Index           index;
PSequence       s1;
mmdb::pstr      archPath,fpath;

  keepRunning = true;

  if (!H)  H = new Hits();
     else  H->deleteHits();
  H->dataKey = Hits::DATA_SequenceScan;

  // Prepare query structure

  s1 = getQuerySequence ( fQuery );
  if (!s1)
    return GSMT_CantReadQuery;

  // Open archive
  
  archPath = NULL;
  copyDirPath ( archiveDir,archPath );
  
  fpath = NULL;
  mmdb::CreateCopCat ( fpath,archPath,mmdb::io::_dir_sep,
                             index_file_name );

  if (!index.restore(fpath))  {
    if (verbosity>=0)
      printf ( " \n\n"
        " *** error: cannot open GESAMT archive at\n"
        "     %s\n"
        "     -- cannot proceed.\n",archPath );
    delete s1;
    return GSMT_CantReadIndex;
  }

  index.markInclusions ( inclChains );
  index.markExclusions ( exclChains );

  if (verbosity>=0)
    printf ( "\n"
           " ... open gesamt archive of %i packs\n"
           " ... total %i entries with %i chains will be scanned\n",
           index.nPacks,index.nPackedEntries,index.nPackedSubEntries );

  // Prepare thread instances

  threads = new PSeqScanThread[nthreads];
  for (int n=0;n<nthreads;n++)  {
    threads[n] = new SeqScanThread();
    threads[n]->s1     =  s1;
    threads[n]->path0  =  NULL;
    threads[n]->index  = &index;
    threads[n]->packNo =  n;
    threads[n]->initAligner ( alignMethod,gapPenalty,spacePenalty,
                              scoreEqual,scoreNotEqual,
                              minMatch1,minMatch2,trimSize );
    mmdb::CreateCopy ( threads[n]->path0,archPath );
  }

  run_threads ( PPThreadBase(threads),nthreads,&lock,&keepRunning,
                &progressBar,verbosity );

  H->mergeHits  ( threads[0]->H );
  for (int n=1;n<nthreads;n++)
    H->mergeHits  ( threads[n]->H  );

  if (keepRunning)  {  // Sort hots and write them out
    H->sortHits();
    if (outFile)  {
      H->writeHits ( outFile );
      if (verbosity>=0)
        printf ( " ... summary of %i hits written in file %s\n",
                 H->nHits,outFile );
    } else if (verbosity>=0)  {  // correct!
      H->writeHits ( "hits.txt" );
      printf ( " ... summary of %i hits written in file hits.txt\n",
               H->nHits );
    }
    if (csvFile)  {
      H->writeHits_csv ( csvFile );
      if (verbosity>=0)
        printf ( " ... details of %i hits written in file %s\n",
                 H->nHits,csvFile );
    }
  }

  for (int n=0;n<nthreads;n++)
    delete threads[n];
  delete[] threads;

  if (archPath) delete[] archPath;
  if (fpath)    delete[] fpath;
  
  delete s1;  

  if (!keepRunning)  {
    if (verbosity>=0)
      printf ( " +++ execution terminated by user\n" );
    return GSMT_Terminated;
  } else
    return GSMT_Ok;

}


gsmt::GSMT_CODE gsmt::Archive::scanSubSeq ( mmdb::cpstr    fQuery,
                                            mmdb::cpstr    archiveDir,
                                            int            subSeqLen,
                                            mmdb::realtype minSeqMatch,
                                            mmdb::cpstr    outFile )  {
PPSubSeqScanThread threads;
Index              index;
PSequence          s1;
mmdb::pstr         archPath,fpath;

  keepRunning = true;
  freeMemory();

  // Prepare query structure

  s1 = getQuerySequence ( fQuery );
  if (!s1)
    return GSMT_CantReadQuery;

  // Open archive
  
  archPath = NULL;
  copyDirPath ( archiveDir,archPath );

  fpath = NULL;
  mmdb::CreateCopCat ( fpath,archPath,mmdb::io::_dir_sep,
                             index_file_name );

  if (!index.restore(fpath))  {
    if (verbosity>=0)
      printf ( " \n\n"
        " *** error: cannot open GESAMT archive at\n"
        "     %s\n"
        "     -- cannot proceed.\n",archPath );
    delete s1;
    return GSMT_CantReadIndex;
  }

  index.markInclusions ( inclChains );
  index.markExclusions ( exclChains );

  if (verbosity>=0)
    printf ( "\n"
           " ... open gesamt archive of %i packs\n"
           " ... total %i entries with %i chains will be scanned\n",
           index.nPacks,index.nPackedEntries,index.nPackedSubEntries );

  // Prepare thread instances
  
  threads = new PSubSeqScanThread[nthreads];
  for (int n=0;n<nthreads;n++)  {
    threads[n] = new SubSeqScanThread();
    threads[n]->initScaner ( s1,subSeqLen,minSeqMatch );
    threads[n]->index  = &index;
    threads[n]->packNo =  n;
    mmdb::CreateCopy ( threads[n]->path0,archPath );
  }

  run_threads ( PPThreadBase(threads),nthreads,&lock,&keepRunning,
                &progressBar,verbosity );

  seqSheafs = threads[0]->takeSeqSheafs();
  for (int n=1;n<nthreads;n++)
    seqSheafs->merge  ( threads[n]->getSeqSheafs() );

  if (keepRunning)  {  // Sort hots and write them out
    seqSheafs->truncateSheafs();
    if (outFile)  {
      seqSheafs->writeSummary ( outFile );
      if (verbosity>=0)
        printf ( " ... %i sheafs written in file %s\n",
                 seqSheafs->nSheafs,outFile );
    } else if (verbosity>=0)  {  // correct!
      seqSheafs->writeSummary ( "seqsheafs.txt" );
      printf ( " ... %i sheafs written in file seqsheafs.txt\n",
               seqSheafs->nSheafs );
    }
  }

  for (int n=0;n<nthreads;n++)
    delete threads[n];
  delete[] threads;

  if (archPath) delete[] archPath;
  if (fpath)    delete[] fpath;
  
  delete s1;  

  if (!keepRunning)  {
    if (verbosity>=0)
      printf ( " +++ execution terminated by user\n" );
    return GSMT_Terminated;
  } else
    return GSMT_Ok;

}


gsmt::GSMT_CODE gsmt::Archive::scanPDB ( mmdb::cpstr fQuery,
                                         mmdb::cpstr selQuery,
                                         mmdb::cpstr pdbDir,
                                         mmdb::cpstr outFile,
                                         mmdb::cpstr csvFile,
                                         bool  SCOPSelSyntax )  {
PPPDBScanThread threads;
Index           index;
PStructure      s1;
mmdb::pstr      pdbPath;
  
  keepRunning = true;

  if (!H)  H = new Hits();
     else  H->deleteHits();
  H->dataKey = Hits::DATA_StructureScan;

  // Prepare query structure
  
  s1 = getQueryStructure ( fQuery,selQuery,SCOPSelSyntax );
  if (!s1)
    return GSMT_CantReadQuery;

  // Read target directory

  pdbPath = NULL;
  copyDirPath ( pdbDir,pdbPath );
  
  if (verbosity>=0)
    printf ( "\n ... will scan PDB directory %s\n",pdbDir );

  index.readDir ( pdbPath );

  if (index.nEntries<=0)  {
    if (verbosity>=0)
      printf (
        " *** error: no structure files found in PDB directory "
        "-- nothing to do.\n" );
    delete s1;
    return GSMT_NoPDBStructures;
  }
  
  printf ( " ... total %i entries to scan\n",index.nEntries );

  // Prepare thread instances
  
  threads = new PPDBScanThread[nthreads];
  for (int n=0;n<nthreads;n++)  {
    threads[n] = new PDBScanThread();
    threads[n]->s1    =  s1;
    threads[n]->path0 =  NULL;
    threads[n]->index = &index;
    threads[n]->initAligner ( performance,minMatch1,minMatch2,
                              trimQ,trimSize,QR0,sigma );
    mmdb::CreateCopy ( threads[n]->path0,pdbPath );
  }

  run_threads ( PPThreadBase(threads),nthreads,&lock,&keepRunning,
                &progressBar,verbosity );

  H->mergeHits  ( threads[0]->H );
  for (int n=1;n<nthreads;n++)
    H->mergeHits  ( threads[n]->H  );

  if (keepRunning)  {  // Sort hots and write them out
    H->sortHits();
    if (outFile)  {
      H->writeHits ( outFile );
      if (verbosity>=0)
        printf ( " ... summary of %i hits written in file %s\n",
                 H->nHits,outFile );
    } else if (verbosity>=0)  {  // correct!
      H->writeHits ( "hits.txt" );
      printf ( " ... summary of %i hits written in file hits.txt\n",
               H->nHits );
    }
    if (csvFile)  {
      H->writeHits_csv ( csvFile );
      if (verbosity>=0)
        printf ( " ... details of %i hits written in file %s\n",
                 H->nHits,csvFile );
    }
  }

  for (int n=0;n<nthreads;n++)
    delete threads[n];
  delete[] threads;

  if (pdbPath) delete[] pdbPath;
  
  delete s1;

  if (!keepRunning)  {
    if (verbosity>=0)
      printf ( " +++ execution terminated by user\n" );
    return GSMT_Terminated;
  } else
    return GSMT_Ok;

}

gsmt::PHits gsmt::Archive::takeHits()  {
PHits hits = H;
  H = NULL;
  return hits;
}

