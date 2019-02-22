//
//  =================================================================
//
//    06.03.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_content  <implementation>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Classes :  rvapi::Content - API Content class
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2013-2014
//
//  =================================================================
//

#include "rvapi_content.h"
#include "rvapi_tasks.h"

rvapi::Content::Content ( const char *contentUri,
                          bool        watchContent,
                          int         hrow,
                          int         hcol,
                          int         hrowSpan,
                          int         hcolSpan  )
              : Node("",hrow,hcol,hrowSpan,hcolSpan)  {
  initContent();
  uri   = contentUri;
  watch = watchContent;
}

rvapi::Content::Content() : Node()  {
  initContent();
}

rvapi::Content::~Content()  {
  freeContent();
}

void rvapi::Content::initContent()  {
  watch = false;
}

void rvapi::Content::freeContent()  {}

void rvapi::Content::write ( std::ofstream & s )  {
  swrite ( s,uri   );
  swrite ( s,watch );
  Node::write ( s );
}

void rvapi::Content::read ( std::ifstream & s )  {
  freeContent();
  sread ( s,uri   );
  sread ( s,watch );
  Node::read ( s );
}

void rvapi::Content::flush_html ( std::string & outDir,
                                  std::string & task )  {

  if (wasCreated())  {
    std::string subtask;
    std::string gridPos;
    std::string content;
    gridPosition ( gridPos );
    Node::flush_html ( outDir,subtask );
    if (!subtask.empty())
         makeContent ( content,subtask,outDir,parent->nodeId() +
                                              "_" + gridPos + ".tsk" );
    else content = " ";
    load_content_grid ( task,uri,watch,content,parent->nodeId(),
                                                             gridPos );
  }

}
