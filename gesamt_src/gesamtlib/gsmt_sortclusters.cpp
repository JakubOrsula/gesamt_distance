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
//    10.02.13   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  GSMT_SortClusters <implementation>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::SortClusters
//       ~~~~~~~~~
//
//  (C) E. Krissinel, 2008-2013
//
// =================================================================
//

#include "gsmt_sortclusters.h"

// =================================================================

/*
 *
gsmt::SortClusters::SortClusters ( mmdb::io::RPStream Object )
                  : mmdb::QuickSort(Object)  {}

int gsmt::SortClusters::Compare ( int i, int j )  {
// sort by decreasing cluster size
  if (((PPSegCluster)data)[i]->n<((PPSegCluster)data)[j]->n)
    return  1;
  if (((PPSegCluster)data)[i]->n>((PPSegCluster)data)[j]->n)
    return -1;
  return 0;
}

void gsmt::SortClusters::Swap ( int i, int j )  {
SegCluster *SC;
  SC = ((PPSegCluster)data)[i];
  ((PPSegCluster)data)[i] = ((PPSegCluster)data)[j];
  ((PPSegCluster)data)[j] = SC;
}

void gsmt::SortClusters::Sort ( PPSegCluster SegCluster,
                                int nClusters ) {
  mmdb::QuickSort::Sort ( SegCluster,nClusters );
}

*/

gsmt::SortClusters::SortClusters() : ShiftSort()  {}
gsmt::SortClusters::~SortClusters() {}

int gsmt::SortClusters::Compare ( void * di, void * dj )  {
// sort by decreasing cluster size
  if (PSegCluster(di)->n<PSegCluster(dj)->n) return  1;
  if (PSegCluster(di)->n>PSegCluster(dj)->n) return -1;
  return 0;
}

void gsmt::SortClusters::Sort ( PPSegCluster SegCluster,
                                int nClusters ) {
  ShiftSort::Sort ( (void**)SegCluster,nClusters );
}

