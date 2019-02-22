//
//  =================================================================
//
//    17.07.17   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_document  <implementation>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Classes :  rvapi::Document - API main document
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2013-2017
//
//  =================================================================
//

#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>

#include "rvapi_document.h"
#include "rvapi_docheader.h"
#include "rvapi_content.h"
#include "rvapi_combobox.h"
#include "rvapi_data.h"
#include "rvapi_form.h"
#include "rvapi_loggraph.h"
#include "rvapi_fieldset.h"
#include "rvapi_grid.h"
#include "rvapi_input.h"
#include "rvapi_panel.h"
#include "rvapi_progressbar.h"
#include "rvapi_radar.h"
#include "rvapi_section.h"
#include "rvapi_tab.h"
#include "rvapi_table.h"
#include "rvapi_tasks.h"
#include "rvapi_text.h"
#include "rvapi_treewidget.h"
#include "rvapi_defs.h"

rvapi::Document::Document() : Node("")  {
  initDocument();
  makeJSLinks ();
  addNode ( new ProgressBar(toolbar_progressbar_id) );
}

rvapi::Document::~Document()  {
//  store ( NULL );
  if (stop_poll>0)
    stop_poll++;
  if (reset_cnt>=0)
    reset_cnt++;
  if (documentId[0])  // was initialised or restored
    flush();
  freeDocument();
}


// ====================  INITIALISATION  ======================

#undef __make_scene

void rvapi::Document::configure ( const char * docId,
                                  const char * outDir,
                                  const char * windowTitle,
                                  OUTPUT_MODE  outMode,
                                  LAYOUT_MODE  layMode,
                                  const char * jsUri,
                                  const char * helpFN,
                                  const char * htmlFN,
                                  const char * taskFN,
                                  const char * xmli2FN,
                                  const bool   initClean )  {
std::ofstream s;

  documentId = docId;

  outputDir  = outDir;
  if (outputDir[outputDir.size()-1]!='/')
    outputDir.append ( "/" );

  outputMode = outMode;
  layoutMode = layMode;
  
  winTitle   = windowTitle;
  if (jsUri)   jsrviewUri = jsUri;
  if (helpFN)  helpFName  = helpFN;
  if (htmlFN)  htmlFName  = htmlFN;
  if (taskFN)  taskFName  = taskFN;
  if (xmli2FN) xmli2FName = xmli2FN;
//  outputMode = OUTPUT_MODE(outMode | MODE_Xmli2);

  if (jsrviewUri[jsrviewUri.size()-1]!='/')
    jsrviewUri.append ( "/" );

  if (jsrviewUri.size()>3)  {
    if ((jsrviewUri.at(1)==':') &&
        ((jsrviewUri.at(2)=='\\') || (jsrviewUri.at(2)=='/')))
      jsrviewUri = "file:///" + jsrviewUri;
  }

  // write html template

  makeJSLinks();

  if (initClean)  {
    std::string fn = outputDir + taskFName;
    remove ( fn.c_str() );
    fn = outputDir + htmlFName;
    remove ( fn.c_str() );
    s.open ( fn.c_str() );

    s <<
"<!DOCTYPE html>\n"
"<html>\n"
"\n"
"  <head>\n"
"    <meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">\n"
"    <meta charset=\"utf-8\">\n"
"    <meta http-equiv=\"pragma\"  content=\"no-cache\">\n"
"    <meta http-equiv=\"expires\" content=\"0\">\n"
"    <title>" << windowTitle << "</title>\n"
"  </head>\n"
"\n";

    for (int i=0;i<(int)uri_css.size();i++)
      s << "  <link rel=\"stylesheet\" type=\"text/css\"\n"
           "       href=\"" << uri_css[i] << "\">\n";

    for (int i=0;i<(int)uri_js.size();i++)
      s << "  <script gesamt_src=\"" << uri_js[i] << "\"></script>\n";

    s <<
"  <!--[if lt IE 9]><script gesamt_src=\"" << jqPlotEx_js << "\"></script><![endif]-->\n"
"\n"
"  <body>\n"
#ifdef __make_scene
"\n"
"    <div id='__scene__'></div>\n"
#endif
"\n"
"    <script>\n"
"\n"
"      $(function() {\n"
"        $( document ).tooltip();\n"
"      });\n"
"\n"
"      $(document).ready(function()  {\n"
"        docURI         = \"" << docUri     << "\";\n"
"        programDocFile = \"" << helpFName  << "\";\n"
"        taskFile       = \"" << taskFName  << "\";\n"
"        _jsrview_uri   = \"" << jsrviewUri << "\";\n"
"        initPage ( ";

    if (layMode & LAYOUT_Header)            s << "1,";
    else if (layMode & LAYOUT_SlimHeader)   s << "2,";
                                     else   s << "0,";
    if (layMode & LAYOUT_Toolbar)  s << "true,";
                             else  s << "false,";
    if (layMode & LAYOUT_Tabs)     s << "true";
                          else     s << "false";

#ifdef __make_scene
//  s << ",'__scene__' );\n"
#else
    s << ",null );\n"
#endif
"        readTask();\n"
"      });\n"
"\n"
"    </script>\n"
"\n"
"  </body>\n"
"\n"
"</html>\n";

    s.close();

    fn = outputDir + xmli2FName;
    remove ( fn.c_str() );
    
  }

}


// ==================  FORMATION OF PAGE  =====================

const char * rvapi::Document::makeId ( std::string & nsId,
                                       const char * itemId )  {
// @-namespacing
  nsId = "";
  if (itemId)  {
    if (itemId[0])  {
      const char * p = itemId;
      while (*p)  {
        if (*p=='@')  nsId += documentId + "-";
                else  nsId += *p;
        p++;
      }
    }
  }
  return nsId.c_str();
}

void rvapi::Document::addHeader ( const char * htmlString )  {
DocHeader *docHeader;
  docHeader = (DocHeader *)findNode ( "__h_e_a_d_e_r__",NTYPE_DocHeader );
  if (!docHeader)  {
    docHeader = new DocHeader("__h_e_a_d_e_r__",htmlString);
    addNode ( docHeader );
  } else
    docHeader->setHeader ( htmlString );
}

void rvapi::Document::addTab ( const char * tabId,
                               const char * tabName,
                               bool         open )  {
std::string  nsId;
Tab        * tab;
  tab = (Tab *)findNode ( makeId(nsId,tabId),NTYPE_Tab );
  if (!tab)  {
    tab = new Tab(nsId.c_str(),tabName);
    addNode ( tab );
  }
  tab->setOpen ( open );
}

void rvapi::Document::insertTab ( const char * tabId,
                                  const char * tabName,
                                  const char * beforeTabId,
                                  bool         open )  {
std::string  nsId;
Tab        * tab;
  tab = (Tab *)findNode ( makeId(nsId,tabId),NTYPE_Tab );
  if (!tab)  {
    tab = new Tab(nsId.c_str(),tabName);
    addNode ( tab );
    tab->setBeforeId ( makeId(nsId,beforeTabId) );
  }
  tab->setOpen ( open );
}

void rvapi::Document::removeTab ( const char * tabId )  {
std::string  nsId;
Node       * node;
  node = findNode ( makeId(nsId,tabId),NTYPE_Tab );
  if (node)
    removeNode ( node );
  remove_tab ( tasks,nsId );
}

rvapi::Tab * rvapi::Document::findTab ( const char * tabId )  {
std::string  nsId;
  return (Tab *)findNode ( makeId(nsId,tabId),NTYPE_Tab );
}


void rvapi::Document::setCellStretch ( const char * gridWidgetId,
                                       const int    width,
                                       const int    height,
                                       const int    row,
                                       const int    col
                                      )  {
std::string  nsId;
Node        *node = findNode ( makeId(nsId,gridWidgetId) );
  if (node)  {
    if ((node->type()==NTYPE_Section) ||
        (node->type()==NTYPE_Tab)     ||
        (node->type()==NTYPE_Panel)   ||
        (node->type()==NTYPE_Fieldset))  {
      set_cell_stretch ( tasks,nsId,width,height,row,col );
    }
  }
}

void rvapi::Document::addSection ( const char * seqId,
                                   const char * seqTitle,
                                   const char * holderId,
                                   int          hrow,
                                   int          hcol,
                                   int          hrowSpan,
                                   int          hcolSpan,
                                   bool         open )  {
std::string  nsId;
Section    * section;
  section = new Section ( makeId(nsId,seqId),seqTitle,
                          hrow,hcol,hrowSpan,hcolSpan );
  if (addNode(section,makeId(nsId,holderId)))
        section->setOpen ( open );
  else  delete section;
}

void rvapi::Document::setSectionState ( const char * secId,
                                        bool         open )  {
std::string nsId;
Section    *section = (Section*)findNode ( makeId(nsId,secId),
                                           NTYPE_Section );
  if (section)  {
    section->setOpen ( open );
    set_section_state ( tasks,nsId,open );
  }
}


rvapi::Section * rvapi::Document::findSection ( const char * secId )  {
std::string  nsId;
  return (Section *)findNode ( makeId(nsId,secId),NTYPE_Section );
}


void rvapi::Document::addGrid ( const char * gridId,
                                bool         filling,
                                const char * holderId,
                                int          hrow,
                                int          hcol,
                                int          hrowSpan,
                                int          hcolSpan )  {
std::string  nsId;
Grid       * grid;
  grid = new Grid ( makeId(nsId,gridId),filling,
                    hrow,hcol,hrowSpan,hcolSpan );
  if (!addNode(grid,makeId(nsId,holderId)))
    delete grid;
}

rvapi::Grid * rvapi::Document::findGrid ( const char * gridId )  {
std::string  nsId;
  return (Grid *)findNode ( makeId(nsId,gridId),NTYPE_Grid );
}


void rvapi::Document::addFieldset ( const char * gridId,
                                    const char * title,
                                    const char * holderId,
                                    int          hrow,
                                    int          hcol,
                                    int          hrowSpan,
                                    int          hcolSpan )  {
std::string  nsId;
Fieldset   * fieldset;
  fieldset = new Fieldset ( makeId(nsId,gridId),title,
                            hrow,hcol,hrowSpan,hcolSpan );
  if (!addNode(fieldset,makeId(nsId,holderId)))
    delete fieldset;
}

rvapi::Fieldset * rvapi::Document::findFieldset ( const char * gridId )  {
std::string  nsId;
  return (Fieldset *)findNode ( makeId(nsId,gridId),NTYPE_Fieldset );
}


void rvapi::Document::addPanel ( const char * gridId,
                                 const char * holderId,
                                 int          hrow,
                                 int          hcol,
                                 int          hrowSpan,
                                 int          hcolSpan )  {
std::string  nsId;
Panel      * panel;
  panel = new Panel ( makeId(nsId,gridId), hrow,hcol,hrowSpan,hcolSpan );
  if (!addNode(panel,makeId(nsId,holderId)))
    delete panel;
}


// =====================  TEXTS AND LABELS  ==========================

void rvapi::Document::addText ( const char * labelId,
                                const char * textString,
                                const char * holderId,
                                int          hrow,
                                int          hcol,
                                int          hrowSpan,
                                int          hcolSpan
                              )  {
std::string  nsId;
Text * text = new Text ( makeId(nsId,labelId),textString,hrow,hcol,
                         hrowSpan,hcolSpan );
  if (!addNode(text,makeId(nsId,holderId)))
    delete text;
}

void rvapi::Document::setText ( const char * labelId,
                                const char * textString,
                                const char * holderId,
                                int          hrow,
                                int          hcol,
                                int          hrowSpan,
                                int          hcolSpan
                              )  {
std::string  nsId;
Text * text = new Text ( makeId(nsId,labelId),textString,hrow,hcol,
                         hrowSpan,hcolSpan );
  text->setAppend ( false );
  if (!addNode(text,makeId(nsId,holderId)))
    delete text;
}

void rvapi::Document::resetText ( const char * labelId,
                                  const char * newText )  {
std::string  nsId;
Text *text = (Text *)findNode ( makeId(nsId,labelId),NTYPE_Text );
  if (text)
    text->setText ( newText );
}


void rvapi::Document::addContent ( const char * uri,
                                   bool         watch,
                                   const char * holderId,
                                   int          hrow,
                                   int          hcol,
                                   int          hrowSpan,
                                   int          hcolSpan
                                 )  {

std::string  nsId;
Content * content = new Content ( uri,watch,hrow,hcol,hrowSpan,hcolSpan );
  if (!addNode(content,makeId(nsId,holderId)))
    delete content;
}


void rvapi::Document::addTable ( const char * tblId,
                                 const char * tblTitle,
                                 const char * holderId,
                                 int          hrow,
                                 int          hcol,
                                 int          hrowSpan,
                                 int          hcolSpan,
                                 int          foldState
                                )  {
std::string    nsId;
rvapi::Table * table;
  table = new Table ( makeId(nsId,tblId),tblTitle,
                      hrow,hcol,hrowSpan,hcolSpan );
  if (addNode(table,makeId(nsId,holderId)))
        table->setFoldState ( foldState );
  else  delete table;
}

rvapi::Table * rvapi::Document::findTable ( const char * tblId )  {
std::string  nsId;
  return (Table *)findNode ( makeId(nsId,tblId),NTYPE_Table );
}

void rvapi::Document::addData ( const char * datId,
                                const char * datTitle,
                                const char * uri,
                                const char * type,
                                const char * holderId,
                                int          hrow,
                                int          hcol,
                                int          hrowSpan,
                                int          hcolSpan,
                                int          foldState )  {
std::string  nsId;
Data * data = new Data ( makeId(nsId,datId),datTitle,
                         hrow,hcol,hrowSpan,hcolSpan );

  if (addNode(data,makeId(nsId,holderId)))  {
    data->addData ( uri,type );
    data->setFoldState ( foldState );
  } else
    delete data;

}

rvapi::Data * rvapi::Document::findData ( const char * datId )  {
std::string  nsId;
  return (Data *)findNode ( makeId(nsId,datId),NTYPE_Data );
}


void rvapi::Document::addLogGraph ( const char * gphId,
                                    const char * holderId,
                                    int          hrow,
                                    int          hcol,
                                    int          hrowSpan,
                                    int          hcolSpan )  {
std::string  nsId;
LogGraph * logGraph = new LogGraph ( makeId(nsId,gphId),
                                     hrow,hcol,hrowSpan,hcolSpan );
  if (!addNode(logGraph,makeId(nsId,holderId)))
    delete logGraph;
}

rvapi::LogGraph * rvapi::Document::findLogGraph ( const char * gphId ) {
std::string  nsId;
  return (LogGraph *)findNode ( makeId(nsId,gphId),NTYPE_LogGraph );
}

void rvapi::Document::addGraph ( const char * graphId,
                                 const char * holderId,
                                 int          hrow,
                                 int          hcol,
                                 int          hrowSpan,
                                 int          hcolSpan )  {
std::string  nsId;
Graph * graph = new Graph ( makeId(nsId,graphId),
                            hrow,hcol,hrowSpan,hcolSpan );
  if (!addNode(graph,makeId(nsId,holderId)))
    delete graph;
}

rvapi::Graph * rvapi::Document::findGraph ( const char * graphId )  {
std::string  nsId;
Graph *graph = (Graph *)findNode ( makeId(nsId,graphId),NTYPE_Graph );
  if (graph)  return graph;
  return (Graph *)findNode ( makeId(nsId,graphId),NTYPE_LogGraph );
}


void rvapi::Document::addRadar ( const char * radarId,
                                 const char * radarTitle,
                                 const char * holderId,
                                 int          hrow,
                                 int          hcol,
                                 int          hrowSpan,
                                 int          hcolSpan,
                                 int          foldState )  {
std::string  nsId;
Radar * radar = new Radar ( makeId(nsId,radarId),radarTitle,
                            hrow,hcol,hrowSpan,hcolSpan );
  if (addNode(radar,makeId(nsId,holderId)))
        radar->setFoldState ( foldState );
  else  delete radar;
}

rvapi::Radar * rvapi::Document::findRadar ( const char * radarId )  {
std::string  nsId;
  return (Radar *)findNode ( makeId(nsId,radarId),NTYPE_Radar );
}


void rvapi::Document::addTreeWidget ( const char * treeId,
                                      const char * title,
                                      const char * holderId,
                                      int          hrow,
                                      int          hcol,
                                      int          hrowSpan,
                                      int          hcolSpan )  {
std::string  nsId;
TreeWidget * treeWidget = new TreeWidget ( makeId(nsId,treeId),title,
                                         hrow,hcol,hrowSpan,hcolSpan );
  if (!addNode(treeWidget,makeId(nsId,holderId)))
    delete treeWidget;
}

rvapi::TreeWidget * rvapi::Document::findTreeWidget (
                                               const char * treeId )  {
std::string  nsId;
  return (TreeWidget *)findNode ( makeId(nsId,treeId),NTYPE_TreeWidget );
}

void rvapi::Document::setTreeNode ( const char * treeId,
                                    const char * leafId,
                                    const char * title,
                                    const char * openState,
                                    const char * parentId
                                  )  {
std::string  nsLeafId;
std::string  nsParentId;
rvapi::TreeWidget * treeWidget = findTreeWidget ( treeId );
  if (treeWidget)
    treeWidget->setTreeNode ( makeId(nsLeafId,leafId),title,openState,
                              makeId(nsParentId,parentId) );
}


// ==================  PROGRESS BARS  ======================

void rvapi::Document::setToolbarProgress ( int key, int value )  {
rvapi::ProgressBar * progressBar = (ProgressBar *)findNode (
                                            toolbar_progressbar_id,
                                            NTYPE_ProgressBar );
  if (progressBar)
    progressBar->setProgressBar ( (ProgressBar::PBAR_KEY)key,value );
}


void rvapi::Document::addProgressBar ( const char * pbarId,
                                       const char * holderId,
                                       int          hrow,
                                       int          hcol,
                                       int          hrowSpan,
                                       int          hcolSpan )  {
std::string   nsId;
ProgressBar * progressBar;
  progressBar = new ProgressBar ( makeId(nsId,pbarId),hrow,hcol,
                                  hrowSpan,hcolSpan );
  if (!addNode(progressBar,makeId(nsId,holderId)))
    delete progressBar;
}

void rvapi::Document::setProgressValue ( const char * pbarId,
                                         int          key,
                                         int          value )  {
std::string   nsId;
ProgressBar * progressBar = (ProgressBar *)findNode (
                              makeId(nsId,pbarId),NTYPE_ProgressBar );
  if (progressBar)
    progressBar->setProgressBar ( (ProgressBar::PBAR_KEY)key,value );

}


// ==================  FORMS AND INPUTS  ======================

void rvapi::Document::addForm ( const char * formId,
                                const char * action,
                                const char * method,
                                const char * holderId,
                                const int    hrow,
                                const int    hcol,
                                const int    hrowSpan,
                                const int    hcolSpan )  {
std::string  nsId;
rvapi::Form * form = new Form ( makeId(nsId,formId),action,method,
                                hrow,hcol,hrowSpan,hcolSpan );
  if (!addNode(form,makeId(nsId,holderId)))
    delete form;
}

rvapi::Input * rvapi::Document::findInput ( const char * inpId )  {
std::string  nsId;
  return (Input *)findNode ( makeId(nsId,inpId) );
}


void rvapi::Document::addFileUpload ( const char * inpId,
                                      const char * name,
                                      const char * value,
                                      const int    length,
                                      const bool   required,
                                      const char * formId,
                                      const int    hrow,
                                      const int    hcol,
                                      const int    hrowSpan,
                                      const int    hcolSpan
                                    )  {
std::string  nsId;
rvapi::Input * input = new Input ( makeId(nsId,inpId),
                                   hrow,hcol,hrowSpan,hcolSpan );
  input->makeFileUpload ( name,value,length,required );
  if (!addNode(input,makeId(nsId,formId)))
    delete input;
}

void rvapi::Document::addLineEdit ( const char * inpId,
                                    const char * name,
                                    const char * text,
                                    const int    length,
                                    const char * formId,
                                    const int    hrow,
                                    const int    hcol,
                                    const int    hrowSpan,
                                    const int    hcolSpan
                                  )  {
std::string  nsId;
rvapi::Input * input = new Input ( makeId(nsId,inpId),
                                   hrow,hcol,hrowSpan,hcolSpan );
  input->makeLineEdit ( name,text,length );
  if (!addNode(input,makeId(nsId,formId)))
    delete input;
}

void rvapi::Document::addHiddenText ( const char * inpId,
                                      const char * name,
                                      const char * text,
                                      const char * formId,
                                      const int    hrow,
                                      const int    hcol,
                                      const int    hrowSpan,
                                      const int    hcolSpan
                                    )  {
std::string  nsId;
rvapi::Input * input = new Input ( makeId(nsId,inpId),
                                   hrow,hcol,hrowSpan,hcolSpan );
  input->makeHiddenText ( name,text );
  if (!addNode(input,makeId(nsId,formId)))
    delete input;
}

void rvapi::Document::addSubmitButton ( const char * inpId,
                                        const char * title,
                                        const char * formAction,
                                        const char * formId,
                                        const int    hrow,
                                        const int    hcol,
                                        const int    hrowSpan,
                                        const int    hcolSpan
                                      )  {
std::string  nsId;
rvapi::Input * input = new Input ( makeId(nsId,inpId),
                                   hrow,hcol,hrowSpan,hcolSpan );
  input->makeSubmitButton ( title,formAction );
  if (!addNode(input,makeId(nsId,formId)))
    delete input;
}

void rvapi::Document::addButton ( const char * inpId,
                                  const char * title,
                                  const char * command,
                                  const char * data,
                                  const bool   rvOnly,
                                  const char * holderId,
                                  const int    hrow,
                                  const int    hcol,
                                  const int    hrowSpan,
                                  const int    hcolSpan
                                )  {
std::string  nsId;
rvapi::Input * input = new Input ( makeId(nsId,inpId),
                                   hrow,hcol,hrowSpan,hcolSpan );
  input->makeButton ( title,command,data,rvOnly );
  if (!addNode(input,makeId(nsId,holderId)))
    delete input;
}

void rvapi::Document::addCheckbox ( const char * inpId,
                                    const char * title,
                                    const char * name,
                                    const char * value,
                                    const char * command,
                                    const char * data,
                                    const bool   checked,
                                    const char * holderId,
                                    const int    hrow,
                                    const int    hcol,
                                    const int    hrowSpan,
                                    const int    hcolSpan
                                  )  {
std::string  nsId;
rvapi::Input * input = new Input ( makeId(nsId,inpId),
                                   hrow,hcol,hrowSpan,hcolSpan );
  input->makeCheckbox ( title,name,value,command,makeId(nsId,data),
                        checked );
  if (!addNode(input,makeId(nsId,holderId)))
    delete input;
}

void rvapi::Document::addCombobox ( const char * cbxId,
                                    const char * name,
                                    const char * onChange,
                                    const int    size,
                                    const char * holderId,
                                    const int    hrow,
                                    const int    hcol,
                                    const int    hrowSpan,
                                    const int    hcolSpan
                                  )  {
std::string       nsId;
rvapi::Combobox * combobox = new Combobox ( makeId(nsId,cbxId),
                                   name,onChange,size,
                                   hrow,hcol,hrowSpan,hcolSpan );
  if (!addNode(combobox,makeId(nsId,holderId)))
    delete combobox;
}

void rvapi::Document::addComboboxOption ( const char * cbxId,
                                          const char * label,
                                          const char * value,
                                          const bool   selected
                                        )  {
std::string       nsId;
rvapi::Combobox * combobox = (Combobox *)findNode (
                                  makeId(nsId,cbxId),NTYPE_Combobox );
  if (combobox)
    combobox->addOption ( label,value,selected );
}

void rvapi::Document::addRadioButton ( const char * inpId,
                                       const char * title,
                                       const char * name,
                                       const char * value,
                                       const bool   checked,
                                       const char * holderId,
                                       const int    hrow,
                                       const int    hcol,
                                       const int    hrowSpan,
                                       const int    hcolSpan
                                     )  {
std::string  nsId;
rvapi::Input * input = new Input ( makeId(nsId,inpId),
                                   hrow,hcol,hrowSpan,hcolSpan );
  input->makeRadioButton( title,name,value,checked );
  if (!addNode(input,makeId(nsId,holderId)))
    delete input;
}

void rvapi::Document::disableForm ( const char * formId,
                                    const bool   disable )  {
std::string  nsId;
  makeId ( nsId,formId );
  tasks.append ( disable_form_key "" key_del + nsId + key_del );
  if (disable)  tasks.append ( "true"  );
          else  tasks.append ( "false" );
  tasks.append ( key_ter );
}

void rvapi::Document::disableInput ( const char * inpId,
                                     const bool   disable )  {
std::string  nsId;
  makeId ( nsId,inpId );
  tasks.append ( disable_input_key "" key_del + nsId + key_del );
  if (disable)  tasks.append ( "true"  );
          else  tasks.append ( "false" );
  tasks.append ( key_ter );
}

// ============================================================

void rvapi::Document::removeWidget ( const char * widgetId )  {
std::string  nsId;
Node       * node;
  node = findNode ( makeId(nsId,widgetId) );
  if (node)  {
    if (node->type()!=NTYPE_Tab)  {
      switch (node->type())  {
        case NTYPE_Table     :  nsId += "_table_dd-grid"; break;
        case NTYPE_Fieldset  :
        case NTYPE_Grid      :
        case NTYPE_Graph     :
        case NTYPE_LogGraph  :  nsId += "-grid";          break;
        case NTYPE_TreeWidget:  nsId += "_outer-grid";    break;
        case NTYPE_Section   :  nsId += "-accordion";     break;
        default : ;
      }
      removeNode ( node );
      tasks.append ( remove_widget_key "" key_del + nsId + key_ter );
    }
  }
}

// ============================================================

void rvapi::Document::initDocument()  {

  documentId   = "";
  Id           = "body";  // fixed also in JS layer
//  jsrviewUri = "/Users/Eugene/Projects/jsRView/share/jsrview/";
  jsrviewUri = "http://ccp4mac1.home/~Eugene/pisa/jsrview/";
  outputDir  = "./";
  outputMode = MODE_Html;
  docFName.clear();
  htmlFName  = "index.html";  // for html output
  taskFName  = "task.tsk";
  xmli2FName = "i2.xml";
  docUri     = "";

  timeQuant  = 1000; // time quant for browser to update, milliseconds
  stop_poll  = 1;
  reset_cnt  = 0;    // resets web page counter for task file

  max_task_file_size = 50000;
  task_file_size     = max_task_file_size + 1;  // makes new task.tsk
                                                // in the beginning
                                                
  meta = "";

}

void rvapi::Document::makeJSLinks()  {

  uri_css.clear();
  uri_js .clear();

  uri_css.push_back ( jsrviewUri + "jquery-ui/css/jquery-ui.css"   );
  uri_css.push_back ( jsrviewUri + "jqplot/css/jquery.jqplot.css"  );
  uri_css.push_back ( jsrviewUri + "jqtree/css/jqtree.css"         );
  uri_css.push_back ( jsrviewUri + "tablesorter/theme.blue.css"    );

//  uri_css.push_back ( jsrviewUri + "jquery-extendeddialog/gesamt_src/fq-ui.extendeddialog.css" );
  uri_css.push_back ( jsrviewUri + "css/jsrview.css"               );

  uri_js.push_back ( jsrviewUri + "jquery-ui/js/jquery.js"         );
  uri_js.push_back ( jsrviewUri + "jquery-ui/js/jquery-ui.js"      );
  uri_js.push_back ( jsrviewUri + "jqplot/js/jquery.jqplot.min.js" );
  uri_js.push_back ( jsrviewUri + "jqplot/js/jqplot.cursor.min.js" );
  uri_js.push_back ( jsrviewUri + "jqplot/js/jqplot.highlighter.min.js" );
  uri_js.push_back ( jsrviewUri + "jqplot/js/jqplot.canvasAxisLabelRenderer.min.js" );
  uri_js.push_back ( jsrviewUri + "jqplot/js/jqplot.canvasTextRenderer.min.js"      );
  uri_js.push_back ( jsrviewUri + "jqplot/js/jqplot.canvasAxisTickRenderer.min.js"  );
  uri_js.push_back ( jsrviewUri + "jqplot/js/jqplot.logAxisRenderer.min.js"         );
  uri_js.push_back ( jsrviewUri + "jqplot/js/jqplot.barRenderer.min.js"             );
  uri_js.push_back ( jsrviewUri + "jqtree/js/tree.jquery.js"  );
  uri_js.push_back ( jsrviewUri + "d3/d3.v3.min.js"           );
  uri_js.push_back ( jsrviewUri + "d3/radar-chart.js"         );
  uri_js.push_back ( jsrviewUri + "tablesorter/jquery.tablesorter.js" );
  uri_js.push_back ( jsrviewUri + "tablesorter/jquery.tablesorter.pager.js" );
//  uri_js.push_back ( jsrviewUri + "jquery-extendeddialog/gesamt_src/fq-ui.extendeddialog.js" );
  uri_js.push_back ( jsrviewUri + "js/jsrview.buttons.js"     );
  uri_js.push_back ( jsrviewUri + "js/jsrview.checkbox.js"    );
  uri_js.push_back ( jsrviewUri + "js/jsrview.combobox.js"    );
  uri_js.push_back ( jsrviewUri + "js/jsrview.content.js"     );
  uri_js.push_back ( jsrviewUri + "js/jsrview.dropdown.js"    );
  uri_js.push_back ( jsrviewUri + "js/jsrview.form.js"        );
  uri_js.push_back ( jsrviewUri + "js/jsrview.global.js"      );
  uri_js.push_back ( jsrviewUri + "js/jsrview.graph.js"       );
  uri_js.push_back ( jsrviewUri + "js/jsrview.grid.js"        );
  uri_js.push_back ( jsrviewUri + "js/jsrview.js"             );
  uri_js.push_back ( jsrviewUri + "js/jsrview.page.js"        );
  uri_js.push_back ( jsrviewUri + "js/jsrview.progressbar.js" );
  uri_js.push_back ( jsrviewUri + "js/jsrview.section.js"     );
  uri_js.push_back ( jsrviewUri + "js/jsrview.tabs.js"        );
  uri_js.push_back ( jsrviewUri + "js/jsrview.toolbar.js"     );
  uri_js.push_back ( jsrviewUri + "js/jsrview.tree.js"        );
  uri_js.push_back ( jsrviewUri + "js/jsrview.utils.js"       );
  uri_js.push_back ( jsrviewUri + "js/jsrview.viewer.js"      );
  uri_js.push_back ( jsrviewUri + "js/jsrview.window.js"      );

  jqPlotEx_js = jsrviewUri + "jqplot/js/excanvas.min.js";

}

void rvapi::Document::freeDocument()  {
  freeNode();
}

void rvapi::Document::putMeta ( const char * metastring )  {
// Putting optional metadata, which can be passed between
// processes sharing the same document
  meta = metastring;
}

const char * rvapi::Document::getMeta()  {
// Getting optional metadata, which can be passed between
// processes sharing the same document
  return meta.c_str();
}



void rvapi::Document::store ( const char *fname )  {

//  if (fname)  docFName = fname;
//  if (!docFName.empty())  {
  if (fname)  {
    std::ofstream s;
    std::string docfn = fname;
    docfn = outputDir + docfn.substr ( docfn.find_last_of("/\\")+1 );
    s.open ( docfn.c_str(),std::ios::out | std::ios::binary );
    if (s.good())  {
      write  ( s );
      s.close();
    }
  }

}


void rvapi::Document::store2 ( const char *fpath )  {
std::string path = fpath;
int         m;

  flush();

  std::ofstream s;
  s.open ( path.c_str(),std::ios::out | std::ios::binary );
  if (s.good())  {

    swrite ( s,documentId  );
    swrite ( s,outputDir   );
    m = outputMode;
    swrite ( s,m           );
    m = layoutMode;
    swrite ( s,m           );
    swrite ( s,winTitle    );
    swrite ( s,jsrviewUri  );
    swrite ( s,helpFName   );
    swrite ( s,htmlFName   );
    swrite ( s,taskFName   );
    swrite ( s,xmli2FName  );

    write  ( s );

    s.close();

  }

}


void rvapi::Document::restore ( const char * fname )  {

  if (fname)  docFName = fname;
  if (!docFName.empty())  {
    std::ifstream s;
    std::string   docfn = outputDir + docFName;
    s.open ( docfn.c_str(),std::ios::in | std::ios::binary );
    if (s.good())  {
      read  ( s );
      s.close();
    }
  }

}

void rvapi::Document::restore2 ( const char *fpath )  {
//std::string path = fpath;
std::string docId,outDir,windowTitle,jsrvUri;
std::string helpFN,htmlFN,taskFN,xmli2FN;
int         m,l;

  std::ifstream s;
  s.open ( fpath,std::ios::in | std::ios::binary );
  if (s.good())  {

    sread ( s,docId       );
    sread ( s,outDir      );
    sread ( s,m           );
    sread ( s,l           );
    sread ( s,windowTitle );
    sread ( s,jsrvUri     );
    sread ( s,helpFN      );
    sread ( s,htmlFN      );
    sread ( s,taskFN      );
    sread ( s,xmli2FN     );
    
    configure ( docId      .c_str(),
                outDir     .c_str(),
                windowTitle.c_str(),
                (OUTPUT_MODE)m,
                (LAYOUT_MODE)l,
                jsrvUri.c_str(),
                helpFN .c_str(),
                htmlFN .c_str(),
                taskFN .c_str(),
                xmli2FN.c_str(),
                false
              );

//    docFName = path.substr ( path.find_last_of("/\\")+1 );
    docFName = fpath;

    read  ( s );

    s.close();

//    setTreeFinished();

  }
  
//  flush();

}


void rvapi::Document::writeNode ( std::ofstream & s, Node * node )  {
int t,nn;

  if (!node)  t = -1;
        else  t = node->type();
  swrite ( s,t );

  if (node)  {
    node->write ( s );
    nn = node->n_nodes();
    swrite ( s,nn );
    for (int i=0;i<nn;i++)
      writeNode ( s,node->node(i) );
  }

}

rvapi::Node * rvapi::Document::readNode ( std::ifstream & s )  {
Node * node = NULL;
int    t,nn;

  sread ( s,t );

  if (t<0)  return NULL;

  switch ((NODE_TYPE)t)  {
    case NTYPE_Document     :                           
    default : printf ( " *** NODE TYPE %i NOT ADOPTED\n",t ); break;
    case NTYPE_Button       : node = new Input      ();  break;
    case NTYPE_Content      : node = new Content    ();  break;
    case NTYPE_Checkbox     : node = new Input      ();  break;
    case NTYPE_Combobox     : node = new Combobox   ();  break;
    case NTYPE_Data         : node = new Data       ();  break;
    case NTYPE_DocHeader    : node = new DocHeader  ();  break;
    case NTYPE_LineEdit     : node = new Input      ();  break;
    case NTYPE_FileUpload   : node = new Input      ();  break;
    case NTYPE_Form         : node = new Form       ();  break;
    case NTYPE_Graph        : node = new Graph      ();  break;
    case NTYPE_Fieldset     : node = new Fieldset   ();  break;
    case NTYPE_Grid         : node = new Grid       ();  break;
    case NTYPE_Hidden       : node = new Input      ();  break;
    case NTYPE_LogGraph     : node = new LogGraph   ();  break;
    case NTYPE_Input        : node = new Input      ();  break;
    case NTYPE_None         : node = new Node       ();  break;
    case NTYPE_Panel        : node = new Panel      ();  break;
    case NTYPE_ProgressBar  : node = new ProgressBar();  break;
    case NTYPE_Radar        : node = new Radar      ();  break;
    case NTYPE_RadioButton  : node = new Input      ();  break;
    case NTYPE_Section      : node = new Section    ();  break;
    case NTYPE_SubmitButton : node = new Input      ();  break;
    case NTYPE_Tab          : node = new Tab        ();  break;
    case NTYPE_Table        : node = new Table      ();  break;
    case NTYPE_Text         : node = new Text       ();  break;
    case NTYPE_TreeWidget   : node = new TreeWidget ();  break;
  }

  if (node)  {
    node->read ( s );
    sread ( s,nn );
    for (int i=0;i<nn;i++)
      node->addNode ( readNode(s) );
  }

  return node;

}


void rvapi::Document::write ( std::ofstream & s )  {
int m  = outputMode;
int nn = nodes.size();
int nc = uri_css.size();
int nj = uri_js .size();
int i;

  swrite ( s,m           );
  swrite ( s,outputDir   );
  swrite ( s,helpFName   );
  swrite ( s,htmlFName   );
  swrite ( s,taskFName   );
  swrite ( s,tasks       );
  swrite ( s,jsrviewUri  );
  swrite ( s,docUri      );
  swrite ( s,jqPlotEx_js );
  swrite ( s,nn          );
  swrite ( s,nc          );
  swrite ( s,nj          );
  swrite ( s,meta        );

  for (i=0;i<nc;i++)  swrite  ( s,uri_css[i] );
  for (i=0;i<nj;i++)  swrite  ( s,uri_js [i] );
  for (i=0;i<nn;i++)  writeNode ( s,nodes[i] );

  Node::write ( s );

}

void rvapi::Document::read ( std::ifstream & s )  {
std::string uri;
int         m,nn,nc,nj,i;

  freeDocument();

  sread ( s,m           );
  sread ( s,outputDir   );
  sread ( s,helpFName   );
  sread ( s,htmlFName   );
  sread ( s,taskFName   );
  sread ( s,tasks       );
  sread ( s,jsrviewUri  );
  sread ( s,docUri      );
  sread ( s,jqPlotEx_js );
  sread ( s,nn          );
  sread ( s,nc          );
  sread ( s,nj          );
  sread ( s,meta        );

  outputMode = (OUTPUT_MODE)m;

  for (i=0;i<nc;i++)  {
    sread ( s,uri );
    uri_css.push_back ( uri );
  }
  for (i=0;i<nj;i++)  {
    sread ( s,uri );
    uri_js.push_back ( uri );
  }

  for (i=0;i<nn;i++)
    addNode ( readNode(s) );

  Node::read ( s );

}

void rvapi::Document::flush_file ( std::string & data,
                                   const bool  timeStamp,
                                   const bool  shrinkFile,
                                   std::string fname )  {
std::ofstream s;
std::string   taskfn  = outputDir + fname;
std::string   taskfn0 = taskfn;
struct stat   buffer;
time_t        crtime  = time(NULL);
bool          fexists = (stat(taskfn.c_str(),&buffer)==0);

  if (fexists)  {
    // file exists and needs to be appended; for safety, we make
    // a copy of task file, append it and move back to original
    // location
    char S[100];
    sprintf ( S,"%li",crtime );
    taskfn0.append ( S );  // temporary task file name
    if (!shrinkFile)  {
      // no request to shrink task file; copy it to temporary file,
      // append with new commands and move back to original location
      std::ifstream  src(taskfn .c_str(),std::ios::binary);
      std::ofstream  dst(taskfn0.c_str(),std::ios::binary);
      std::string    L;
      // read and analyse 1st line of task file in case the file
      // was just shrinked: then the line must be modified
      getline ( src,L );
      if (!L.compare(0,strlen(task_stamp_key),task_stamp_key))  {
        int p = L.find ( "RELOAD" );
        int n = 0;
        if (p>0)  // existing file commands to reload the page after
          n = 6;  // shrinking; prepare to replace this
        else  {
          p = L.find ( "RESET" );
          if (p>0)  // existing file commands to reset the task counter;
            n = 5;  // prepare to replace
        }
        if (n>0)  {  // the command needs to be replaced
          if (reset_cnt>0)  // there is a request to reset task counter
                L.replace ( p,n,"RESET" );  // reset task counter
          else  L.replace ( p,n,"PASS"  );  // do nothing
        }
      }
      dst << L << std::endl;  // write 1st line into temporary file
      getline ( src,L );
      dst << set_time_quant_key << " " << timeQuant << key_ter;
      if (L.compare(0,strlen(set_time_quant_key),set_time_quant_key))
        dst << L << std::endl;  // write 2nd line into temporary file
      dst << src.rdbuf();     // copy all the rest of the file
      fexists = (buffer.st_size>2);  // task file must be larger than 2
                                     // bytes, otherwise a possible
                                     // file system artefact
    }
  }

  if ((!shrinkFile) && fexists)
    s.open ( taskfn0.c_str(),std::ios::out | std::ios::app );
  else  {
    s.open ( taskfn0.c_str(),std::ios::out );
    if (timeStamp)  {
      std::string tsk;
      put_task_stamp ( tsk,shrinkFile,(reset_cnt>0) );
      s << tsk;
    }
    s << set_time_quant_key << " " << timeQuant << key_ter;
  }
  s << data;
  task_file_size = s.tellp();
  s.close();
  
  if (fexists)  {
    remove ( taskfn.c_str() );
    rename ( taskfn0.c_str(),taskfn.c_str() );
  }

}


void str_replace ( std::string & s, const std::string & substr,
                                    const std::string & replace )  {
size_t pos = 0;
  while ((pos=s.find(substr,pos))!=std::string::npos)  {
    s.replace(pos,substr.length(),replace);
    pos += replace.length();
  }
}


void rvapi::Document::flush()  {
bool shrinkFile;

  if (outputMode & MODE_Xmli2)  {
    std::string xml;

    flush_xmli2 ( outputDir,xml );
    if (!xml.empty())  {
      str_replace ( xml,"<br>"  ,"\n" );
      str_replace ( xml,"<BR>"  ,"\n" );
      str_replace ( xml,"&nbsp;"," "  );
      str_replace ( xml,"&emsp;"," "  );
      str_replace ( xml," & "," and " );
      xml = "<xmli2>\n" + xml + "</xmli2>\n";
//      flush_file ( xml,false,false,xmli2FName );
      std::ofstream s;
      std::string   fname = outputDir + xmli2FName;
      s.open ( fname.c_str(),std::ios::out );
      s << xml;
      s.close();
    }

  }

  if (outputMode & MODE_Html)  {

    // check that task file still has a reasonable size. If it does not,
    // the whole task.tsk will be reset to its current state and the
    // output html page reloaded (causes a blink).  
    shrinkFile = (task_file_size>max_task_file_size);
    if (shrinkFile)
      setTreeCreated();
    
    fflush(stdout);

    std::string tsk = tasks;
    tasks.clear();
    flush_html ( outputDir,tasks );
    tasks.append ( tsk );

    if (stop_poll>1)  {
      // put stop poll signal
      tasks.append ( stop_poll_key "" key_ter );
      stop_poll = 1;
    }

    if (!tasks.empty())  {
      flush_file ( tasks,true,shrinkFile,taskFName );
      if (reset_cnt>=0)
        reset_cnt--;
    }

    tasks.clear();

  }

}


/* ----------------------------------------------------------------
 * rewrites optimised task file when polls are stopped. The problem
 * is, live presentation in browser may miss a few ending lines
 * in this case, so that a manual refresh of page is needed.

void rvapi::Document::flush()  {
time_t crtime = time(NULL);

  if ((outputMode & MODE_Html) && (crtime>t_reference))  {

    if (stop_poll>1)
      setTreeCreated();

    flush_html ( outputDir,tasks );

    if (stop_poll>1)
      tasks.append ( stop_poll_key""key_ter );

    if (!tasks.empty())  {
      std::ofstream s;
      std::string   taskfn  = outputDir + taskFName;
      std::string   taskfn0 = taskfn;
      struct stat   buffer;
      bool          fexists = (stat(taskfn.c_str(),&buffer)==0);

      if (fexists)  {
        char S[100];
        sprintf ( S,"%li",crtime );
        taskfn0.append ( S );
        if (stop_poll<=1)  {
          std::ifstream  gesamt_src(taskfn .c_str(),std::ios::binary);
          std::ofstream  dst(taskfn0.c_str(),std::ios::binary);
          dst << gesamt_src.rdbuf();
        }
      }

      if (stop_poll>1)
        stop_poll = 1;

      s.open ( taskfn0.c_str(),std::ios::out | std::ios::app );
      if (s.tellp()<=2)  {
        std::string tsk;
        put_task_stamp ( tsk );
        s << tsk;
      }
      s << tasks;
      s.close();

      if (fexists)  {
        remove ( taskfn.c_str() );
        rename ( taskfn0.c_str(),taskfn.c_str() );
      }

      t_reference = crtime;

    }

    tasks.clear();

  }

}
*/

void rvapi::Document::setMaxTaskFileSize ( const long max_size )  {
  max_task_file_size = 1024*long(max_size);
}

void rvapi::Document::setPolling ( bool on )  {
  if (on)  stop_poll = 0;
     else  stop_poll = 1;
}

void rvapi::Document::resetTask()  {
// resets task counter
  reset_cnt = 1;  // till the next flush()
}

