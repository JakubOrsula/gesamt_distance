//
//  =================================================================
//
//    06.03.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_docheader  <implementation>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Classes :  rvapi::DocHeader - API Section class
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2013-2014
//
//  =================================================================
//

#include "rvapi_docheader.h"
#include "rvapi_tasks.h"

rvapi::DocHeader::DocHeader ( const char * hdrId,
                              const char * htmlString )
                : Node(hdrId)  {
  initHeader();
  header = htmlString;
}

rvapi::DocHeader::DocHeader() : Node()  {
  initHeader();
}

rvapi::DocHeader::~DocHeader()  {
  freeHeader();
}

void rvapi::DocHeader::initHeader()  {}

void rvapi::DocHeader::freeHeader()  {}

void rvapi::DocHeader::setHeader ( std::string & h )  {
  header = h;
  setModified();
}

void rvapi::DocHeader::setHeader ( const char * h )  {
  header = h;
  setModified();
}

void rvapi::DocHeader::write ( std::ofstream & s )  {
  swrite ( s,header );
  Node::write ( s );
}

void rvapi::DocHeader::read  ( std::ifstream & s )  {
  freeHeader();
  sread ( s,header );
  Node::read ( s );
}

void rvapi::DocHeader::flush_html ( std::string & outDir,
                                    std::string & task )  {
  if (wasCreated() || wasModified())
    set_header ( task,header );
  Node::flush_html ( outDir,task );
}

