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
//  **** Module  :  GSMT_Structure <interface>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::Structure
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2013-2014
//
// =================================================================
//

#ifndef __GSMT_Structure__
#define __GSMT_Structure__

#include "mmdb2/mmdb_manager.h"
#include "json_.h"

// =================================================================

namespace gsmt {

  DefineClass(Structure);

  class Structure : public mmdb::io::Stream  {

    public :

      Structure ();
      Structure ( mmdb::io::RPStream Object );
      ~Structure();

      mmdb::ERROR_CODE getStructure  ( mmdb::cpstr fname,
                                       mmdb::cpstr select,
                                       int         chainNo,
                                       bool        SCOPSelSyntax );
      mmdb::ERROR_CODE getStructure  ( mmdb::PManager MMDB,
                                       mmdb::cpstr select,
                                       int         chainNo,
                                       bool        SCOPSelSyntax );
      mmdb::pstr getErrDesc ( mmdb::ERROR_CODE rc, mmdb::pstr & S );

      bool   isCrystInfo();

      inline mmdb::realtype getRadius() { return R; }
      
      void   prepareStructure ( mmdb::realtype maxContact );
      void   fetchData        ( mmdb::PPAtom  & Atoms, int & nat,
                                mmdb::rmatrix & DMat );
      void   selectSubStructure ( mmdb::cpstr CID );

      void   selectAtom       ( mmdb::PAtom         atom,
                                mmdb::SELECTION_KEY selKey );
      void   finishAtomSelection();

      int    getNCalphas   ();
      void   getCalphas    ( mmdb::PPAtom & Atoms, int & nat );
      inline mmdb::PManager getMMDBManager()  { return M;       }
      inline int            getDomainSel  ()  { return selDHnd; }
      mmdb::PManager getSelectedStructure ( mmdb::SELECTION_TYPE type );

      void   getChainRes   ( mmdb::PPResidue & Res, int & nRes );
      inline mmdb::realtype getERadius ()  { return R;        }
      inline mmdb::cpstr    getFilePath()  { return filePath; }

      void   getCentroid  ( mmdb::realtype & x,
                            mmdb::realtype & y,
                            mmdb::realtype & z );
      bool   getCentroidF ( mmdb::realtype & xf,
                            mmdb::realtype & yf,
                            mmdb::realtype & zf );
      
      mmdb::cpstr getSelString   ()  { return selection; }
      bool        isSCOPSelection()  { return scopSel;   }
      
      void   setOutFile ( mmdb::cpstr fname, mmdb::cpstr note );
      inline mmdb::cpstr getOutputFile()  { return outFile; }
      inline mmdb::cpstr getOutputNote()  { return outNote; }
      void   setRefName ( mmdb::cpstr rname );
      inline mmdb::cpstr getRefName   ()  { return refName; }
      mmdb::cpstr getPDBTitle ( mmdb::pstr & title );
      void   getCoordSequence ( mmdb::pstr & seq   );

      void   copy ( PStructure S ); //!< deep-copy from S to this

      void   writeSelectedCoordinates ( mmdb::cpstr outFile );

      void   read  ( mmdb::io::RFile f );
      void   write ( mmdb::io::RFile f );

    protected:
      mmdb::SegID    refName;   //!< reference name for output
      mmdb::PManager M;         //!< mmdb instance
      mmdb::PPAtom   A;         //!< selected atoms
      mmdb::rmatrix  D;         //!< distance matrix
      mmdb::realtype xc,yc,zc;  //!< mass center
      mmdb::realtype R;         //!< embedding radius
      mmdb::pstr     selection; //!< selection string
      mmdb::pstr     filePath;  //!< path to file read
      mmdb::pstr     outFile;   //!< path to output file
      mmdb::pstr     outNote;   //!< output note
      int            natoms;    //!< number of selected atoms
      bool           scopSel;   //!< true if SCOP selection
      int            selDHnd;   //!< domain selection
      int            selAHnd;   //!< Ca-selection
      int            selRHnd;   //!< residue selection

      void initClass ();
      void freeMemory();

      mmdb::ERROR_CODE analyseStructure (
                              mmdb::PManager MMDB,
                              mmdb::cpstr    select,
                              int            chainNo,
                              bool           SCOPSelSyntax );

      void selectCalphasSCOP ( mmdb::cpstr select );
      void selectCalphasCID  ( mmdb::cpstr CID    );
      void deselectAltLocs();
      
    private:
      int nd_alloc;

  };
  
  void printStructureSummary     ( PPStructure s, int nStruct,
                                   bool isOutput );

  void makeStructureSummary_rvapi ( mmdb::cpstr tableId,
                                    PPStructure s,
                                    int         nStruct );

//  void writeStructureSummary     ( mmdb::io::RFile f,
//                                   PPStructure s, int nStruct,
//                                   bool isOutput );

  void writeStructureSummary_csv ( mmdb::io::RFile f,
                                   PPStructure s, int nStruct,
                                   bool isOutput );
  void addStructureSummaryJSON   ( gsmt::RJSON json,
                                   PPStructure s, int nStruct,
                                   bool        isOutput );

}

extern void printMat4 ( mmdb::cpstr title, mmdb::mat44 & T );

#endif

