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
//    25.01.17   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  GSMT_Aligner <implementation>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::Aligner
//       ~~~~~~~~~
//
//  (C) E. Krissinel, 2008-2017
//
// =================================================================
//

#include <string.h>
#include <time.h>

#include "rvapi/rvapi_interface.h"

#include "gsmt_aligner.h"
#include "gsmt_output.h"

// =================================================================

gsmt::Aligner::Aligner() : Refiner()  {
  InitAligner();
}

gsmt::Aligner::Aligner ( mmdb::io::RPStream Object ) : Refiner(Object) {
  InitAligner();
}

gsmt::Aligner::~Aligner()  {
  FreeMemory();
}

void gsmt::Aligner::InitAligner()  {
}

void gsmt::Aligner::FreeMemory()  {
}

void gsmt::Aligner::setPerformanceLevel (
                                      PERFORMANCE_CODE performance )  {
  switch (performance)  {
    default :
    case PERFORMANCE_Efficient :  minSegLen   = 9;
                                  segTol      = 2.25;
                                  clustTol2   = 16.0;
                                  filterTol2  = 4.0*clustTol2;
                                  maxContact  = 7.0;
                                  iterMax     = 30;
                                  refineDepth = 0.7;
                               break;
    case PERFORMANCE_High      :  minSegLen   = 5;
                                  segTol      = 7.00;
                                  clustTol2   = 16.0;
                                  filterTol2  = 4.0*clustTol2;
                                  maxContact  = 7.0;
                                  iterMax     = 30;
                                  refineDepth = 0.4;
  }

}

void gsmt::Aligner::setSimilarityThresholds (
                                         mmdb::realtype minPart1,
                                         mmdb::realtype minPart2 )  {
  minMatch1 = mmdb::RMin ( 1.0,fabs(minPart1) );
  minMatch2 = mmdb::RMin ( 1.0,fabs(minPart2) );
}


gsmt::GSMT_CODE gsmt::Aligner::Align ( PStructure s1, PStructure s2,
                                       bool measure_cpu )  {
mmdb::realtype clapse1,clapse2;
clock_t        ct,ct1;
GSMT_CODE      rc;

  clapse1 = 0.0;
  clapse2 = 0.0;
  if (measure_cpu) ct = clock();
              else ct = 0;

  rc = makeSegClusters ( s1,s2 );
  
  if (measure_cpu)  {
    ct1     = clock();
    clapse1 = mmdb::realtype(ct1-ct)/CLOCKS_PER_SEC;
    ct      = ct1;
  }

  if (rc==GSMT_Ok)  {

    Refine ( s1,s2 );

    if (measure_cpu)  {
      ct1     = clock();
      clapse2 = mmdb::realtype(ct1-ct)/CLOCKS_PER_SEC;
      ct      = ct1;
    }

  }

  if (measure_cpu)
    printf ( "\n"
           " CPU stage 1 (clustering):  %8.5f secs\n"
           " CPU stage 2 (refinement):  %8.5f secs\n",
           clapse1,clapse2 );

  return rc;

}


gsmt::PSuperposition gsmt::Aligner::getMatch ( int matchNo )  {

  if (!Cluster)            return NULL;
  if (matchNo>=nClusters)  return NULL;
  if (!Cluster[matchNo])   return NULL;

  if (Cluster[matchNo]->SD)  {
    Cluster[matchNo]->SD->natoms1 = natoms1;
    Cluster[matchNo]->SD->natoms2 = natoms2;
  }
  return Cluster[matchNo]->SD;

}


void gsmt::Aligner::getBestMatch ( RPSuperposition SD, int & matchNo ) {
mmdb::realtype  Q;
int             i;

  SD      = NULL;
  matchNo = -1;
  Q       = -1.0;
  for (i=0;i<nClusters;i++)
    if (Cluster[i]->SD)  {
      if (Cluster[i]->SD->Q>Q)  {
        Q       = Cluster[i]->SD->Q;
        matchNo = i;
        SD      = Cluster[i]->SD;
      }
    }

  if (SD)  {
    SD->natoms1 = natoms1;
    SD->natoms2 = natoms2;
  }

}

void gsmt::Aligner::getSeedAlignment ( int matchNo, mmdb::ivector & ac1,
                                       mmdb::ivector & ac2 ) {
  mmdb::FreeVectorMemory ( ac1,0 );
  mmdb::FreeVectorMemory ( ac2,0 );
  if (matchNo<nClusters)  {
    mmdb::GetVectorMemory ( ac1,natoms1,0 );
    mmdb::GetVectorMemory ( ac2,natoms2,0 );
    Cluster[matchNo]->GetAlignment ( ac1,natoms1,ac2,natoms2 );
  }
}

void gsmt::Aligner::read  ( mmdb::io::RFile f )  {
  Refiner::read ( f );
}

void gsmt::Aligner::write ( mmdb::io::RFile f )  {
  Refiner::write ( f );
}


void gsmt::Aligner::writeAlignTable ( mmdb::io::RFile f,
                                      PSuperposition  SD )  {
XAlignText CXA;
PXTAlign   XTA;
int        nr,j;

  CXA.Align ( A1,SD->c1,natoms1,
              A2,SD->c2,natoms2,
              SD->dist1,nr );
  f.LF();
  f.WriteLine ( ".-------------.------------.-------------." );
  f.WriteLine ( "|   FIXED     |  Dist.(A)  |   MOVING    |" );
  f.WriteLine ( "|-------------+------------+-------------|" );
  XTA = CXA.getTextRows();
  for (j=0;j<nr;j++)
    XTA[j].print ( f );
  f.WriteLine ( "`-------------'------------'-------------'" );
  f.LF();
  f.WriteLine ( " Notations:" );
  f.WriteLine ( " S/H   residue belongs to a strand/helix" );
  f.WriteLine ( " +/-/. hydrophylic/hydrophobic/neutral residue" );
  f.WriteLine ( " **    identical residues matched: similarity 5" );
  f.WriteLine ( " ++    similarity 4" );
  f.WriteLine ( " ==    similarity 3" );
  f.WriteLine ( " --    similarity 2" );
  f.WriteLine ( " ::    similarity 1" );
  f.WriteLine ( " ..    dissimilar residues: similarity 0" );

}


void gsmt::Aligner::makeAlignTable_rvapi ( mmdb::cpstr tableId,
                                           PSuperposition  SD )  {
// Puts pairwise residue alignment in the pre-existing table with
// given table Id
XAlignText CXA;
PXTAlign   XTA;
int        nr,j;

  CXA.Align ( A1,SD->c1,natoms1,
              A2,SD->c2,natoms2,
              SD->dist1,nr );
  XTA = CXA.getTextRows();
  
  rvapi_put_horz_theader ( tableId,"FIXED","Residues of fixed molecule",
                           0 );
  rvapi_put_horz_theader ( tableId,"Dist. (&Aring;)",
                           "DIstance between aligned C-alpha atoms",1 );
  rvapi_put_horz_theader ( tableId,"MOVING",
                           "Residues of moving (rotated-translated) "
                           "molecule",2 );
  for (j=0;j<nr;j++)
    XTA[j].put_rvapi ( tableId,j,false );
  
}


void gsmt::Aligner::makeAlignGraph_rvapi ( mmdb::cpstr graphId,
                                           PSuperposition  SD )  {
// Puts alignment distance profile in graph with given graph id
XAlignText CXA;
PXTAlign   XTA;
int        nr,j;

  CXA.Align ( A1,SD->c1,natoms1,
              A2,SD->c2,natoms2,
              SD->dist1,nr );
  XTA = CXA.getTextRows();

  rvapi_add_graph_data    ( "pwal_data",graphId,"R.m.s.d profile" );
  rvapi_add_graph_dataset ( "pos","pwal_data",graphId,
                            "Alignment position","Alignment position" );
  rvapi_add_graph_dataset ( "rmsd","pwal_data",graphId,
                            "Rigid-body r.m.s.d.","r.m.s.d." );

  for (j=0;j<nr;j++)
    if (XTA[j].alignKey==0)  {
      rvapi_add_graph_int  ( "pos" ,"pwal_data",graphId,j );
      rvapi_add_graph_real ( "rmsd","pwal_data",graphId,XTA[j].dist,"%g" );
    }

  rvapi_add_graph_plot ( "pwal_dist_plot",graphId,
                         "Distance profile of alignment",
                         "Alignment position",
                         "R.m.s.d."
                       );

  rvapi_add_plot_line ( "pwal_dist_plot","pwal_data",graphId,"pos","rmsd" );
  
  rvapi_set_line_options ( "rmsd","pwal_dist_plot","pwal_data",graphId,
                           RVAPI_COLOR_DarkBlue,RVAPI_LINE_Off,
                           RVAPI_MARKER_filledCircle,2.5,true );
  rvapi_set_plot_legend  ( "pwal_dist_plot",graphId,
                           RVAPI_LEGEND_LOC_N,
                           RVAPI_LEGEND_PLACE_Inside );
 
}


void gsmt::Aligner::writeAlignTable_csv ( mmdb::io::RFile f,
                                          PSuperposition  SD )  {
XAlignText CXA;
PXTAlign   XTA;
char       S[1000];
int        nr,j;

  CXA.Align ( A1,SD->c1,natoms1,
              A2,SD->c2,natoms2,
              SD->dist1,nr );
  XTA = CXA.getTextRows();
  XTAlign::csv_title ( S,", " );
  f.WriteLine ( S );
  for (j=0;j<nr;j++)  {
    XTA[j].write_csv ( S,", " );
    f.WriteLine ( S );
  }

}

void gsmt::Aligner::addResidueAlignmentJSON ( gsmt::RJSON json,
                                              PSuperposition SD ) {
XAlignText CXA;
PXTAlign   XTA;
int        nr,j;

  CXA.Align ( A1,SD->c1,natoms1,
              A2,SD->c2,natoms2,
              SD->dist1,nr );
  XTA = CXA.getTextRows();
  for (j=0;j<nr;j++)
    json.addJSON ( "alignment",XTA[j].getJSON() );

}


void gsmt::Aligner::writeSeqAlignment ( mmdb::io::RFile f,
                                        mmdb::cpstr     title1,
                                        mmdb::cpstr     title2,
                                        PSuperposition  SD )  {
XAlignText CXA;
mmdb::pstr S1,S2;
int        nr;

  CXA.Align ( A1,SD->c1,natoms1,
              A2,SD->c2,natoms2,
              SD->dist1,nr );

  S1 = NULL;
  S2 = NULL;
  CXA.getAlignments ( S1,S2 );
  if (S1 && S2)  {
    f.Write     ( ">"    );
    f.WriteLine ( title1 );
    f.WriteLine ( S1     );
    f.LF();
    f.Write     ( ">"    );
    f.WriteLine ( title2 );
    f.WriteLine ( S2     );
  }

  if (S1)  delete[] S1;
  if (S2)  delete[] S2;

}

void gsmt::Aligner::getSeqAlignment ( mmdb::pstr    & qSeq,
                                      mmdb::pstr    & tSeq,
                                      mmdb::rvector * dist,
                                      PSuperposition  SD )  {
XAlignText CXA;
int        nr;

  CXA.Align ( A1,SD->c1,natoms1,
              A2,SD->c2,natoms2,
              SD->dist1,nr );

  CXA.getAlignments ( qSeq,tSeq,dist );
                                      
}
