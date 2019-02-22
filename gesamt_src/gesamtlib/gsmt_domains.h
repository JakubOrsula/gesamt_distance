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
//    11.05.16   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  GSMT_Domains <interface>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::Domains
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2008-2016
//
// =================================================================
//

#ifndef __GSMT_Domains__
#define __GSMT_Domains__

#include "gsmt_aligner.h"
#include "gsmt_structure.h"
#include "gsmt_domdata.h"

// =================================================================

namespace gsmt {

  DefineClass(Domains);

  class Domains : public Aligner  {

    public :

      Domains ();
      Domains ( mmdb::io::RPStream Object );
      virtual ~Domains();
      
      // Optional set functions if defaults are not good

      void setOutputDir  ( mmdb::cpstr outputDir  );

      GSMT_CODE findDomains ( PStructure s1, PStructure s2 );

      PDomData   getDomains ()  { return domData; }
      void writeDomAlignTable ( PStructure s1, PStructure s2,
                                mmdb::io::RFile f );
      void makeDomAlignTable_rvapi ( mmdb::cpstr tableId,
                                     PStructure  s1,
                                     PStructure  s2 );
                    
      void resetStructureData ( PStructure s1, PStructure s2 );


    protected :
      mmdb::pstr      outDir;        //!< path to output directory + "/"
      int             minDomainSize; //!< in residues
      int             maxTrimSize;   //!< in residues
      int             maxGapSize;    //!< in residues
      mmdb::realtype  R0factor;      //!< R0 decrease factor <1
      mmdb::realtype  minR0;         //!< minimum R0 to try

      PPSuperposition DSD;      //!< domain alignments
      int             nDomains; //!< number of identified domains
      PDomData        domData;  //!< final domain definitions

      mmdb::ivector   asn1;     //!< serial atom numbers in 1st structure
      mmdb::ivector   asn2;     //!< serial atom numbers in 2nd structure
      int             natoms01; //!< number of atoms in full 1st structure
      int             natoms02; //!< number of atoms in full 2nd structure

      void initDomains();
      void freeMemory ();
      void freeDomains();

      void trimAlignment   ( mmdb::ivector c1, mmdb::ivector c2 );
      void addDomain       ( PSuperposition & SD, bool copy );

      GSMT_CODE findDomain ( PStructure s1, PStructure s2 );

      void R0scan ( PStructure s1, PStructure s2 );
      
  };

}

#endif
