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
//    05.06.16   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  GSMT_Index <i5nterface>
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

#ifndef __GSMT_Index__
#define __GSMT_Index__

#include "mmdb2/mmdb_utils.h"

// =================================================================

namespace gsmt  {

  DefineStructure(SubEntry);

  struct SubEntry  {

    mmdb::ChainID  id;
    long           offset_struct;
    long           offset_seq;
    short          size;
    bool           selected; // not stored used locally, true by default

    void  init  ();
    void  copy  ( PSubEntry source );

    void  write ( mmdb::io::RFile f );
    void  read  ( mmdb::io::RFile f );

  };

  DefineClass(Entry);

  class Entry  {

    public:
      mmdb::IDCode id;
      mmdb::pstr   fname;
      short        packNo;
      short        nSubEntries;
      PPSubEntry   subEntries;

      Entry ();
      ~Entry();

      void clear();

      void addSubEntry ( PSubEntry subEntry );

      void  write ( mmdb::io::RFile f );
      void  read  ( mmdb::io::RFile f );

  };

  DefineClass(Index);

  class Index  {

    public:

      enum SORT_STATE {
        UNSORTED,
        SORTED_BY_Ids,
        SORTED_BY_FNames,
        SORTED_BY_FPaths,
        SORTED_BY_PackNo
      };

      PPEntry       entries;
      int           nEntries;
      mmdb::ivector pack_index;
      mmdb::ivector pack_size;
      mmdb::lvector pack_struct_fsize;
      mmdb::lvector pack_seq_fsize;
      int           nPacks;
      int           nPackedEntries;
      int           nPackedSubEntries;
      bool          compressed;
      bool          prepared;
      SORT_STATE    sortState;

      Index ();
      ~Index();

      void clear();

      void addEntry     ( PEntry      entry   );
      void readDir      ( mmdb::cpstr dirPath );

      void initPacks    ( int n_packs );
      void makePackIndex();

      void sortByIds    ();
      void sortByFNames ();
      void sortByFPaths ();
      void sortByPackNo ();

      bool  store       ( mmdb::cpstr fname );
      bool  restore     ( mmdb::cpstr fname );
      bool  readSummary ( mmdb::cpstr fname );

      int   mergeIndex  ( RIndex index0 );
      
      gsmt::PEntry getEntry ( const mmdb::IDCode pdbCode );

      //  Lists must contain records in form '1XYZ:A' or '2XYZ:*' and
      //  be terminated with and empty record 
      void  markInclusions ( mmdb::IDCode * list );
      void  markExclusions ( mmdb::IDCode * list );

      void  write       ( mmdb::io::RFile f );
      void  read        ( mmdb::io::RFile f );

    private:
      mmdb::pstr path0;
      int        nEntriesAlloc;

      void read_dir    ( mmdb::cpstr path );
      void markEntries ( mmdb::IDCode * list, bool mark );

  };

}

#endif

