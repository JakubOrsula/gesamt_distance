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
//    28.01.16   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  GSMT_Superpose <interface>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Functions :  gsmt::superpose
//       ~~~~~~~~~~~
//
//  (C) E. Krissinel, 2008-2016
//
// =================================================================
//

#ifndef __GSMT_Superpose__
#define __GSMT_Superpose__

#include "mmdb2/mmdb_utils.h"

// =================================================================

namespace gsmt  {

  enum SUP_RC {
    SUP_Ok,
    SUP_Empty,
    SUP_Fail
  };

  SUP_RC superpose ( mmdb::PPAtom  a1,
                     mmdb::PPAtom  a2,
                     mmdb::ovector mask,
                     int           natoms,
                     mmdb::mat44 & T );

  SUP_RC superpose ( mmdb::rvector x1,
                     mmdb::rvector y1,
                     mmdb::rvector z1,
                     mmdb::PPAtom  a2,
                     mmdb::ovector mask,
                     int           natoms,
                     mmdb::mat44 & T );

  SUP_RC calcRTMat ( mmdb::mat33 & svd_A,
                     mmdb::vect3 & cm1,
                     mmdb::vect3 & cm2,
                     mmdb::mat44 & T );

}


#endif

