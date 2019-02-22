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
//    04.03.17   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  Gesamt <interface>
//       ~~~~~~~~~
//  **** Project :  GESAMT General Gesamt Utilities
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

#include "gesamtlib/gsmt_mastructure.h"
#include "gesamtlib/gsmt_domdata.h"
#include "gesamt_input.h"

#define rvapi_tab_id            "report_page"
#define rvapi_tab_name          "Results"
#define rvapi_panel_id          "report_panel"
#define rvapi_struct_tbl_id     "structure_table"
#define rvapi_summary_tbl_id    "summary_table"
#define rvapi_resalign_sec_id   "resalign_sec"
#define rvapi_resalign_tbl_id   "resalign_table"
#define rvapi_consensus_sec_id  "consensus_sec"
#define rvapi_domain0_tbl_id    "domain0_table"
#define rvapi_domains_tbl_id    "domains_table"
#define rvapi_res_graph_id      "res_graph"

extern mmdb::ERROR_CODE readStructures ( gsmt::PPStructure   & M,
                                         gsmt::RInput      Input );

extern mmdb::ERROR_CODE readStructures ( gsmt::PPMAStructure & M,
                                         gsmt::RInput      Input );

extern void print_matrix ( mmdb::cpstr name, mmdb::rmatrix m,
                           mmdb::cpstr fmt,  gsmt::PPStructure M,
                           int nStructures );

extern void print_matrix ( mmdb::cpstr name, mmdb::rmatrix m,
                           mmdb::cpstr fmt,  gsmt::PPMAStructure M,
                           int nStructures );

extern void write_matrix ( mmdb::io::RFile f,
                           mmdb::cpstr name, mmdb::rmatrix m,
                           mmdb::cpstr fmt,  gsmt::PPStructure M,
                           int nStructures );

extern void write_matrix ( mmdb::io::RFile f,
                           mmdb::cpstr name, mmdb::rmatrix m,
                           mmdb::cpstr fmt,  gsmt::PPMAStructure M,
                           int nStructures );

extern void write_matrix_csv ( mmdb::io::RFile f,
                               mmdb::cpstr name, mmdb::rmatrix m,
                               mmdb::cpstr fmt,  gsmt::PPStructure M,
                               int nStructures );

extern void write_matrix_csv ( mmdb::io::RFile f,
                               mmdb::cpstr name, mmdb::rmatrix m,
                               mmdb::cpstr fmt,  gsmt::PPMAStructure M,
                               int nStructures );

extern void printInputParameters   ( gsmt::RInput Input );

extern void printHorzLine          ( char C, int len );

extern void printSuperpositionData2 ( mmdb::cpstr       title,
                                      gsmt::PSuperposition SD,
                                      char sep, int printWidth,
                                      int qSize, int tSize );

extern void printSuperpositionData2 ( mmdb::cpstr    title,
                                      gsmt::PDomData DD,
                                      char sep, int printWidth );

extern void printSuperpositionData2_rvapi ( mmdb::cpstr    title,
                                            gsmt::PDomData DD );

extern void makeFileName ( mmdb::pstr & newFName,
                           mmdb::cpstr  oldFName,
                           mmdb::cpstr  preSuffix,
                           int          mod );
 
extern int  initRVAPIDomOutput ( gsmt::RInput Input );

extern void makeScoreMatrix_rvapi ( mmdb::cpstr  widgetId,
                                    mmdb::cpstr    gridId,
                                    int               row,
                                    int               col,
                                    int           rowSpan,
                                    int           colSpan,
                                    int         openState,
                                    mmdb::cpstr      name,
                                    mmdb::rmatrix       m,
                                    mmdb::cpstr       fmt,
                                    gsmt::PPMAStructure M,
                                    int           nStruct );
