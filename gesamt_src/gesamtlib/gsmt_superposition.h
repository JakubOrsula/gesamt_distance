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
//    03.02.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  GSMT_Superposition <interface>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::Superposition
//       ~~~~~~~~~
//
//  (C) E. Krissinel, 2008-2014
//
// =================================================================
//

#ifndef __GSMT_Superposition__
#define __GSMT_Superposition__

#include "mmdb2/mmdb_utils.h"

// =================================================================

namespace gsmt  {

  DefineClass(Superposition);

  class Superposition  {

    public :
      mmdb::mat44        T;  //!< transformation matrix
      mmdb::realtype  rmsd;  //!< best superposition r.m.s.d.
      mmdb::realtype     Q;  //!< Q-score
      mmdb::realtype seqId;  //!< sequence identity
      int          natoms1;  //!< length of 1st structure
      int          natoms2;  //!< length of 2nd structure
      int            Nalgn;  //!< alignment length
      mmdb::ivector     c1;  //!< correspondence vector for structure #1
      mmdb::ivector     c2;  //!< correspondence vector for structure #2
      mmdb::rvector  dist1;  //!< distances  i <-> c1[i]

      Superposition ();
      Superposition ( PSuperposition SD );
      ~Superposition();

      void FreeMemory();
      void Init ();

      void SetSuperposition ( mmdb::mat44  & sup_T,
                              mmdb::realtype sup_rmsd,
                              mmdb::realtype sup_seqId,
                              mmdb::realtype sup_Q,
                              int            sup_Nalgn,
                              mmdb::ivector  sup_c1,
                              mmdb::rvector  sup_dist1,
                              int            sup_nat1,
                              mmdb::ivector  sup_c2,
                              int            sup_nat2 );
      
      void CopyFrom ( PSuperposition SDfrom );

  };

}


#endif

