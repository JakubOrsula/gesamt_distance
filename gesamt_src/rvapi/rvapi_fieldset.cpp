//
//  =================================================================
//
//    15.07.17   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_grid  <implementation>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Classes :  rvapi::Grid - API Grid class
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2013-2017
//
//  =================================================================
//

#include "rvapi_fieldset.h"
#include "rvapi_tasks.h"


rvapi::Fieldset::Fieldset ( const char *panelId, const char * title,
                            int hrow, int hcol, int hrowSpan, int hcolSpan  )
               : Node(panelId,hrow,hcol,hrowSpan,hcolSpan)  {
  initFieldset();
  legend = title;
}

rvapi::Fieldset::Fieldset() : Node()  {
  initFieldset();
}

rvapi::Fieldset::~Fieldset()  {
  freeFieldset();
}

void rvapi::Fieldset::initFieldset()  {}

void rvapi::Fieldset::freeFieldset()  {}

void rvapi::Fieldset::write ( std::ofstream & s )  {
  swrite ( s,legend );
  Node::write ( s );
}

void rvapi::Fieldset::read ( std::ifstream & s )  {
  freeFieldset();
  sread ( s,legend );
  Node::read ( s );
}

void rvapi::Fieldset::flush_html ( std::string & outDir,
                                   std::string & task )  {
std::string pos;
  if (wasCreated())
    add_fieldset ( task,nodeId(),legend,parent->nodeId(),gridPosition(pos) );
  Node::flush_html ( outDir,task );
}

