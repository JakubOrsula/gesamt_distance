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
//  **** Module  :  GSMT_MAStructure <interface>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::MAStructure
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2013-2015
//
// =================================================================
//

#ifndef __GSMT_MAStructure__
#define __GSMT_MAStructure__

#include "gsmt_structure.h"

// =================================================================

namespace gsmt {

  struct MAMap;

  DefineClass(MAStructure);

  class MAStructure : public Structure  {

    public :

      int            serNo;    //!< structure serial number (0,1,...)
      mmdb::realtype Qsum;     //!< multiple superposition Q-score
      int            nres;     //!< number of residues (C-alphas)
      mmdb::PPAtom   Calpha;   //!< selected C-alphas
      mmdb::ChainID *chID;     //!< chain association for C-alphas
      mmdb::ivector  c0;       //!< reference atom match vector
      mmdb::ivector  c;        //!< atom match vector
      mmdb::ivector  unmap1;   //!< atom unmap vector
      mmdb::rvector  x0,y0,z0; //!< atom coordinate buffer
      mmdb::mat44    RT0,RT;   //!< rotation-translation matrices

      MAStructure ();
      MAStructure ( mmdb::io::RPStream Object );
      ~MAStructure();

      void  prepareMAStructure();

      void  saveCoordinates   ();
      void  restoreCoordinates();
      void  transform         ();
      bool  isMC ( int pos1, PMAStructure S, int pos2 );
      void  calcCorrelationMatrix ( mmdb::mat33   & A,
                                    MAMap         * Map,
                                    mmdb::realtype  sigma2 );
      void  calcTranslation();

      void  copy ( PMAStructure S ); //!< deep-copy from S to this      

      void  read  ( mmdb::io::RFile f );
      void  write ( mmdb::io::RFile f );

    protected :
      mmdb::realtype xm,ym,zm;  //!< center of mass
      mmdb::realtype cx,cy,cz;  //!< consensus center of mass
      int            nalign;    //!< number of aligned residues

      void initClass ();
      void freeMemory();

  };

}

#endif

