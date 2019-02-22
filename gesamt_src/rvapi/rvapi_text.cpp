//
//  =================================================================
//
//    06.02.17   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_text  <implementation>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Classes :  rvapi::Text - API Text class
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2013-2017
//
//  =================================================================
//

#include "rvapi_text.h"
#include "rvapi_tasks.h"

rvapi::Text::Text ( const char * nodeId, const char * textString,
                    int hrow, int hcol, int hrowSpan, int hcolSpan  )
           : Node(nodeId,hrow,hcol,hrowSpan,hcolSpan)  {
  initText();
  text = textString;
}

rvapi::Text::Text() : Node()  {
  initText();
}

rvapi::Text::~Text()  {
  freeText();
}

void rvapi::Text::initText()  {
  append = true;
}

void rvapi::Text::freeText()  {}

void rvapi::Text::setText ( const char * newText )  {
  text = newText;
  setModified();
}

void rvapi::Text::write ( std::ofstream & s )  {
  swrite ( s,text   );
  swrite ( s,append );
  Node::write ( s );
}

void rvapi::Text::read ( std::ifstream & s )  {
  freeText();
  sread ( s,text   );
  sread ( s,append );
  Node::read ( s );
}

void rvapi::Text::flush_html ( std::string & outDir,
                               std::string & task )  {

  if (wasCreated())  {
    std::string gridPos;
    gridPosition ( gridPos );
    if ((text.find(key_ter)!=std::string::npos) ||
        (text.find(key_del)!=std::string::npos) ||
        (text.size()>inline_size_threshold))  {

      std::string fname;
      std::string subtask;
      Node::flush_html ( outDir,subtask );
      if (!subtask.empty())
           subtask = makeContent ( fname,subtask,outDir,parent->nodeId() +
                                              "_" + gridPos + ".tsk" );
      else {
        subtask = " ";
        std::ofstream s;
        std::string   fn;
        fname = getNodeFName ( fname,parent->nodeId(),"part" );
        fn    = outDir + fname;
        s.open ( fn.c_str() );
        s << text;
        s.close();
      }

      load_content_grid ( task,fname,false,subtask,parent->nodeId(),
                                                             gridPos );
 
    } else if (append)  {
      add_html_grid ( task,text,parent->nodeId(),gridPos );
      Node::flush_html ( outDir,task );
    } else  {
      set_html_grid ( task,text,parent->nodeId(),gridPos );
      Node::flush_html ( outDir,task );
    }
  } else if (wasModified())  {
    std::string gridPos;
    gridPosition ( gridPos );
    set_html_grid ( task,text,parent->nodeId(),gridPos );
    Node::flush_html ( outDir,task );  
  }

}

void rvapi::Text::make_xmli2_content ( std::string & tag,
                                       std::string & content )  {
  tag     = "text";
  content = text + "\n";
}
