//
//  =================================================================
//
//    06.02.17   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_progressbar  <implementation>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Classes :  rvapi::ProgressBar - API Progress Bar class
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2013-2017
//
//  =================================================================
//

#include "rvapi_progressbar.h"
#include "rvapi_tasks.h"


rvapi::ProgressBar::ProgressBar ( const char * pbarId,
                                  int hrow,     int hcol,
                                  int hrowSpan, int hcolSpan )
                  : Node(pbarId,hrow,hcol,hrowSpan,hcolSpan)  {
  initProgressBar();
}

rvapi::ProgressBar::ProgressBar() : Node()  {
  initProgressBar();
}

rvapi::ProgressBar::~ProgressBar()  {
  freeProgressBar();
}

void rvapi::ProgressBar::initProgressBar()  {
  range     = 100;
  pbr_width = 200;
}

void rvapi::ProgressBar::freeProgressBar() {}

void rvapi::ProgressBar::setProgressBar ( PBAR_KEY key, int value )  {

  switch (key)  {
    default :
    case ProgressBar::Hide  :
              set_progress_bar ( tasks,nodeId(),"HIDE" ,value ); break;
    case ProgressBar::Show  :
              set_progress_bar ( tasks,nodeId(),"SHOW" ,value ); break;
    case ProgressBar::Range :
              set_progress_bar ( tasks,nodeId(),"RANGE",value );
              range = value;
            break;
    case ProgressBar::Value :
              set_progress_bar ( tasks,nodeId(),"VALUE",value ); break;
  }

}

void rvapi::ProgressBar::write ( std::ofstream & s )  {
  swrite ( s,tasks     );
  swrite ( s,range     );
  swrite ( s,pbr_width );
  Node::write ( s );
}

void rvapi::ProgressBar::read ( std::ifstream & s )  {
  freeProgressBar();
  sread ( s,tasks     );
  sread ( s,range     );
  sread ( s,pbr_width );
  Node::read ( s );
}

void rvapi::ProgressBar::flush_html ( std::string & outDir,
                                      std::string & task )  {
std::string pos;

  if (wasCreated() && (parent->type()!=NTYPE_Document))
    add_progress_bar ( task,nodeId(),range,pbr_width,parent->nodeId(),
                       gridPosition(pos) );

  task.append ( tasks );
  tasks.clear();

  Node::flush_html ( outDir,task );

}

