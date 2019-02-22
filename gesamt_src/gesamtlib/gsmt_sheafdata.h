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
//  **** Module  :  GSMT_SheafData <interface>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::SheafData
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2008-2016
//
// =================================================================
//

#ifndef __GSMT_SheafData__
#define __GSMT_SheafData__

#include "gsmt_structure.h"
#include "shiftsort_.h"

// =================================================================

namespace gsmt {

  DefineClass(SheafData);

  class SheafData  {

    public :
      mmdb::ivector  sId;     //!< structure ids (indexes)
      PPStructure    S;       //!< copy pointers to structures
      mmdb::mat44   *T;       //!< structure transformation matrices
      mmdb::ovector  mask;    //!< atom mask
      mmdb::rvector  cx;      //!< consensus x
      mmdb::rvector  cy;      //!< consensus y
      mmdb::rvector  cz;      //!< consensus z
      mmdb::rvector  var2;    //!< square distance variance
      mmdb::realtype Q;       //!< Q-score
      mmdb::realtype rmsd;    //!< r.m.s.d.
      mmdb::realtype seqId;   //!< average sequence identity
      int            nStruct; //!< number of structures
      int            nAtoms;  //!< number of atoms in each structure
      int            Nalign;  //!< number of aligned atoms

      SheafData ( int n_struct, int n_atoms );
      virtual ~SheafData();
      
      void clear();
      
      void addStructure  ( PStructure st, int structNo );
      bool hasStructure  ( int  structNo );
      bool hasStructure  ( PSheafData sd );
      
      void copyFrom      ( PSheafData source );

      void writeResAlign ( mmdb::io::RFile f );

    protected:
      void initSheafData ( int n_struct, int n_atoms );
      void freeMemory    ();

  };

  DefineClass(SortSheafData);

  class SortSheafData : public ShiftSort  {

    public:

      SortSheafData();
      virtual ~SortSheafData();
      
      virtual int Compare ( void * di, void * dj );

      void Sort ( PPSheafData d, int dataLen );
      
  };


}

#endif
