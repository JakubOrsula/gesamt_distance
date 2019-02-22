//
//  =================================================================
//
//    06.02.17   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_node  <implementation>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Classes :  rvapi::Node - API base node class
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2013-2017
//
//  =================================================================
//

#include <string.h>

#include "rvapi_interface.h"

#include "rvapi_document.h"
#include "rvapi_table.h"
#include "rvapi_data.h"
#include "rvapi_loggraph.h"
#include "rvapi_grid.h"
#include "rvapi_radar.h"
#include "rvapi_plot.h"
#include "rvapi_input.h"

rvapi::Document document;

std::vector<std::string> split ( std::string s )  {
std::vector<std::string> list;
int                      n;
int                      pos   = 0;
size_t                   index = 0;

  while (index!=std::string::npos)  {
    index = s.find_first_of ( '/',pos );
    if (index!=std::string::npos)
         n = index-pos;
    else n = s.size()-pos;
    if (n>0)
      list.push_back ( s.substr ( pos,n) );
    pos += n+1;
  }

  return list;
}



bool silent = true;

extern "C"
void rvapi_init_document ( const char * docId,     // mandatory
                           const char * outDir,    // mandatory
                           const char * winTitle,  // mandatory
                           const int    mode,      // mandatory
                           const int    layout,    // mandatory
                           const char * jsUri,     // may be NULL
                           const char * helpFName, // may be NULL
                           const char * htmlFName, // may be NULL
                           const char * taskFName, // may be NULL
                           const char * xmli2FName // may be NULL
                         )  {
  try {
    rvapi::setRVAPIError(0);
    silent = mode & RVAPI_MODE_Silent;
    if (!silent)
      document.configure ( docId,outDir,winTitle,
                           rvapi::OUTPUT_MODE(mode & (~RVAPI_MODE_Silent)),
                           rvapi::LAYOUT_MODE(layout),
                           jsUri,helpFName,htmlFName,
                           taskFName,xmli2FName );
  
  } catch(...) { rvapi::setRVAPIError(1); }
}

// Tries to restore document saved in file 'fname' in directory
// used for initialisation. If file is not found, then does
// nothing apart from storing file name in the document.
extern "C"
void rvapi_restore_document ( const char * fname )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.restore ( fname );
  } catch(...) { rvapi::setRVAPIError(1); }
}


// Tries to initialise and restore document saved in file 'fpath'.
// If file is not found, then does nothing.
extern "C"
void rvapi_restore_document2 ( const char * fpath )  {
  try {
    rvapi::setRVAPIError(0);
    silent = false;
    document.restore2 ( fpath );
  } catch(...) { rvapi::setRVAPIError(1); }
}


// Tries to store document in file 'fname' in directory used
// for initialisation. If fname is NULL, then attempts to use file
// name used in rvapi_restore_document(). The file name is saved
// in the document and replaces any previous one used for
// storing/restoring.
extern "C"
void rvapi_store_document ( const char * fname )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.store ( fname );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_store_document2 ( const char * fpath )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.store2 ( fpath );
  } catch(...) { rvapi::setRVAPIError(1); }
}



extern "C"
void rvapi_put_meta ( const char * metastring ) {
// Putting optional metadata, which can be passed between
// processes sharing the same document
  document.putMeta ( metastring );
}


extern "C"
const char * rvapi_get_meta()  {
// Getting optional metadata, which can be passed between
// processes sharing the same document
  return document.getMeta();
}



extern "C"
void rvapi_add_header ( const char * htmlString )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.addHeader ( htmlString );
  } catch(...) { rvapi::setRVAPIError(1); }
}


  /*  ===============
        TABS
      ===============  */


extern "C"
void rvapi_add_tab ( const char * tabId,
                     const char * tabName,
                     const bool   open ) {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.addTab ( tabId,tabName,open );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_insert_tab ( const char * tabId,
                        const char * tabName,
                        const char * beforeTabId,
                        const bool   open ) {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.insertTab ( tabId,tabName,beforeTabId,open );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_remove_tab ( const char * tabId )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.removeTab ( tabId );
  } catch(...) { rvapi::setRVAPIError(1); }
}


  /*  ===============
        SECTIONS
      ===============  */

extern "C"
void rvapi_add_section  ( const char * secId,
                          const char * secTitle,
                          const char * holderId,
                          const int    row,
                          const int    col,
                          const int    rowSpan,
                          const int    colSpan,
                          const bool   open ) {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.addSection ( secId,secTitle,holderId,
                            row,col,rowSpan,colSpan, open );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_add_section1  ( const char * path,
                           const char * secTitle,
                           const int    row,
                           const int    col,
                           const int    rowSpan,
                           const int    colSpan,
                           const bool   open ) {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      std::vector<std::string> list = split ( path );
      if (list.size()==2)
        rvapi_add_section ( list[1].c_str(),secTitle,list[0].c_str(),
                            row,col,rowSpan,colSpan,open );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_append_section  ( const char * secId,
                             const char * secTitle,
                             const char * holderId,
                             const bool   open ) {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.addSection ( secId,secTitle,holderId,-1,0,1,1,open );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_append_section1 ( const char * path, // "holderId/secId"
                             const char * secTitle,
                             const bool   open
                           )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      std::vector<std::string> list = split ( path );
      if (list.size()==2)
        rvapi_append_section ( list[1].c_str(),secTitle,
                               list[0].c_str(),open );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_set_section_state ( const char * secId,
                               const bool   open
                             )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.setSectionState ( secId,open );
  } catch(...) { rvapi::setRVAPIError(1); }
}


  /*  ===============
        GRIDS
      ===============  */

extern "C"
void rvapi_add_grid  ( const char * gridId,
                       const bool   filling,
                       const char * holderId,
                       const int    row,
                       const int    col,
                       const int    rowSpan,
                       const int    colSpan ) {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.addGrid ( gridId,filling,holderId,row,col,rowSpan,colSpan );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_add_grid1 ( const char * path,
                       const bool   filling,
                       const int    row,
                       const int    col,
                       const int    rowSpan,
                       const int    colSpan ) {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      std::vector<std::string> list = split ( path );
      if (list.size()==2)
        rvapi_add_grid ( list[1].c_str(),filling,list[0].c_str(),
                         row,col,rowSpan,colSpan );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_append_grid  ( const char * gridId,
                          const bool   filling,
                          const char * holderId ) {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.addGrid ( gridId,filling,holderId,-1,0,1,1 );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_append_grid1 ( const char * path, // "holderId/gridId"
                          const bool   filling )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      std::vector<std::string> list = split ( path );
      if (list.size()==2)
        rvapi_append_grid ( list[1].c_str(),filling,list[0].c_str() );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

/*
extern "C"
void rvapi_add_stretch  ( const char * stretchId,
                          const bool   horizontal,
                          const bool   vertical,
                          const char * holderId,
                          const int    row,
                          const int    col,
                          const int    rowSpan,
                          const int    colSpan
                        )  {
try {
  rvapi::setRVAPIError(0);
  if (!silent)
    document.addStretch ( stretchId,horizontal,vertical,
                          holderId,row,col,rowSpan,colSpan );
} catch(...) { rvapi::setRVAPIError(1); }
}
*/

extern "C"
void rvapi_set_cell_stretch ( const char * gridId,
                              const int    width,
                              const int    height,
                              const int    row,
                              const int    col
                            )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      rvapi::Grid *grid = document.findGrid ( gridId );
      if (grid)
        grid->setCellStretch ( width,height,row,col );
      else
        document.setCellStretch ( gridId,width,height,row,col );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}


  /*  ===============
        PANELS
      ===============  */

extern "C"
void rvapi_add_panel ( const char * panelId,
                       const char * holderId,
                       const int    row,
                       const int    col,
                       const int    rowSpan,
                       const int    colSpan ) {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.addPanel ( panelId,holderId,row,col,rowSpan,colSpan );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_add_panel1 ( const char * path,
                        const int    row,
                        const int    col,
                        const int    rowSpan,
                        const int    colSpan ) {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      std::vector<std::string> list = split ( path );
      if (list.size()==2)
        rvapi_add_panel ( list[1].c_str(),list[0].c_str(),
                          row,col,rowSpan,colSpan );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_append_panel ( const char * panelId,
                          const char * holderId ) {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.addPanel ( panelId,holderId,-1,0,1,1 );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_append_panel1 ( const char * path )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      std::vector<std::string> list = split ( path );
      if (list.size()==2)
        rvapi_append_panel ( list[1].c_str(),list[0].c_str() );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}



  /*  ===============
        FIELDSETS
      ===============  */

extern "C"
void rvapi_add_fieldset ( const char * fsetId,
                          const char * title,
                          const char * holderId,
                          const int    row,
                          const int    col,
                          const int    rowSpan,
                          const int    colSpan ) {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.addFieldset ( fsetId,title,holderId,row,col,rowSpan,colSpan );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_add_fieldset1 ( const char * path,
                           const char * title,
                           const int    row,
                           const int    col,
                           const int    rowSpan,
                           const int    colSpan ) {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      std::vector<std::string> list = split ( path );
      if (list.size()==2)
        rvapi_add_fieldset ( list[1].c_str(),title,list[0].c_str(),
                             row,col,rowSpan,colSpan );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_append_fieldset ( const char * fsetId,
                             const char * title,
                             const char * holderId ) {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.addFieldset ( fsetId,title,holderId,-1,0,1,1 );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_append_fieldset1 ( const char * path, const char * title )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      std::vector<std::string> list = split ( path );
      if (list.size()==2)
        rvapi_append_fieldset ( list[1].c_str(),title,list[0].c_str() );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}



  /*  ===============
        TEXT WIDGET
      ===============  */

extern "C"
void rvapi_set_text ( const char * textString,
                      const char * holderId,
                      const int    row,
                      const int    col,
                      const int    rowSpan,
                      const int    colSpan ) {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.setText ( "",textString, holderId,row,col,rowSpan,colSpan );
  } catch(...) { rvapi::setRVAPIError(1); }
}


extern "C"
void rvapi_add_text ( const char * textString,
                      const char * holderId,
                      const int    row,
                      const int    col,
                      const int    rowSpan,
                      const int    colSpan ) {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.addText ( "",textString, holderId,row,col,rowSpan,colSpan );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_append_text ( const char * textString,
                         const char * holderId )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.addText ( "",textString, holderId );
  } catch(...) { rvapi::setRVAPIError(1); }
}


extern "C"
void rvapi_set_label ( const char * labelId,
                       const char * holderId,
                       const char * labelText,
                       const int    row,
                       const int    col,
                       const int    rowSpan,
                       const int    colSpan
                     )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.setText ( labelId,labelText, holderId,row,col,rowSpan,colSpan );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_add_label ( const char * labelId,
                       const char * holderId,
                       const char * labelText,
                       const int    row,
                       const int    col,
                       const int    rowSpan,
                       const int    colSpan
                     )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.addText ( labelId,labelText, holderId,row,col,rowSpan,colSpan );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_reset_label ( const char * labelId,
                         const char * newTextString
                       )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.resetText ( labelId,newTextString );
  } catch(...) { rvapi::setRVAPIError(1); }
}


  /*  ==================
        CONTENT WIDGET
      ==================  */

extern "C"
void rvapi_append_content ( const char * uri,
                            const bool   watch,
                            const char * holderId )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.addContent ( uri,watch,holderId );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_add_content ( const char * uri,
                         const bool   watch,
                         const char * holderId,
                         const int    row,
                         const int    col,
                         const int    rowSpan,
                         const int    colSpan
                       )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.addContent ( uri,watch,holderId,row,col,rowSpan,colSpan );
  } catch(...) { rvapi::setRVAPIError(1); }
}

  /*  ==========
        TABLES
      ==========  */

extern "C"
void rvapi_add_table ( const char * tblId,
                       const char * tblTitle,
                       const char * holderId,
                       const int    row,
                       const int    col,
                       const int    rowSpan,
                       const int    colSpan,
                       const int    foldState )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.addTable ( tblId,tblTitle,holderId,row,col,rowSpan,colSpan,
                          foldState );
  } catch(...) { rvapi::setRVAPIError(1); }
}


extern "C"
void rvapi_set_table_type ( const char * tblId,
                            const bool sortable,
                            const bool paging )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      rvapi::Table *table;
      table = document.findTable ( tblId );
      if (table)
        table->setTableType ( sortable,paging );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}


extern "C"
void rvapi_add_table1 ( const char * path,
                        const char * tblTitle,
                        const int    row,
                        const int    col,
                        const int    rowSpan,
                        const int    colSpan,
                        const int    foldState )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      std::vector<std::string> list = split ( path );
      if (list.size()==2)
        rvapi_add_table ( list[1].c_str(),tblTitle,list[0].c_str(),
                          row,col,rowSpan,colSpan, foldState );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_append_table ( const char * tblId,
                          const char * tblTitle,
                          const char * holderId,
                          const int    foldState )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.addTable ( tblId,tblTitle,holderId,-1,0,1,1,foldState );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_append_table1 ( const char * path,
                           const char * tblTitle,
                           const int    foldState )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      std::vector<std::string> list = split ( path );
      if (list.size()==2)
        rvapi_append_table ( list[1].c_str(),tblTitle,
                             list[0].c_str(), foldState );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_set_table_style  ( const char * tblId,
                              const char * css,
                              const char * td_style
                            )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      rvapi::Table *table;
      table = document.findTable ( tblId );
      if (table)
        table->setStyle ( css,td_style );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}


extern "C"
void rvapi_put_horz_theader ( const char * tblId,
                              const char * header,
                              const char * tooltip,
                              const int    tcol )  {
  try {
    rvapi::setRVAPIError(0);
    rvapi::Table *table;
    if (!silent)  {
      table = document.findTable ( tblId );
      if (table)
        table->putHorzHeader ( header,tooltip,tcol );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_shape_horz_theader ( const char * tblId,
                                const int    tcol,
                                const char * cell_style,
                                const char * cell_css,
                                const int    rowSpan,
                                const int    colSpan )  {

  try {
    rvapi::setRVAPIError(0);
    rvapi::Table *table;
    if (!silent)  {
      table = document.findTable ( tblId );
      if (table)
        table->shapeHorzHeader ( tcol,cell_style,cell_css,
                                 rowSpan,colSpan );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}


extern "C"
void rvapi_put_vert_theader ( const char * tblId,
                              const char * header,
                              const char * tooltip,
                              const int    trow )  {
  try {
    rvapi::setRVAPIError(0);
    rvapi::Table *table;
    if (!silent)  {
      table = document.findTable ( tblId );
      if (table)
        table->putVertHeader ( header,tooltip,trow );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_shape_vert_theader ( const char * tblId,
                                const int    trow,
                                const char * cell_style,
                                const char * cell_css,
                                const int    rowSpan,
                                const int    colSpan )  {

  try {
    rvapi::setRVAPIError(0);
    rvapi::Table *table;
    if (!silent)  {
      table = document.findTable ( tblId );
      if (table)
        table->shapeVertHeader ( trow,cell_style,cell_css,
                                 rowSpan,colSpan );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_put_table_string ( const char * tblId,
                              const char * s,
                              const int    trow,
                              const int    tcol )  {
  try {
    rvapi::setRVAPIError(0);
    rvapi::Table *table;
    if (!silent)  {
      table = document.findTable ( tblId );
      if (table)
        table->putValue ( s,trow,tcol );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_put_table_int ( const char * tblId,
                           const int    v,
                           const int    trow,
                           const int    tcol )  {
  try {
    rvapi::setRVAPIError(0);
    rvapi::Table *table;
    if (!silent)  {
      table = document.findTable ( tblId );
      if (table)
        table->putValue ( v,trow,tcol );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_put_table_real ( const char * tblId,
                            const double  v,
                            const char * fmt,
                            const int    trow,
                            const int    tcol )  {
  try {
    rvapi::setRVAPIError(0);
    rvapi::Table *table;
    if (!silent)  {
      table = document.findTable ( tblId );
      if (table)
        table->putValue ( v,fmt,trow,tcol );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_shape_table_cell ( const char * tblId,
                              const int    trow,
                              const int    tcol,
                              const char * tooltip,
                              const char * cell_style,
                              const char * cell_css,
                              const int    rowSpan,
                              const int    colSpan )  {
  try {
    rvapi::setRVAPIError(0);
    rvapi::Table *table;
    if (!silent)  {
      table = document.findTable ( tblId );
      if (table)
        table->shapeCell ( trow,tcol,tooltip,cell_style,cell_css,
                           rowSpan,colSpan );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}


  /*  ===========
      DATA BLOCKS
      ===========  */

extern "C"
void rvapi_add_data ( const char * datId,
                      const char * datTitle,
                      const char * uri,
                      const char * type,
                      const char * holderId,
                      const int    row,
                      const int    col,
                      const int    rowSpan,
                      const int    colSpan,
                      const int    foldState
                    )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.addData ( datId,datTitle,uri,type,holderId,
                         row,col,rowSpan,colSpan,foldState );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_add_data1 ( const char * path,
                       const char * datTitle,
                       const char * uri,
                       const char * type,
                       const int    row,
                       const int    col,
                       const int    rowSpan,
                       const int    colSpan,
                       const int    foldState
                     )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      std::vector<std::string> list = split ( path );
      if (list.size()==2)
        rvapi_add_data ( list[1].c_str(),datTitle,uri,type,list[0].c_str(),
                         row,col,rowSpan,colSpan, foldState );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_append_data ( const char * datId,
                         const char * datTitle,
                         const char * uri,
                         const char * type,
                         const char * holderId,
                         const int    foldState
                       )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.addData ( datId,datTitle,uri,type,holderId,
                         -1,0,1,1,foldState );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_append_data1 ( const char * path,
                          const char * datTitle,
                          const char * uri,
                          const char * type,
                          const int    foldState
                        )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      std::vector<std::string> list = split ( path );
      if (list.size()==2)
        rvapi_append_data ( list[1].c_str(),datTitle,uri,type,
                            list[0].c_str(), foldState );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_append_to_data ( const char * datId,
                            const char * uri,
                            const char * type
                          )  {
  try {
    rvapi::setRVAPIError(0);
    rvapi::Data *data;
    if (!silent)  {
      data = document.findData ( datId );
      if (data)
        data->addData ( uri,type );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}



  /*  ==========
        GRAPHS
      ==========  */

extern "C"
void rvapi_add_loggraph ( const char * gwdId,
                          const char * holderId,
                          const int    row,
                          const int    col,
                          const int    rowSpan,
                          const int    colSpan
                        )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.addLogGraph ( gwdId,holderId,row,col,rowSpan,colSpan );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_add_loggraph1 ( const char * path,
                           const int    row,
                           const int    col,
                           const int    rowSpan,
                           const int    colSpan
                         )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      std::vector<std::string> list = split ( path );
      if (list.size()==2)
        rvapi_add_loggraph ( list[1].c_str(),list[0].c_str(),
                             row,col,rowSpan,colSpan );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_append_loggraph ( const char * gwdId,
                             const char * holderId
                           )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.addLogGraph ( gwdId,holderId,-1,0,1,1 );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_append_loggraph1 ( const char * path  // "holderId/gwdId"
                            )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      std::vector<std::string> list = split ( path );
      if (list.size()==2)
        rvapi_append_loggraph ( list[1].c_str(),list[0].c_str() );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_add_graph ( const char * graphId,
                       const char * holderId,
                       const int    row,
                       const int    col,
                       const int    rowSpan,
                       const int    colSpan
                     )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.addGraph ( graphId,holderId,row,col,rowSpan,colSpan );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_set_graph_size ( const char * gwdId,
                            const int    width,
                            const int    height
                          )  {
  try {
    rvapi::setRVAPIError(0);
    rvapi::Graph *graph;
    if (!silent)  {
      graph = document.findGraph ( gwdId );
      if (graph)
        graph->setGraphSize ( width,height );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_add_graph_data ( const char * gdtId,
                            const char * gwdId,
                            const char * gdtTitle
                          )  {
  try {
    rvapi::setRVAPIError(0);
    rvapi::Graph *graph;
    if (!silent)  {
      graph = document.findGraph ( gwdId );
      if (graph)
        graph->addGraphData ( gdtId,gdtTitle );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_add_graph_data1 ( const char * path,
                             const char * gdtTitle
                           )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      std::vector<std::string> list = split ( path );
      if (list.size()==2)
        rvapi_add_graph_data ( list[1].c_str(),list[0].c_str(),
                               gdtTitle );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}


extern "C"
void rvapi_add_graph_dataset ( const char * setId,
                               const char * gdtId,
                               const char * gwdId,
                               const char * setName,
                               const char * setHeader
                             )  {
rvapi::Graph     *graph;
rvapi::GraphData *graphData;
  rvapi::setRVAPIError(0);
  try {
    if (!silent)  {
      graph = document.findGraph ( gwdId );
      if (graph)  {
        graphData = graph->getGraphData ( gdtId );
        if (graphData)
          graphData->putColSpec ( setId,setName,setHeader,-1 );
      }
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_add_graph_dataset1 ( const char * path, // "gwdId/gdtId/setId"
                                const char * setName,
                                const char * setHeader
                              )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      std::vector<std::string> list = split ( path );
      if (list.size()==3)
        rvapi_add_graph_dataset ( list[2].c_str(),list[1].c_str(),
                                  list[0].c_str(),setName,setHeader );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}


extern "C"
void rvapi_reset_graph_dataset ( const char * setId,
                                 const char * gdtId,
                                 const char * gwdId
                               )  {
rvapi::Graph     *graph;
rvapi::GraphData *graphData;
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      graph = document.findGraph ( gwdId );
      if (graph)  {
        graphData = graph->getGraphData ( gdtId );
        if (graphData)
          graphData->resetData ( setId );
      }
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_reset_graph_dataset1 ( const char * path // "gwdId/gdtId/setId"
                                )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      std::vector<std::string> list = split ( path );
      if (list.size()==3)
        rvapi_reset_graph_dataset ( list[2].c_str(),list[1].c_str(),
                                    list[0].c_str() );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}


extern "C"
void rvapi_add_graph_int ( const char * setId,
                           const char * gdtId,
                           const char * gwdId,
                           const int    v
                         )  {
rvapi::Graph     *graph;
rvapi::GraphData *graphData;
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      graph = document.findGraph ( gwdId );
      if (graph)  {
        graphData = graph->getGraphData ( gdtId );
        if (graphData)
          graphData->putValue ( v,setId );
      }
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_add_graph_int1 ( const char * path, // "gwdId/gdtId/setId"
                            const int    v
                          )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      std::vector<std::string> list = split ( path );
      if (list.size()==3)
        rvapi_add_graph_int ( list[2].c_str(),list[1].c_str(),
                              list[0].c_str(),v );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_add_graph_real ( const char * setId,
                            const char * gdtId,
                            const char * gwdId,
                            const double  v,
                            const char * fmt
                          )  {
rvapi::Graph     *graph;
rvapi::GraphData *graphData;
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      graph = document.findGraph ( gwdId );
      if (graph)  {
        graphData = graph->getGraphData ( gdtId );
        if (graphData)
          graphData->putValue ( v,fmt,setId );
      }
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_add_graph_real1 ( const char * path, // "gwdId/gdtId/setId"
                             const double  v,
                             const char * fmt
                           )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      std::vector<std::string> list = split ( path );
      if (list.size()==3)
        rvapi_add_graph_real ( list[2].c_str(),list[1].c_str(),
                               list[0].c_str(),v,fmt );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_add_graph_plot ( const char * pltId,
                            const char * gwdId,
                            const char * pltTitle,
                            const char * xName,
                            const char * yName
                          )  {
rvapi::Graph *graph;
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      graph = document.findGraph ( gwdId );
      if (graph)
        graph->addPlot ( pltId,pltTitle,xName,yName );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_add_graph_plot1 ( const char * path, // "gwdId/pltId"
                             const char * pltTitle,
                             const char * xName,
                             const char * yName
                           )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      std::vector<std::string> list = split ( path );
      if (list.size()==2)
        rvapi_add_graph_plot ( list[1].c_str(),list[0].c_str(),
                               pltTitle,xName,yName );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}


rvapi::Plot * _get_plot ( const char * pltId, const char * gwdId )  {
rvapi::Graph *graph;
  graph = document.findGraph ( gwdId );
  if (graph)  return graph->getPlot ( pltId );
        else  return NULL;
}

extern "C"
void rvapi_add_plot_line ( const char * pltId,  // plot id
                           const char * gdtId,  // data id
                           const char * gwdId,  // graph widget id
                           const char * xsetId,
                           const char * ysetId
                         )  {
rvapi::Plot *plot;
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      plot = _get_plot ( pltId,gwdId );
      if (plot)
        plot->addPlotLine ( new rvapi::PlotLine(gdtId,xsetId,ysetId) );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_add_plot_line1 ( const char * path,   // "gwdId/gdtId/pltId"
                            const char * xsetId, // "xsetId"
                            const char * ysetId  // "ysetId"
                          )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      std::vector<std::string> p = split ( path );
      if (p.size()==3)
        rvapi_add_plot_line ( p[2].c_str(),p[1].c_str(),p[0].c_str(),
                              xsetId,ysetId );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_set_line_options   ( const char * ysetId, // "ysetId"
                                const char * pltId,  // plot id
                                const char * gdtId,  // data id
                                const char * gwdId,  // graph widget id
                                const char * color,  // Html color
                                const char * style,  // RVAPI_LINE_XXX
                                const char * marker, // RVAPI_MARKER_XXX
                                const double width,  // 2.5 default
                                const bool   shown
                              )  {
rvapi::Plot *plot;
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      plot = _get_plot ( pltId,gwdId );
      if (plot)  {
        rvapi::PlotLine *line = plot->getLine ( gdtId,ysetId );
        if (line)  {
          if (color)  {
            if (color[0])  line->color  = color;
          }
          if (style)  {
            if (style[0])  line->style  = style;
          }
          if (marker)  {
            if (marker[0]) line->marker = marker;
          }
          if (width>0.0)
            line->width = width;
          line->shown = shown;
        }
      }
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_set_line_options1 ( const char * path, // "gwdId/gdtId/pltId/ysetId"
                               const char * color,  // Html color
                               const char * style,  // RVAPI_LINE_XXX
                               const char * marker, // RVAPI_MARKER_XXX
                               const double  width,  // 2.5 default
                               const bool   shown
                              )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      std::vector<std::string> p = split ( path );
      if (p.size()==4)
        rvapi_set_line_options ( p[3].c_str(),p[2].c_str(),
                                 p[1].c_str(),p[0].c_str(),
                                 color,style,marker,width,shown );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}


extern "C"
void rvapi_set_line_fill ( const char * ysetId, // "ysetId"
                           const char * pltId,  // plot id
                           const char * gdtId,  // data id
                           const char * gwdId,  // graph widget id
                           const bool   fill,   // fill under line
                           const bool   fillAndStroke, // keep line
                           const char * fillColor, // Html color
                           const double fillAlpha  // 0..1
                         )  {
rvapi::Plot *plot;
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      plot = _get_plot ( pltId,gwdId );
      if (plot)  {
        rvapi::PlotLine *line = plot->getLine ( gdtId,ysetId );
        if (line)  {
          line->fill          = fill;
          line->fillAndStroke = fillAndStroke;
          line->fillColor     = fillColor;
          line->fillAlpha     = fillAlpha;
        }
      }
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

void rvapi_set_line_fill1 ( const char * path, // "gwdId/gdtId/pltId/ysetId"
                            const bool   fill,   // fill under line
                            const bool   fillAndStroke, // keep line
                            const char * fillColor,  // Html color
                            const double fillAlpha   // 0..1
                          )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      std::vector<std::string> p = split ( path );
      if (p.size()==4)
        rvapi_set_line_fill ( p[3].c_str(),p[2].c_str(),
                              p[1].c_str(),p[0].c_str(),
                              fill,fillAndStroke,fillColor,fillAlpha );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_set_default_plot ( const char * pltId,
                              const char * gwdId
                            )  {
rvapi::Graph *graph;
  try {
    rvapi::setRVAPIError(0);
    graph = document.findGraph ( gwdId );
    if (graph)
      graph->setDefPlot ( pltId );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_set_default_plot1 ( const char * path )  {
  try {
    rvapi::setRVAPIError(0);
    std::vector<std::string> list = split ( path );
    if (list.size()==2)
      rvapi_set_default_plot ( list[1].c_str(),list[0].c_str() );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_set_plot_log ( const char * pltId,
                          const char * gwdId,
                          const bool   logx,
                          const bool   logy
                        )  {
rvapi::Plot *plot;
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      plot = _get_plot ( pltId,gwdId );
      if (plot)  {
        plot->setLogX ( logx );
        plot->setLogY ( logy );
      }
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_set_plot_log1 ( const char * path,
                           const bool   logx,
                           const bool   logy
                         )  {
  try {
    rvapi::setRVAPIError(0);
    std::vector<std::string> list = split ( path );
    if (list.size()==2)
      rvapi_set_plot_log ( list[1].c_str(),list[0].c_str(),
                           logx,logy );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_set_plot_int ( const char * pltId,
                          const char * gwdId,
                          const bool   intx,
                          const bool   inty
                        )  {
rvapi::Plot *plot;
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      plot = _get_plot ( pltId,gwdId );
      if (plot)  {
        plot->setIntX ( intx );
        plot->setIntY ( inty );
      }
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_set_plot_int1 ( const char * path,
                           const bool   intx,
                           const bool   inty
                         )  {
  try {
    rvapi::setRVAPIError(0);
    std::vector<std::string> list = split ( path );
    if (list.size()==2)
      rvapi_set_plot_log ( list[1].c_str(),list[0].c_str(),
                           intx,inty );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_set_plot_xmin ( const char * pltId,
                           const char * gwdId,
                           const double xmin
                         )  {
rvapi::Plot *plot;
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      plot = _get_plot ( pltId,gwdId );
      if (plot)
        plot->setXMin ( xmin );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_set_plot_xmin1 ( const char * path,
                            const double xmin
                          )  {
  try {
    rvapi::setRVAPIError(0);
    std::vector<std::string> list = split ( path );
    if (list.size()==2)
      rvapi_set_plot_xmin ( list[1].c_str(),list[0].c_str(),xmin );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_set_plot_xmax ( const char * pltId,
                           const char * gwdId,
                           const double xmax
                         )  {
rvapi::Plot *plot;
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      plot = _get_plot ( pltId,gwdId );
      if (plot)
        plot->setXMax ( xmax );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_set_plot_xmax1 ( const char * path,
                            const double xmax
                          )  {
  try {
    rvapi::setRVAPIError(0);
    std::vector<std::string> list = split ( path );
    if (list.size()==2)
      rvapi_set_plot_xmax ( list[1].c_str(),list[0].c_str(),xmax );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_set_plot_xrange ( const char * pltId,
                             const char * gwdId,
                             const double  xmin,
                             const double  xmax
                           )  {
rvapi::Plot *plot;
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      plot = _get_plot ( pltId,gwdId );
      if (plot)
        plot->setXRange ( xmin,xmax );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_set_plot_xrange1 ( const char * path,
                              const double xmin,
                              const double xmax
                            )  {
  try {
    rvapi::setRVAPIError(0);
    std::vector<std::string> list = split ( path );
    if (list.size()==2)
      rvapi_set_plot_xrange ( list[1].c_str(),list[0].c_str(),xmin,xmax );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_set_plot_ymin ( const char * pltId,
                           const char * gwdId,
                           const double  ymin
                         )  {
rvapi::Plot *plot;
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      plot = _get_plot ( pltId,gwdId );
      if (plot)
        plot->setYMin ( ymin );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_set_plot_ymin1 ( const char * path,
                            const double ymin
                          )  {
  try {
    rvapi::setRVAPIError(0);
    std::vector<std::string> list = split ( path );
    if (list.size()==2)
      rvapi_set_plot_ymin ( list[1].c_str(),list[0].c_str(),ymin );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_set_plot_ymax ( const char * pltId,
                           const char * gwdId,
                           const double  ymax
                         )  {
rvapi::Plot *plot;
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      plot = _get_plot ( pltId,gwdId );
      if (plot)
        plot->setYMax ( ymax );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_set_plot_ymax1 ( const char * path,
                            const double ymax
                          )  {
  try {
    rvapi::setRVAPIError(0);
    std::vector<std::string> list = split ( path );
    if (list.size()==2)
      rvapi_set_plot_ymax ( list[1].c_str(),list[0].c_str(),ymax );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_set_plot_yrange ( const char * pltId,
                             const char * gwdId,
                             const double  ymin,
                             const double  ymax
                           )  {
rvapi::Plot *plot;
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      plot = _get_plot ( pltId,gwdId );
      if (plot)
        plot->setYRange ( ymin,ymax );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_set_plot_yrange1 ( const char * path,
                              const double ymin,
                              const double ymax
                            )  {
  try {
    rvapi::setRVAPIError(0);
    std::vector<std::string> list = split ( path );
    if (list.size()==2)
      rvapi_set_plot_yrange ( list[1].c_str(),list[0].c_str(),ymin,ymax );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_set_plot_xslider ( const char * pltId,
                              const char * gwdId,
                              const double smin,
                              const double smax
                            )  {
rvapi::Plot *plot;
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      plot = _get_plot ( pltId,gwdId );
      if (plot)
        plot->setXSlider ( smin,smax );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_set_plot_xslider1 ( const char * path,
                               const double smin,
                               const double smax
                             )  {
  try {
    rvapi::setRVAPIError(0);
    std::vector<std::string> list = split ( path );
    if (list.size()==2)
      rvapi_set_plot_xslider ( list[1].c_str(),list[0].c_str(),smin,smax );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_set_plot_yslider ( const char * pltId,
                              const char * gwdId,
                              const double smin,
                              const double smax
                            )  {
rvapi::Plot *plot;
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      plot = _get_plot ( pltId,gwdId );
      if (plot)
        plot->setYSlider ( smin,smax );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_set_plot_yslider1 ( const char * path,
                               const double smin,
                               const double smax
                             )  {
  try {
    rvapi::setRVAPIError(0);
    std::vector<std::string> list = split ( path );
    if (list.size()==2)
      rvapi_set_plot_yslider ( list[1].c_str(),list[0].c_str(),smin,smax );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_set_plot_legend ( const char * pltId,
                             const char * gwdId,
                             const char * legendLocation,
                             const char * legendPlacement
                           )  {
rvapi::Plot *plot;
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      plot = _get_plot ( pltId,gwdId );
      if (plot)  {
        plot->setLegendLocation  ( legendLocation  );
        plot->setLegendPlacement ( legendPlacement );
      }
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_set_plot_legend1 ( const char * path,
                              const char * legendLocation,
                              const char * legendPlacement
                            )  {
  try {
    rvapi::setRVAPIError(0);
    std::vector<std::string> list = split ( path );
    if (list.size()==2)
      rvapi_set_plot_legend ( list[1].c_str(),list[0].c_str(),
                              legendLocation,legendPlacement );
  } catch(...) { rvapi::setRVAPIError(1); }
}


extern "C"
void rvapi_reset_plot_xticks ( const char * pltId,
                               const char * gwdId
                             )  {
rvapi::Plot *plot;
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      plot = _get_plot ( pltId,gwdId );
      if (plot)
        plot->clearXTicks();
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_reset_plot_xticks1 ( const char * path   // "gwdId/pltId"
                              )  {
  try {
    rvapi::setRVAPIError(0);
    std::vector<std::string> list = split ( path );
    if (list.size()==2)
      rvapi_reset_plot_xticks ( list[1].c_str(),list[0].c_str() );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_reset_plot_yticks ( const char * pltId,
                               const char * gwdId
                             )  {
rvapi::Plot *plot;
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      plot = _get_plot ( pltId,gwdId );
      if (plot)
        plot->clearYTicks();
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_reset_plot_yticks1 ( const char * path   // "gwdId/pltId"
                              )  {
  try {
    rvapi::setRVAPIError(0);
    std::vector<std::string> list = split ( path );
    if (list.size()==2)
      rvapi_reset_plot_yticks ( list[1].c_str(),list[0].c_str() );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_add_plot_xtick ( const char * pltId,
                            const char * gwdId,
                            const double value,
                            const char * label
                          )  {
rvapi::Plot *plot;
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      plot = _get_plot ( pltId,gwdId );
      if (plot)
        plot->addXTick ( value,label );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_add_plot_xtick1 ( const char * path,   // "gwdId/pltId"
                             const double value,
                             const char * label
                           )  {
  try {
    rvapi::setRVAPIError(0);
    std::vector<std::string> list = split ( path );
    if (list.size()==2)
      rvapi_add_plot_xtick ( list[1].c_str(),list[0].c_str(),value,label );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_add_plot_ytick ( const char * pltId,
                            const char * gwdId,
                            const double value,
                            const char * label
                          )  {
rvapi::Plot *plot;
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      plot = _get_plot ( pltId,gwdId );
      if (plot)
        plot->addYTick ( value,label );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_add_plot_ytick1 ( const char * path,   // "gwdId/pltId"
                             const double value,
                             const char * label
                           )  {
  try {
    rvapi::setRVAPIError(0);
    std::vector<std::string> list = split ( path );
    if (list.size()==2)
      rvapi_add_plot_ytick ( list[1].c_str(),list[0].c_str(),value,label );
  } catch(...) { rvapi::setRVAPIError(1); }
}



  /*  =================
        RADAR WIDGET
      =================  */


extern "C"
void rvapi_add_radar ( const char * radarId,
                       const char * radarTitle,
                       const char * holderId,
                       const int    row,
                       const int    col,
                       const int    rowSpan,
                       const int    colSpan,
                       const int    foldState
                     )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.addRadar ( radarId,radarTitle,holderId,
                          row,col,rowSpan,colSpan,foldState );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_add_radar1 ( const char * path,  // "holderId/radarId"
                        const char * radarTitle,
                        const int    row,
                        const int    col,
                        const int    rowSpan,
                        const int    colSpan,
                        const int    foldState
                      )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      std::vector<std::string> list = split ( path );
      if (list.size()==2)
        rvapi_add_radar ( list[1].c_str(),radarTitle,list[0].c_str(),
                          row,col,rowSpan,colSpan,foldState );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_append_radar ( const char * radarId,
                          const char * radarTitle,
                          const char * holderId,
                          const int    foldState
                        )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.addRadar ( radarId,radarTitle,holderId,-1,0,1,1,foldState );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_append_radar1 ( const char * path,  // "holderId/radarId"
                           const char * radarTitle,
                           const int    foldState
                         )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      std::vector<std::string> list = split ( path );
      if (list.size()==2)
        rvapi_append_radar ( list[1].c_str(),radarTitle,list[0].c_str(),
                             foldState );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_add_radar_property ( const char * radarId,
                                const char * name,
                                const double  value
                              )  {
rvapi::Radar * radar;
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      radar = document.findRadar ( radarId );
      if (radar)
        radar->addProperty ( name,value );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}



  /*  ==========
        TREES
      ==========  */

extern "C"
void rvapi_add_tree_widget ( const char * treeId,
                             const char * title,
                             const char * holderId,
                             const int    row,
                             const int    col,
                             const int    rowSpan,
                             const int    colSpan
                           )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.addTreeWidget ( treeId,title,holderId,
                               row,col,rowSpan,colSpan );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_add_tree_widget1 ( const char * path,
                              const char * title,
                              const int    row,
                              const int    col,
                              const int    rowSpan,
                              const int    colSpan
                            )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      std::vector<std::string> list = split ( path );
      if (list.size()==2)
        rvapi_add_tree_widget ( list[1].c_str(),title,list[0].c_str(),
                                 row,col,rowSpan,colSpan );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_append_tree_widget ( const char * gwdId,
                                const char * title,
                                const char * holderId
                              )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.addTreeWidget ( gwdId,title,holderId,-1,0,1,1 );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_append_tree_widget1 ( const char * path, // "holderId/gwdId"
                                 const char * title
                               )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      std::vector<std::string> list = split ( path );
      if (list.size()==2)
        rvapi_append_tree_widget ( list[1].c_str(),title,list[0].c_str() );
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_set_tree_node ( const char * treeId,
                           const char * nodeId,    // node to set
                           const char * title,     // title to set
                           const char * openState, // title to set
                           const char * parentId   // parent tree node
                         )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.setTreeNode ( treeId,nodeId,title,openState,parentId );
  } catch(...) { rvapi::setRVAPIError(1); }
}



  /*  ================
        PROGRESS BAR
      ================  */

extern "C"
void rvapi_toolbar_progress ( int key,   // 0: hide; 1: show;
                                         // 2: set range;
                                         // 3: set value
                              int value  // either range or value
                            )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.setToolbarProgress ( key,value );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_add_progress_bar ( const char * pbarId,
                              const char * holderId,
                              const int    row,
                              const int    col,
                              const int    rowSpan,
                              const int    colSpan
                            )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.addProgressBar ( pbarId,holderId,row,col,rowSpan,colSpan );
  } catch(...) { rvapi::setRVAPIError(1); }
}


extern "C"
void rvapi_set_progress_value ( const char * pbarId,
                                const int key,  // 0: hide; 1: show;
                                                // 2: set range;
                                                // 3: set value
                                const int value // either range or value
                              )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.setProgressValue ( pbarId,key,value );
  } catch(...) { rvapi::setRVAPIError(1); }
}

  /*  ===================
        FORMS and INPUTS
      ===================  */

extern "C"
void rvapi_add_form ( const char * formId,
                      const char * action,
                      const char * method,
                      const char * holderId,
                      const int    hrow,
                      const int    hcol,
                      const int    hrowSpan,
                      const int    hcolSpan
                    )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.addForm ( formId,action,method,holderId,
                         hrow,hcol,hrowSpan,hcolSpan );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_add_file_upload ( const char * inpId,
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
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.addFileUpload ( inpId,name,value,length,required,formId,
                               hrow,hcol,hrowSpan,hcolSpan );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_add_line_edit ( const char * inpId,
                           const char * name,
                           const char * value,
                           const int    length,
                           const char * formId,
                           const int    hrow,
                           const int    hcol,
                           const int    hrowSpan,
                           const int    hcolSpan
                         )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.addLineEdit ( inpId,name,value,length,formId,
                               hrow,hcol,hrowSpan,hcolSpan );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_add_hidden_text ( const char * inpId,
                             const char * name,
                             const char * text,
                             const char * formId,
                             const int    hrow,
                             const int    hcol,
                             const int    hrowSpan,
                             const int    hcolSpan
                           )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.addHiddenText ( inpId,name,text,formId,
                               hrow,hcol,hrowSpan,hcolSpan );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_add_submit_button ( const char * inpId,
                               const char * title,
                               const char * formAction,
                               const char * formId,
                               const int    hrow,
                               const int    hcol,
                               const int    hrowSpan,
                               const int    hcolSpan
                             )  {

  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.addSubmitButton ( inpId,title,formAction,formId,
                                 hrow,hcol,hrowSpan,hcolSpan );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_add_button ( const char * inpId,
                        const char * value,
                        const char * command,
                        const char * data,
                        const bool   rvOnly,
                        const char * holderId,
                        const int    hrow,
                        const int    hcol,
                        const int    hrowSpan,
                        const int    hcolSpan
                      )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.addButton ( inpId,value,command,data,rvOnly,holderId,
                           hrow,hcol,hrowSpan,hcolSpan );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_add_checkbox ( const char * inpId,
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
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.addCheckbox ( inpId,title,name,value,command,data,checked,
                             holderId, hrow,hcol,hrowSpan,hcolSpan );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_add_combobox ( const char * cbxId,
                          const char * name,
                          const char * onChange,
                          const int    size,
                          const char * holderId,
                          const int    hrow,
                          const int    hcol,
                          const int    hrowSpan,
                          const int    hcolSpan
                        )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.addCombobox ( cbxId,name,onChange,size,
                             holderId,hrow,hcol,hrowSpan,hcolSpan );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_add_combobox_option ( const char * cbxId,
                                 const char * label,
                                 const char * value,
                                 const bool   selected
                               )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.addComboboxOption ( cbxId,label,value,selected );
  } catch(...) { rvapi::setRVAPIError(1); }
}


extern "C"
void rvapi_add_radio_button ( const char * inpId,
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
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.addRadioButton ( inpId,title,name,value,checked,holderId,
                                hrow,hcol,hrowSpan,hcolSpan );
  } catch(...) { rvapi::setRVAPIError(1); }
}


extern "C"
void rvapi_set_action_on_click ( const char * inpId,
                                 const char * actId,
                                 const char * action,
                                 const char * onClick )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      rvapi::Input * input = document.findInput ( inpId );
      if (input)  {
        std::string nsId;
        document.makeId ( nsId,actId );
        std::string cmd = "setValue('" + nsId +
                                   "','value','" + action + "');" +
                                   onClick;
        input->setOnClick ( cmd.c_str() );
      }
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_set_action_on_change ( const char * inpId,
                                  const char * actId,
                                  const char * action,
                                  const char * onChange )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)  {
      rvapi::Input * input = document.findInput ( inpId );
      if (input)  {
        std::string nsId;
        std::string cmd;
        document.makeId ( nsId,actId );
        if (input->type()==rvapi::NTYPE_RadioButton)
              cmd = "makeRadioButtonsAction('" + std::string(action) +
                        "','" + nsId +
                        "','value');";
        else  cmd = "setValue('" + nsId +
                             "','value','" + action + "');";
        cmd.append ( onChange );
        input->setOnChange ( cmd.c_str() );
      }
    }
  } catch(...) { rvapi::setRVAPIError(1); }
}


extern "C"
void rvapi_disable_form ( const char * formId,
                          const bool   disable )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.disableForm ( formId,disable );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_disable_input ( const char * inpId,
                           const bool   disable )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.disableInput ( inpId,disable );
  } catch(...) { rvapi::setRVAPIError(1); }
}



  /*  ==========
        OTHER
      ==========  */

extern "C"
void rvapi_remove_widget ( const char * widgetId )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.removeWidget ( widgetId );
  } catch(...) { rvapi::setRVAPIError(1); }
}



  /*  ==========
        OUTPUT
      ==========  */

extern "C"
void rvapi_flush() {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.flush();
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_set_time_quant ( const int quant ) {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.setTimeQuant ( quant );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_keep_polling ( const bool on )  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.setPolling ( on );
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
void rvapi_reset_task()  {
  try {
    rvapi::setRVAPIError(0);
    if (!silent)
      document.resetTask();
  } catch(...) { rvapi::setRVAPIError(1); }
}


  /*  ==========
        UTILS
      ==========  */

extern "C"
void rvapi_version ( char * version )  {
  sprintf ( version,"%i.%i.%i",
       rvapi::MAJOR_VERSION,rvapi::MINOR_VERSION,rvapi::MICRO_VERSION );
}

extern "C"
void rvapi_set_max_task_size ( const int max_size )  {
  if (!silent)
    document.setMaxTaskFileSize ( max_size );
}

extern "C"
void rvapi_make_hard_spaces ( char * dest, const char * source )  {
int i = 0;
int j = 0;
  try {
    rvapi::setRVAPIError(0);
  
    while (source[i])  {
      if (source[i]==' ')  {
        dest[j++] = '&';
        dest[j++] = 'n';
        dest[j++] = 'b';
        dest[j++] = 's';
        dest[j++] = 'p';
        dest[j++] = ';';
      } else
        dest[j++] = source[i];
      i++;
    }
  
    dest[j] = char(0);
  
  } catch(...) { rvapi::setRVAPIError(1); }
}

extern "C"
bool rvapi_exists ( const char * elementId )  {
std::string nsId;
  if (silent)  return false;
  return (document.findNode(document.makeId(nsId,elementId))!=NULL);
}


extern "C"
void rvapi_url_encode ( char * dest, const char * source )  {
char hex[20];
int  n = strlen(source);
int  i,j;

  try {
    rvapi::setRVAPIError(0);
    j = 0;
    for (i=0;i<n;i++)  {
      sprintf ( hex,"%X",int(source[i]) );
      dest[j++] = '%';
      if (strlen(hex)<2) {
        dest[j++] = '0';
        dest[j++] = hex[0];
      } else  {
        dest[j++] = hex[0];
        dest[j++] = hex[1];
      }
    }
  
    dest[j] = char(0);
  
  } catch(...) { rvapi::setRVAPIError(1); }

}


extern "C"
int rvapi_error()  { return rvapi::getRVAPIError(); }

