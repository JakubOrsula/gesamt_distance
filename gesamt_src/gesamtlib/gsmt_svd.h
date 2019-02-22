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
//  **** Module  :  GSMT_SVD <interface>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::SVD
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2008-2015
//
// =================================================================
//

#ifndef __GSMT_SVD__
#define __GSMT_SVD__

#include "mmdb2/mmdb_defs.h"

namespace gsmt  {

  extern void SVD3 ( mmdb::mat33 & A,  mmdb::mat33 & U, mmdb::mat33 & V,
                     mmdb::vect3 & W,  mmdb::vect3 & RV1,
                     int & RetCode );
  //
  //      13.12.01  <--  Last Modification Date
  //                    ------------------------
  //
  // ================================================
  //
  //         The    Singular Value Decomposition
  //    of the matrix  A  by the algorithm from
  //      G.Forsait, M.Malkolm, K.Mouler.  Numerical
  //    methods of mathematical calculations
  //    M., Mir, 1980.
  //
  //         Matrix  A  is represented as
  //
  //         A  =  U * W * VT
  //
  // ------------------------------------------------
  //
  //  All dimensions are indexed from 1 on.
  //
  // ------------------------------------------------
  //
  //         Input  parameters:
  //
  //     A   -   matrix [0..2][0..2] to be decomposed.
  //             The matrix does not change
  //     U    - should be always supplied as an array of
  //            [0..2][0..2] .
  //     V    - should be suuplied as an array of
  //            [0..2][0..2].
  //
  // ------------------------------------------------
  //
  //          Output parameters  are  :
  //
  //     W   -   3 non-ordered singular values,
  //           if  RetCode=0. If RetCode<>0, the
  //           RetCode+1 ... 3-th values are still
  //           valid
  //     U   -   matrix of right singular vectors
  //           (arranged in columns),  corresponding
  //           to the singular values in  W,  if
  //           RetCode=0. If RetCode<>0 then
  //           the  RetCode+1 ... 3-th vectors
  //           are  valid
  //     V   -   matrix of left singular vectors
  //           (arranged in columns),  corresponding
  //           to the singular values in  W,  if
  //           RetCode=0. If RetCode<>0 then the
  //           RetCode+1 ... 3-th vectors are valid
  //     RetCode - the error key :
  //            = 0   <=>   O'K
  //              else
  //            = k, if the k-th singular value
  //                 was not computed after 30 iterations.
  //
  // ------------------------------------------------
  //
  //          Key  Variables  are  :
  //
  //     ItnLimit  -  the limit for iterations
  //
  //     This routine does not use any machine-dependent
  //  constants.
  //
  // ================================================
  //
  //

  extern void  OrderSVD3 ( mmdb::mat33 & U, mmdb::mat33 & V,
                           mmdb::vect3 & W );

}

#endif
