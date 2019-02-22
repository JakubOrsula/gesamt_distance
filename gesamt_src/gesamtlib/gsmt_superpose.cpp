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
//    28.01.16   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  GSMT_Superpose <implementation>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Functions :  gsmt::superpose
//       ~~~~~~~~~~~
//
//  (C) E. Krissinel, 2008-2016
//
// =================================================================
//

#include "mmdb2/mmdb_atom.h"
#include "gsmt_superpose.h"
#include "gsmt_svd.h"

// =================================================================

gsmt::SUP_RC gsmt::superpose ( mmdb::PPAtom  a1,
                               mmdb::PPAtom  a2,
                               mmdb::ovector mask,
                               int           natoms,
                               mmdb::mat44 & T )  {
mmdb::mat33    svd_A;
mmdb::vect3    vc1,vc2;
mmdb::vect3    cm1,cm2;
int            i,j,k;

  //  1.  Calculate mass centers

  for (i=0;i<3;i++)  {
    cm1[i] = 0.0;
    cm2[i] = 0.0;
  }
  
  k = 0;
  for (i=0;i<natoms;i++)
    if (mask[i])  {
      cm1[0] += a1[i]->x;
      cm1[1] += a1[i]->y;
      cm1[2] += a1[i]->z;
      cm2[0] += a2[i]->x;
      cm2[1] += a2[i]->y;
      cm2[2] += a2[i]->z;
      k++;
    }
    
  if (k<=2)  {
    mmdb::Mat4Init ( T );
    return SUP_Empty;
  }
    
  for (i=0;i<3;i++)  {
    cm1[i] /= k;
    cm2[i] /= k;
  }

  //  2.  Calculate the correlation matrix

  for (i=0;i<3;i++)
    for (j=0;j<3;j++)
      svd_A[i][j] = 0.0;

  for (i=0;i<natoms;i++)
    if (mask[i])  {
      vc1[0] = a1[i]->x - cm1[0];
      vc1[1] = a1[i]->y - cm1[1];
      vc1[2] = a1[i]->z - cm1[2];
      vc2[0] = a2[i]->x - cm2[0];
      vc2[1] = a2[i]->y - cm2[1];
      vc2[2] = a2[i]->z - cm2[2];
      for (j=0;j<3;j++)
        for (k=0;k<3;k++)
          svd_A[j][k] += vc1[k]*vc2[j];
    }

  return calcRTMat ( svd_A,cm1,cm2,T );

}


gsmt::SUP_RC gsmt::superpose ( mmdb::rvector x1,
                               mmdb::rvector y1,
                               mmdb::rvector z1,
                               mmdb::PPAtom  a2,
                               mmdb::ovector mask,
                               int           natoms,
                               mmdb::mat44 & T )  {
mmdb::mat33    svd_A;
mmdb::vect3    vc1,vc2;
mmdb::vect3    cm1,cm2;
int            i,j,k;

  //  1.  Calculate mass centers

  for (i=0;i<3;i++)  {
    cm1[i] = 0.0;
    cm2[i] = 0.0;
  }
  
  k = 0;
  for (i=0;i<natoms;i++)
    if (mask[i])  {
      cm1[0] += x1[i];
      cm1[1] += y1[i];
      cm1[2] += z1[i];
      cm2[0] += a2[i]->x;
      cm2[1] += a2[i]->y;
      cm2[2] += a2[i]->z;
      k++;
    }
    
  if (k<=0)  {
    mmdb::Mat4Init ( T );
    return SUP_Empty;
  }
    
  for (i=0;i<3;i++)  {
    cm1[i] /= k;
    cm2[i] /= k;
  }

  //  2.  Calculate the correlation matrix

  for (i=0;i<3;i++)
    for (j=0;j<3;j++)
      svd_A[i][j] = 0.0;

  for (i=0;i<natoms;i++)
    if (mask[i])  {
      vc1[0] = x1[i] - cm1[0];
      vc1[1] = y1[i] - cm1[1];
      vc1[2] = z1[i] - cm1[2];
      vc2[0] = a2[i]->x - cm2[0];
      vc2[1] = a2[i]->y - cm2[1];
      vc2[2] = a2[i]->z - cm2[2];
      for (j=0;j<3;j++)
        for (k=0;k<3;k++)
          svd_A[j][k] += vc1[k]*vc2[j];
    }

  return calcRTMat ( svd_A,cm1,cm2,T );

}

gsmt::SUP_RC gsmt::calcRTMat ( mmdb::mat33 & svd_A,
                               mmdb::vect3 & cm1,
                               mmdb::vect3 & cm2,
                               mmdb::mat44 & T )  {
mmdb::mat33    svd_U,svd_V;
mmdb::vect3    svd_W,svd_RV1;
mmdb::realtype det, B;
int            i,j,k;

  det = svd_A[0][0]*svd_A[1][1]*svd_A[2][2] +
        svd_A[0][1]*svd_A[1][2]*svd_A[2][0] +
        svd_A[1][0]*svd_A[2][1]*svd_A[0][2] -
        svd_A[0][2]*svd_A[1][1]*svd_A[2][0] -
        svd_A[0][0]*svd_A[1][2]*svd_A[2][1] -
        svd_A[2][2]*svd_A[0][1]*svd_A[1][0];


  //  Calculate the transformation matrix
  //  (to be applied to 2nd structure)

  SVD3 ( svd_A,svd_U,svd_V,svd_W,svd_RV1,k );

  if (k)  {
    mmdb::Mat4Init ( T );
    return SUP_Fail;
  } else  {

    if (det<=0.0)  {
      k = -1;
      B = mmdb::MaxReal;
      for (i=0;i<3;i++)
        if (svd_W[i]<B)  {
          B = svd_W[i];
          k = i;
      }
      for (i=0;i<3;i++)
        svd_V[i][k] = -svd_V[i][k];
    }

    //  Calculate rotational part of T

    for (i=0;i<3;i++)
      for (j=0;j<3;j++)  {
        B = 0.0;
        for (k=0;k<3;k++)
          B += svd_U[i][k]*svd_V[j][k];
        T[j][i] = B;
      }

    //  Add translational part to T

    T[0][3] = cm1[0] - T[0][0]*cm2[0] - T[0][1]*cm2[1] - T[0][2]*cm2[2];
    T[1][3] = cm1[1] - T[1][0]*cm2[0] - T[1][1]*cm2[1] - T[1][2]*cm2[2];
    T[2][3] = cm1[2] - T[2][0]*cm2[0] - T[2][1]*cm2[1] - T[2][2]*cm2[2];

    T[3][0] = 0.0;  T[3][1] = 0.0;  T[3][2] = 0.0;  T[3][3] = 1.0;

  }
  
  return SUP_Ok;

}
