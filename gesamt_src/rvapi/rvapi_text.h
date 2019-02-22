//
//  =================================================================
//
//    06.02.17   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_text  <interface>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Classes :  rvapi::Text - API Text class
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2013-2017
//
//  =================================================================
//

#ifndef RVAPI_TEXT_H
#define RVAPI_TEXT_H

#include "rvapi_node.h"

namespace rvapi  {

  class Text : public Node  {

    public:
      Text ( const char * nodeId,
             const char * textString,
             const int    hrow     = -1,
             const int    hcol     = 0,
             const int    hrowSpan = 1,
             const int    hcolSpan = 1
           );
      Text ();
      ~Text();

      virtual NODE_TYPE type() { return NTYPE_Text; }

      inline  void setAppend ( const bool a )  { append = a; }
      
      void setText ( const char * newText );

      virtual void write ( std::ofstream & s );
      virtual void read  ( std::ifstream & s );

      virtual void flush_html ( std::string & outDir,
                                std::string & task );

  protected:
    std::string text;   //!< to appear in the document
    bool        append; //!< appends to holder if true

    void initText();
    void freeText();

    virtual void make_xmli2_content ( std::string & tag,
                                      std::string & content );

  };

}

#endif // RVAPI_TEXT_H
