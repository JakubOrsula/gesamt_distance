//
//  =================================================================
//
//    05.06.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_combobox  <interface>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Classes :  rvapi::Combobox - API Combobox class
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2013-2014
//
//  =================================================================
//

#ifndef RVAPI_COMBOBOX_H
#define RVAPI_COMBOBOX_H

#include "rvapi_node.h"

namespace rvapi  {

  class CbxOption;

  class Combobox : public Node  {

    public:
      Combobox ( const char * cbxId,
                 const char * name,
                 const char * onChange,     //!< js onChange code
                 const int    size     = 0, //!< number of displayed options
                 const int    hrow     = -1,
                 const int    hcol     = 0,
                 const int    hrowSpan = 1,
                 const int    hcolSpan = 1
               );
      Combobox ();
      ~Combobox();

      virtual NODE_TYPE type() { return NTYPE_Combobox; }

      void addOption ( const char * label,
                       const char * value,
                       const bool   selected
                     );

      virtual void write ( std::ofstream & s );
      virtual void read  ( std::ifstream & s );

      virtual void flush_html  ( std::string & outDir,
                                 std::string & task );

  protected:
    std::vector<CbxOption *> options;
    std::string              cbxName;
    std::string              cbxOnChange; // js function for onChange
    int                      cbxSize;     // number of displayed options

    void initCombobox();
    void freeCombobox();

    virtual void make_xmli2_content ( std::string & tag,
                                      std::string & content );

  };

}

#endif // RVAPI_COMBOBOX_H
