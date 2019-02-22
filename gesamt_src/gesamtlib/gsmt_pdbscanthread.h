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
//  **** Module  :  GSMT_PDBScanThread <interface>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::PDBScanThread
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2008-2016
//
// =================================================================
//

#ifndef __GSMT_PDBScanThread__
#define __GSMT_PDBScanThread__

#include "gsmt_threadbase.h"
#include "gsmt_aligner.h"
#include "gsmt_hits.h"
#include "gsmt_index.h"

// =================================================================

namespace gsmt  {

  DefineClass(PDBScanThread);

  class PDBScanThread : public ThreadBase  {

    public:
      gsmt::PStructure s1;
      gsmt::Hits       H;
      mmdb::pstr       path0;
      gsmt::PIndex     index;
      mmdb::realtype   trimQ;
      mmdb::realtype   trimSize;

      PDBScanThread ();
      virtual ~PDBScanThread();

      void initAligner ( gsmt::PERFORMANCE_CODE performance,
                         mmdb::realtype         minMatch1,
                         mmdb::realtype         minMatch2,
                         mmdb::realtype         trim_Q,
                         mmdb::realtype         trim_size,
                         mmdb::realtype         QR0,
                         mmdb::realtype         sigma );

      virtual void run();
      
    protected:
      gsmt::PAligner A;

      void freeMemory();  

  };

}

#endif
