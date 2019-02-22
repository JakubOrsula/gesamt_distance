//
//  =================================================================
//
//    15.07.17   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_fieldset  <interface>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Classes :  rvapi::Fieldset - API fieldset class
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2014-2017
//
//  =================================================================
//

#ifndef RVAPI_FIELDSET_H
#define RVAPI_FIELDSET_H

#include "rvapi_node.h"

namespace rvapi  {

  class Fieldset : public Node  {

    public:
      Fieldset ( const char * fsetId,
                 const char * title,
                 int  hrow     = -1,
                 int  hcol     = 0,
                 int  hrowSpan = 1,
                 int  hcolSpan = 1
               );
      Fieldset ();
      ~Fieldset();

      virtual NODE_TYPE type() { return NTYPE_Fieldset; }

      virtual void write ( std::ofstream & s );
      virtual void read  ( std::ifstream & s );

      virtual void flush_html ( std::string & outDir,
                                std::string & task );

  protected:
    std::string legend;

    void initFieldset();
    void freeFieldset();

  };

}

#endif // RVAPI_FIELDSET_H
