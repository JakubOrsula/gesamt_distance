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
//  **** Module  :  GSMT_Clusterer <interface>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::Clusterer
//       ~~~~~~~~~
//
//  (C) E. Krissinel, 2008-2013
//
// =================================================================
//

#ifndef __GSMT_Clusterer__
#define __GSMT_Clusterer__

#include "gsmt_base.h"
#include "gsmt_segcluster.h"
#include "gsmt_structure.h"

// =================================================================

namespace gsmt  {

  DefineClass(Clusterer);

  class Clusterer : public Base  {

    public :

      Clusterer ();
      Clusterer ( mmdb::io::RPStream Object );
      ~Clusterer();

      bool stop;

      inline mmdb::PPAtom getCalphas1() const { return A1; }
      inline mmdb::PPAtom getCalphas2() const { return A2; }
      
      void read  ( mmdb::io::RFile f );
      void write ( mmdb::io::RFile f );

    protected :

      //  --------- Fixed parameters

      int            minSegLen;  //!< minimal segment length to initiate
                                 /// a cluster
      mmdb::realtype segTol;     //!< distance tolerance for segment
                                 /// matching
      mmdb::realtype clustTol2;  //!< square distance tolerance for cluster
                                 /// matching
      mmdb::realtype filterTol2; //!< square distance tolerance for cluster
                                 /// filtering
      int          pruneSegLen;  //!< maximal segment length for pruning
      int          pruneSegGap;  //!< minimal inter-segment gap for pruning

      //  --------- Data and derived parameters

      mmdb::PPAtom  A1;         //!< structure #1 C-alphas (copy pointer)
      int           natoms1;    //!< structure #1 length
      mmdb::PPAtom  A2;         //!< structure #2 C-alphas (copy pointer)
      int           natoms2;    //!< structure #2 length
      int           minMatch;   //!< minimal number of atoms to be matched

      //  --------- Internal data structures

      mmdb::rmatrix D1,D2;      //!< distance matrices (copy pointers)
      mmdb::imatrix sid;        //!< segment ID matrix

      PPSegCluster  Cluster;    //!< segment superposition clusters
      int           nClusters;  //!< number of clusters;

      //  --------- Memory pools

      mmdb::mat33 svd_A,svd_U,svd_V; //!< memory pools for
      mmdb::vect3 svd_W,svd_RV1;    ///   superposition routines

      //  Initialization and memory management

      void initClusterer ();
      void freeMemory    ();
      void GetDMatMemory ( int nat1, int nat2 );
      void FreeClusters  ();

      //  Segment superposition and clustering

      void calcMassCenters ( mmdb::PPAtom  atom1,
                             mmdb::PPAtom  atom2,
                             int           n,
                             mmdb::rvector mc1,
                             mmdb::rvector mc2 );
      bool isRotoinversion ( mmdb::PPAtom  atom1,
                             mmdb::PPAtom  atom2,
                             int           n,
                             mmdb::rvector mc1,
                             mmdb::rvector mc2 );
      bool     checkAndAdd ( PSegCluster C, int i1, int i2, int n,
                             mmdb::rvector mc1, mmdb::rvector mc2 );
      void   pruneClusters ( int i1 );
      void      addSegment ( int i1, int i2, int n, int & clusterID,
                             mmdb::rvector mc1, mmdb::rvector mc2 );

      GSMT_CODE makeSegClusters ( PStructure s1, PStructure s2 );

    private :
      int  n1alloc,n2alloc; //!< allocated dimensions of distance matrices
      int  nClAlloc;        //!< length of allocated cluster array

  };

}

#endif

