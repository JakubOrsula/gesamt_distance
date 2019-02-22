//
//  =================================================================
//
//    05.06.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_combobox  <implementation>
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

#include "rvapi_combobox.h"
#include "rvapi_tasks.h"

namespace rvapi  {

  class CbxOption  {

    public:
      std::string label;
      std::string value;
      bool        selected;
      bool        disabled;

      CbxOption();
      virtual ~CbxOption() {}

      virtual void write ( std::ofstream & s );
      virtual void read  ( std::ifstream & s );

      std::string & getOption ( std::string & s );

  };

  CbxOption::CbxOption()  {
    selected = false;
    disabled = false;
  }

  void CbxOption::write ( std::ofstream & s )  {
    swrite ( s,label    );
    swrite ( s,value    );
    swrite ( s,selected );
    swrite ( s,disabled );
  }

  void CbxOption::read  ( std::ifstream & s )  {
    sread ( s,label    );
    sread ( s,value    );
    sread ( s,selected );
    sread ( s,disabled );
  }

  std::string & CbxOption::getOption ( std::string & s )  {
    if (label.empty())  label = " ";
    if (value.empty())  value = " ";
    s = label + "+++" + value + "+++";
    if (selected)  s.append ( "yes+++" );
             else  s.append ( "no+++"  );
    if (disabled)  s.append ( "yes" );
             else  s.append ( "no"  );
    return s;
  }

}


rvapi::Combobox::Combobox ( const char * cbxId,
                            const char * name,
                            const char * onChange,
                            const int    size,
                            const int    hrow,
                            const int    hcol,
                            const int    hrowSpan,
                            const int    hcolSpan )
                : Node(cbxId,hrow,hcol,hrowSpan,hcolSpan)  {
  initCombobox();
  cbxName     = name;
  cbxOnChange = onChange;
  cbxSize     = size;
}

rvapi::Combobox::Combobox() : Node()  {
  initCombobox();
}

rvapi::Combobox::~Combobox()  {
  freeCombobox();
}

void rvapi::Combobox::initCombobox()  {
  cbxName    .clear();
  cbxOnChange.clear();
  cbxSize = 0;
}

void rvapi::Combobox::freeCombobox()  {
  for (int i=0;i<(int)options.size();i++)
    if (options.at(i))
      delete options.at(i);
  options.clear();
}


void rvapi::Combobox::addOption ( const char * label,
                                  const char * value,
                                  const bool   selected
                                )  {
CbxOption *opt = new CbxOption();
  opt->label    = label;
  opt->value    = value;
  opt->selected = selected;
  options.push_back ( opt );
  setModified();
}


void rvapi::Combobox::write ( std::ofstream & s )  {
int i,n,k;

  n = (int)options.size();
  swrite ( s,n );
  for (i=0;i<n;i++)
    if (options.at(i))  {
      k = 1;
      swrite ( s,k );
      options.at(i)->write ( s );
    } else  {
      k = 0;
      swrite ( s,k );
    }

  swrite ( s,cbxName     );
  swrite ( s,cbxOnChange );
  swrite ( s,cbxSize     );

  Node::write ( s );

}

void rvapi::Combobox::read ( std::ifstream & s )  {
CbxOption *opt;
int        i,n,k;

  freeCombobox();

  sread ( s,n );
  for (i=0;i<n;i++)  {
    sread ( s,k );
    if (k>0)  {
      opt = new CbxOption();
      opt->read ( s );
      options.push_back ( opt );
    }
  }

  sread ( s,cbxName     );
  sread ( s,cbxOnChange );
  sread ( s,cbxSize     );

  Node::read ( s );

}


void rvapi::Combobox::flush_html ( std::string & outDir,
                                   std::string & task )  {

  if (wasCreated() || wasModified())  {
    std::string gridPos;
    std::string cbxOptions;

    for (int i=0;i<(int)options.size();i++)
      if (options.at(i))  {
        if (!cbxOptions.empty())
          cbxOptions.append ( "====" );  // delimiter
        cbxOptions.append ( options.at(i)->getOption(gridPos) );
      }
    add_combobox_grid ( task,nodeId(),cbxName,cbxOptions,cbxOnChange,
                        cbxSize,parent->nodeId(),
                        gridPosition(gridPos) );
    Node::flush_html ( outDir,task );

  }

}


void rvapi::Combobox::make_xmli2_content ( std::string & tag,
                                           std::string & content )  {
std::string s,cbxOptions;

  tag = "combobox";

  for (int i=0;i<(int)options.size();i++)
    if (options.at(i))  {
      if (!cbxOptions.empty())
        cbxOptions.append ( "====" );  // delimiter
      cbxOptions.append ( options.at(i)->getOption(s) );
    }

  content = "<name>" + cbxName + "</name>\n" +
            "<options>" + cbxOptions + "</options>\n";

}
