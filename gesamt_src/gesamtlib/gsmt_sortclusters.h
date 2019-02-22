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
//    10.02.13   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  GSMT_SortClusters <interface>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::SortClusters
//       ~~~~~~~~~
//
//  (C) E. Krissinel, 2008-2013
//
// =================================================================
//

#ifndef __GSMT_SortClusters__
#define __GSMT_SortClusters__

#include "gsmt_segcluster.h"
#include "shiftsort_.h"

// =================================================================

namespace gsmt  {

  DefineClass(SortClusters);

  /*
  class SortClusters : public mmdb::QuickSort  {

    public :
      SortClusters () {}
      SortClusters ( mmdb::io::RPStream Object );
      ~SortClusters() {}
      int  Compare ( int i, int j );
      void Swap    ( int i, int j );
      void Sort    ( PPSegCluster SegCluster, int nClusters );
  };
  */

  class SortClusters : public ShiftSort  {

    public :
      SortClusters ();
      ~SortClusters();
      int  Compare ( void * di, void * dj );
      void Sort    ( PPSegCluster SegCluster, int nClusters );
  };

}

#endif

