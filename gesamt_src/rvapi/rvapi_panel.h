//
//  =================================================================
//
//    06.03.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_panel  <interface>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Classes :  rvapi::Panel - API panel class
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2014
//
//  =================================================================
//

#ifndef RVAPI_PANEL_H
#define RVAPI_PANEL_H

#include "rvapi_node.h"

namespace rvapi  {

  class Panel : public Node  {

    public:
      Panel ( const char * panelId,
              int  hrow     = -1,
              int  hcol     = 0,
              int  hrowSpan = 1,
              int  hcolSpan = 1
            );
      Panel ();
      ~Panel();

      virtual NODE_TYPE type() { return NTYPE_Panel; }

      virtual void write ( std::ofstream & s );
      virtual void read  ( std::ifstream & s );

      virtual void flush_html ( std::string & outDir,
                                std::string & task );

  protected:

    void initPanel();
    void freePanel();

  };

}

#endif // RVAPI_PANEL_H
