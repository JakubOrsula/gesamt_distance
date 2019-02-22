//
//  =================================================================
//
//    06.03.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_tab  <interface>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Classes :  rvapi::Tab - API Tab class
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2013-2014
//
//  =================================================================
//

#ifndef RVAPI_TAB_H
#define RVAPI_TAB_H

#include "rvapi_node.h"

namespace rvapi  {

  class Tab : public Node  {

    public:
      Tab ( const char * tabId, const char * tabName );
      Tab ();
      ~Tab();

      virtual NODE_TYPE type() { return NTYPE_Tab; }

      inline void setOpen ( bool open )  { createOpened = open; }

      virtual void write ( std::ofstream & s );
      virtual void read  ( std::ifstream & s );

      virtual void flush_html ( std::string & outDir,
                                std::string & task );

  protected:
    std::string   name; //!< to appear on the tab
    bool  createOpened; //!< tab to appear opened

    void initTab();
    void freeTab();

    virtual void make_xmli2_content ( std::string & tag,
                                      std::string & content );

  };

}

#endif // RVAPI_TAB_H
