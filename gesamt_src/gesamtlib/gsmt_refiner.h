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
//  **** Module  :  GSMT_Refiner <interface>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::Refiner
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2008-2013
//
// =================================================================
//

#ifndef __GSMT_Refiner__
#define __GSMT_Refiner__

#include "gsmt_clusterer.h"
#include "gsmt_dynam.h"
#include "gsmt_sortpairs.h"

// =================================================================

namespace gsmt {

  DefineClass(Refiner);

  class Refiner : public Clusterer  {

    public :

      Refiner ();
      Refiner ( mmdb::io::RPStream Object );
      ~Refiner();

      void read  ( mmdb::io::RFile f );
      void write ( mmdb::io::RFile f );

    protected :

      //  --------- Fixed parameters
      int            iterMax;    //!< limit for iterative refinement

      //  --------- Input parameters

      mmdb::realtype refineDepth; //!< depth of going into cluster list at
                                  /// refinement [0..1]
      mmdb::realtype     deltaRT; //!< threshold for comparison of RT
                                  /// matrices, angstrom
      int         maxClustRefine; //!< maximal number of clusters to refine

      //  --------- Structure's emedding sphere

      mmdb::realtype  radius2;    //!< radius of embedding sphere

      //  --------- Internal data structures

      Dynam           DA;       //!< dynamic alignment
      SortPairs       QSP;      //!< atom contact sorting
      mmdb::PManager  M1;       //!< MMDB manager for contact seeking

      //  --------- Memory pools

      // memory pools for iterative refinement
      mmdb::vect3   * xyz2;     //!< working copy of structure #2
      mmdb::ivector   c10,c20;  //!< best correspondence vectors
      mmdb::rvector   cdist;    //!< contact distances to sort
      mmdb::ivector   cix;      //!< contact indexes to sort
      mmdb::PContact  contact;  //!< pool for atom contacts

      void  initRefiner   ();
      void  freeMemory    ();

      void  getRefMemory  ();
      void  freeRefMemory ();

      void  printClusterData();

      void  Refine        ( PStructure s1, PStructure s2 );
      void  Superpose     ( mmdb::ivector ac1, mmdb::mat44 & T,
                            bool useSigma );
      bool  checkRTMatrix ( mmdb::mat44 & T, int clusterNo );
      void  refineCluster ( int clusterNo );

    private :
      int   nContAlloc;   //!< allocated length of atom contacts pool

  };

}

#endif
