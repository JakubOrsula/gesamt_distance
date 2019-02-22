//
//  =================================================================
//
//    06.03.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_inpur  <interface>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Classes :  rvapi::Input - API Input class
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2014
//
//  =================================================================
//

#ifndef RVAPI_INPUT_H
#define RVAPI_INPUT_H

#include "rvapi_node.h"

namespace rvapi  {

  class Input : public Node  {

    public:
      Input ( const char * inpId,
              const int    hrow     = -1,
              const int    hcol     = 0,
              const int    hrowSpan = 1,
              const int    hcolSpan = 1
            );
      Input ();
      ~Input();

      virtual NODE_TYPE type() { return itype; }

      void makeFileUpload   ( const char * name,
                              const char * value,
                              const int    length,
                              const bool   required
                            );

      void makeLineEdit     ( const char * name,
                              const char * text,
                              const int    length );

      void makeHiddenText   ( const char * name,
                              const char * text );

      void makeSubmitButton ( const char * title,
                              const char * formAction );

      void makeButton       ( const char * title,
                              const char * command,
                              const char * data,
                              const bool   rvOnly );

      void makeCheckbox     ( const char * title,
                              const char * name,
                              const char * value,
                              const char * command,
                              const char * data,
                              const bool   checked );

      void makeRadioButton  ( const char * title,
                              const char * name,
                              const char * value,
                              const bool   checked );

      void setOnClick       ( const char * onClick  );
      void setOnChange      ( const char * onChange );

      virtual void write ( std::ofstream & s );
      virtual void read  ( std::ifstream & s );

      virtual void flush_html ( std::string & outDir,
                                std::string & task );

  protected:
    NODE_TYPE   itype;
    std::string ititle;
    std::string iname;
    std::string ivalue;
    std::string icommand;
    std::string idata;
    std::string ionChange;
    int         isize;   //!< in characters
    bool        irequired,irvOnly,ichecked;

    void initInput();
    void freeInput();

  };

}

#endif // RVAPI_INPUT_H
