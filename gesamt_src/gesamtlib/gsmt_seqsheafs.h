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
//  **** Module  :  GSMT_SeqSheafs <interface>
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

#ifndef __GSMT_SeqSheafs__
#define __GSMT_SeqSheafs__

#include "gsmt_index.h"
#include "gsmt_structure.h"
#include "gsmt_sheafdata.h"
#include "shiftsort_.h"

// =================================================================

namespace gsmt  {

  DefineClass(SeqSheafItem);

  class SeqSheafItem  {

    public:
      mmdb::realtype score;
      mmdb::IDCode   id;
      int            startPos;
      short          packNo;
      SubEntry       subEntry;

      SeqSheafItem ();
      ~SeqSheafItem() {}

      static void writeSummaryTitle ( mmdb::io::RFile f );
      void writeSummary ( mmdb::io::RFile f );
      
      PStructure getStructure ( mmdb::io::PFile structFile,
                                int sheafLen );
  
  };

  DefineClass(SeqSheaf);

  class SeqSheaf : public ShiftSort  {

    public:
    
      enum SORT_KEY  {
        SORT_ITEMS_Score,
        SORT_ITEMS_File,
        SORT_SHEAFS_SeqId,
        SORT_SHEAFS_Size
      };
    
      PPSeqSheafItem items;         //!< all subsequence matches
      PPStructure    S;             //!< all structures of subsequences
      int            nItems;        //!< number of sequence matcjes
      int            sheafLen;      //!< length of subsequences
      PPSheafData    structSheafs;  //!< structure sheafs
      int            nStructSheafs; //!< number of structure sheafs 

      SeqSheaf ();
      ~SeqSheaf();
      
      void  addItem    ( PSeqSheafItem item );
      void  merge      ( PSeqSheaf   source );
      void  sort       ( SORT_KEY   sortKey );
      void  truncate   ( int     maxItems=0 );
      void  writeSummary ( mmdb::io::RFile f );

      void  calcSeqScores();  // works when sheafs are calculated

      PPStructure getStructureSheaf ( mmdb::cpstr archiveDir );

    protected:
      int  nMaxItems;  //!< maximum number of subsequence matches
      
      void freeSeqData   ();
      void freeStructData();
      void freeSheafData ();
      void freeMemory    ();
      virtual int Compare ( void * di, void * dj );
      
    private:
      SORT_KEY sKey;
  
  };


  DefineClass(SeqSheafs);

  class SeqSheafs  {

    public:
      PPSeqSheaf sheafs;
      int        nSheafs;
      int        sheafLen;

      SeqSheafs ();
      SeqSheafs ( int n_sheafs, int sheaf_len );
      ~SeqSheafs();

      void init  ( int n_sheafs, int sheaf_len );
      void merge ( PSeqSheafs source ); // removes all items from source

      void truncateSheafs ( int maxItems=0 );
      void sortSheafs     ( SeqSheaf::SORT_KEY sortKey );
      void writeSummary   ( mmdb::cpstr fpath );

    protected:
      void freeMemory();
      
  };

}

#endif
