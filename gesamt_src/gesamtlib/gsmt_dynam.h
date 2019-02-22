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
//  **** Module  :  GSMT_Dynam <interface>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::Dynam
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2008-2014
//
// =================================================================
//

#ifndef __GSMT_Dynam__
#define __GSMT_Dynam__

#include "mmdb2/mmdb_coormngr.h"

// =================================================================

namespace gsmt  {

  DefineClass(Dynam);

  class  Dynam  {

    public :

      Dynam ();
      ~Dynam();

      void Align   ( mmdb::PPAtom A1, int natoms1,
                     mmdb::PPAtom A2, int natoms2,
                     mmdb::realtype max_cont );
      
      void MLAlign ( mmdb::PPAtom A1, int natoms1,
                     mmdb::PPAtom A2, int natoms2,
                     mmdb::realtype sigma );

      void Align   ( int natoms1, int natoms2, mmdb::PContact contact,
                     int nContacts, mmdb::realtype max_cont );

      void Align   ( mmdb::PPAtom A1, int natoms1,
                     mmdb::PPAtom A2, int natoms2,
                     mmdb::realtype R0, mmdb::realtype rmsd_est );

      void Align   ( int natoms1, int natoms2, mmdb::PContact contact,
                     int nContacts, mmdb::realtype R0,
                     mmdb::realtype rmsd_est );

      void getAlignment ( mmdb::ivector & ac1, mmdb::ivector & ac2,
                          mmdb::rvector scores = NULL );

    protected :
      mmdb::realtype maxCont2;
      int            n1,n2;
      mmdb::imatrix  V;
      mmdb::imatrix  mi;
      mmdb::ivector  c1,c2;

      void  InitClass ();
      void  FreeMemory();
      void  GetMemory ( int natoms1, int natoms2 );

      void  CalcScores    ( mmdb::PPAtom A1, mmdb::PPAtom A2 );
      void  CalcMLScores  ( mmdb::PPAtom A1, mmdb::PPAtom A2,
                            mmdb::realtype sigma2 );
      void  CalcScores    ( mmdb::PContact contact, int nContacts );
      void  BuildPathTable();
      void  Backtrace     ();

    private :
      int  n1alloc,n2alloc;

  };

}

#endif

