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
//  **** Module  :  Gesamt <implementation>
//       ~~~~~~~~~
//  **** Project :  GESAMT Pairwise alignment driver
//       ~~~~~~~~~
//  **** Classes :
//       ~~~~~~~~~
//
//  GESAMT: General and Efficient Structural Alignment of
//          Macromoecular Targets
//
//  (C) E. Krissinel, 2008-2015
//
// =================================================================
//

#include <string.h>

#include "gesamt_input.h"
#include "gesamtlib/gsmt_aligner.h"
#include "gesamtlib/gsmt_viewer.h"

// =================================================================

extern void alignToTarget         ( gsmt::RInput Input );

extern void printFracAnalysis     ( mmdb::mat44 & T, mmdb::cpstr name,
                                    mmdb::PManager M );
extern void writeFracAnalysis_csv ( mmdb::pstr S,
                                    mmdb::mat44 & T, mmdb::cpstr name,
                                    mmdb::PManager M );

