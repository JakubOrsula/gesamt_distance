//
//  =================================================================
//
//    06.03.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_tab  <implementation>
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

#include "rvapi_tab.h"

rvapi::Tab::Tab ( const char *tabId, const char * tabName )
          : Node(tabId)  {
  initTab();
  name = tabName;
}

rvapi::Tab::Tab() : Node()  {
  initTab();
}

rvapi::Tab::~Tab()  {
  freeTab();
}

void rvapi::Tab::initTab()  {
  createOpened = true;
}

void rvapi::Tab::freeTab()  {}

void rvapi::Tab::write ( std::ofstream & s )  {
  swrite ( s,name         );
  swrite ( s,createOpened );
  Node::write ( s );
}

void rvapi::Tab::read ( std::ifstream & s )  {
  freeTab();
  sread ( s,name         );
  sread ( s,createOpened );
  Node::read ( s );
}

void rvapi::Tab::flush_html ( std::string & outDir,
                              std::string & task )  {
  if (wasCreated())  {
    if (beforeId.empty())
         task.append ( add_tab_key key_del + nodeId() + key_del + name );
    else task.append ( insert_tab_key key_del + beforeId + key_del +
                                      nodeId() + key_del + name  );
    if (createOpened)  task.append ( key_del " true"  key_ter );
                 else  task.append ( key_del " false" key_ter );
  }
  Node::flush_html ( outDir,task );
}

void rvapi::Tab::make_xmli2_content ( std::string & tag,
                                      std::string & content )  {
  tag     = "tab";
  content = "<name>" + name + "</name>\n" +
            "<open>" + bool2str(createOpened) + "</open>\n";
}
