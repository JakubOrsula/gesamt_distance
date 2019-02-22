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
//  **** Module  :  Gesamt <implementation>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :
//       ~~~~~~~~~
//
//  GESAMT: General and Efficient Structural Alignment of
//          Macromoecular Targets
//
//  (C) E. Krissinel, 2008-2017
//
// =================================================================
//

#include <string.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>


#include "gesamtlib/gsmt_defs.h"
#include "gesamtlib/gsmt_aligner.h"
#include "distance.h"

// ================================================================


int main(int argc, char **argv, char **env) {

    std::cout << get_distance("12AS:A", "12AS:A") << std::endl;
    std::cout << get_distance("1A0N:A", "12AS:A") << std::endl;

    return 0;
}
