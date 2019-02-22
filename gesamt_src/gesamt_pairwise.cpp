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
//  **** Module  :  Gesamt <implementation>
//       ~~~~~~~~~
//  **** Project :  GESAMT Pairwise alignment driver
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

#include <string.h>

#include "gesamt_pairwise.h"
#include "gesamt_utils.h"
#include "gesamtlib/gsmt_utils.h"
#include "gesamtlib/gsmt_defs.h"
#include "gesamtlib/json_.h"
#include "rvapi/rvapi_interface.h"


// =================================================================

void calcCCP4rot ( const mmdb::mat44 & m,
                   mmdb::realtype    & om,
                   mmdb::realtype    & ph,
                   mmdb::realtype    & ka,
                   mmdb::realtype    & al,
                   mmdb::realtype    & be,
                   mmdb::realtype    & ga )  {
double w_, x_, y_, z_;
double d = 180.0/mmdb::Pi;
double tr = m[0][0] + m[1][1] + m[2][2] + 1.0;

  // check the diagonal
  if (tr>1.0e-8)  {
    double s( sqrt(tr) );
    w_ = s * 0.5;
    s = 0.5 / s;
    x_ = s * ( m[2][1] - m[1][2] );
    y_ = s * ( m[0][2] - m[2][0] );
    z_ = s * ( m[1][0] - m[0][1] );
  } else  {
    if ( m[0][0] > m[1][1] && m[0][0] > m[2][2] ) {
      double s( sqrt(1.0 + m[0][0] - m[1][1] - m[2][2] ) );
      x_ = 0.5 * s;
      if ( s != 0.0 ) s = 0.5 / s;
      w_ = s * ( m[2][1] - m[1][2] );
      y_ = s * ( m[0][1] + m[1][0] );
      z_ = s * ( m[0][2] + m[2][0] );
    } else if ( m[1][1] > m[2][2] ) {
      double s( sqrt(1.0 + m[1][1] - m[2][2] - m[0][0] ) );
      y_ = 0.5 * s;
      if ( s != 0.0 ) s = 0.5 / s;
      w_ = s * ( m[0][2] - m[2][0] );
      z_ = s * ( m[1][2] + m[2][1] );
      x_ = s * ( m[1][0] + m[0][1] );
    } else {
      double s( sqrt(1.0 + m[2][2] - m[0][0] - m[1][1] ) );
      z_ = 0.5 * s;
      if ( s != 0.0 ) s = 0.5 / s;
      w_ = s * ( m[1][0] - m[0][1] );
      x_ = s * ( m[2][0] + m[0][2] );
      y_ = s * ( m[2][1] + m[1][2] );
    }
  }
  om = ph = ka = 0.0;
  if ( fabs(w_) < 0.999999 ) {
    double r = sqrt( x_*x_ + y_*y_ );
    om = d*atan2( r, z_ );
    if ( r > 0.000001 ) ph = d*atan2( y_, x_ );
    ka = d*2.0*acos( w_ );
  }
  double ca, cb, cg, sa, sb, sg;
  cb = 1.0 - 2.0 * (x_*x_ + y_*y_);
  sb = 2.0 * sqrt( (x_*x_ + y_*y_) * (w_*w_ + z_*z_) );
  if ( sb > 0.0001 ) {
    ca = 2.0 * (x_*z_ + w_*y_);
    sa = 2.0 * (y_*z_ - w_*x_);
    cg = 2.0 * (w_*y_ - x_*z_);
    sg = 2.0 * (y_*z_ + w_*x_);
  } else {
    ca = 1.0;
    sa = 0.0;
    cg = cb;
    sg = 2.0*(y_*z_ + w_*x_);
  }
  al = d*atan2(sa,ca);
  be = d*atan2(sb,cb);
  ga = d*atan2(sg,cg);

}


void printccp4rot ( const mmdb::mat44 & m )  {
mmdb::realtype  om,ph,ka, al,be,ga;

  calcCCP4rot ( m,om,ph,ka,al,be,ga );

  printf ( "\n"
           " CCP4 format rotation-translation operator\n"
           " ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n" );
  printf ( " Polar angles (omega,phi,kappa)   : %12.5f %12.5f %12.5f\n",
           om, ph, ka );
  printf ( " Euler angles (alpha,beta,gamma)  : %12.5f %12.5f %12.5f\n",
           al, be, ga );
  printf ( " Orthogonal translation (Angstrom): %12.5f %12.5f %12.5f\n",
           m[0][3],m[1][3],m[2][3] );

}


void writeccp4rot_csv ( mmdb::io::RFile f, const mmdb::mat44 & m )  {
char L[1000];
mmdb::realtype  om,ph,ka, al,be,ga;

  calcCCP4rot ( m,om,ph,ka,al,be,ga );

  f.Write ( "\n"
            " CCP4 format rotation-translation operator\n" );
  sprintf ( L,
     " Polar angles (omega,phi,kappa), %14.7f, %14.7f, %14.7f\n"
     " Euler angles (alpha,beta,gamma), %14.7f, %14.7f, %14.7f\n"
     " Orthogonal translation (Angstrom), %14.7f, %14.7f, %14.7f\n",
     om, ph, ka, al, be, ga, m[0][3],m[1][3],m[2][3] );
     
  f.Write ( L );

}


void printFracAnalysis ( mmdb::mat44 & T, mmdb::cpstr name,
                         mmdb::PManager M )   {
mmdb::mat44 TF;

  if (M->CrystReady()!=mmdb::CRRDY_NoTransfMatrices)  {

    if (M->Orth2Frac(T,TF))
      printf ( "\n"
        " Transformation matrix in fractional coordinates of %s:\n\n"
        "          Rx           Ry           Rz             T\n"
        " %12.5f %12.5f %12.5f   %12.5f\n"
        " %12.5f %12.5f %12.5f   %12.5f\n"
        " %12.5f %12.5f %12.5f   %12.5f\n",
        name,
        TF[0][0],TF[0][1],TF[0][2],TF[0][3],
        TF[1][0],TF[1][1],TF[1][2],TF[1][3],
        TF[2][0],TF[2][1],TF[2][2],TF[2][3] );
    else
      printf ( "\n"
        " *** orthogonal-fractional transformations failed for structure\n"
        " %s\n",name );

  }

}


void writeFracAnalysis_csv ( mmdb::pstr S,
                             mmdb::mat44 & T, mmdb::cpstr name,
                             mmdb::PManager M )   {
mmdb::mat44 TF;

  if (M->CrystReady()!=mmdb::CRRDY_NoTransfMatrices)  {

    if (M->Orth2Frac(T,TF))
      sprintf ( S,"\n"
        "STRUCTURE, %s:\n\n"
        "Rx, Ry, Rz, T\n"
        "%14.7f, %14.7f, %14.7f, %14.7f\n"
        "%14.7f, %14.7f, %14.7f, %14.7f\n"
        "%14.7f, %14.7f, %14.7f, %14.7f\n",
        name,
        TF[0][0],TF[0][1],TF[0][2],TF[0][3],
        TF[1][0],TF[1][1],TF[1][2],TF[1][3],
        TF[2][0],TF[2][1],TF[2][2],TF[2][3] );
    else
      sprintf ( S,"\n"
        " *** orthogonal-fractional transformations failed for structure,"
        " %s\n",name );

  } else
    S[0] = char(0);

}


void writeLogFile ( gsmt::PPStructure    structs,
                    gsmt::PAligner       Aligner,
                    gsmt::PSuperposition SD,
                    bool                 isOutput,
                    int                  printWidth )  {
mmdb::io::File  f;
char            L1[2000],L2[200];
mmdb::realtype  x1,y1,z1;
mmdb::realtype  x2,y2,z2;
mmdb::realtype  dx,dy,dz,d;
mmdb::mat44     TF;
mmdb::mat33     vrm;
mmdb::realtype  alpha,vx,vy,vz;
bool            crystQ,crystT;

  crystQ = structs[0]->isCrystInfo();
  crystT = structs[1]->isCrystInfo();

  printStructureSummary ( structs,2,isOutput );
  
  printSuperpositionData2 ( " SUPERPOSITION\n"
                            " ~~~~~~~~~~~~~",
                            SD,'=',printWidth,-1,-1 );

  for (int i=0;i<3;i++)
    for (int j=0;j<3;j++)
      vrm[i][j] = SD->T[i][j];
 
  mmdb::GetRotParameters ( vrm,alpha,vx,vy,vz );
  printf (
    "\n"
    " Direction cosines of the rotation axis: %7.5f %7.5f %7.5f\n"
    " Rotation angle                        : %-10.5f\n",
    vx,vy,vz, alpha*180.0/mmdb::Pi
  );

  if (crystQ)  {

    if (structs[0]->getMMDBManager()->Orth2Frac(SD->T,TF))
      printf ( "\n"
          " in fractional coordinates of FIXED structure:\n\n"
          "          Rx           Ry           Rz             T\n"
          " %12.5f %12.5f %12.5f   %12.5f\n"
          " %12.5f %12.5f %12.5f   %12.5f\n"
          " %12.5f %12.5f %12.5f   %12.5f\n",
          TF[0][0],TF[0][1],TF[0][2],TF[0][3],
          TF[1][0],TF[1][1],TF[1][2],TF[1][3],
          TF[2][0],TF[2][1],TF[2][2],TF[2][3] );
    else
      printf ( "\n"
          " *** orthogonal-fractional transformations failed for "
          "Query structure\n" );

  }

  if (crystT)  {

    if (structs[1]->getMMDBManager()->Orth2Frac(SD->T,TF))
      printf ( "\n"
          " in fractional coordinates of MOVING structure:\n\n"
          "          Rx           Ry           Rz             T\n"
          " %12.5f %12.5f %12.5f   %12.5f\n"
          " %12.5f %12.5f %12.5f   %12.5f\n"
          " %12.5f %12.5f %12.5f   %12.5f\n",
          TF[0][0],TF[0][1],TF[0][2],TF[0][3],
          TF[1][0],TF[1][1],TF[1][2],TF[1][3],
          TF[2][0],TF[2][1],TF[2][2],TF[2][3] );
    else
      printf ( "\n"
          " *** orthogonal-fractional transformations failed for "
          "Target structure\n" );

  }
  
  if (structs[0]->getCentroidF(x1,y1,z1))
        sprintf ( L1,"%8.5f %8.5f %8.5f",x1,y1,z1 );
  else  strcpy  ( L1," -.--     -.--     -.--   " );
  structs[0]->getCentroid ( x1,y1,z1 );

  if (structs[1]->getCentroidF(x2,y2,z2))
        sprintf ( L2,"%8.5f %8.5f %8.5f",x2,y2,z2 );
  else  strcpy  ( L2," -.--     -.--     -.--   " );
  structs[1]->getCentroid ( x2,y2,z2 );

  printHorzLine ( '-',printWidth );
  printf (
    "\n"
    " CENTROIDS\n"
    " ~~~~~~~~~            Orthogonal                       Fractional\n"
    "               X          Y          Z            XF       YF       ZF\n"
    " %6s   %10.5f %10.5f %10.5f     %s\n"
    " %6s   %10.5f %10.5f %10.5f     %s\n",
    structs[0]->getRefName(),x1,y1,z1, L1,
    structs[1]->getRefName(),x2,y2,z2, L2
  );

  dx = x2-x1;
  dy = y2-y1;
  dz = z2-z1;
  d  = sqrt ( dx*dx + dy*dy + dz*dz );
  printf (
    "\n"
    " Distance between centroids                   : %-10.5f\n",d );
  if (d>0.0)  {
    dx /= d;
    dy /= d;
    dz /= d;
    printf (
      " Direction cosines of vector between centroids: %7.5f %7.5f %7.5f\n"
      " Angle between rotation axis and vector between centroids: %-9.5f\n",
      dx,dy,dz,acos(dx*vx+dy*vy+dz*vz)*180.0/mmdb::Pi );
  }

  printHorzLine ( '-',printWidth );

  printccp4rot ( SD->T );

  printHorzLine ( '=',printWidth );
  printf (
    "\n"
    " RESIDUE ALIGNMENT\n"
    " ~~~~~~~~~~~~~~~~~\n"
  );

#ifdef compile_for_ccp4
  printf ( "$$\n" );
#endif

  f.assign ( "stdout" );
  f.rewrite();
  Aligner->writeAlignTable ( f,SD );
  f.shut();

}

/*
void makeRVAPIOutput ( gsmt::RInput         Input,
                       gsmt::PPStructure    structs,
                       gsmt::PAligner       Aligner,
                       gsmt::PSuperposition SD,
                       bool                 isOutput )  {

  if (Input.rvapiRDir)
    rvapi_init_document ( "gesamt_report",Input.rvapiRDir,
                          "GESAMT - Pairwise Alignment",
                          RVAPI_MODE_Html,
                          RVAPI_LAYOUT_Tabs,
                          Input.rvapiJSUri,
                          NULL,NULL,NULL,NULL );

}
*/


void makeJSONOutput ( gsmt::RInput         Input,
                      gsmt::PPStructure    structs,
                      gsmt::PAligner       Aligner,
                      gsmt::PSuperposition SD,
                      bool                 isOutput )  {
gsmt::JSON  json;

  json.addValue ( "outputId","pairwise" );

  addStructureSummaryJSON ( json,structs,2,isOutput );

  json.addValue ( "qscore",SD->Q   ,"%.4f" );
  json.addValue ( "rmsd"  ,SD->rmsd,"%.4f" );
  json.addValue ( "nalign",SD->Nalgn       );
  
  Aligner->addResidueAlignmentJSON ( json,SD );

  if (Input.jsonCompact)
        json.write ( Input.jsonOutFile,0 );
  else  json.write ( Input.jsonOutFile,2 );

}


void writeCSVFile ( mmdb::io::RFile      f,
                    gsmt::PPStructure    structs,
                    gsmt::PAligner       Aligner,
                    gsmt::PSuperposition SD,
                    bool                 isOutput )  {
char            L1[2000],L2[200];
mmdb::realtype  x1,y1,z1;
mmdb::realtype  x2,y2,z2;
mmdb::realtype  dx,dy,dz,d;
mmdb::mat44     TF;
mmdb::mat33     vrm;
mmdb::realtype  alpha,vx,vy,vz;
bool            crystQ,crystT;

  crystQ = structs[0]->isCrystInfo();
  crystT = structs[1]->isCrystInfo();

  writeStructureSummary_csv ( f,structs,2,isOutput );
  
  f.WriteLine ( "\nSUPERPOSITION" );
  sprintf ( L1,
    " Q-score, %-10.3f\n"
    " RMSD, %-10.3f\n"
    " Aligned residues, %i\n"
    "\n"
    " Transformation matrix for Query is identity.\n"
    "\n"
    " Transformation matrix for Target:\n"
    "\n"
    " Rx, Ry, Rz, T\n"
    " %14.7f, %14.7f, %14.7f, %14.7f\n"
    " %14.7f, %14.7f, %14.7f, %14.7f\n"
    " %14.7f, %14.7f, %14.7f, %14.7f\n",
    SD->Q,SD->rmsd,SD->Nalgn,
    SD->T[0][0],SD->T[0][1],SD->T[0][2],SD->T[0][3],
    SD->T[1][0],SD->T[1][1],SD->T[1][2],SD->T[1][3],
    SD->T[2][0],SD->T[2][1],SD->T[2][2],SD->T[2][3]
  );
  f.Write ( L1 );

  for (int i=0;i<3;i++)
    for (int j=0;j<3;j++)
      vrm[i][j] = SD->T[i][j];
  mmdb::GetRotParameters ( vrm,alpha,vx,vy,vz );
  sprintf ( L1,
    "\n"
    " Direction cosines of the rotation axis, %9.7f, %9.7f, %9.7f\n"
    " Rotation angle, %-12.7f\n",
    vx,vy,vz, alpha*180.0/mmdb::Pi
  );
  f.Write ( L1 );

  if (crystQ)  {

    if (structs[0]->getMMDBManager()->Orth2Frac(SD->T,TF))
      sprintf ( L1,"\n"
          " in fractional coordinates of Query:\n\n"
          " Rx, Ry, Rz, T,\n"
          " %14.7f, %14.7f, %14.7f, %14.7f\n"
          " %14.7f, %14.7f, %14.7f, %14.7f\n"
          " %14.7f, %14.7f, %14.7f, %14.7f\n",
          TF[0][0],TF[0][1],TF[0][2],TF[0][3],
          TF[1][0],TF[1][1],TF[1][2],TF[1][3],
          TF[2][0],TF[2][1],TF[2][2],TF[2][3] );
    else
      strcpy ( L1,"\n"
          " *** orthogonal-fractional transformations failed for "
          "Query structure\n" );
    f.Write ( L1 );

  }

  if (crystT)  {

    if (structs[1]->getMMDBManager()->Orth2Frac(SD->T,TF))
      sprintf ( L1,"\n"
          " in fractional coordinates of Target:\n\n"
          " Rx, Ry, Rz, T,\n"
          " %14.7f, %14.7f, %14.7f, %14.7f\n"
          " %14.7f, %14.7f, %14.7f, %14.7f\n"
          " %14.7f, %14.7f, %14.7f, %14.7f\n",
          TF[0][0],TF[0][1],TF[0][2],TF[0][3],
          TF[1][0],TF[1][1],TF[1][2],TF[1][3],
          TF[2][0],TF[2][1],TF[2][2],TF[2][3] );
    else
      strcpy ( L1,"\n"
          " *** orthogonal-fractional transformations failed for "
          "Target structure\n" );
    f.Write ( L1 );

  }

  f.Write (
    "\n"
    " CENTROIDS\n"
    " , , Orthogonal, , , Fractional, \n"
    " , X, Y, Z, XF, YF, ZF\n" );
  
  if (structs[0]->getCentroidF(x1,y1,z1))
        sprintf ( L2,"%10.7f, %10.7f, %10.7f",x1,y1,z1 );
  else  strcpy  ( L2," , , " );
  structs[0]->getCentroid ( x1,y1,z1 );

  sprintf ( L1,
    " Query, %10.5f, %10.5f, %10.5f, %s\n", x1,y1,z1, L2 );
  f.Write ( L1 );

  if (structs[1]->getCentroidF(x2,y2,z2))
        sprintf ( L2,"%10.7f, %10.7f, %10.7f",x2,y2,z2 );
  else  strcpy  ( L2," , , " );
  structs[1]->getCentroid ( x2,y2,z2 );

  sprintf ( L1,
    " Target, %10.5f, %10.5f, %10.5f, %s\n", x2,y2,z2, L2 );
  f.Write ( L1 );

  dx = x2-x1;
  dy = y2-y1;
  dz = z2-z1;
  d  = sqrt ( dx*dx + dy*dy + dz*dz );
  sprintf ( L1,
    "\n"
    " Distance between centroids, %-12.7f\n",d );
  f.Write ( L1 );
  if (d>0.0)  {
    dx /= d;
    dy /= d;
    dz /= d;
    sprintf ( L1,
      " Direction cosines of vector between centroids, %9.7f, %9.7f, %9.7f\n"
      " Angle between rotation axis and vector between centroids, %-11.7f\n",
      dx,dy,dz,acos(dx*vx+dy*vy+dz*vz)*180.0/mmdb::Pi );
    f.Write ( L1 );
  }

  writeccp4rot_csv ( f,SD->T );

  f.WriteLine ( "\n"
                " RESIDUE ALIGNMENT\n" );
  Aligner->writeAlignTable_csv ( f,SD );
  f.shut();

}



void alignToTarget ( gsmt::RInput Input )  {

mmdb::pstr            fQuery        = Input.fstruct[0];
mmdb::pstr            selQuery      = Input.sel    [0];
bool                  SCOPSelQuery  = Input.scopSel[0];
mmdb::pstr            fTarget       = Input.fstruct[1];
mmdb::pstr            selTarget     = Input.sel    [1];
bool                  SCOPSelTarget = Input.scopSel[1];

gsmt::PStructure      s1;
gsmt::PStructure      s2;
gsmt::PStructure      structs[2];
gsmt::PAligner        Aligner;
gsmt::PSuperposition  SD;
mmdb::io::File        f;
mmdb::mat44           TT[2];
mmdb::ERROR_CODE      rc1, rc2;
mmdb::pstr            S,S1;
int                   matchNo,printWidth;
bool                  crystQ,crystT,isOutput;

  printWidth = 80;

  S  = NULL;
  S1 = NULL;
  printHorzLine ( '=',printWidth );
  printf (
    "\n"
    " ... reading FIXED structure : file '%s', selection '%s'\n",
    fQuery,selQuery );

  s1  = new gsmt::Structure();
  rc1 = s1->getStructure ( fQuery,selQuery,-1,SCOPSelQuery );
  crystQ = s1->isCrystInfo();

  if (!rc1)
    printf ( "      %5i atoms selected\n",s1->getNCalphas() );
  else  {
    if (rc1>0)
      printf ( "      %5i atoms selected with warning (rc=%i)\n",
               s1->getNCalphas(),rc1 );
    else
      printf ( " *error* (rc=%i)\n",rc1 );
    s1->getErrDesc ( rc1,S );
    printf ( "     %s\n",S );
  }
  if ((!crystQ) && (rc1>=0))
    printf ( "            crystal data not found\n" );

  printf ( " ... reading MOVING structure: file '%s', selection '%s'\n",
           fTarget,selTarget );
  s2  = new gsmt::Structure();
  rc2 = s2->getStructure ( fTarget,selTarget,-1,SCOPSelTarget );
  crystT = s2->isCrystInfo();

  if (!rc2)
    printf ( "      %5i atoms selected\n",s2->getNCalphas() );
  else {
    if (rc2>0)
      printf ( "      %5i atoms selected with warning (rc=%i)\n",
               s2->getNCalphas(),rc2 );
    else
      printf ( " **error** (rc=%i)\n",rc2 );
    s2->getErrDesc ( rc2,S );
    printf ( "     %s\n",S );
  }
  if ((!crystT) && (rc2>=0))
    printf ( "            crystal data not found\n" );

  if ((rc1<0) || (rc2<0))  {
    printf (
      "\n\n STOP DUE TO READ ERRORS\n"
      " --- check input file format\n"
           );
    printHorzLine ( '=',printWidth );
    if (S)  delete[] S;
    return;
  }

#ifdef compile_for_ccp4
  printf ( "<!--SUMMARY_END-->\n" );
#endif

  printHorzLine ( '=',printWidth );

  Aligner = new gsmt::Aligner();
  Aligner->setPerformanceLevel     ( Input.mode  );
  Aligner->setSimilarityThresholds ( 0.0,0.0     );
  Aligner->setQR0                  ( Input.QR0   );
  Aligner->setSigma                ( Input.sigma );

  Aligner->Align        ( s1,s2,true  );
  Aligner->getBestMatch ( SD,matchNo  );

  if (SD)  {
    
    s1->setRefName ( "FIXED"  );
    s2->setRefName ( "MOVING" );
    structs[0] = s1;
    structs[1] = s2;

    mmdb::Mat4Init ( TT[0]       );
    mmdb::Mat4Copy ( SD->T,TT[1] );

    isOutput = writeSuperposed ( structs,TT,2,Input.outFile,
                                     Input.out_units,Input.out_multi );

    writeLogFile ( structs,Aligner,SD,isOutput,printWidth );
    
    if (Input.outSeqFile)  {
      f.assign ( Input.outSeqFile,true,false );
      f.rewrite();

      mmdb::CreateCopCat ( S,
            mmdb::io::GetFName(fQuery,mmdb::io::syskey_all),
            "(",selQuery ,")" );
      mmdb::CreateCopCat ( S1,
            mmdb::io::GetFName(fTarget,mmdb::io::syskey_all),
            "(",selTarget,")" );

      Aligner->writeSeqAlignment ( f,S,S1,SD );
      f.shut();
    }

    if (Input.csvFile)  {
      f.assign ( Input.csvFile,true,false );
      f.rewrite();
      writeCSVFile ( f,structs,Aligner,SD,isOutput );
      f.shut();
    }

    if (Input.jsonOutFile)
      makeJSONOutput ( Input,structs,Aligner,SD,isOutput );

//    if (Input.rvapiRDir || Input.rvapiRDoc)
//      makeRVAPIOutput ( Input,structs,Aligner,SD,isOutput );

    if (Input.viewer)
      Input.viewer->View ( s1,s2,SD,NULL,NULL );

  } else  {
    printHorzLine ( '=',printWidth );
    printf (
      "\n"
      " Query      %s(%s)\n"
      " and Target %s(%s)\n\n"
      " are DISSIMILAR and cannot be reasonably aligned.\n",
      fQuery,selQuery,fTarget,selTarget );
    printHorzLine ( '=',printWidth );
  }

  delete Aligner;
  delete s1;
  delete s2;

  if (S)   delete[] S;
  if (S1)  delete[] S1;

  return;

}

