//
//  =================================================================
//
//    06.03.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_content  <interface>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Classes :  rvapi::Content - API Content class
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2013-2014
//
//  =================================================================
//

#ifndef RVAPI_CONTENT_H
#define RVAPI_CONTENT_H

#include "rvapi_node.h"

namespace rvapi  {

  class Content : public Node  {

    public:
      Content ( const char * contentUri,
                bool       watchContent,
                int hrow     = -1,
                int hcol     = 0,
                int hrowSpan = 1,
                int hcolSpan = 1
              );
      Content ();
      ~Content();

      virtual NODE_TYPE type() { return NTYPE_Content; }

      virtual void write ( std::ofstream & s );
      virtual void read  ( std::ifstream & s );

      virtual void flush_html ( std::string & outDir,
                                std::string & task );

  protected:
    std::string uri; //!< uri of content to display
    bool      watch; //!< whether to follow changes

    void initContent();
    void freeContent();

  };

}

#endif // RVAPI_CONTENT_H
