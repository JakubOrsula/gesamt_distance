//
//  =================================================================
//
//    06.03.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_docheader  <interface>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Classes :  rvapi::DocHeader - API Section class
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2013-2014
//
//  =================================================================
//

#ifndef RVAPI_DOCHEADER_H
#define RVAPI_DOCHEADER_H

#include "rvapi_node.h"

namespace rvapi  {

  class DocHeader : public Node  {

    public:
      DocHeader ( const char * hdrId, const char * htmlString );
      DocHeader ();
      ~DocHeader();

      virtual NODE_TYPE type() { return NTYPE_DocHeader; }
      
      void setHeader ( std::string & h );
      void setHeader ( const char *  h );

      virtual void write ( std::ofstream & s );
      virtual void read  ( std::ifstream & s );

      virtual void flush_html ( std::string & outDir,
                                std::string & task );

  protected:
    std::string header; //!< to appear in the header

    void initHeader();
    void freeHeader();

  };

}

#endif // RVAPI_DOCHEADER_H
