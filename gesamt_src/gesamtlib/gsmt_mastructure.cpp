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
//    03.02.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  GSMT_MAStructure <implementation>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::MAStructure
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2013-2015
//
// =================================================================
//

#include <string.h>

#include "gsmt_mastructure.h"
#include "gsmt_mamap.h"

// =================================================================

gsmt::MAStructure::MAStructure() : Structure()  {
  initClass();
}

gsmt::MAStructure::MAStructure ( mmdb::io::RPStream Object )
                 : Structure(Object)  {
  initClass();
}

gsmt::MAStructure::~MAStructure()  {
  freeMemory();
}

void  gsmt::MAStructure::initClass()  {

  serNo  = 0;     // serial number
  Qsum   = 0.0;   // multiple superposition Q-score
  Calpha = NULL;
  chID   = NULL;
  c0     = NULL;  // reference atom match vector
  c      = NULL;  // atom match vector
  unmap1 = NULL;  // atom unmap vector
  x0     = NULL;  // atom
  y0     = NULL;  //   coordinate
  z0     = NULL;  //      buffer

  mmdb::Mat4Init ( RT0 );
  mmdb::Mat4Init ( RT  );

}

void  gsmt::MAStructure::freeMemory()  {
  Calpha = NULL;
  if (chID)  {
    delete[] chID;
    chID = NULL;
  }
  mmdb::FreeVectorMemory ( c0    ,0 );
  mmdb::FreeVectorMemory ( c     ,0 );
  mmdb::FreeVectorMemory ( unmap1,0 );
  mmdb::FreeVectorMemory ( x0    ,0 );
  mmdb::FreeVectorMemory ( y0    ,0 );
  mmdb::FreeVectorMemory ( z0    ,0 );
}

void  gsmt::MAStructure::prepareMAStructure()  {
  getCalphas ( Calpha,nres );
  chID = new mmdb::ChainID[nres];
  for (int i=0;i<nres;i++)
    strcpy ( chID[i],Calpha[i]->GetChainID() );
  mmdb::GetVectorMemory ( c     ,nres,0 );
  mmdb::GetVectorMemory ( c0    ,nres,0 );
  mmdb::GetVectorMemory ( unmap1,nres,0 );
  mmdb::GetVectorMemory ( x0    ,nres,0 );
  mmdb::GetVectorMemory ( y0    ,nres,0 );
  mmdb::GetVectorMemory ( z0    ,nres,0 );
}


void  gsmt::MAStructure::saveCoordinates()  {
  for (int i=0;i<nres;i++)  {
    x0[i] = Calpha[i]->x;
    y0[i] = Calpha[i]->y;
    z0[i] = Calpha[i]->z;
  }
}

void  gsmt::MAStructure::restoreCoordinates()  {
  for (int i=0;i<nres;i++)  {
    Calpha[i]->x = x0[i];
    Calpha[i]->y = y0[i];
    Calpha[i]->z = z0[i];
  }
}

void  gsmt::MAStructure::transform()  {
//   This function applies the rotation-translation transformation,
// given by matrix RT, to the structure.
  for (int i=0;i<nres;i++)
    Calpha[i]->Transform ( RT );
}


bool gsmt::MAStructure::isMC ( int pos1, PMAStructure S, int pos2 )  {
//   Returns true if matching the Calpha pair in the positions
// (pos1,pos2) of the chains would contradict to the already
// aligned pairs and allowMC is set false.
int i;

//  if (allowMC)  return false;

  i = pos1 + 1;
  while (i<nres)
    if (c[i]>=0)  break;
            else  i++;
  if (i<nres) {
    if (pos2>=c[i])  {
      if ((!strcmp(chID[pos1],chID[i])) &&
          (!strcmp(S->chID[pos2],S->chID[c[i]])))
        return true;
    }
  }

  i = pos1 - 1;
  while (i>=0)
    if (c[i]>=0)  break;
            else  i--;
  if (i>=0) {
    if (pos2<=c[i])  {
      if ((!strcmp(chID[pos1],chID[i])) &&
          (!strcmp(S->chID[pos2],S->chID[c[i]])))
        return true;
    }
  }

  return false;

}

/*
void gsmt::MAStructure::calcCorrelationMatrix ( mmdb::rmatrix & A,
                                                mmdb::rvector  xcons,
                                                mmdb::rvector  ycons,
                                                mmdb::rvector  zcons,
                                                mmdb::rvector  w ) {
mmdb::vect3 vc1,vc2;
int         i,j,i1,i2;

  for (i=1;i<=3;i++)
    for (j=1;j<=3;j++)
      A[i][j] = 0.0;

  xm = 0.0;
  ym = 0.0;
  zm = 0.0;
  cx = 0.0;
  cy = 0.0;
  cz = 0.0;
  nalign = 0;
  for (i1=0;i1<nres;i1++)  {
    i2 = c[i1];
    if (i2>=0)  {
      xm += x0[i1];
      ym += y0[i1];
      zm += z0[i1];
      cx += xcons[i2];
      cy += ycons[i2];
      cz += zcons[i2];
      nalign++;
    }
  }
  xm /= nalign;
  ym /= nalign;
  zm /= nalign;
  cx /= nalign;
  cy /= nalign;
  cz /= nalign;

  if (w)  {
    for (i1=0;i1<nres;i1++)  {
      i2 = c[i1];
      if (i2>=0)  {
        vc1[0] = x0[i1] - xm;
        vc1[1] = y0[i1] - ym;
        vc1[2] = z0[i1] - zm;
        vc2[0] = xcons[i2] - cx;
        vc2[1] = ycons[i2] - cy;
        vc2[2] = zcons[i2] - cz;
        for (i=1;i<=3;i++)
          for (j=1;j<=3;j++)
            A[i][j] += w[i2]*vc1[j-1]*vc2[i-1];
      }
    }
  } else  {
    for (i1=0;i1<nres;i1++)  {
      i2 = c[i1];
      if (i2>=0)  {
        vc1[0] = x0[i1] - xm;
        vc1[1] = y0[i1] - ym;
        vc1[2] = z0[i1] - zm;
        vc2[0] = xcons[i2] - cx;
        vc2[1] = ycons[i2] - cy;
        vc2[2] = zcons[i2] - cz;
        for (i=1;i<=3;i++)
          for (j=1;j<=3;j++)
            A[i][j] += vc1[j-1]*vc2[i-1];
      }
    }
  }

}
*/

void gsmt::MAStructure::calcCorrelationMatrix ( mmdb::mat33  & A,
                                                MAMap        * Map,
                                                mmdb::realtype sigma2 ) {
mmdb::vect3    vc1,vc2;
mmdb::realtype weight;
int            i,j,i1,i2;

  for (i=0;i<3;i++)
    for (j=0;j<3;j++)
      A[i][j] = 0.0;

  xm = 0.0;
  ym = 0.0;
  zm = 0.0;
  cx = 0.0;
  cy = 0.0;
  cz = 0.0;
  nalign = 0;
  for (i1=0;i1<nres;i1++)  {
    i2 = c[i1];
    if (i2>=0)  {
      xm += x0[i1];
      ym += y0[i1];
      zm += z0[i1];
      cx += Map[i2].xc;
      cy += Map[i2].yc;
      cz += Map[i2].zc;
      nalign++;
    }
  }
  xm /= nalign;
  ym /= nalign;
  zm /= nalign;
  cx /= nalign;
  cy /= nalign;
  cz /= nalign;

  if (sigma2>0.0)  {
    for (i1=0;i1<nres;i1++)  {
      i2 = c[i1];
      if (i2>=0)  {
        vc1[0] = x0[i1] - xm;
        vc1[1] = y0[i1] - ym;
        vc1[2] = z0[i1] - zm;
        vc2[0] = Map[i2].xc - cx;
        vc2[1] = Map[i2].yc - cy;
        vc2[2] = Map[i2].zc - cz;
        weight = mmdb::Exp ( -Map[i2].rmsd/sigma2 );
//        weight = 1.0 / (1.0 + Map[i2].rmsd/sigma2 );
//        weight = weight*weight;
        for (i=0;i<3;i++)
          for (j=0;j<3;j++)
            A[i][j] += weight*vc1[j]*vc2[i];
      }
    }
  } else  {
    for (i1=0;i1<nres;i1++)  {
      i2 = c[i1];
      if (i2>=0)  {
        vc1[0] = x0[i1] - xm;
        vc1[1] = y0[i1] - ym;
        vc1[2] = z0[i1] - zm;
        vc2[0] = Map[i2].xc - cx;
        vc2[1] = Map[i2].yc - cy;
        vc2[2] = Map[i2].zc - cz;
        for (i=0;i<3;i++)
          for (j=0;j<3;j++)
            A[i][j] += vc1[j]*vc2[i];
      }
    }
  }

}


void gsmt::MAStructure::calcTranslation()  {
  RT[0][3] = cx - RT[0][0]*xm - RT[0][1]*ym - RT[0][2]*zm;
  RT[1][3] = cy - RT[1][0]*xm - RT[1][1]*ym - RT[1][2]*zm;
  RT[2][3] = cz - RT[2][0]*xm - RT[2][1]*ym - RT[2][2]*zm;
}

void  gsmt::MAStructure::copy ( PMAStructure S )  {
// deep-copy from S to this
  
  freeMemory();
  
  Structure::copy ( S );
  
  serNo = S->serNo;    // structure serial number (0,1,...)
  Qsum  = S->Qsum;     // multiple superposition Q-score
  nres  = S->nres;     // number of residues (C-alphas)
  if (S->Calpha || S->x0)
    prepareMAStructure();
  mmdb::Mat4Copy ( S->RT0,RT0 );  // rotation-translation
  mmdb::Mat4Copy ( S->RT ,RT  );  //    matrices
  
}

void  gsmt::MAStructure::read ( mmdb::io::RFile f )  {
  Structure::read ( f );
}

void  gsmt::MAStructure::write ( mmdb::io::RFile f )  {
  Structure::write ( f );
}

