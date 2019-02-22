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
//  **** Module  :  GSMT_SeqScanThread <interface>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::SeqScanThread
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2008-2016
//
// =================================================================
//

#ifndef __GSMT_SeqScanThread__
#define __GSMT_SeqScanThread__

#include "gsmt_threadbase.h"
#include "gsmt_sequence.h"
#include "mmdb2/mmdb_math_align.h"
#include "gsmt_hits.h"
#include "gsmt_index.h"

// =================================================================

namespace gsmt  {

  DefineClass(SeqScanThread);

  class SeqScanThread : public ThreadBase  {

    public:
      PSequence      s1;
      Hits           H;
      mmdb::pstr     path0;
      PIndex         index;
      int            packNo;
      mmdb::realtype trimSize;

      SeqScanThread ();
      virtual ~SeqScanThread();

      void initAligner ( mmdb::math::ALIGN_METHOD method,
                         mmdb::realtype       gapPenalty,
                         mmdb::realtype     spacePenalty,
                         mmdb::realtype       scoreEqual,
                         mmdb::realtype    scoreNotEqual,
                         mmdb::realtype       min_match1,
                         mmdb::realtype       min_match2,
                         mmdb::realtype        trim_size );

      virtual void run();
      
    protected:
      mmdb::math::PAlignment   A;
      mmdb::math::ALIGN_METHOD alignMethod;
      mmdb::realtype           minMatch1,minMatch2;

      void freeMemory();  

  };

}

#endif
