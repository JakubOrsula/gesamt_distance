//
//  =================================================================
//
//    06.03.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_form  <interface>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Classes :  rvapi::Form - API form class
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2014
//
//  =================================================================
//

#ifndef RVAPI_FORM_H
#define RVAPI_FORM_H

#include "rvapi_node.h"

namespace rvapi  {

  class Form : public Node  {

    public:
      Form ( const char * formId,
             const char * action,
             const char * method = "POST",
             int  hrow           = -1,
             int  hcol           = 0,
             int  hrowSpan       = 1,
             int  hcolSpan       = 1
           );
      Form ();
      ~Form();

      virtual NODE_TYPE type() { return NTYPE_Form; }

      virtual void write ( std::ofstream & s );
      virtual void read  ( std::ifstream & s );

      virtual void flush_html ( std::string & outDir,
                                std::string & task );

  protected:
    std::string form_action;
    std::string form_method;

    void initForm();
    void freeForm();

  };

}

#endif // RVAPI_FORM_H
