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

#include <stdio.h>
#include <math.h>

#include "gsmt_svd.h"


//  ----------------------------------------------------

inline mmdb::realtype Sign ( mmdb::realtype A, mmdb::realtype B )  {
  if (B>=0.0)  return  A;
         else  return -A;
}

inline mmdb::realtype SrX2Y2 ( mmdb::realtype X, mmdb::realtype Y )  {
mmdb::realtype Ax,Ay;
  Ax = fabs(X);
  Ay = fabs(Y);
  if (Ay>Ax)   return  Ay*sqrt((X*X)/(Y*Y)+1.0);
  if (Ay==Ax)  return  Ax*sqrt(2.0);
  return  Ax*sqrt((Y*Y)/(X*X)+1.0);
}


//  ----------------------------------------------------

void gsmt::SVD3 ( mmdb::mat33 & A,  mmdb::mat33 & U, mmdb::mat33 & V,
                  mmdb::vect3 & W,  mmdb::vect3 & RV1,
                  int & RetCode )  {
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
mmdb::realtype C,G,F,X,S,H,Y,Z,Scale,ANorm,GG;
int            l,k1,l1,its,ExitKey;
int            ItnLimit=300;

  l1      = 0;  // this is to keep compiler happy
  RetCode = 0;

  for (int i=0;i<3;i++)
    for (int j=0;j<3;j++)
      U[i][j] = A[i][j];

  G     = 0.0;
  Scale = 0.0;
  ANorm = 0.0;

  for (int i=0;i<3;i++)  {
    l      = i+1;
    RV1[i] = Scale*G;
    G      = 0.0;
    S      = 0.0;
    Scale  = 0.0;
    for (int k=i;k<3;k++)
      Scale += fabs(U[k][i]);
    if (Scale!=0.0)  {
      for (int k=i;k<3;k++)  {
        U[k][i] /= Scale;
        S       += U[k][i]*U[k][i];
      }
      F = U[i][i];
      G = -Sign(sqrt(S),F);
      H = F*G-S;
      U[i][i] = F-G;
      if (i!=2)  {
        for (int j=l;j<3;j++)  {
          S = 0.0;            
          for (int k=i;k<3;k++)
            S += U[k][i]*U[k][j];
          F = S/H;
          for (int k=i;k<3;k++)
            U[k][j] += F*U[k][i];
        }
      }
      for (int k=i;k<3;k++)
        U[k][i] *= Scale;
    }

    W[i]  = Scale*G;
    G     = 0.0;
    S     = 0.0;
    Scale = 0.0;

    if (i<2)  {
      for (int k=l;k<3;k++)
        Scale += fabs(U[i][k]);
      if (Scale!=0.0)  {
        for (int k=l;k<3;k++)  {
          U[i][k] /= Scale;
          S       += U[i][k]*U[i][k];
        }
        F = U[i][l];
        G = -Sign(sqrt(S),F);
        H = F*G-S;
        U[i][l] = F-G;
        for (int k=l;k<3;k++)
          RV1[k] = U[i][k]/H;
        if (i!=2)
          for (int j=l;j<3;j++)  {
            S = 0.0;
            for (int k=l;k<3;k++)
              S += U[j][k]*U[i][k];
            for (int k=l;k<3;k++)
              U[j][k] += S*RV1[k];
          }
        for (int k=l;k<3;k++)
          U[i][k] *= Scale;
      }
    }

    ANorm = mmdb::RMax( ANorm,fabs(W[i])+fabs(RV1[i]) );

  }

  //   Accumulation of the right-hand transformations

  for (int i=2;i>=0;i--)  {
    if (i!=2)  {
      if (G!=0.0)  {
        for (int j=l;j<3;j++)
          V[j][i] = (U[i][j]/U[i][l]) / G;
        for (int j=l;j<3;j++)  {
          S = 0.0;
          for (int k=l;k<3;k++)
            S += U[i][k]*V[k][j];
          for (int k=l;k<3;k++)
            V[k][j] += S*V[k][i];
        }
      }
      for (int j=l;j<3;j++)  {
        V[i][j] = 0.0;
        V[j][i] = 0.0;
      }
    }

    V[i][i] = 1.0;
    G       = RV1[i];
    l       = i;

  }


  //   Accumulation of the left-hand transformations

  for (int i=2;i>=0;i--)  {
    l = i+1;
    G = W[i];
    if (i!=2)
      for (int j=l;j<3;j++)
        U[i][j] = 0.0;
    if (G!=0.0)  {
      if (i!=2)
        for (int j=l;j<3;j++)  {
          S = 0.0;
          for (int k=l;k<3;k++)
            S += U[k][i]*U[k][j];
          F = (S/U[i][i]) / G;
          for (int k=i;k<3;k++)
            U[k][j] += F*U[k][i];
        }
      for (int j=i;j<3;j++)
        U[j][i] /= G;
    } else
      for (int j=i;j<3;j++)
        U[j][i] = 0.0;

    U[i][i] += 1.0;

  }

  //   Diagonalization of the two-diagonal form.

  for (int k=2;k>=0;k--)  {
    k1  = k-1;
    its = 0;

    do  {
      ExitKey  = 0;
      l        = k+1;
      while ((ExitKey==0) && (l>0))  {
        l--;
        l1 = l-1;
        if (fabs(RV1[l])+ANorm==ANorm)   ExitKey=1;
        else if (l1>=0)  {
          if (fabs(W[l1])+ANorm==ANorm)  ExitKey=2;
        }
      }

//      if (ExitKey!=1)  {  <-- this is original statement
      if (ExitKey>1)  {  // <-- prevents from corruption due to l1<1.
                         // This is a rare case as RV1[1] should be
                         // always 0.0 . Apparently this logics is
                         // on the edge of float-point arithmetic,
                         // therefore extra precaution for the case
                         // of l1<1 was found necessary.
        C       = 0.0;
        S       = 1.0;
        ExitKey = 0;
        int i   = l;
        while ((ExitKey==0) && (i<=k))  {
          F       =  S*RV1[i];
          RV1[i]  =  C*RV1[i];
          if (fabs(F)+ANorm==ANorm)  ExitKey = 1;
          else  {
            G = W[i];
            H = SrX2Y2(F,G);
            W[i] = H;
            C = G/H;
            S = -F/H;
            for (int j=0;j<3;j++)  {
              Y         =  U[j][l1];
              Z         =  U[j][i];
              U[j][l1]  =  Y*C+Z*S;
              U[j][i]   =  -Y*S+Z*C;
            }
            i++;
          }
        }
      }

      //    Convergence  Checking

      Z = W[k];
      if (l!=k)  {
        if (its>=ItnLimit)  {
          RetCode = k;
          return;
        }
        its++;
        X  =  W[l];
        Y  =  W[k1];
        G  =  RV1[k1];
        H  =  RV1[k];
        F  =  ((Y-Z)*(Y+Z) + (G-H)*(G+H)) / ( 2.0*H*Y );
        if (fabs(F)<=1.0)  GG = Sign(sqrt(F*F+1.0),F);
                     else  GG = F*sqrt(1.0+1.0/F/F);
        F  =  ((X-Z)*(X+Z) + H*(Y/(F+GG)-H)) / X;

        //   Next  QR - Transformation

        C  =  1.0;
        S  =  1.0;
        for (int i1=l;i1<=k1;i1++)  {
          int i = i1+1;
          G = RV1[i];
          Y = W[i];
          H = S*G;
          G = C*G;
          Z = SrX2Y2(F,H);
          RV1[i1] = Z;
          C = F/Z;
          S = H/Z;
          F = X*C+G*S;
          G = -X*S+G*C;
          H = Y*S;
          Y = Y*C;
          for (int j=0;j<3;j++)  {
            X        = V[j][i1];
            Z        = V[j][i];
            V[j][i1] = X*C+Z*S;
            V[j][i]  = -X*S+Z*C;
          }

          Z = SrX2Y2(F,H);
          W[i1] = Z;
          if (Z!=0.0)  {
            C = F/Z;
            S = H/Z;
          }
          F = C*G+S*Y;
          X = -S*G+C*Y;
          for (int j=0;j<3;j++)  {
            Y        = U[j][i1];
            Z        = U[j][i];
            U[j][i1] = Y*C+Z*S;
            U[j][i]  = -Y*S+Z*C;
          }

        }

        RV1[l] = 0.0;
        RV1[k] = F;
        W[k]   = X;

      } else if (Z<0.0)  {

        W[k] = -Z;
        for (int j=0;j<3;j++)
          V[j][k] = -V[j][k];
      }

    } while (l!=k);

  }

}


//  -----------------------------------------------------

void gsmt::OrderSVD3 ( mmdb::mat33 & U, mmdb::mat33 & V,
                       mmdb::vect3 & W )  {
mmdb::realtype P;
int            i,k,j;

  //  External loop of the re-ordering
  for (i=0;i<2;i++)  {
    k = i;
    P = W[i];

    //  Internal loop :  finding of the index of greatest
    //  singular value over the remaining ones.
    for (j=i+1;j<3;j++)
      if (W[j]>P)  {
        k = j;
        P = W[j];
      }

    if (k!=i)  {
      //  Swapping the singular value
      W[k] = W[i];
      W[i] = P;
      //  Swapping the U's columns (  if  needed  )
      for (j=0;j<3;j++)  {
        P       = U[j][i];
        U[j][i] = U[j][k];
        U[j][k] = P;
      }
      //  Swapping the V's columns ( if  needed )
      for (j=0;j<3;j++)  {
        P       = V[j][i];
        V[j][i] = V[j][k];
        V[j][k] = P;
      }
    }

  }

}


//  ----------------------------------------------------
/*
void gsmt::SVD3 ( mmdb::mat44 & A,  mmdb::mat44 & U, mmdb::mat44 & V,
                  mmdb::vect4 & W,  mmdb::vect4 & RV1,
                  int & RetCode )  {
mmdb::mat33 aa,uu,vv;
mmdb::vect3 ww,rr;

   for (int i=0;i<3;i++)
     for (int j=0;j<3;j++)
       aa[i][j] = A[i+1][j+1];

   SVD_3 ( aa,uu,vv,ww,rr,RetCode );

   for (int i=0;i<3;i++)  {
     for (int j=0;j<3;j++)  {
       U[i+1][j+1] = uu[i][j];
       V[i+1][j+1] = vv[i][j];
     }
     W  [i+1] = ww[i];
     RV1[i+1] = rr[i];
   }

}

*/
