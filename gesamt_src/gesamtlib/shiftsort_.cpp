//  $Id: shiftsort_.cpp $
// =================================================================
//  This code is distributed under the terms and conditions of the
//  CCP4 Program Suite Licence Agreement as 'Part 2' (Annex 2)
//  software. A copy of the CCP4 licence can be obtained by writing
//  to CCP4, Research Complex at Harwell, Rutherford Appleton
//  Laboratory, Didcot OX11 0FA, UK, or from
//  http://www.ccp4.ac.uk/ccp4license.php.
//  =================================================================
//
//    04.01.16   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  shiftsort_  <implementation>
//       ~~~~~~~~~
//  **** Classes :  gsmt::ShiftSort  - abstract sorting
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2016
//
//  =================================================================
//

#include <string.h>

#include "shiftsort_.h"

namespace gsmt  {

  ShiftSort::ShiftSort()  {
    nDataAlloc = 0;
    data       = NULL;
  }

  ShiftSort::~ShiftSort()  {
    if (data)
      delete[] data;
  }

  int ShiftSort::Compare ( void * di, void * dj )  {
  // sorting integers by increasing
    if (long(di)<long(dj))       return -1;
    else if (long(di)>long(dj))  return  1;
                           else  return  0;
  }

  void ShiftSort::Sort ( void **d, int dataLen )  {
  int  n,m,k,k1,k2, cmp;

    if (dataLen<2)  return;
    
    if (dataLen>nDataAlloc)  {
      if (data)
        delete[] data;
      data = new void*[dataLen];
      nDataAlloc = dataLen;
    }

    k2 = dataLen - 1;
    k1 = dataLen - 2;
    if (Compare(d[0],d[1])<0)  {
      data[k2] = d[1];
      data[k1] = d[0];
    } else  {
      data[k2] = d[0];
      data[k1] = d[1];    
    }
   
    for (n=2;n<dataLen;n++)  {

      m  = dataLen-n;
      k1 = m;
      k2 = dataLen-1;
      do {
        k   = (k1+k2)/2;
        cmp = Compare ( d[n],data[k] );
        if (cmp<0)       k2 = k;
        else if (cmp>0)  k1 = k;
      } while (cmp && (k2-k1>1));

      if (cmp>0)  {
        cmp = Compare ( d[n],data[k2] );
        if (cmp>0)  k = k2;
              else  k = k1;
      } else if (cmp<0)  {
        cmp = Compare ( d[n],data[k1] );
        if (cmp<0)  k = k1-1;
              else  k = k1;
      }

      if (k>=m)
        memmove ( &(data[m-1]),&(data[m]),sizeof(void*)*(k-m+1) );
      data[k] = d[n];

    }

    memmove ( d,data,dataLen*sizeof(void*) );
    
  }

/*
#include <stdio.h>

  void testShiftSort()  {
  ShiftSort shiftSort;
  long      d[10];
  
    d[0] =  0;
    d[1] = -1;
    d[2] =  2;
    d[3] = -3;
    d[4] = -5;
    d[5] =  5;
    d[6] =  4;
    d[7] =  8;
    d[8] =  7;
    d[9] = -6;

    for (int i=0;i<10;i++)
      d[i] = 10 - 2*i;

    for (int i=0;i<10;i++)
      printf ( " %2li",d[i] );
    printf ( "\n" );
    
    shiftSort.Sort ( (void**)d,10 );
    
    for (int i=0;i<10;i++)
      printf ( " %2li",d[i] );
    printf ( "\n" );

  }
*/

}

