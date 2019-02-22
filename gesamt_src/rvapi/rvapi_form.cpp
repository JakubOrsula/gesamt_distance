//
//  =================================================================
//
//    06.03.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_form  <implementation>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Classes :  rvapi::Form - API Form class
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2014
//
//  =================================================================
//

#include "rvapi_form.h"
#include "rvapi_tasks.h"

rvapi::Form::Form ( const char *formId, const char * action,
                    const char *method,
                    int hrow, int hcol, int hrowSpan, int hcolSpan  )
           : Node(formId,hrow,hcol,hrowSpan,hcolSpan)  {
  initForm();
  form_action = action;
  form_method = method;
}

rvapi::Form::Form() : Node()  {
  initForm();
}

rvapi::Form::~Form()  {
  freeForm();
}

void rvapi::Form::initForm()  {}

void rvapi::Form::freeForm()  {}

void rvapi::Form::write ( std::ofstream & s )  {
  swrite ( s,form_action );
  swrite ( s,form_method );
  Node::write ( s );
}

void rvapi::Form::read ( std::ifstream & s )  {
  freeForm();
  sread ( s,form_action );
  sread ( s,form_method );
  Node::read ( s );
}

void rvapi::Form::flush_html ( std::string & outDir,
                               std::string & task )  {
std::string pos;
  if (wasCreated())
    add_form ( task,nodeId(),form_action,form_method,
               parent->nodeId(),gridPosition(pos) );
  Node::flush_html ( outDir,task );
}

