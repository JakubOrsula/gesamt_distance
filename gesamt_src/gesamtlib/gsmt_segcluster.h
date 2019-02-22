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
//  **** Module  :  GSMT_SegCluster <interface>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::SegCluster
//       ~~~~~~~~~
//
//  (C) E. Krissinel, 2008-2013
//
// =================================================================
//


#ifndef __GSMT_SegCluster__
#define __GSMT_SegCluster__

#include "gsmt_superposition.h"

// =================================================================

namespace gsmt {

  DefineClass(SegCluster);

  class SegCluster  {

    public :
      int               id; //!< cluster id
      int                n; //!< cluster size
      mmdb::ivector   segn; //!< lengths of segments
      mmdb::ivector   seg1; //!< starting indices of segments in 1st structure
      mmdb::ivector   seg2; //!< starting indices of segments in 2nd structure
      int            nSegs; //!< number of segment sections
      mmdb::vect3      mc1; //!< mass center #1
      mmdb::vect3      mc2; //!< mass center #2
      mmdb::realtype rmsd2; //!< square rmsd of the cluster
      mmdb::realtype     Q; //!< Q-score of the cluster
      mmdb::mat44        T; //!< transformation matrix
      mmdb::realtype  C[9]; //!< correlation matrix
      Superposition   * SD; //!< superposition data

      SegCluster ( int cid );
      ~SegCluster();

      void FreeSuperposition();
      void Reset  ( int cid );

      /// Calculates RMSD2 = |A1 - T*A2|^2 for first natoms
      mmdb::realtype getRMSD2 ( mmdb::PPAtom A1, mmdb::PPAtom A2,
                                int natoms );

      /// Returns True if rmsd2=|A1 - T*A2|^2 for first natoms of
      /// A1 and A2 is less than square_rmsd
      bool isSuperposable ( mmdb::PPAtom A1, mmdb::PPAtom A2,
                            int natoms, mmdb::realtype square_rmsd );

      void AddSegment   ( int i1, int i2, int segLen,
                          mmdb::rvector  CM,  // correlation matrix
                          mmdb::mat44  & TM,
                          mmdb::rvector  cm1,
                          mmdb::rvector  cm2,
                          mmdb::realtype rms2,
                          mmdb::realtype Qscore );

      void GetAlignment ( mmdb::ivector c1, int nat1,
                          mmdb::ivector c2, int nat2 );

      void SetSuperposition ( mmdb::mat44 &  T, // transformation matrix
                              mmdb::realtype rmsd,
                              mmdb::realtype seqId,
                              mmdb::realtype Q,
                              int            Nalgn,
                              mmdb::ivector  c1,
                              mmdb::rvector  sdist1,
                              int            nat1,
                              mmdb::ivector  c2,
                              int            nat2 );
   
      void calcCorrMatrix ( mmdb::rvector       CM,
                            mmdb::PPAtom        A1,
                            mmdb::PPAtom        A2,
                            int                 i1,
                            int                 i2,
                            int                 n,
                            const mmdb::vect3 & cm1,
                            const mmdb::vect3 & cm2 );

    protected :
      void InitClass ( int cid );
      void FreeMemory();

    private :
      int nalloc;

  };

}


#endif

