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
//    24.07.15   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  GSMT_MAMap <interface>
//       ~~~~~~~~~
//  **** Project :  GESAMT - multiple structure aligner
//       ~~~~~~~~~
//  **** Classes :  gsmt::MAMap
//       ~~~~~~~~~
//
//  (C) E. Krissinel, 2008-2015
//
// =================================================================
//

#ifndef __GSMT_MAMap__
#define __GSMT_MAMap__

#include "mmdb2/mmdb_utils.h"

// =================================================================

namespace gsmt {

  enum UNMAP_KEY  {
    UNMAP_YES = -2,
    UNMAP_NO  = -1
  };

  DefineStructure(MAMap)

  struct MAMap  {

    mmdb::realtype rmsd;
    mmdb::ivector  map;      // 0:i is mapped onto j:SMAMap[i].map[j]
    mmdb::realtype xc,yc,zc; // consensus coordinates

    void Init   ( int nStruct );
    void Dispose();

  };


  DefineClass(SortMappings)

  class SortMappings : public mmdb::QuickSort  {
    public :
      SortMappings() : mmdb::QuickSort() {}
      int  Compare ( int i, int j );
      void Sort    ( mmdb::ivector ci, int nc, MAMap *Map );
    protected :
      MAMap *M;
  };


}

#endif

