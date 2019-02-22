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
//    23.12.15   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  GSMT_Sequence <interface>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::Sequence
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2013-2015
//
// =================================================================
//

#ifndef __GSMT_Sequence__
#define __GSMT_Sequence__

#include "mmdb2/mmdb_defs.h"
#include "mmdb2/mmdb_io_stream.h"

// =================================================================

namespace gsmt {

  DefineClass(Structure);
  DefineClass(Sequence);

  class Sequence : public mmdb::io::Stream  {

    public :
    
      enum RETURN_CODE  {
        RC_Ok,
        RC_CantOpenFile,
        RC_WrongFormat,
        RC_NoSequence
      };

      Sequence ();
      Sequence ( mmdb::io::RPStream Object );
      ~Sequence();

      RETURN_CODE loadSequence ( mmdb::cpstr filePath ); 

      inline mmdb::cpstr  getIDCode  () { return id;    }
      inline mmdb::pstr   getTitle   () { return title; }
      inline mmdb::pstr   getSequence() { return seq;   }
      int  getSeqLength();

      void setSequence ( PStructure  structure );
      void setSequence ( mmdb::cpstr sequence  );

      mmdb::pstr getErrDesc ( RETURN_CODE rc, mmdb::pstr & S );

      void copy  ( PSequence S ); //!< deep-copy from S to this

      void read  ( mmdb::io::RFile f );
      void write ( mmdb::io::RFile f );

    protected:
      mmdb::IDCode id;
      mmdb::pstr   seq;
      mmdb::pstr   title;

      void initClass ();
      void freeMemory();

  };
  
}

#endif

