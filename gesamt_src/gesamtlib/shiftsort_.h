//  $Id: shiftsort_.h $
// =================================================================
//  This code is distributed under the terms and conditions of the
//  CCP4 Program Suite Licence Agreement as 'Part 2' (Annex 2)
//  software. A copy of the CCP4 licence can be obtained by writing
//  to CCP4, Research Complex at Harwell, Rutherford Appleton
//  Laboratory, Didcot OX11 0FA, UK, or from
//  http://www.ccp4.ac.uk/ccp4license.php.
//  =================================================================
//
//    15.12.15   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  shiftsort_  <interface>
//       ~~~~~~~~~
//  **** Classes :  gsmt::ShiftSort  - abstract sorting
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2015
//
//  =================================================================
//

#ifndef __GSMT_SHIFTSORT_H
#define __GSMT_SHIFTSORT_H

#include "mmdb2/mmdb_mattype.h"

namespace gsmt  {

  DefineClass(ShiftSort);

  class ShiftSort  {

    public:

      ShiftSort();
      virtual ~ShiftSort();
      
      virtual int Compare ( void * di, void * dj );

      void Sort ( void **d, int dataLen );
      
    protected:
      int    nDataAlloc;
      void **data;

  };

}  // namespace gsmt

#endif // __GSMT_SHIFTSORT_H
