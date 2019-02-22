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
//    30.05.16   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  GSMT_DomData <interface>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::DomData
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2008-2016
//
// =================================================================
//

#ifndef __GSMT_DomData__
#define __GSMT_DomData__

#include "gsmt_msuperposition.h"
#include "gsmt_aligner.h"

// =================================================================

namespace gsmt {

  DefineClass(DomData);

  class DomData  {

    public :
      int      nStructures;  //!< number of structures with defined domains
      int         nDomains;  //!< number of domains defined
      mmdb::ivector  sSize;  //!< structure full sizes in residues
      PPMSuperposition  SD;  //!< [domNo][structNo] superposition data
      mmdb::imatrix   ddef;  //!< [structNo][i] gives domain number
      mmdb::realtype     Q;  //!< combined Q-score
      mmdb::realtype  rmsd;  //!< combined rmsd
      mmdb::realtype seqId;  //!< combined sequence identity
      int           Nalign;  //!< combined alignment length

      DomData ();
      virtual ~DomData();

      void setData ( PPStructure     S,
                     PPSuperposition D,
                     PAligner        A,  //!< NULL for no realignment
                     int             n_structures,
                     int             n_domains );

      void getCombinedAlignment ( mmdb::imatrix & c,
                                  mmdb::rvector & var );

    protected :

      void initDomData();
      void freeMemory ();

      void set_data_area ( PPStructure     S,
                           PPSuperposition D,
                           int  n_structures,
                           int     n_domains );
      void lay_domains   ();
      void align_domains ( PPStructure S, PAligner A );

      
  };

}

#endif
