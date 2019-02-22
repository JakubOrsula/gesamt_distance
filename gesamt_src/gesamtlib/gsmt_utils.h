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
//    02.12.15   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  GSMT_Utils <interface>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2008-2015
//
// =================================================================
//

#ifndef __GSMT_Utils__
#define __GSMT_Utils__

#include <sys/stat.h>
#include "gsmt_structure.h"

namespace gsmt  {

  extern void getHours        ( mmdb::realtype secs,
                                int & ihour, int & imin, int & isec );

  extern void getOutFilePath  ( mmdb::pstr  & fpath,
                                mmdb::cpstr   orgFName,
                                mmdb::cpstr   outFName,
                                int           serNo,
                                int           out_key );

  extern bool writeSuperposed ( const PPStructure   s,
                                const mmdb::mat44  *T,
                                int                  nStruct,
                                mmdb::cpstr          outFName,
                                mmdb::SELECTION_TYPE selType,
                                int                  out_multi );


#ifdef  _WIN32
  extern bool mkdirp ( mmdb::cpstr path );
#else
  extern bool mkdirp ( mmdb::cpstr path,
     mode_t mode = S_IRWXU | S_IRGRP |  S_IXGRP | S_IROTH | S_IXOTH );
#endif

}

#endif
