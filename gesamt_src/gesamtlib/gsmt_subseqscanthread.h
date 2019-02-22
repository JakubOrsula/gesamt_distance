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
//  **** Module  :  GSMT_SubSeqScanThread <interface>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::SubSeqScanThread
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2008-2016
//
// =================================================================
//

#ifndef __GSMT_SubSeqScanThread__
#define __GSMT_SubSeqScanThread__

#include "gsmt_threadbase.h"
#include "gsmt_sequence.h"
#include "gsmt_seqsheafs.h"

// =================================================================

namespace gsmt  {

  DefineClass(SubSeqScanThread);

  class SubSeqScanThread : public ThreadBase  {

    public:
      mmdb::pstr  path0;
      PIndex      index;
      int         packNo;

      SubSeqScanThread ();
      virtual ~SubSeqScanThread();

      void initScaner ( PSequence      seq,
                        int            subSeqLen,
                        mmdb::realtype minSeqMatch );

      virtual void run();

      inline PSeqSheafs getSeqSheafs()  { return seqSheafs; }
      PSeqSheafs takeSeqSheafs();

    protected:
      PSequence      s1;
      PSeqSheafs     seqSheafs;
      mmdb::realtype minSeqSim;
      int            seqLen;
      int            sheafLen;
      void freeMemory();  

  };

}

#endif
