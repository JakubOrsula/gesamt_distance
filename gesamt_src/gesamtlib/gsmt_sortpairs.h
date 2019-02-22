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
//  **** Module  :  GSMT_SortPairs <interface>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::SortPairs
//       ~~~~~~~~~
//
//  (C) E. Krissinel, 2008-2014
//
// =================================================================
//

#ifndef __GSMT_SortPairs__
#define __GSMT_SortPairs__

#include "mmdb2/mmdb_utils.h"
//#include "mmdb2/mmdb_mattype.h"

// =================================================================

namespace gsmt  {

  DefineClass(SortPairs);
  
  /*
  class SortPairs : public mmdb::QuickSort  {
    public :
      SortPairs () {}
      SortPairs ( mmdb::io::RPStream Object );
      ~SortPairs() {}
      int  Compare ( int i, int j );
      void Swap    ( int i, int j );
      void Sort    ( mmdb::ivector ix, mmdb::rvector dist2, int nPairs );
    protected :
      mmdb::ivector kix;
  };
  */
  
  class SortPairs  {
    public :
      SortPairs ();
      ~SortPairs();
      void Sort ( mmdb::ivector ix, mmdb::rvector dist2, int nPairs );
    protected :
      int nDataAlloc;
      mmdb::rvector data;
      mmdb::ivector kix;
  };

}

#endif
