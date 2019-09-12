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
//  **** Module  :  GSMT_Aligner <interface>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::Aligner
//       ~~~~~~~~~
//
//  (C) E. Krissinel, 2008-2016
//
// =================================================================
//

#ifndef __GSMT_Aligner__
#define __GSMT_Aligner__

#include "gsmt_refiner.h"
#include "gsmt_defs.h"
#include "json_.h"

// =================================================================


namespace gsmt {

  DefineClass(Aligner);

  class Aligner : public Refiner  {

    public :

      Aligner ();
      Aligner ( mmdb::io::RPStream Object );
      ~Aligner();

      void  setPerformanceLevel     ( PERFORMANCE_CODE performance );
      void  setSimilarityThresholds ( mmdb::realtype minPart1,
                                      mmdb::realtype minPart2 );

      // s1 is the reference structure, s2 will be superposed over s1
      // (the resulting matrix to be applied to s2)
      GSMT_CODE Align ( PStructure s1, PStructure s2, bool measure_cpu, double *cpu_time = nullptr);

      inline int getNAtoms1()  { return natoms1; }
      inline int getNAtoms2()  { return natoms2; }

      PSuperposition getMatch  ( int matchNo );
      void       getBestMatch  ( RPSuperposition SD, int & matchNo );
      void   getSeedAlignment  ( int matchNo, mmdb::ivector & ac1,
                                              mmdb::ivector & ac2 );

      void writeAlignTable      ( mmdb::io::RFile f, PSuperposition SD );
      void writeAlignTable_csv  ( mmdb::io::RFile f, PSuperposition SD );
      void makeAlignTable_rvapi ( mmdb::cpstr tableId, PSuperposition SD );
      void makeAlignGraph_rvapi ( mmdb::cpstr graphId, PSuperposition SD );

      void addResidueAlignmentJSON ( gsmt::RJSON  json,
                                     PSuperposition SD );

      void writeSeqAlignment   ( mmdb::io::RFile f,
                                 mmdb::cpstr     title1,
                                 mmdb::cpstr     title2,
                                 PSuperposition  SD );
      void getSeqAlignment     ( mmdb::pstr    & qSeq,
                                 mmdb::pstr    & tSeq,
                                 mmdb::rvector * dist,
                                 PSuperposition  SD );
      
      void read  ( mmdb::io::RFile f );
      void write ( mmdb::io::RFile f );

    protected :

      //  Initialization and memory management
      void InitAligner();
      void FreeMemory ();

  };

  
}

#endif

