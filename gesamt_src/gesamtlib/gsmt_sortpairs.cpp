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
//    04.01.16   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  GSMT_SortPairs <implementation>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::SortPairs
//       ~~~~~~~~~
//
//  (C) E. Krissinel, 2008-2016
//
// =================================================================
//

#include <string.h>

#include "gsmt_sortpairs.h"

// =================================================================

/*
gsmt::SortPairs::SortPairs ( mmdb::io::RPStream Object )
               : mmdb::QuickSort(Object)  {}

int gsmt::SortPairs::Compare ( int i, int j )  {
// sort by increasing contact distance
  if (((mmdb::rvector)data)[i]<((mmdb::rvector)data)[j])  return -1;
  if (((mmdb::rvector)data)[i]>((mmdb::rvector)data)[j])  return  1;
  return 0;
}

void gsmt::SortPairs::Swap ( int i, int j )  {
mmdb::realtype r;
int            k;
  k = kix[i];   kix[i] = kix[j];   kix[j] = k;
  r = ((mmdb::rvector)data)[i];
  ((mmdb::rvector)data)[i] = ((mmdb::rvector)data)[j];
  ((mmdb::rvector)data)[j] = r;
}

void gsmt::SortPairs::Sort ( mmdb::ivector ix, mmdb::rvector dist2,
                             int nPairs )  {
  kix = ix;
  mmdb::QuickSort::Sort ( dist2,nPairs );
}

*/


gsmt::SortPairs::SortPairs()  {
  nDataAlloc = 0;
  data       = NULL;
  kix        = NULL;
}

gsmt::SortPairs::~SortPairs()  {
  if (data)  delete[] data;
  if (kix)   delete[] kix;
}

void gsmt::SortPairs::Sort ( mmdb::ivector ix, mmdb::rvector dist2,
                             int nPairs )  {
int  n,m,k,k1,k2;

  if (nPairs<2)  return;
  
  if (nPairs>nDataAlloc)  {
    if (data)  delete[] data;
    if (kix)   delete[] kix;
    data = new mmdb::realtype[nPairs];
    kix  = new int[nPairs];
    nDataAlloc = nPairs;
  }

  k2 = nPairs - 1;
  k1 = nPairs - 2;
  if (dist2[0]<dist2[1])  {
    data[k2] = dist2[1];
    data[k1] = dist2[0];
    kix [k2] = ix[1];
    kix [k1] = ix[0];
  } else  {
    data[k2] = dist2[0];
    data[k1] = dist2[1];    
    kix [k2] = ix[0];
    kix [k1] = ix[1];
  }

  for (n=2;n<nPairs;n++)  {

    m  = nPairs-n;
    k1 = m;
    k2 = nPairs-1;
    do {
      k = (k1+k2)/2;
      if (dist2[n]<data[k])       k2 = k;
      else if (dist2[n]>data[k])  k1 = k;
      else  {
        k1 = k;
        k2 = k1+1;
      }
    } while (k2-k1>1);

    if (dist2[n]>data[k2])       k = k2;
    else if (dist2[n]<data[k1])  k = k1-1;
                           else  k = k1;
                          
    if (k>=m)  {
      k1 = k-m+1;
      memmove ( &(data[m-1]),&(data[m]),k1*sizeof(mmdb::realtype) );
      memmove ( &(kix [m-1]),&(kix [m]),k1*sizeof(int) );
    }
    data[k] = dist2[n];
    kix [k] = ix   [n];

  }

  memmove ( dist2,data,nPairs*sizeof(mmdb::realtype) );
  memmove ( ix   ,kix ,nPairs*sizeof(int) );

}

