//
//  =================================================================
//
//    06.03.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_panel  <implementation>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Classes :  rvapi::Panel - API Panel class
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2014
//
//  =================================================================
//

#include "rvapi_panel.h"
#include "rvapi_tasks.h"

rvapi::Panel::Panel ( const char *panelId,
                      int hrow, int hcol, int hrowSpan, int hcolSpan  )
            : Node(panelId,hrow,hcol,hrowSpan,hcolSpan)  {
  initPanel();
}

rvapi::Panel::Panel() : Node()  {
  initPanel();
}

rvapi::Panel::~Panel()  {
  freePanel();
}

void rvapi::Panel::initPanel()  {}

void rvapi::Panel::freePanel()  {}

void rvapi::Panel::write ( std::ofstream & s )  {
  Node::write ( s );
}

void rvapi::Panel::read ( std::ifstream & s )  {
  freePanel();
  Node::read ( s );
}

void rvapi::Panel::flush_html ( std::string & outDir,
                                std::string & task )  {
std::string pos;
  if (wasCreated())
    add_panel ( task,nodeId(),parent->nodeId(),gridPosition(pos) );
  Node::flush_html ( outDir,task );
}

