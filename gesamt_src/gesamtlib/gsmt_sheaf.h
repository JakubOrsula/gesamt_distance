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
//    06.02.16   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  GSMT_Sheaf <interface>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::Sheaf
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2008-2016
//
// =================================================================
//

#ifndef __GSMT_Sheaf__
#define __GSMT_Sheaf__

#include "gsmt_structure.h"
#include "gsmt_sheafdata.h"
#include "gsmt_sortpairs.h"
#include "gsmt_defs.h"

// =================================================================

namespace gsmt {

  enum SHEAF_MODE  {
    SHEAF_None   = 0x0,
    SHEAF_Auto   = 0x1,
    SHEAF_Atoms  = 0x2,
    SHEAF_Chains = 0x4,
    SHEAF_All    = 0x6,
    SHEAF_X      = 0xA
  };

  DefineClass(Sheaf);

  class Sheaf  {

    public :

      Sheaf ();
      virtual ~Sheaf();

      inline void setSheafMode ( SHEAF_MODE smode   ) { mode  = smode; }
      inline void setQR0       ( mmdb::realtype QR0 ) { R0      = QR0; }
      inline void setQThreshold( mmdb::realtype QT  ) { Qthresh = QT;  }
      inline void setUseOcc    ( bool occ )           { useOcc  = occ; }
      inline void setNThreads  ( int nt )             { nthreads = nt; }
      inline void setVerbosity ( int v )              { verbosity = v; }

      GSMT_CODE Align  ( PPStructure structures,
                         int         nStructures,
                         bool        measure_cpu );

      inline int getNofSheafs() { return nSheafs; }
      PSheafData getSheafData ( int index );
      void takeSheafData ( PPSheafData & sdata, int & nsdata );
      
    protected :
      SortPairs      QSP;       //!< atom contact sorting
      SHEAF_MODE     mode;      //!< sheaf mode
      mmdb::realtype R0;        //!< Q-score parameter
      mmdb::realtype Qthresh;   //!< threshold Q for sheafs (when >0)
      bool           useOcc;    //!< flag to use atom occupancy
      int            nthreads;  //!< number of threads to use
      int            verbosity; //!< verbosity level
      int            iterMax;   //!< limit for iterative refinement

      PPStructure    S;         //!< copy pointer on structure vector
      int            nAtoms;    //!< number of atoms (sheaf length)
      int            nStruct;   //!< number of structures
      PPSheafData    sheafData; //!< sheafs
      int            nSheafs;   //!< number of sheafs found

      // working arrays - atoms
      mmdb::rvector  var2;      //!< square coordinate variance
      mmdb::rvector  cx;        //!< consensus x
      mmdb::rvector  cy;        //!< consensus y
      mmdb::rvector  cz;        //!< consensus z
      mmdb::ivector  ix;        //!< atom indices
      mmdb::ovector  mask;      //!< atom mask
      mmdb::ovector  mask0;     //!< temporary atom mask

      // working arrays - structures
      mmdb::ovector  smask;     //!< structure mask
      mmdb::PPAtom  *atm;       //!< atom data
      mmdb::mat44   *RT;        //!< rotation-translation matrices
      mmdb::rvector  ax;        //!< temporary atom x-coordinates
      mmdb::rvector  ay;        //!< temporary atom y-coordinates
      mmdb::rvector  az;        //!< temporary atom z-coordinates

      void  initSheaf ();
      void  freeMemory();
      void  makeMemoryAllocations();

      GSMT_CODE makeCrossAlignments();
      GSMT_CODE sheaf2       ( PSheafData sdata ); //!< sheaf data
      GSMT_CODE addStructure ( PSheafData sdata, int structNo );

    private:
      int   allocNAtoms,allocNStruct,allocNSheafs;

  };

}

#endif
