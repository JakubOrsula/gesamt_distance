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
//  **** Module  :  GSMT_Output <interface>
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

#ifndef __GSMT_OUTPUT__
#define __GSMT_OUTPUT__

#include "mmdb2/mmdb_atom.h"
#include "json_.h"

// =================================================================

namespace gsmt  {

  //  ---------------------------  XBlock --------------------------

  DefineStructure(XBlock);

  struct XBlock  {
    int      i1,i2;    //!< the outer block boundaries
    int      ip1,ip2;  //!< the alignment boundaries (ip1>=i1, ip2<=i2)
    int      icol;     //!< the block "column" number
    mmdb::realtype mc; //!< center of "index mass"
  };

  //  --------------------------  CXAlign --------------------------

  DefineClass(XAlign);

  class XAlign  {

    public :
      XAlign();
      virtual ~XAlign();

      void Align ( mmdb::PPAtom Calpha1, mmdb::ivector Ca1, int nat1,
                   mmdb::PPAtom Calpha2, mmdb::ivector Ca2, int nat2,
                   mmdb::rvector dist1, int & nr );
                   
      virtual int getAlignmentLength()  { return algnLen; } // ==nr
      int  GetNCols2() { return nCols2; }

    protected :
      PXBlock  XBlock1,XBlock2;
      int      nBlock1,nBlock2;
      int      na1,na2,nCols1,nCols2,nRows,algnLen;

      mmdb::ivector  a1,a2;
      mmdb::PPAtom   alpha1,alpha2;
      mmdb::rvector  d1;
      mmdb::realtype maxdist;

      virtual void FreeMemory();
      virtual void customInit();
      int   makeXBlocks  ( mmdb::ivector Ca, int nat, RPXBlock XB,
                           int & nBlocks );
      void  alignXBlocks ( RXBlock B1, RXBlock B2, int & nr );

      virtual void makeRow ( mmdb::PAtom A1, int sseType1,
                             mmdb::PAtom A2, int sseType2,
                             mmdb::realtype dist, int rowNo, int icol,
                             bool  aligned );
  };


  //  -------------------------  XTAlign --------------------------

  DefineStructure(XTAlign);

  struct XTAlign  {
    mmdb::realtype hydropathy1,hydropathy2,dist;
    mmdb::ChainID  chID1,chID2;
    mmdb::ResName  resName1,resName2;
    mmdb::InsCode  insCode1,insCode2;
    int      alignKey; //!< 0: aligned, 1: not aligned, 2,3: NULL 1,2
    int      loopNo;
    int      sseType1,sseType2;
    int      seqNum1,seqNum2;
    int      serNum1,serNum2;
    int      simindex;
    
    void  print           ( mmdb::io::RFile   f, int serNumWidth=0 );
    void  put_rvapi       ( mmdb::cpstr tableId, int row, bool serNum );
    void  getAlignSymbols ( char & c1, char & c2 );
    
    static int getWriteSize();
    void mem_write ( mmdb::pstr  S, int & l );
    void mem_read  ( mmdb::cpstr S, int & l );

    static void csv_title ( mmdb::pstr S, mmdb::cpstr sep );
    void write_csv ( mmdb::pstr S, mmdb::cpstr sep );
    PJSON  getJSON ();

  };


  //  ------------------------  XAlignText -------------------------

  DefineClass(XAlignText);

  class XAlignText : public XAlign  {

    public :
      XAlignText ();
      virtual ~XAlignText();

      inline PXTAlign getTextRows() { return R; }
      PXTAlign takeTextRows  ();
      void     getAlignments ( mmdb::pstr & algn1, mmdb::pstr & algn2,
                               mmdb::rvector * dist = NULL );
      void     wipeTextRows  ();

    protected :
      PXTAlign R;

      void customFree();
      void customInit();
      void makeRow   ( mmdb::PAtom A1, int sseType1,
                       mmdb::PAtom A2, int sseType2,
                       mmdb::realtype dist, int rowNo, int icol,
                       bool aligned );
  };

}

#endif // GSMT_OUTPUT_H
