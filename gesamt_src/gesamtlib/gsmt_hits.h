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
//    07.02.17   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  GSMT_Hits <i5nterface>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::Hit
//       ~~~~~~~~~  gsmt::Hits
//
//  (C) E. Krissinel 2008-2017
//
// =================================================================
//

#ifndef __GSMT_Hits__
#define __GSMT_Hits__

#include "gsmt_superposition.h"
#include "gsmt_aligner.h"

// =================================================================

namespace gsmt  {


  DefineClass(Hit);

  class Hit  {

    public:
      mmdb::IDCode  pdbCode;
      mmdb::pstr   fileName;
      mmdb::ChainID chainID;
      mmdb::realtype   rmsd;  //!< best superposition r.m.s.d.
      mmdb::realtype      Q;  //!< Q-score
      mmdb::realtype  seqId;  //!< sequence identity
      mmdb::pstr   pdbTitle;  //!< PDB title
      mmdb::pstr       qSeq;  //!< aligned query sequence
      mmdb::pstr       tSeq;  //!< aligned target sequence
      mmdb::rvector    dist;  //!< C-alpha distances at superposition
      mmdb::mat44        RT;  //!< rotation-translation matrix
      int             Nalgn;  //!< alignment length
      int              nRes;
      bool              sel;  //!< selection attribute

      Hit ();
      ~Hit();

      static int getWriteSize();
      void mem_write ( mmdb::pstr  S, int & l );
      void mem_read  ( mmdb::cpstr S, int & l );
      static void csv_title ( mmdb::pstr S, mmdb::cpstr sep );
      void write_csv ( mmdb::pstr S, mmdb::cpstr sep );

  };

  
  DefineClass(Hits);

  class Hits : public mmdb::QuickSort  {

    public:
    
      enum DATA_KEY {
        DATA_StructureScan,
        DATA_SequenceScan
      };
    
      PPHit    hits;
      int      nHits;
      DATA_KEY dataKey;
      
      Hits ();
      ~Hits();

      void deleteHits      ();
      void checkHitsBuffer ();
      
      //  structure hits
      void setHit          ( mmdb::IDCode         pdbId,
                             mmdb::cpstr          fname,
                             mmdb::ChainID        chainId,
                             mmdb::cpstr          title,
                             gsmt::PSuperposition SD,
                             gsmt::PAligner       Aligner,
                             int                  nAtoms );
                             
      // sequence hits
      void setHit          ( mmdb::cpstr          pdbId,
                             mmdb::cpstr          fname,
                             mmdb::ChainID        chainId,
                             mmdb::cpstr          title,
                             mmdb::cpstr          qAlignSeq,
                             mmdb::cpstr          tAlignSeq,
                             mmdb::realtype       seqId,
                             mmdb::realtype       similarity,
                             mmdb::realtype       score,
                             int                  Nalign,
                             int                  nRes );

      void setHit          ( RHit  h );
      void mergeHits       ( RHits H );      
      void sortHits        ();
      void writeHits       ( mmdb::cpstr fName );
      void writeHits_csv   ( mmdb::cpstr fName );
      void writeHits_json  ( PStructure s, mmdb::cpstr fName );
      
      // Removes pointer as is; releasing memory is responsibility
      // of calling process. Both array and its elements should be
      // deallocated.
      void takeHits ( PPHit & hit_array, int & n_hits );

    private:
      int nHitsAlloc;

      virtual int  Compare ( int i, int j );
      virtual void Swap    ( int i, int j );

      void writeStructureHits      ( mmdb::cpstr fName );
      void writeSequenceHits       ( mmdb::cpstr fName );

      void writeStructureHits_csv  ( mmdb::cpstr fName );
      void writeSequenceHits_csv   ( mmdb::cpstr fName );

      void writeStructureHits_json ( PStructure s, mmdb::cpstr fName );
      void writeSequenceHits_json  ( PStructure s, mmdb::cpstr fName );
      
  };

}

#endif

