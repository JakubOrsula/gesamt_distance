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
//  **** Module  :  GSMT_Base <interface>
//       ~~~~~~~~~
//  **** Project :  GESAMT - multiple structure aligner
//       ~~~~~~~~~
//  **** Classes :  gsmt::Base
//       ~~~~~~~~~
//
//  (C) E. Krissinel, 2008-2017
//
// =================================================================
//

#ifndef __GSMT_Base__
#define __GSMT_Base__

#include "mmdb2/mmdb_math_align.h"

#include "gsmt_structure.h"
#include "gsmt_sequence.h"
#include "gsmt_defs.h"

// =================================================================


namespace gsmt {

  class Base : public mmdb::io::Stream  {

    public :

      Base ();
      Base ( mmdb::io::RPStream Object );
      ~Base();

      inline void setPerformanceLevel ( PERFORMANCE_CODE perf )
                                            {  performance = perf; }

      inline void setVerbosity ( int v )    {  verbosity = v;  }
      
      void setSimilarityThresholds ( mmdb::realtype minPart1,
                                     mmdb::realtype minPart2 );

      void setTrimFactors          ( mmdb::realtype trim_Q,
                                     mmdb::realtype trim_size );
      
      // sets target size filter in units of size of the query
      void setTargetSizeFilter     ( mmdb::realtype minSize,
                                     mmdb::realtype maxSize );

      inline void setQR0      ( mmdb::realtype R0 )  { QR0      = R0; }
      inline void setSigma    ( mmdb::realtype s  )  { sigma    = s;  }
      inline void setNThreads ( int            n  )  { nthreads = n;  }

      inline mmdb::realtype getQR0      ()  { return QR0;       }
      inline mmdb::realtype getSigma    ()  { return sigma;     }
      inline mmdb::realtype getMinMatch1()  { return minMatch1; }
      inline mmdb::realtype getMinMatch2()  { return minMatch2; }
      
      void read  ( mmdb::io::RFile f );
      void write ( mmdb::io::RFile f );

    protected :

      // Structure match parameters

      PERFORMANCE_CODE performance; //!< high/normal modes
      mmdb::realtype   maxContact;  //!< maximal contact distance
      mmdb::realtype   QR0;         //!< parameter of Q-score
      mmdb::realtype   sigma;       //!< superposition sigma

      // Sequence match parameters

      mmdb::math::ALIGN_METHOD alignMethod; //!< sequence alignment mode
      mmdb::realtype   gapPenalty;    //!< gap penalty
      mmdb::realtype   spacePenalty;  //!< space penalty 
      mmdb::realtype   scoreEqual;    //!< score for residue match
      mmdb::realtype   scoreNotEqual; //!< score for residue mismatch

      // Common match parameters

      mmdb::realtype   minMatch1;   //!< minimal fraction of matched
                                    /// residues in 1st structure [0..1]
      mmdb::realtype   minMatch2;   //!< minimal fraction of matched
                                    /// residues in 2nd structure [0..1]
      mmdb::realtype   trimSize;    //!< size-based trim factor for
                                    /// scan results [0..1]
      mmdb::realtype   trimQ;       //!< Q-based trim factor for
                                    /// scan results [0..1]

      int              nthreads;    //!< number of threads to use
      int              verbosity;

      void initBase();

      PStructure getQueryStructure ( mmdb::cpstr fQuery,
                                     mmdb::cpstr selQuery,
                                     bool SCOPSelSyntax );

      PSequence  getQuerySequence  ( mmdb::cpstr fQuery );

  };

}

#endif

