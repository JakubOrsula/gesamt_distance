//
//  =================================================================
//
//    06.03.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_section  <interface>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Classes :  rvapi::Section - API Section class
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2013-2014
//
//  =================================================================
//

#ifndef RVAPI_SECTION_H
#define RVAPI_SECTION_H

#include "rvapi_node.h"

namespace rvapi  {

  class Section : public Node  {

    public:
      Section ( const char * secId,
                const char * secTitle,
                int hrow     = -1,
                int hcol     = 0,
                int hrowSpan = 1,
                int hcolSpan = 1
              );
      Section ();
      ~Section();

      virtual NODE_TYPE type() { return NTYPE_Section; }

      inline void setOpen ( bool open )  { createOpened = open; }

      virtual void write ( std::ofstream & s );
      virtual void read  ( std::ifstream & s );

      virtual void flush_html ( std::string & outDir,
                                std::string & task );

  protected:
    std::string   name; //!< to appear on the accordion
    bool  createOpened; //!< tab to appear opened

    void initSection();
    void freeSection();

    virtual void make_xmli2_content ( std::string & tag,
                                      std::string & content );

  };

}

#endif // RVAPI_SECTION_H
