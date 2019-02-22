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
//    06.05.16   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  Gesamt <implementation>
//       ~~~~~~~~~
//  **** Project :  GESAMT Common Definitions
//       ~~~~~~~~~
//  **** Classes :
//       ~~~~~~~~~
//
// =================================================================
//
//  GESAMT: General and Efficient Structural Alignment of
//          Macromoecular Targets
//
//  (C) E. Krissinel, 2008-2016
//
// =================================================================
//

#ifndef __GSMT_Defs__
#define __GSMT_Defs__

// =================================================================

//#undef compile_for_ccp4

#define GESAMT_Version  "1.15"
#define GESAMT_Date     "25-Jan-2017"

// default parameter for the Q-score
#define QR0_default         3.0

// default parameter for the superposition sigma
#define sigma_default       0.0

// default segment length
#define seg_length_default  9

namespace gsmt {

  enum PERFORMANCE_CODE  {
    PERFORMANCE_Efficient = 1,
    PERFORMANCE_High
  };
  
  enum GSMT_CODE {

    GSMT_Ok = 0,          //  0
    
    // alignment errors
    GSMT_ShortStruct,     //  1
    GSMT_NoMinimalMatch,  //  2
    GSMT_SVDFail,         //  3
    GSMT_NoStructures,    //  4
    GSMT_IterLimit,       //  5
    GSMT_UnknownSupError,
    GSMT_NoAlignmentAchieved,
    GSMT_NoDomainFound,
    
    // archive errors
    GSMT_NoPDBStructures,
    GSMT_NoNewStructures,
    GSMT_CantWriteLog,
    GSMT_CantReadIndex,
    GSMT_CantWritePackage,
    GSMT_NumbersMismatch,
    GSMT_CantWriteIndex,
    GSMT_CantReadQuery,
    GSMT_Terminated,

    // model errors
    GSMT_NoSeqSheafs,
    
    // custom error offset
    GSMT_UserError

  };

}

#define  struct_pack_name_tempate   "gesamt.archive.struct.%03i.pack"
#define  seq_pack_name_tempate      "gesamt.archive.seq.%03i.pack"
#define  index_file_name            "gesamt.archive.index"
#define  pbar_len                   50
#define  __use_memory_pool


#endif // __GSMT_Defs__
