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
//  **** Module  :  GSMT_MAligner <interface>
//       ~~~~~~~~~
//  **** Project :  GESAMT - multiple structure aligner
//       ~~~~~~~~~
//  **** Classes :  gsmt::MAligner
//       ~~~~~~~~~
//
//  (C) E. Krissinel, 2008-2015
//
// =================================================================
//

#ifndef __GSMT_MAligner__
#define __GSMT_MAligner__

#include "gsmt_aligner.h"
#include "gsmt_mastructure.h"
#include "gsmt_maoutput.h"
#include "gsmt_superposition.h"

// =================================================================


namespace gsmt {

  struct MAMap;

  DefineClass(MAligner);

  class MAligner : public Base  {

    public :

      MAligner ();
      MAligner ( mmdb::io::RPStream Object );
      ~MAligner();

      GSMT_CODE Align ( PPMAStructure structures,
                        int           nStructures,
                        bool          measure_cpu );

      void getAlignScores     ( int            & n_align,
                                mmdb::realtype & rmsd,
                                mmdb::realtype & Qscore );
      void getConsensusScores ( mmdb::rvector  & cons_x,
                                mmdb::rvector  & cons_y,
                                mmdb::rvector  & cons_z,
                                int            & cons_len,
                                mmdb::rmatrix  & m_rmsd,
                                mmdb::rmatrix  & m_Qscore,
                                mmdb::rmatrix  & m_seqId );

      void getMAOutput        ( PPMAOutput & MAOut,
                                int        & nrows,
                                int        & ncols );
      void writeMultAlign     ( mmdb::io::RFile f );
      void writeMultAlign_csv ( mmdb::io::RFile f );
      void writeSeqAlignment  ( mmdb::io::RFile f,
                                mmdb::psvector fnames,
                                mmdb::psvector sel
                              );

      void makeMultAlignTable_rvapi ( mmdb::cpstr tableId );
      void makeMultAlignGraph_rvapi ( mmdb::cpstr graphId );
                              
      inline int getNofStructures()  { return nStruct; }
      void takeConsensusScores ( mmdb::rmatrix  & m_rmsd,
                                 mmdb::rmatrix  & m_Qscore,
                                 mmdb::rmatrix  & m_seqId );

      void read  ( mmdb::io::RFile f );
      void write ( mmdb::io::RFile f );

    protected :
      int              minIter;     //!< minimal number of iterations
      int              maxIter;     //!< minimal number of iterations
      int              maxHollowIt; //!< maximal allowed number of
                                    /// consequitive iterations without
                                    /// quality improvement
      PPMAStructure    S;
      PPSuperposition *SPS;
      mmdb::ivector    v1;          //!< working array [1..nStruct]
      int              nStruct;

      MAMap           *Map;          //!< Map maps 0:i<->j:Map[i].map[j]
      int              map_nrows;    //!< number of rows in Map
      int              nStructAlloc; //!< allocated number of structures

      int              Nalign;       //!< number of multuply-aligned rows
      mmdb::realtype   rmsd_achieved; //!< achieved RMSD
      mmdb::realtype   Q_achieved;    //!< achieved Q

      int              minNres,maxNres; //!< structure size range
      mmdb::rvector    xc;   //!< consensus X-coordinates
      mmdb::rvector    yc;   //!< consensus Y-coordinates
      mmdb::rvector    zc;   //!< consensus Z-coordinates
//      mmdb::rvector    disp; //!< consensus dispersions
      mmdb::rmatrix    mx_rmsd;   //!< matrix of inter-structure rmsds
      mmdb::rmatrix    mx_Qscore; //!< matrix of inter-structure Q-scores
      mmdb::rmatrix    mx_seqId;  //!< matrix of inter-structure seq identities

      mmdb::mat33      svdA,svdZ,svdV; //!< corr-n matrix, left and right SVD vectors
      mmdb::vect3      svdW,svdRV;     //!< singular values

      //  Initialization and memory management
      void  initClass               ();
      void  freeMemory              ();
      void  freeSuperpositions      ();
      void  allocateMap             ();
      void  freeMap                 ();

      GSMT_CODE makeCrossSuperpositions ();
      GSMT_CODE makeCrossSuperpositions1();
      GSMT_CODE makeFirstGuess          ();
      bool  evaluateMapping         ( mmdb::PMContact C        );
      void  correspondContacts      ( mmdb::realtype  contDist );
      void  calcRMSD                ( int             mappos   );
      mmdb::realtype MatchQuality   ( int             Nalgn,
                                      int             N1,
                                      int             N2,
                                      mmdb::realtype  dist2    );
      mmdb::realtype matchQuality2  ( int             Nalgn,
                                      mmdb::realtype  dist2    );
      void  calcConsensus           ();
      int   calcRotation            ( mmdb::mat44   & R        );
      void  optimizeAlignments      ();
      void  sortStructures          ();
      void  calcConsensusScores     ();
      void  deselectCalphas         ();

  };
  
  void calcSeqAlignment ( PPMAOutput  MAOut,
                          int         nrows,
                          int         ncols,
                          mmdb::psvector & S  );

}

#endif

