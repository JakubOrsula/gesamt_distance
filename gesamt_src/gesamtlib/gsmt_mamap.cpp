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
//  **** Module  :  GSMT_MAMap <implementation>
//       ~~~~~~~~~
//  **** Project :  GESAMT - multiple structure aligner
//       ~~~~~~~~~
//  **** Classes :  gsmt::MAMap
//       ~~~~~~~~~
//
//  (C) E. Krissinel, 2008-2015
//
// =================================================================
//

#include "gsmt_mamap.h"

// =================================================================

void gsmt::MAMap::Init ( int nStruct )  {
  rmsd = 0.0;
  mmdb::GetVectorMemory ( map,nStruct,1 );
  xc   = 0.0;
  yc   = 0.0;
  zc   = 0.0;
}

void gsmt::MAMap::Dispose()  {
  mmdb::FreeVectorMemory ( map,1 );
}


int gsmt::SortMappings::Compare ( int i, int j )  {
mmdb::realtype ri,rj;
  ri = M[((mmdb::ivector)data)[i]].rmsd;
  rj = M[((mmdb::ivector)data)[j]].rmsd;
  if (ri<rj)  return  1;
  if (ri>rj)  return -1;
  return 0;
}

void gsmt::SortMappings::Sort ( mmdb::ivector ci, int nc, MAMap *Map )  {
  M = Map;
  mmdb::QuickSort::Sort ( &(ci[0]),nc );
}

