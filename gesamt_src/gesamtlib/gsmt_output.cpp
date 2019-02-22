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
//  **** Module  :  GSMT_Output <implementation>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::XBlock
//       ~~~~~~~~~  gsmt::XAlign
//                  gsmt::XTAlign
//                  gsmt::XAlignText
//
//  (C) E. Krissinel 2008-2016
//
// =================================================================
//

#include <string.h>
#include <ctype.h>

#include "gsmt_output.h"
#include "mmdb2/mmdb_tables.h"
#include "rvapi/rvapi_interface.h"


//  -----------------------------  XAlign --------------------------

gsmt::XAlign::XAlign()  {
  XBlock1 = NULL;
  nBlock1 = 0;
  XBlock2 = NULL;
  nBlock2 = 0;
  algnLen = 0;
}

gsmt::XAlign::~XAlign()  {
  FreeMemory();
}

void gsmt::XAlign::FreeMemory()  {
  if (XBlock1)  delete[] XBlock1;
  if (XBlock2)  delete[] XBlock2;
  XBlock1 = NULL;
  nBlock1 = 0;
  XBlock2 = NULL;
  nBlock2 = 0;
  algnLen = 0;
}

void gsmt::XAlign::customInit() {}

void gsmt::XAlign::Align (
                    mmdb::PPAtom Calpha1, mmdb::ivector Ca1, int nat1,
                    mmdb::PPAtom Calpha2, mmdb::ivector Ca2, int nat2,
                    mmdb::rvector dist1, int & nr )  {
int i,j;

  FreeMemory();

  a1     = Ca1;
  a2     = Ca2;
  alpha1 = Calpha1;
  alpha2 = Calpha2;
  d1     = dist1;
  na1    = nat1;
  na2    = nat2;

  nCols1 = makeXBlocks ( Ca1,nat1,XBlock1,nBlock1 );
  nCols2 = makeXBlocks ( Ca2,nat2,XBlock2,nBlock2 );
  nRows  = nat1 + nat2 + 2;

  maxdist = 0.0;
  for (i=0;i<nat1;i++)
    if (Ca1[i]>=0)  {
      if (dist1[i]>maxdist)  maxdist = dist1[i];
    }
  if (maxdist<=1.0e-2)  maxdist = 1.0;

  customInit();
  nr = 0;
  for (i=0;i<nBlock1;i++)
    for (j=0;j<nBlock2;j++)
      alignXBlocks ( XBlock1[i],XBlock2[j],nr );

  algnLen = nr;

}


int gsmt::XAlign::makeXBlocks ( mmdb::ivector Ca, int nat, RPXBlock XB,
                                int & nBlocks )  {
//    Ca is considered as blocks of non-negative,
//  increasing-by-one numbers Ca[i]>=0, and negative
//  Ca[i]<0 surrounding them. Block boundaries are drawn
//  at the middle of negative-Ca[i] gaps.
//    nBlocks returns the number of such blocks, each block is
//  identified by the initial and final indices i1 and i2, and
//  by "index mass center" mc used for sorting.
//    Returns the number of fold-columns.
PXBlock        XB1;
mmdb::realtype mc;
int            nAlloc,i,j,i1,i2,ip1,ip2,iv,k,icol;

  if (XB)  delete[] XB;
  XB      = NULL;
  nBlocks = 0;
  nAlloc  = 0;

  i  = 0;
  i1 = 0;  // begining of a block
  // begining of first block, check for leading negatives
  while (i<nat)
    if (Ca[i]<0)  i++;
            else  break;
  do  {
    if (i<nat)  {
      // check for increasing-by-one positives
      ip1 = i;
      iv  = Ca[i++];
      mc  = iv;
      k   = 1;
      while (i<nat)
        if (Ca[i]==iv+1)  {
          iv  = Ca[i++];
          mc += iv;
          k++;
        } else
          break;
      mc /= k;
      ip2 = i-1;  // increasing-by-one has stopped, check for negatives
      while (i<nat)
        if (Ca[i]<0)  i++;
                else  break;
      //  get i2 as end of a block
      if (i>=nat)       i2 = nat-1;     // the last block
      else if (i-ip2>1) i2 = (ip2+i)/2; // take the medium
                   else i2 = ip2;
    } else  {
      i2  = nat-1;  // the only empty (all-negatives) block
      ip1 = -1;
      ip2 = -1;
      mc  = 0.0;
    }
    // create new block
    if (nBlocks>=nAlloc)  {
      nAlloc += 20;
      XB1 = new XBlock[nAlloc];
      for (j=0;j<nBlocks;j++)  {
        XB1[j].i1   = XB[j].i1;
        XB1[j].i2   = XB[j].i2;
        XB1[j].ip1  = XB[j].ip1;
        XB1[j].ip2  = XB[j].ip2;
        XB1[j].mc   = XB[j].mc;
        XB1[j].icol = XB[j].icol;
      }
      delete[] XB;
      XB = XB1;
    }
    XB[nBlocks].i1   = i1;
    XB[nBlocks].i2   = i2;
    XB[nBlocks].ip1  = ip1;
    XB[nBlocks].ip2  = ip2;
    XB[nBlocks].mc   = mc;
    XB[nBlocks].icol = 0;
    nBlocks++;
    i1 = i2+1;
  } while (i<nat);

  // assign fold-columns to the blocks
  icol = 0;
  do  {
    icol++;
    i  = 0;
    iv = 0;
    while (i<nBlocks)  {
      mc = mmdb::MaxReal;
      k  = -1;
      for (j=i;j<nBlocks;j++)
        if ((XB[j].icol==0) && (XB[j].mc<mc))  {
          mc = XB[j].mc;
          k  = j;
        }
      if (k>=0)  {
        XB[k].icol = icol;
        i  = k+1;
        iv = 1;
      } else
        i = nBlocks;
    }
  } while (iv);

  return icol-1;

}


void gsmt::XAlign::alignXBlocks ( RXBlock B1, RXBlock B2, int & nr )  {
int  l1,l2, i1,i2, sseType1,sseType2, icol;

  if (((a1[B1.ip1]>=B2.ip1) && (a1[B1.ip1]<=B2.ip2)) ||
      ((a1[B1.ip2]>=B2.ip1) && (a1[B1.ip2]<=B2.ip2)) ||
      ((a2[B2.ip1]>=B1.ip1) && (a2[B2.ip1]<=B1.ip2)) ||
      ((a2[B2.ip2]>=B1.ip1) && (a2[B2.ip2]<=B1.ip2)))  {

    if (a1[B1.ip1]<B2.ip1)  {
      l1 = 0;
      l2 = B2.ip1 - B2.i1;
      i1 = a2[B2.ip1];
      i2 = B2.i1;
    } else if (a1[B1.ip1]==B2.ip1)  {
      l1 = B1.ip1 - B1.i1;
      l2 = B2.ip1 - B2.i1;
      i1 = B1.i1;
      i2 = B2.i1;
    } else  {
      l1 = B1.ip1 - B1.i1;  // number of leading unmappings
      l2 = 0;
      i1 = B1.i1;
      i2 = a1[B1.ip1];
    }

    icol = B1.icol;

    while (l1>l2)  {
      if (alpha1[i1])  sseType1 = alpha1[i1]->GetSSEType();
                 else  sseType1 = mmdb::SSE_None;
      makeRow ( alpha1[i1],sseType1,NULL,mmdb::SSE_None,
                d1[i1],nr++,icol,false );
      i1++;
      l1--;
    }
    while (l2>l1)  {
      if (alpha2[i2])  sseType2 = alpha2[i2]->GetSSEType();
                 else  sseType2 = mmdb::SSE_None;
      makeRow ( NULL,mmdb::SSE_None,alpha2[i2++],sseType2,
                -1.0,nr++,icol,false );
      l2--;
    }
    while (l2>0)  {
      if (alpha1[i1])  sseType1 = alpha1[i1]->GetSSEType();
                 else  sseType1 = mmdb::SSE_None;
      if (alpha2[i2])  sseType2 = alpha2[i2]->GetSSEType();
                 else  sseType2 = mmdb::SSE_None;
      makeRow ( alpha1[i1],sseType1,alpha2[i2++],sseType2,
                d1[i1],nr++,icol,false );
      i1++;
      l2--;
    }

    l1 = mmdb::IMin ( B1.ip2-i1, B2.ip2-i2 ) + 1;
    while (l1>0)  {
      if (alpha1[i1])  sseType1 = alpha1[i1]->GetSSEType();
                 else  sseType1 = mmdb::SSE_None;
      if (alpha2[i2])  sseType2 = alpha2[i2]->GetSSEType();
                 else  sseType2 = mmdb::SSE_None;
      makeRow ( alpha1[i1],sseType1,alpha2[i2++],sseType2,
                d1[i1],nr++,icol,true );
      i1++;
      l1--;
    }

    if (i1<=B1.ip2)  {
      l1 = 0;
      l2 = B2.i2 - i2 + 1;
    } else if (i2<=B2.ip2)  {
      l1 = B1.i2 - i1 + 1;
      l2 = 0;
    } else  {
      l1 = B1.i2 - i1 + 1;
      l2 = B2.i2 - i2 + 1;
    }
    while ((l1>0) && (l2>0))  {
      if (alpha1[i1])  sseType1 = alpha1[i1]->GetSSEType();
                 else  sseType1 = mmdb::SSE_None;
      if (alpha2[i2])  sseType2 = alpha2[i2]->GetSSEType();
                 else  sseType2 = mmdb::SSE_None;
      makeRow ( alpha1[i1],sseType1,alpha2[i2++],sseType2,
                d1[i1],nr++,icol,false );
      i1++;
      l1--;
      l2--;
    }
    while (l1>0)  {
      if (alpha1[i1])  sseType1 = alpha1[i1]->GetSSEType();
                 else  sseType1 = mmdb::SSE_None;
      makeRow ( alpha1[i1],sseType1,NULL,mmdb::SSE_None,
                d1[i1],nr++,icol,false );
      i1++;
      l1--;
    }
    while (l2>0)  {
      if (alpha2[i2])  sseType2 = alpha2[i2]->GetSSEType();
                 else  sseType2 = mmdb::SSE_None;
      makeRow ( NULL,mmdb::SSE_None,alpha2[i2++],sseType2,
                -1.0,nr++,icol,false );
      l2--;
    }

  }

}


void gsmt::XAlign::makeRow ( mmdb::PAtom A1, int sseType1,
                             mmdb::PAtom A2, int sseType2,
                             mmdb::realtype dist, int rowNo,
                             int icol, bool aligned )  {
UNUSED_ARGUMENT(A1);
UNUSED_ARGUMENT(sseType1);
UNUSED_ARGUMENT(A2);
UNUSED_ARGUMENT(sseType2);
UNUSED_ARGUMENT(dist);
UNUSED_ARGUMENT(rowNo);
UNUSED_ARGUMENT(icol);
UNUSED_ARGUMENT(aligned);
}



//  ----------------------------  CXTAlign --------------------------


void sprintAtomData ( mmdb::pstr     S,
                      int            sseType,
                      mmdb::realtype hydropathy,
                      mmdb::ChainID  chID,
                      mmdb::ResName  resName,
                      int            seqNum,
                      mmdb::InsCode  insCode )  {
char sse[2],hp[2],ch[3];

  if (sseType==mmdb::SSE_Helix)       sse[0] = 'H';
  else if (sseType==mmdb::SSE_Strand) sse[0] = 'S';
                                 else sse[0] = ' ';
  sse[1] = char(0);

  if ((-5.0<hydropathy) && (hydropathy<5.0))  {
    if (hydropathy>=-0.5)      hp[0] = '-';
    else if (hydropathy<=-1.5) hp[0] = '+';
                          else hp[0] = '.';
  } else
    hp[0] = ' ';
  hp[1] = char(0);

  if ((!chID[0]) || (chID[0]==' '))  {
    ch[0] = ' ';      ch[1] = ' ';
  } else  {
    ch[0] = chID[0];  ch[1] = ':';
  }
  ch[2] = char(0);

  sprintf ( S,"%1s%1s %2s%3s%4i%1s",
              sse,hp,ch,resName,seqNum,insCode );
              
}
              
void  printAtomData ( mmdb::io::RFile f, int sseType,
                      mmdb::realtype hydropathy,
                      mmdb::ChainID  chID,
                      mmdb::ResName  resName,
                      int            seqNum,
                      mmdb::InsCode  insCode )  {
char S[200];
  sprintAtomData ( S,sseType,hydropathy,chID,resName,seqNum,insCode );
  f.Write ( S );
}


void gsmt::XTAlign::print ( mmdb::io::RFile f, int serNumWidth )  {
char S[100],SI[10];
int  i;

  if (alignKey<4)  {

    f.Write ( "|" );
    if (alignKey!=2)
      printAtomData ( f,sseType1,hydropathy1,chID1,
                        resName1,seqNum1,insCode1 );
    else
      f.Write ( "             " );
    f.Write ( "|" );

    if (serNumWidth>0)  {
      if (alignKey!=2)
            sprintf ( S,"%*i|",serNumWidth,serNum1 );
      else  sprintf ( S,"%*s|",serNumWidth,"" );
      f.Write ( S );
    }

    if (alignKey==0)  {
      switch (simindex)  {
        case 5 :  strcpy ( SI,"*****" );  break;
        case 4 :  strcpy ( SI,"+++++" );  break;
        case 3 :  strcpy ( SI,"=====" );  break;
        case 2 :  strcpy ( SI,"-----" );  break;
        case 1 :  strcpy ( SI,":::::" );  break;
        default:
        case 0 :  strcpy ( SI,"....." );  break;
      }
      SI[1] = char(0);
      sprintf ( S," <%1s%5.2f%1s%1s",SI,dist,SI,SI );
      if (S[3]==' ')  S[3] = SI[0];
      f.Write ( S );
      SI[1] = SI[0];
      for (i=1;i<loopNo;i++)  f.Write ( SI );
      f.Write ( "> " );
    } else  {
      f.Write ( "          " );
      for (i=1;i<loopNo;i++)  f.Write ( "     " );
      f.Write ( "  " );
    }

    if (serNumWidth>0)  {
      if (alignKey!=3)
            sprintf ( S,"|%*i",serNumWidth,serNum2 );
      else  sprintf ( S,"|%*s",serNumWidth,"" );
      f.Write ( S );
    }

    f.Write ( "|" );
    if (alignKey!=3)
      printAtomData ( f,sseType2,hydropathy2,chID2,
                        resName2,seqNum2,insCode2 );
    else
      f.Write ( "             " );
    f.Write ( "|" );

    f.LF();

  }

}


void gsmt::XTAlign::put_rvapi ( mmdb::cpstr tableId, int row,
                                bool serNum )  {
char S[100],D[1000];
int  col = 0;

  if (alignKey<4)  {

    sprintf ( S,"%i",row );
    rvapi_put_vert_theader ( tableId,S,"",row );

    if (alignKey!=2)  {
      sprintAtomData ( S,sseType1,hydropathy1,chID1,
                         resName1,seqNum1,insCode1 );
      mmdb::CutSpaces ( S,mmdb::SCUTKEY_BEGEND );
      rvapi_make_hard_spaces ( D,S );
      rvapi_put_table_string ( tableId,D,row,col++ );
      if (serNum)
        rvapi_put_table_int ( tableId,serNum1,row,col++ );
    } else  {
      rvapi_put_table_string ( tableId,"",row,col++ );
      if (serNum)
        rvapi_put_table_string ( tableId,"",row,col++ );
    }

    if (alignKey==0)
         rvapi_put_table_real   ( tableId,dist,"%.2f",row,col++ );
    else rvapi_put_table_string ( tableId,"",row,col++ );
    
    if (alignKey!=3)  {
      if (serNum)
        rvapi_put_table_int ( tableId,serNum2,row,col++ );
      sprintAtomData ( S,sseType2,hydropathy2,chID2,
                         resName2,seqNum2,insCode2 );
      mmdb::CutSpaces ( S,mmdb::SCUTKEY_BEGEND );
      rvapi_make_hard_spaces ( D,S );
      rvapi_put_table_string ( tableId,D,row,col++ );
    } else  {
      rvapi_put_table_string ( tableId,"",row,col++ );
      if (serNum)
        rvapi_put_table_string ( tableId,"",row,col++ );
    }

  }

}


/*
void gsmt::XTAlign::csv_title ( mmdb::pstr S, mmdb::cpstr sep )  {
  sprintf ( S,"Query%sDist [A]%sSim.%sLoop%sTarget",
              sep,sep,sep,sep );
}

void gsmt::XTAlign::write_csv ( mmdb::pstr S, mmdb::cpstr sep )  {
char L[200];

  if (alignKey<4)  {

    if (alignKey!=2)
      sprintAtomData ( S,sseType1,hydropathy1,chID1,
                         resName1,seqNum1,insCode1 );
    else
      S[0] = char(0);

    if (alignKey==0)
      sprintf ( L,"%s%.3f%s%i%s%i%s",sep,dist,sep,simindex,
                                     sep,loopNo,sep );
    else
      sprintf ( L,"%s%s%s%s",sep,sep,sep,sep );
    strcat  ( S,L );

    if (alignKey!=3)  {
      sprintAtomData ( L,sseType2,hydropathy2,chID2,
                        resName2,seqNum2,insCode2 );
      strcat ( S,L );
    }

  }

}
*/

void gsmt::XTAlign::csv_title ( mmdb::pstr S, mmdb::cpstr sep )  {
  sprintf ( S,"Dist [A]%sSim.%sLoop%sQuery%sTarget",
              sep,sep,sep,sep );
}

void gsmt::XTAlign::write_csv ( mmdb::pstr S, mmdb::cpstr sep )  {
char L[200];

  if (alignKey<4)  {

    if (alignKey==0)
      sprintf ( S,"%.3f%s%i%s%i%s",dist,sep,simindex,
                                   sep,loopNo,sep );
    else
      sprintf ( S,"%s%s%s",sep,sep,sep );

    if (alignKey!=2)  {
      sprintAtomData ( L,sseType1,hydropathy1,chID1,
                         resName1,seqNum1,insCode1 );
      strcat ( S,L );
    }
    strcat ( S,sep );

    if (alignKey!=3)  {
      sprintAtomData ( L,sseType2,hydropathy2,chID2,
                        resName2,seqNum2,insCode2 );
      strcat ( S,L );
    }

  }

}


gsmt::PJSON gsmt::XTAlign::getJSON()  {
gsmt::PJSON json,json1;
char L[200];

  json = new gsmt::JSON();
   
  json->addValue ( "dist"    ,dist,"%.4f" );    
  json->addValue ( "alignKey",alignKey );    
  json->addValue ( "loopNo"  ,loopNo   );    
  json->addValue ( "simindex",simindex );    

  json1 = new gsmt::JSON();
  if (alignKey!=2)  {
    json1->addValue ( "chainId"   ,chID1         );    
    json1->addValue ( "resName"   ,resName1      );    
    json1->addValue ( "insCode"   ,insCode1      );    
    json1->addValue ( "seqNum"    ,seqNum1       );
    json1->addValue ( "sseType"   ,sseType1      );
    json1->addValue ( "hydropathy",hydropathy1,"%.2f" );
    sprintAtomData  ( L,sseType1,hydropathy1,chID1,
                        resName1,seqNum1,insCode1 );
  } else  {
    json1->addValue ( "chainId"   ,""    );    
    json1->addValue ( "resName"   ,""    );    
    json1->addValue ( "insCode"   ,""    );    
    json1->addValue ( "seqNum"    ,-1    );
    json1->addValue ( "sseType"   ,-1    );
    json1->addValue ( "hydropathy",0.0,"%.2f" );
    L[0] = char(0);
  }
  json1->addValue ( "label",L );
  
  json->addJSON ( "fixed",json1 );

  json1 = new gsmt::JSON();

  if (alignKey!=3)  {
    json1->addValue ( "chainId"   ,chID2         );    
    json1->addValue ( "resName"   ,resName2      );    
    json1->addValue ( "insCode"   ,insCode2      );    
    json1->addValue ( "seqNum"    ,seqNum2       );
    json1->addValue ( "sseType"   ,sseType2      );
    json1->addValue ( "hydropathy",hydropathy2,"%.2f" );  
    sprintAtomData  ( L,sseType2,hydropathy2,chID2,
                        resName2,seqNum2,insCode2 );
  } else  {
    json1->addValue ( "chainId"   ,""    );    
    json1->addValue ( "resName"   ,""    );    
    json1->addValue ( "insCode"   ,""    );    
    json1->addValue ( "seqNum"    ,-1    );
    json1->addValue ( "sseType"   ,-1    );
    json1->addValue ( "hydropathy",0.0,"%.2f" );
    L[0] = char(0);
  }
  json1->addValue ( "label",L );

  json->addJSON ( "moving",json1 );
    
  return json;

}



void gsmt::XTAlign::getAlignSymbols ( char & c1, char & c2 )  {

  if (alignKey<4)  {

    if (alignKey==2)  c1 = '-';   // gap in structure 1
                else  mmdb::Get1LetterCode ( resName1,c1 );

    if (alignKey==3)  c2 = '-';   // gap in structure 2
                else  mmdb::Get1LetterCode ( resName2,c2 );

    if (alignKey==1)  {
      c1 = 'a' + int(c1-'A');
      c2 = 'a' + int(c2-'A');
    }

  } else  {
    c1 = '?';
    c2 = '?';
  }

}

int gsmt::XTAlign::getWriteSize()  {
  return 3*sizeof(mmdb::realUniBin) +
         2*sizeof(mmdb::ChainID)    +
         2*sizeof(mmdb::ResName)    +
         2*sizeof(mmdb::InsCode)    +
         8*sizeof(mmdb::intUniBin)  + 1;
}

void gsmt::XTAlign::mem_write ( mmdb::pstr S, int & l )  {
int version=2;
  mmdb::mem_write ( version    ,S,l );
  mmdb::mem_write ( hydropathy1,S,l );
  mmdb::mem_write ( hydropathy2,S,l );
  mmdb::mem_write ( dist       ,S,l );
  mmdb::mem_write ( chID1   ,sizeof(chID1)   ,S,l );
  mmdb::mem_write ( chID2   ,sizeof(chID2)   ,S,l );
  mmdb::mem_write ( resName1,sizeof(resName1),S,l );
  mmdb::mem_write ( resName2,sizeof(resName2),S,l );
  mmdb::mem_write ( insCode1,sizeof(insCode1),S,l );
  mmdb::mem_write ( insCode2,sizeof(insCode2),S,l );
  mmdb::mem_write ( alignKey   ,S,l );
  mmdb::mem_write ( loopNo     ,S,l );
  mmdb::mem_write ( sseType1   ,S,l );
  mmdb::mem_write ( sseType2   ,S,l );
  mmdb::mem_write ( seqNum1    ,S,l );
  mmdb::mem_write ( seqNum2    ,S,l );
  mmdb::mem_write ( simindex   ,S,l );
  mmdb::mem_write ( serNum1    ,S,l );
  mmdb::mem_write ( serNum2    ,S,l );
}


void gsmt::XTAlign::mem_read  ( mmdb::cpstr S, int & l )  {
int version;
  mmdb::mem_read ( version    ,S,l );
  mmdb::mem_read ( hydropathy1,S,l );
  mmdb::mem_read ( hydropathy2,S,l );
  mmdb::mem_read ( dist       ,S,l );
  mmdb::mem_read ( chID1   ,sizeof(chID1)   ,S,l );
  mmdb::mem_read ( chID2   ,sizeof(chID2)   ,S,l );
  mmdb::mem_read ( resName1,sizeof(resName1),S,l );
  mmdb::mem_read ( resName2,sizeof(resName2),S,l );
  mmdb::mem_read ( insCode1,sizeof(insCode1),S,l );
  mmdb::mem_read ( insCode2,sizeof(insCode2),S,l );
  mmdb::mem_read ( alignKey   ,S,l );
  mmdb::mem_read ( loopNo     ,S,l );
  mmdb::mem_read ( sseType1   ,S,l );
  mmdb::mem_read ( sseType2   ,S,l );
  mmdb::mem_read ( seqNum1    ,S,l );
  mmdb::mem_read ( seqNum2    ,S,l );
  mmdb::mem_read ( simindex   ,S,l );
  if (version>1)  {
    mmdb::mem_read ( serNum1    ,S,l );
    mmdb::mem_read ( serNum2    ,S,l );
  }
}


//  ---------------------------  XAlignText ------------------------

gsmt::XAlignText::XAlignText() : gsmt::XAlign() {
  R = NULL;
}

gsmt::XAlignText::~XAlignText() {
  customFree();
}

void gsmt::XAlignText::customFree()  {
  if (R)  delete[] R;
  R = NULL;
}

void gsmt::XAlignText::customInit()  {
int i;
  customFree();
  R = new XTAlign[nRows];
  for (i=0;i<nRows;i++)
    R[i].alignKey = 5;
}

void  gsmt::XAlignText::wipeTextRows()  {
  R = NULL;
}

gsmt::PXTAlign gsmt::XAlignText::takeTextRows()  {
gsmt::PXTAlign RR = R;
  R = NULL;
  return RR;
}

void gsmt::XAlignText::makeRow ( mmdb::PAtom A1, int sseType1,
                                 mmdb::PAtom A2, int sseType2,
                                 mmdb::realtype dist,
                                 int rowNo, int icol,
                                 bool aligned )  {

  if (aligned)  R[rowNo].alignKey = 0;
          else  R[rowNo].alignKey = 1;

  if (A1)  {
    R[rowNo].sseType1    = sseType1;
    R[rowNo].hydropathy1 = A1->GetAAHydropathy();
    R[rowNo].seqNum1     = A1->GetSeqNum      ();
    R[rowNo].serNum1     = A1->serNum;
    strcpy ( R[rowNo].chID1   ,A1->GetChainID() );
    strcpy ( R[rowNo].resName1,A1->GetResName() );
    strcpy ( R[rowNo].insCode1,A1->GetInsCode() );
  } else
    R[rowNo].alignKey = 2;

  if (A2)  {
    R[rowNo].sseType2    = sseType2;
    R[rowNo].hydropathy2 = A2->GetAAHydropathy();
    R[rowNo].seqNum2     = A2->GetSeqNum      ();
    R[rowNo].serNum2     = A2->serNum;
    strcpy ( R[rowNo].chID2   ,A2->GetChainID() );
    strcpy ( R[rowNo].resName2,A2->GetResName() );
    strcpy ( R[rowNo].insCode2,A2->GetInsCode() );
  } else
    R[rowNo].alignKey = 3;

  if ((!A1) && (!A2))  R[rowNo].alignKey = 4;

  R[rowNo].simindex = -5;
  R[rowNo].dist     = -1.0;
  if (aligned)  {
    if (A1 && A2)  R[rowNo].simindex = A1->GetAASimilarity ( A2 );
             else  R[rowNo].simindex = -5;
    R[rowNo].dist = dist;
  }

  R[rowNo].loopNo = icol;

}

void  gsmt::XAlignText::getAlignments ( mmdb::pstr    & algn1,
                                        mmdb::pstr    & algn2,
                                        mmdb::rvector * dist )  {
char rn1[10];
char rn2[10];
int i;
  if (algn1)  delete[] algn1;
  if (algn2)  delete[] algn2;
  if (dist)
    mmdb::FreeVectorMemory ( *dist,0 );
  if (algnLen>0)  {
    algn1 = new char[algnLen+1];
    algn2 = new char[algnLen+1];
    if (dist)
      mmdb::GetVectorMemory ( *dist,algnLen,0 );
    for (i=0;i<algnLen;i++)  {
      if (dist)
        (*dist)[i] = R[i].dist;
      if (R[i].alignKey<=3)  {
        if (R[i].alignKey!=2)
              mmdb::Get1LetterCode ( R[i].resName1,rn1 );
        else  strcpy ( rn1,"-" );
        if (R[i].alignKey!=3)
              mmdb::Get1LetterCode ( R[i].resName2,rn2 );
        else  strcpy ( rn2,"-" );
        if (R[i].alignKey==0)  {
          rn1[0] = char(toupper(int(rn1[0])));
          rn2[0] = char(toupper(int(rn2[0])));
        } else  {
          rn1[0] = char(tolower(int(rn1[0])));
          rn2[0] = char(tolower(int(rn2[0])));
        }
      } else  {
        strcpy ( rn1,"-" );
        strcpy ( rn2,"-" );
      }
      algn1[i] = rn1[0];
      algn2[i] = rn2[0];
    }
    algn1[algnLen] = char(0);
    algn2[algnLen] = char(0);
  } else  {
    algn1 = NULL;
    algn2 = NULL;
  }
}

