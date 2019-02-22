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
//  **** Module  :  GSMT_MAOutput <interface>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::MAOutput
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2008-2015
//
// =================================================================
//

#ifndef __GSMT_MAOUTPUT__
#define __GSMT_MAOUTPUT__

#include "mmdb2/mmdb_atom.h"

// =================================================================

namespace gsmt  {

  DefineStructure(MAOutput);

  struct MAOutput {
    mmdb::ResName  name;
    mmdb::ChainID  chID;
    int            seqNum;
    int            sseType;
    mmdb::InsCode  insCode;
    mmdb::realtype rmsd;      // not used
    bool           aligned;

    void  Init  ();
    void  Fill  ( mmdb::PAtom A, bool align );
    void  Copy  ( RMAOutput M );

    void  write ( mmdb::io::RFile f );
    void  read  ( mmdb::io::RFile f );
    
    static int getWriteSize();
    void mem_write ( mmdb::pstr  S, int & l );
    void mem_read  ( mmdb::cpstr S, int & l );

  };

  extern void FreeMSOutput ( PPMAOutput & MAOut, int & nrows );

}

#endif // GSMT_MAOUTPUT_H
