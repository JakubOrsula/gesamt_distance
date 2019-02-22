//
//  =================================================================
//
//    06.03.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_input  <implementation>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Classes :  rvapi::Input - API Input class
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2014
//
//  =================================================================
//

#include "rvapi_input.h"
#include "rvapi_tasks.h"

rvapi::Input::Input ( const char * inpId,
                      const int    hrow,
                      const int    hcol,
                      const int    hrowSpan,
                      const int    hcolSpan  )
            : Node(inpId,hrow,hcol,hrowSpan,hcolSpan)  {
  initInput();
}

rvapi::Input::Input() : Node()  {
  initInput();
}

rvapi::Input::~Input()  {
  freeInput();
}

void rvapi::Input::initInput()  {
  itype = NTYPE_Input;
  ititle   .clear();
  iname    .clear();
  ivalue   .clear();
  idata    .clear();
  ionChange.clear();
  isize           = 0;
  irequired       = false;
  irvOnly         = false;
  setModified();
}

void rvapi::Input::freeInput()  {}


void rvapi::Input::makeFileUpload ( const char * name,
                                    const char * value,
                                    const int    length,
                                    const bool   required )  {
  initInput();
  itype     = NTYPE_FileUpload;
  iname     = name;
  ivalue    = value;
  isize     = length;
  irequired = required;
}


void rvapi::Input::makeLineEdit ( const char * name,
                                  const char * text,
                                  const int    length )  {
  initInput();
  itype  = NTYPE_LineEdit;
  iname  = name;
  ivalue = text;
  isize  = length;
}

void rvapi::Input::makeHiddenText ( const char * name,
                                    const char * text )  {
  initInput();
  itype  = NTYPE_Hidden;
  iname  = name;
  ivalue = text;
}

void rvapi::Input::makeSubmitButton ( const char * title,
                                      const char * formAction )  {

  initInput();
  itype     = NTYPE_SubmitButton;
  ivalue    = title;
  icommand  = formAction;
}

void rvapi::Input::makeButton ( const char * title,
                                const char * command,
                                const char * data,
                                const bool   rvOnly )  {
  initInput();
  itype     = NTYPE_Button;
  ivalue    = title;
  icommand  = command;
  idata     = data;
  irvOnly   = rvOnly;
}

void rvapi::Input::makeCheckbox ( const char * title,
                                  const char * name,
                                  const char * value,
                                  const char * command,
                                  const char * data,
                                  const bool   checked )  {
  initInput();
  itype     = NTYPE_Checkbox;
  ititle    = title;
  iname     = name;
  ivalue    = value;
  icommand  = command;
  idata     = data;
  ichecked  = checked;
}

void rvapi::Input::makeRadioButton ( const char * title,
                                     const char * name,
                                     const char * value,
                                     const bool   checked )  {
  initInput();
  itype     = NTYPE_RadioButton;
  ititle    = title;
  iname     = name;
  ivalue    = value;
  ichecked  = checked;
}

void rvapi::Input::setOnClick ( const char * onClick )  {
  idata = onClick;
  setModified();
}

void rvapi::Input::setOnChange ( const char * onChange )  {
  ionChange = onChange;
  setModified();
}

void rvapi::Input::write ( std::ofstream & s )  {
int ntype = itype;
  swrite ( s,ntype     );
  swrite ( s,ititle    );
  swrite ( s,iname     );
  swrite ( s,ivalue    );
  swrite ( s,icommand  );
  swrite ( s,idata     );
  swrite ( s,ionChange );
  swrite ( s,irequired );
  swrite ( s,irvOnly   );
  swrite ( s,isize     );
  swrite ( s,ichecked  );
  Node::write ( s );
}

void rvapi::Input::read ( std::ifstream & s )  {
int ntype;
  freeInput();
  sread ( s,ntype     );
  sread ( s,ititle    );
  sread ( s,iname     );
  sread ( s,ivalue    );
  sread ( s,icommand  );
  sread ( s,idata     );
  sread ( s,ionChange );
  sread ( s,irequired );
  sread ( s,irvOnly   );
  sread ( s,isize     );
  sread ( s,ichecked  );
  itype = (NODE_TYPE)ntype;
  Node::read ( s );
}

void rvapi::Input::flush_html ( std::string & outDir,
                                std::string & task )  {
std::string pos;
  if (wasCreated())  {
    switch (itype)  {
      case NTYPE_FileUpload:
             add_file_upload   ( task,nodeId(),iname,ivalue,isize,
                                 irequired,ionChange,parent->nodeId(),
                                 gridPosition(pos) );
           break;
      case NTYPE_LineEdit:
             add_line_edit     ( task,nodeId(),iname,ivalue,isize,
                                 parent->nodeId(),gridPosition(pos) );
           break;
      case NTYPE_Hidden:
             add_hidden_text   ( task,nodeId(),iname,ivalue,
                                 parent->nodeId(),gridPosition(pos) );
           break;
      case NTYPE_SubmitButton:
             add_submit_button ( task,nodeId(),ivalue,icommand,
                                 parent->nodeId(),gridPosition(pos) );
           break;
      case NTYPE_Button:
             add_button_grid   ( task,nodeId(),ivalue,icommand,idata,
                                 irvOnly,parent->nodeId(),
                                 gridPosition(pos) );
           break;
      case NTYPE_Checkbox:
             add_checkbox_grid ( task,nodeId(),ititle,iname,ivalue,
                                 icommand,idata,ichecked,ionChange,
                                 parent->nodeId(),gridPosition(pos) );
           break;
      case NTYPE_RadioButton:
             add_radio_button_grid ( task,nodeId(),ititle,iname,ivalue,
                                     ichecked,ionChange,parent->nodeId(),
                                     gridPosition(pos) );
           break;
      default : ;
    }
  }
  Node::flush_html ( outDir,task );
}

