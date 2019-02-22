//
//  =================================================================
//
//    06.03.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_section  <implementation>
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

#include "rvapi_section.h"
#include "rvapi_tasks.h"

rvapi::Section::Section ( const char *secId,
                          const char * secTitle,
                          int hrow, int hcol,
                          int hrowSpan, int hcolSpan  )
              : Node(secId,hrow,hcol,hrowSpan,hcolSpan)  {
  initSection();
  name = secTitle;
}

rvapi::Section::Section() : Node()  {
  initSection();
}

rvapi::Section::~Section()  {
  freeSection();
}

void rvapi::Section::initSection()  {
  createOpened = true;
}

void rvapi::Section::freeSection()  {}

void rvapi::Section::write ( std::ofstream & s )  {
  swrite ( s,name         );
  swrite ( s,createOpened );
  Node::write ( s );
}

void rvapi::Section::read ( std::ifstream & s )  {
  freeSection();
  sread ( s,name         );
  sread ( s,createOpened );
  Node::read ( s );
}

void rvapi::Section::flush_html ( std::string & outDir,
                                  std::string & task )  {
std::string pos;
  if (wasCreated())
    add_section ( task,nodeId(),name,parent->nodeId(),
                  createOpened,gridPosition(pos) );
  Node::flush_html ( outDir,task );
}

void rvapi::Section::make_xmli2_content ( std::string & tag,
                                          std::string & content )  {
  tag     = "section";
  content = "<name>" + name + "</name>\n" +
            "<open>" + bool2str(createOpened) + "</open>\n";
}
