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
//    06.02.17   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  GSMT_Base <implementation>
//       ~~~~~~~~~
//  **** Project :  GESAMT - multiple structure aligner
//       ~~~~~~~~~
//  **** Classes :  gsmt::Base
//       ~~~~~~~~~
//
//  (C) E. Krissinel, 2008-2017
//
// =================================================================
//

#include <string.h>

#include "gsmt_base.h"

// =================================================================

gsmt::Base::Base() : mmdb::io::Stream()  {
  initBase();
}

gsmt::Base::Base ( mmdb::io::RPStream Object )
          : mmdb::io::Stream(Object) {
  initBase();
}

gsmt::Base::~Base()  {}

void gsmt::Base::initBase()  {

  // Structure match parameters

  performance = PERFORMANCE_Efficient;
  maxContact  = 8.0;           // angstrom; maximal contact distance
  QR0         = QR0_default;   // parameter of Q-score
  sigma       = sigma_default; // superposition sigma

  // Sequence match parameters

  alignMethod   = mmdb::math::ALIGN_GLOBLOC; // sequence alignment mode
  gapPenalty    = -1.0;        // gap penalty
  spacePenalty  = -1.0;        // space penalty 
  scoreEqual    =  2.0;        // score for residue match
  scoreNotEqual = -1.0;        // score for residue mismatch

  // Common match parameters

  minMatch1   = 0.0;
  minMatch2   = 0.0;
  trimQ       = 0.0;  // Q-based trim factor for scan results [0..1]
  trimSize    = 0.0;  // Size-based trim factor for scan results [0..1]

  verbosity   = 1;
  nthreads    = 1;

}

void gsmt::Base::setSimilarityThresholds ( mmdb::realtype minPart1,
                                           mmdb::realtype minPart2 ) {
  minMatch1 = mmdb::RMin ( 1.0,fabs(minPart1) );
  minMatch2 = mmdb::RMin ( 1.0,fabs(minPart2) );
}

void gsmt::Base::setTrimFactors ( mmdb::realtype trim_Q,
                                  mmdb::realtype trim_size )  {
  trimQ    = trim_Q;
  trimSize = trim_size;
}

void gsmt::Base::setTargetSizeFilter ( mmdb::realtype minSize,
                                       mmdb::realtype maxSize )  {
  setSimilarityThresholds ( minSize,1.0/maxSize );
}

gsmt::PStructure gsmt::Base::getQueryStructure ( mmdb::cpstr fQuery,
                                                 mmdb::cpstr selQuery,
                                                 bool SCOPSelSyntax )  {
PStructure       s   = new gsmt::Structure();
mmdb::pstr       msg = NULL;
mmdb::ERROR_CODE rc;

  if (verbosity>=0)
    printf ( "\n"
      " ... reading QUERY structure : file '%s', selection '%s'\n",
      fQuery,selQuery );  

  rc = s->getStructure ( fQuery,selQuery,-1,SCOPSelSyntax );

  if (!rc)  {
    if (verbosity>=0)
      printf ( "      %5i atoms selected\n",s->getNCalphas() );
    s->prepareStructure ( maxContact );
  } else  {
    if (verbosity>=0)  {
      if (rc>0)
        printf ( "      %5i atoms selected with warning (rc=%i)\n",
                 s->getNCalphas(),rc );
      else
        printf ( " *error* (rc=%i)\n",rc );
      s->getErrDesc ( rc,msg );
      printf ( "     %s\n",msg );
      printf (
        "\n\n STOP DUE TO READ ERRORS\n"
        " --- check input file format\n"
             );
    }
    delete s;
    s = NULL;
  }
  
  if (msg)  delete[] msg;
  return s;

}


gsmt::PSequence gsmt::Base::getQuerySequence ( mmdb::cpstr fQuery )  {
PSequence        s   = new gsmt::Sequence();
mmdb::pstr       msg = NULL;
Sequence::RETURN_CODE rc;

  if (verbosity>=0)
    printf ( "\n"
      " ... reading QUERY sequence from file '%s'\n",
      fQuery );  

  rc = s->loadSequence ( fQuery );

  if (rc==Sequence::RC_Ok)  {
    if (verbosity>=0)
      printf ( "      %5i residues\n",s->getSeqLength() );
  } else  {
    if (verbosity>=0)  {
      printf ( " *error* (rc=%i)\n",rc );
      s->getErrDesc ( rc,msg );
      printf ( "     %s\n",msg );
      printf (
        "\n\n STOP DUE TO READ ERRORS\n"
        " --- check input file format\n"
             );
    }
    delete s;
    s = NULL;
  }
  
  if (msg)  delete[] msg;
  return s;

}


void gsmt::Base::read  ( mmdb::io::RFile f )  {
  mmdb::io::Stream::read ( f );
}

void gsmt::Base::write ( mmdb::io::RFile f )  {
  mmdb::io::Stream::write ( f );
}

