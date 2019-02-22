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
//    21.05.16   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  GSMT_MSuperposition <interface>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::MSuperposition
//       ~~~~~~~~~
//
//  (C) E. Krissinel, 2008-2016
//
// =================================================================
//

#ifndef __GSMT_MSuperposition__
#define __GSMT_MSuperposition__

#include "gsmt_superposition.h"

// =================================================================

namespace gsmt  {

  DefineClass(MSuperposition);

  class MSuperposition  {

    public :
      int          nStruct;  //!< number of structures
      mmdb::mat44       *T;  //!< transformation matrices
      mmdb::realtype  rmsd;  //!< best superposition r.m.s.d.
      mmdb::realtype     Q;  //!< Q-score
      mmdb::realtype seqId;  //!< sequence identity
      mmdb::ivector  sSize;  //!< structure sizes
      mmdb::ivector  dSize;  //!< domain sizes
      int            Nalgn;  //!< alignment length
      mmdb::imatrix      c;  //!< atom correspondence vectors
      mmdb::rvector    var;  //!< distance variances  i <-> c[0][i]

      MSuperposition ();
      MSuperposition ( PSuperposition SD );
      ~MSuperposition();

      void FreeMemory();
      void Init ();

      void CopyFrom ( PSuperposition SD );
      void CopyTo   ( PSuperposition SD );

  };

}


#endif

