//
//  =================================================================
//
//    06.02.17   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_interface  <interface>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Classes :  rvapi::Node - API external functions
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2013-2017
//
//  =================================================================
//


#ifndef RVAPI_INTERFACE_H
#define RVAPI_INTERFACE_H


#include "rvapi_global.h"

extern "C" {

  /*  ==================
        INITIALISATION
      ==================  */

  /// \brief Initialisation of the report document.  
  /// \param docId  document Id, which must start with a letter and may
  ///               contain both letters and digits. The Id identifies a
  ///               particular report document, and in most cases it is
  ///               a fixed string value within a program. Document Id
  ///               serves as a namespace for all report elements with
  ///               Ids beginning with the '@' symbol. This namespacing
  ///               is for preventing clashes when report documents are
  ///               continued through a chain of subsequent runs of the
  ///               same or different programs, re-utilising the same
  ///               element ids within their codes. The actual internal
  ///               Ids are obtained by replacing the lead '@' symbol
  ///               with the document Id.
  /// \param outDir path to the existing output directory. All HTML
  ///               report files will be placed in this directory. This
  ///               parameter is mandatory.
  /// \param winTitle title to be placed on top of the window
  ///               (mandatory).
  /// \param mode   report output mode (mandatory). Output modes are
  ///               given as a combination of bitwise flags:
  ///    \arg \b RVAPI_MODE_Silent (0x00100000): no report is produced
  ///    \arg \b RVAPI_MODE_Html   (0x00000001): HTML5 report is produced
  ///    \arg \b RVAPI_MODE_Xmli2  (0x00000002): XML report for CCP4i2
  ///                                            is produced
  /// \param layout report layout mode (mandatory). Layout modes are
  ///               given as a combination of bitwise flags:
  ///    \arg \b RVAPI_LAYOUT_Header (0x00000001): report page will contain
  ///                             a header consisting of a logo (on the
  ///                             left) and  'winTitle' shown next to
  ///                             it
  ///    \arg \b RVAPI_LAYOUT_Toolbar (0x00000002): report page will
  ///                             contain a toolbar
  ///    \arg \b RVAPI_LAYOUT_Tabs (0x00000004): report page will
  ///                             have a tabbed layout
  ///    \arg \b RVAPI_LAYOUT_Full (0x00000007): report page will
  ///                             have all header, toolbar and tab row
  ///                             on the top
  /// \param jsUri  Uri to javascript support files (mandatory). Depending
  ///               on RVAPI setup, this may be either a Url to the
  ///               respective web-server, including the path to
  ///                jsrview directory, or a full path to same
  ///               directory on local file system. Although the HTML 
  ///               report may be generated in any directory specified
  ///               by  'outDir' parameter, the bootrstrap HTML page
  ///               will have fixed Uri references to  'jsrview'
  ///               javascript codes and style documents. Therefore,
  ///               RVAPI reports with local jsUri are movable only
  ///               within the system where they were produced.
  /// \param helpFName  optional name of HTML documentation related to
  ///               report. This documentation is displayed when
  ///                'Help' button in the Toolbar is pressed.
  ///               If NULL is passed for this parameter, no HTML
  ///               documentation will be used
  /// \param htmlFName  optional name of the report's bootrstrap HTML
  ///               file. If NULL is passed for this parameter,
  ///                'index.html' will be used. Note that  'htmlFName'
  ///               must not contain any path, and the file will be placed
  ///               in  'outDir' directory.
  /// \param taskFName  optional name of the report's task file which
  ///               contains report chronology and layout. If NULL is
  ///               passed for this parameter,  'task.tsk' will be used.
  ///               Note that  'taskFName' must not contain any path,
  ///               and the file will be placed in  'outDir' directory.
  /// \param xmli2FName  optional name of the XML output used in CCP4i2
  ///               instead of HTML report document. If NULL is
  ///               passed for this parameter,  'i2.xml' will be used.

  void rvapi_init_document ( const char * docId,     // mandatory
                             const char * outDir,    // mandatory
                             const char * winTitle,  // mandatory
                             const int    mode,      // mandatory
                             const int    layout,    // mandatory
                             const char * jsUri,     // needed
                             const char * helpFName, // may be NULL
                             const char * htmlFName, // may be NULL
                             const char * taskFName, // may be NULL
                             const char * xmli2FName // may be NULL
                           );


  /// \brief Stores the report document in file 'fname' in directory used
  /// for report initialisation; the document may be then restored and
  /// appended with new data.
  ///  \param fname file name to receive current report state. If
  ///                'fname' is NULL and the document was restored
  ///               from a saved state, then the same file will
  ///               be re-used. Note that  'fname' must not contain
  ///               any path, and the file will be placed in directory
  ///               where the document was initialised.

  void rvapi_store_document ( const char * fname );


  /// \brief Stores the report document in file given by path 'fpath';
  /// the document may be then restored and appended with new data.
  ///  \param fpath path to file to receive current report state. The
  ///               file may be located anywhere. In difference of
  ///               rvapi_store_document() function, this one stores
  ///               data in such way that no preliminary initialisation
  ///               of the document is required (see
  ///               rvapi_restore_document2()).

  void rvapi_store_document2 ( const char * fpath );


  /// \brief Restores the report document saved in file 'fname' in directory
  /// used for report initialisation; the restored document may be
  /// appended with new data and saved again if necessary.
  ///  \param fname file name to read the report state from. Note that
  ///               the report document should be initialised before
  ///               using this function.  'fname' must not contain
  ///               any path, and the file will be placed in directory
  ///               where the document was initialised. If file  'fname'
  ///               is not found, then the function does nothing apart
  ///               from storing  'fname' in the document.

  void rvapi_restore_document ( const char * fname );

  /// \brief Restores the report document saved in file given by path
  /// 'fpath'; the restored document may be appended with new data
  /// and saved again if necessary.
  ///  \param fpath path to file to read the report state from. Note
  ///               that this file must have been created by 
  ///               function rvapi_store_document2(), and there is no
  ///               need to intialise the document before using this
  ///               function.

  void rvapi_restore_document2 ( const char * fpath );


  /// \brief Putting optional metadata, which can be passed between
  /// processes sharing the same document

  void rvapi_put_meta ( const char * metastring );


  /// \brief Getting optional metadata, which can be passed between
  /// processes sharing the same document

  const char * rvapi_get_meta();



  /*  ==================
        PAGE FORMATION
      ==================  */


  /// \brief Adds header to document initialised with  'RVAPI_LAYOUT_Header'
  /// layout flag; the header is displayed on top of the page, on the
  /// right from logo icon.
  ///   \param htmlString a string with possible HTML formatting,
  ///                     representing the header
  void rvapi_add_header   ( const char * htmlString );

  /// \brief Adds tab to report document initialised with  'RVAPI_LAYOUT_Tabs'
  /// layout flag.
  ///  \param tabId  a string of letters and numbers representing a
  ///                \bold unique Id in the document. All future operations
  ///                with the tan will reference to this Id
  ///  \param tabName a string of characters representing tab name, which
  ///                 appears on the tab
  ///  \param open    specifies whether the tab should appear open or
  ///                 closed. First tab will be always open once created.
  ///                 Other tabs may be created open ( 'open'==true) or
  ///                 closed ( 'open'==false). Open tabs will receive
  ///                 focus and show their content immediately upon
  ///                 creation; closed tabs will remain closed until
  ///                 user clicks on them.

  void rvapi_add_tab      ( const char * tabId,
                            const char * tabName,
                            const bool   open
                          );

  /// \brief Inserts a tab into report document initialised with
  ///  'RVAPI_LAYOUT_Tabs' layout flag.
  ///  \param tabId  a string of letters and numbers representing a
  ///                \bold unique Id in the document. All future operations
  ///                with the tan will reference to this Id
  ///  \param tabName a string of characters representing tab name, which
  ///                 appears on the tab
  ///  \param beforeTabId  Id of an existing tab, on the left-hand side
  ///                 of which the new tab will be created
  ///  \param open    specifies whether the tab should appear open or
  ///                 closed. First tab will be always open once created.
  ///                 Other tabs may be created open ( 'open'==true) or
  ///                 closed ( 'open'==false). Open tabs will receive
  ///                 focus and show their content immediately upon
  ///                 creation; closed tabs will remain closed until
  ///                 user clicks on them.

  void rvapi_insert_tab   ( const char * tabId,
                            const char * tabName,
                            const char * beforeTabId,
                            const bool   open
                          );

  /// \brief Removes tab from the report document initialised with
  ///  'RVAPI_LAYOUT_Tabs' layout flag.
  ///  \param tabId  Id of tab to be removed. This must be the Id used
  ///                for creating (adding or inserting) the tab.

  void rvapi_remove_tab   ( const char * tabId );


  /*  ============
        SECTIONS
      ============  */

  void rvapi_add_section     ( const char * secId,
                               const char * secTitle,
                               const char * holderId,
                               const int    row,
                               const int    col,
                               const int    rowSpan,
                               const int    colSpan,
                               const bool   open
                             );

  void rvapi_add_section1    ( const char * path, // "holderId/secId"
                               const char * secTitle,
                               const int    row,
                               const int    col,
                               const int    rowSpan,
                               const int    colSpan,
                               const bool   open
                             );

  void rvapi_append_section  ( const char * secId,
                               const char * secTitle,
                               const char * holderId,
                               const bool   open
                             );

  void rvapi_append_section1 ( const char * path, // "holderId/secId"
                               const char * secTitle,
                               const bool   open
                             );

  void rvapi_set_section_state ( const char * secId,
                                 const bool   open
                               );


  /*  ============
        GRIDS
      ============  */

  void rvapi_add_grid     ( const char * gridId,
                            const bool   filling,
                            const char * holderId,
                            const int    row,
                            const int    col,
                            const int    rowSpan,
                            const int    colSpan
                          );

  void rvapi_add_grid1    ( const char * path, // "holderId/gridId"
                            const bool   filling,
                            const int    row,
                            const int    col,
                            const int    rowSpan,
                            const int    colSpan
                          );

  void rvapi_append_grid  ( const char * gridId,
                            const bool   filling,
                            const char * holderId
                          );

  void rvapi_append_grid1 ( const char * path, // "holderId/gridId"
                            const bool   filling
                          );

  void rvapi_set_cell_stretch ( const char * gridId,
                                const int    width,
                                const int    height,
                                const int    row,
                                const int    col
                              );

  /*  ============
        PANELS
      ============  */

  //  Panels are areas with grids

  void rvapi_add_panel    ( const char * panelId,
                            const char * holderId,
                            const int    row,
                            const int    col,
                            const int    rowSpan,
                            const int    colSpan
                          );

  void rvapi_add_panel1   ( const char * path, // "holderId/panelId"
                            const int    row,
                            const int    col,
                            const int    rowSpan,
                            const int    colSpan
                          );

  void rvapi_append_panel ( const char * panelId,
                            const char * holderId
                          );

  void rvapi_append_panel1 ( const char * path ); // "holderId/panelId"


  /*  ============
        FIELDSETS
      ============  */

  //  Fieldsets are areas with grids

  void rvapi_add_fieldset ( const char * fsetId,
                            const char * title,
                            const char * holderId,
                            const int    row,
                            const int    col,
                            const int    rowSpan,
                            const int    colSpan
                          );

  void rvapi_add_fieldset1 ( const char * path, // "holderId/fsetId"                           
                             const char * title,
                             const int    row,
                             const int    col,
                             const int    rowSpan,
                             const int    colSpan
                           );

  void rvapi_append_fieldset ( const char * fsetId,
                               const char * title,
                               const char * holderId
                             );

  void rvapi_append_fieldset1 ( const char * path,  // "holderId/fsetId"
                                const char * title
                              );



  /*  ===============
        TEXT WIDGET
      ===============  */


  void rvapi_append_text     ( const char * textString,
                               const char * holderId
                             );

  void rvapi_set_text        ( const char * textString,
                               const char * holderId,
                               const int    row,
                               const int    col,
                               const int    rowSpan,
                               const int    colSpan
                             );

  void rvapi_add_text        ( const char * textString,
                               const char * holderId,
                               const int    row,
                               const int    col,
                               const int    rowSpan,
                               const int    colSpan
                             );

  void rvapi_set_label       ( const char * labelId,
                               const char * holderId,
                               const char * labelText,
                               const int    row,
                               const int    col,
                               const int    rowSpan,
                               const int    colSpan
                             );

  void rvapi_add_label       ( const char * labelId,
                               const char * holderId,
                               const char * labelText,
                               const int    row,
                               const int    col,
                               const int    rowSpan,
                               const int    colSpan
                             );

  void rvapi_reset_label     ( const char * labelId,
                               const char * newTextString
                             );

  /*  ==================
        CONTENT WIDGET
      ==================  */


  void rvapi_append_content  ( const char * uri,
                               const bool   watch,
                               const char * holderId
                             );

  void rvapi_add_content     ( const char * uri,
                               const bool   watch,
                               const char * holderId,
                               const int    row,
                               const int    col,
                               const int    rowSpan,
                               const int    colSpan
                             );

  /*  ==========
        TABLES
      ==========  */


  /// Fold states:
  ///   0: the table is not foldable
  /// 100: the table is not foldable and spans to the whole width of
  ///      table holder
  ///  -1: the table is foldable and is initially folded
  ///   1: the table is foldable and is initially unfolded
  ///  -2: the table is foldable and is initially folded and spans to
  ///      the whole width of table holder
  ///   2: the table is foldable and is initially unfolded and spans to
  ///      the whole width of table holder
  void rvapi_add_table        ( const char * tblId,
                                const char * tblTitle,
                                const char * holderId,
                                const int    row,
                                const int    col,
                                const int    rowSpan,
                                const int    colSpan,
                                const int    foldState
                              );

  void rvapi_set_table_type   ( const char * tblId,
                                const bool sortable,
                                const bool paging );

  void rvapi_add_table1       ( const char * path,  // "holderId/tblId"
                                const char * tblTitle,
                                const int    row,
                                const int    col,
                                const int    rowSpan,
                                const int    colSpan,
                                const int    foldState
                              );

  void rvapi_append_table     ( const char * tblId,
                                const char * tblTitle,
                                const char * holderId,
                                const int    foldState
                              );

  void rvapi_append_table1    ( const char * path, // "holderId/tblId"
                                const char * tblTitle,
                                const int    foldState
                              );

  /// Table styles and alignments for data cells should be taken
  /// from rvapi_global.h
  void rvapi_set_table_style  ( const char * tblId,
                                const char * css,
                                const char * td_style
                              );

  void rvapi_put_horz_theader   ( const char * tblId,
                                  const char * header,
                                  const char * tooltip,
                                  const int    tcol
                                );
  void rvapi_shape_horz_theader ( const char * tblId,
                                  const int    tcol,
                                  const char * cell_style,
                                  const char * cell_css,
                                  const int    rowSpan,
                                  const int    colSpan
                                );

  void rvapi_put_vert_theader   ( const char * tblId,
                                  const char * header,
                                  const char * tooltip,
                                  const int    trow
                                );
  void rvapi_shape_vert_theader ( const char * tblId,
                                  const int    trow,
                                  const char * cell_style,
                                  const char * cell_css,
                                  const int    rowSpan,
                                  const int    colSpan
                                );
  void rvapi_put_table_string   ( const char * tblId,
                                  const char * s,
                                  const int    trow,
                                  const int    tcol
                                );
  void rvapi_put_table_int      ( const char * tblId,
                                  const int    v,
                                  const int    trow,
                                  const int    tcol
                                );
  void rvapi_put_table_real     ( const char * tblId,
                                  const double v,
                                  const char * fmt,
                                  const int    trow,
                                  const int    tcol
                                );
  void rvapi_shape_table_cell   ( const char * tblId,
                                  const int    trow,
                                  const int    tcol,
                                  const char * tooltip,
                                  const char * cell_style,
                                  const char * cell_css,
                                  const int    rowSpan,
                                  const int    colSpan
                                );

  /*  =======================
        DATA BLOCKS (FILES)
      =======================  */

  void rvapi_add_data       ( const char * datId,
                              const char * datTitle,
                              const char * uri,
                              const char * type,
                              const char * holderId,
                              const int    row,
                              const int    col,
                              const int    rowSpan,
                              const int    colSpan,
                              const int    foldState
                            );

  void rvapi_add_data1      ( const char * path, // "holderId/datId"
                              const char * datTitle,
                              const char * uri,
                              const char * type,
                              const int    row,
                              const int    col,
                              const int    rowSpan,
                              const int    colSpan,
                              const int    foldState
                            );

  void rvapi_append_data    ( const char * datId,
                              const char * datTitle,
                              const char * uri,
                              const char * type,
                              const char * holderId,
                              const int    foldState
                            );

  void rvapi_append_data1   ( const char * path, // "holderId/datId"
                              const char * datTitle,
                              const char * uri,
                              const char * type,
                              const int    foldState
                            );

  void rvapi_append_to_data ( const char * datId,
                              const char * uri,
                              const char * type
                            );


  /*  =================
        GRAPH WIDGETS
      =================  */

  void rvapi_add_loggraph       ( const char * gwdId,
                                  const char * holderId,
                                  const int    row,
                                  const int    col,
                                  const int    rowSpan,
                                  const int    colSpan
                                );

  void rvapi_add_loggraph1      ( const char * path,  // "holderId/gwdId"
                                  const int    row,
                                  const int    col,
                                  const int    rowSpan,
                                  const int    colSpan
                                );

  void rvapi_append_loggraph    ( const char * gwdId,
                                  const char * holderId
                                );

  void rvapi_append_loggraph1   ( const char * path  // "holderId/gwdId"
                                );

  void rvapi_add_graph          ( const char * graphId,
                                  const char * holderId,
                                  const int    row,
                                  const int    col,
                                  const int    rowSpan,
                                  const int    colSpan
                                );

  void rvapi_set_graph_size     ( const char * gwdId,
                                  const int    width,
                                  const int    height
                                );

  void rvapi_add_graph_data     ( const char * gdtId,
                                  const char * gwdId,
                                  const char * gdtTitle
                                );

  void rvapi_add_graph_data1    ( const char * path, // "gdtId/gwdId"
                                  const char * gdtTitle
                                );

  void rvapi_add_graph_dataset  ( const char * setId,
                                  const char * gdtId,
                                  const char * gwdId,
                                  const char * setName,
                                  const char * setHeader
                                );

  void rvapi_add_graph_dataset1 ( const char * path, // "gwdId/gdtId/setId"
                                  const char * setName,
                                  const char * setHeader
                                );

  void rvapi_reset_graph_dataset ( const char * setId,
                                   const char * gdtId,
                                   const char * gwdId
                                 );

  void rvapi_reset_graph_dataset1 ( const char * path // "gwdId/gdtId/setId"
                                  );

  void rvapi_add_graph_int      ( const char * setId,
                                  const char * gdtId,
                                  const char * gwdId,
                                  const int    v
                                );

  void rvapi_add_graph_int1     ( const char * path, // "gwdId/gdtId/setId"
                                  const int    v
                                );

  void rvapi_add_graph_real     ( const char * setId,
                                  const char * gdtId,
                                  const char * gwdId,
                                  const double v,
                                  const char * fmt
                                );

  void rvapi_add_graph_real1    ( const char * path, // "gphId/gdtId/setId"
                                  const double v,
                                  const char * fmt
                                );

  void rvapi_add_graph_plot     ( const char * pltId,
                                  const char * gwdId,
                                  const char * pltTitle,
                                  const char * xName,
                                  const char * yName
                                );

  void rvapi_add_graph_plot1    ( const char * path, // "gwdId/pltId"
                                  const char * pltTitle,
                                  const char * xName,
                                  const char * yName
                                );

  void rvapi_add_plot_line      ( const char * pltId, // plot id
                                  const char * gdtId, // data id
                                  const char * gwdId, // graph widget id
                                  const char * xsetId,
                                  const char * ysetId
                                );

  void rvapi_add_plot_line1     ( const char * path, // "gwdId/gdtId/pltId"
                                  const char * xsetId, // "xsetId"
                                  const char * ysetId  // "ysetId"
                                );

  /// empty color, style, marker and negative/zero width don't change
  /// the corresponding settings
  void rvapi_set_line_options   ( const char * ysetId, // "ysetId"
                                  const char * pltId,  // plot id
                                  const char * gdtId,  // data id
                                  const char * gwdId,  // graph widget id
                                  const char * color,  // Html color
                                  const char * style,  // RVAPI_LINE_XXX
                                  const char * marker, // RVAPI_MARKER_XXX
                                  const double width,  // 2.5 default
                                  const bool   shown
                                );

  void rvapi_set_line_options1  ( const char * path,   // "gwdId/gdtId/pltId/ysetId"
                                  const char * color,  // Html color
                                  const char * style,  // RVAPI_LINE_XXX
                                  const char * marker, // RVAPI_MARKER_XXX
                                  const double width,  // 2.5 default
                                  const bool   shown
                                );

  void rvapi_set_line_fill      ( const char * ysetId, // "ysetId"
                                  const char * pltId,  // plot id
                                  const char * gdtId,  // data id
                                  const char * gwdId,  // graph widget id
                                  const bool   fill,   // fill under line
                                  const bool   fillAndStroke, // keep line
                                  const char * fillColor, // Html color
                                  const double fillAlpha  // 0..1
                                );
  void rvapi_set_line_fill1     ( const char * path, // "gwdId/gdtId/pltId/ysetId"
                                  const bool   fill,   // fill under line
                                  const bool   fillAndStroke, // keep line
                                  const char * fillColor,  // Html color
                                  const double fillAlpha   // 0..1
                                );

  void rvapi_set_default_plot   ( const char * pltId,
                                  const char * gwdId
                                );
  void rvapi_set_default_plot1  ( const char * path  // "gwdId/pltId"
                                );

  void rvapi_set_plot_log       ( const char * pltId,
                                  const char * gwdId,
                                  const bool   logx,
                                  const bool   logy
                                );
  void rvapi_set_plot_log1      ( const char * path,   // "gwdId/pltId"
                                  const bool   logx,
                                  const bool   logy
                                );

  void rvapi_set_plot_int       ( const char * pltId,
                                  const char * gwdId,
                                  const bool   intx,
                                  const bool   inty
                                );
  void rvapi_set_plot_int1      ( const char * path,   // "gwdId/pltId"
                                  const bool   intx,
                                  const bool   inty
                                );

  /// this will set xmin and make xmax automatically calculated
  void rvapi_set_plot_xmin      ( const char * pltId,
                                  const char * gwdId,
                                  const double xmin
                                );
  void rvapi_set_plot_xmin1     ( const char * path,   // "gwdId/pltId"
                                  const double xmin
                                );

  /// this will set xmax and make xmin automatically calculated
  void rvapi_set_plot_xmax      ( const char * pltId,
                                  const char * gwdId,
                                  const double xmax
                                );
  void rvapi_set_plot_xmax1     ( const char * path,   // "gwdId/pltId"
                                  const double xmax
                                );

  /// this will set both xmin and xmax
  void rvapi_set_plot_xrange    ( const char * pltId,
                                  const char * gwdId,
                                  const double xmin,
                                  const double xmax
                                );
  void rvapi_set_plot_xrange1   ( const char * path,   // "gwdId/pltId"
                                  const double xmin,
                                  const double xmax
                                );

  /// this will set ymin and make ymax automatically calculated
  void rvapi_set_plot_ymin      ( const char * pltId,
                                  const char * gwdId,
                                  const double ymin
                                );
  void rvapi_set_plot_ymin1     ( const char * path,   // "gwdId/pltId"
                                  const double ymin
                                );

  /// this will set ymax and make ymin automatically calculated
  void rvapi_set_plot_ymax      ( const char * pltId,
                                  const char * gwdId,
                                  const double ymax
                                );
  void rvapi_set_plot_ymax1     ( const char * path,   // "gwdId/pltId"
                                  const double ymax
                                );

  /// this will set both ymin and ymax
  void rvapi_set_plot_yrange    ( const char * pltId,
                                  const char * gwdId,
                                  const double ymin,
                                  const double ymax
                                );
  void rvapi_set_plot_yrange1   ( const char * path,   // "gwdId/pltId"
                                  const double ymin,
                                  const double ymax
                                );

  void rvapi_set_plot_xslider   ( const char * pltId,
                                  const char * gwdId,
                                  const double smin,
                                  const double smax
                                );
  void rvapi_set_plot_xslider1  ( const char * path,   // "gwdId/pltId"
                                  const double smin,
                                  const double smax
                                );

  void rvapi_set_plot_yslider   ( const char * pltId,
                                  const char * gwdId,
                                  const double smin,
                                  const double smax
                                );
  void rvapi_set_plot_yslider1  ( const char * path,   // "gwdId/pltId"
                                  const double smin,
                                  const double smax
                                );

  void rvapi_set_plot_legend    ( const char * pltId,
                                  const char * gwdId,
                                  const char * legendLocation,
                                  const char * legendPlacement
                                );
  void rvapi_set_plot_legend1   ( const char * path,   // "gwdId/pltId"
                                  const char * legendLocation,
                                  const char * legendPlacement
                                );

  void rvapi_reset_plot_xticks  ( const char * pltId,
                                  const char * gwdId
                                );
  void rvapi_reset_plot_xticks1 ( const char * path   // "gwdId/pltId"
                                );

  void rvapi_reset_plot_yticks  ( const char * pltId,
                                  const char * gwdId
                                );
  void rvapi_reset_plot_yticks1 ( const char * path   // "gwdId/pltId"
                                );

  void rvapi_add_plot_xtick     ( const char * pltId,
                                  const char * gwdId,
                                  const double value,
                                  const char * label
                                );
  void rvapi_add_plot_xtick1    ( const char * path,   // "gwdId/pltId"
                                  const double value,
                                  const char * label
                                );

  void rvapi_add_plot_ytick     ( const char * pltId,
                                  const char * gwdId,
                                  const double value,
                                  const char * label
                                );
  void rvapi_add_plot_ytick1    ( const char * path,   // "gwdId/pltId"
                                  const double value,
                                  const char * label
                                );


  /*  =================
        RADAR WIDGET
      =================  */


  void rvapi_add_radar     ( const char * radarId,
                             const char * radarTitle,
                             const char * holderId,
                             const int    row,
                             const int    col,
                             const int    rowSpan,
                             const int    colSpan,
                             const int    foldState
                           );

  void rvapi_add_radar1    ( const char * path,  // "holderId/radarId"
                             const char * radarTitle,
                             const int    row,
                             const int    col,
                             const int    rowSpan,
                             const int    colSpan,
                             const int    foldState
                           );

  void rvapi_append_radar  ( const char * radarId,
                             const char * radarTitle,
                             const char * holderId,
                             const int    foldState
                           );

  void rvapi_append_radar1 ( const char * path,  // "holderId/radarId"
                             const char * radarTitle,
                             const int    foldState
                           );

  void rvapi_add_radar_property ( const char * radarId,
                                  const char * name,
                                  const double  value
                                );


  /*  =================
        TREE WIDGETS
      =================  */

  void rvapi_add_tree_widget   ( const char * treeId,
                                 const char * title,
                                 const char * holderId,
                                 const int    row,
                                 const int    col,
                                 const int    rowSpan,
                                 const int    colSpan
                               );

  void rvapi_add_tree_widget1  ( const char * path, // "holderId/treeId"
                                 const char * title,
                                 const int    row,
                                 const int    col,
                                 const int    rowSpan,
                                 const int    colSpan
                               );

  void rvapi_append_tree_widget ( const char * treeId,
                                  const char * title,
                                  const char * holderId
                                );

  void rvapi_append_tree_widget1 ( const char * path, // "holderId/treeId"
                                   const char * title
                                 );

  // add widgets to the tree as usual, using treeId as holderId,
  // then shape the tree using  rvapi_set_tree_node()

  void rvapi_set_tree_node ( const char * treeId,
                             const char * nodeId,    // node to set
                             const char * title,     // title to set
                             const char * openState, // node open state
                             const char * parentId   // parent tree node
                           );


  /*  ================
        PROGRESS BAR
      ================  */

  void rvapi_toolbar_progress ( const int key,  // 0: hide; 1: show;
                                                // 2: set range;
                                                // 3: set value
                                const int value // either range or value
                              );

  void rvapi_add_progress_bar ( const char * pbarId,
                                const char * holderId,
                                const int    row,
                                const int    col,
                                const int    rowSpan,
                                const int    colSpan
                              );

  void rvapi_set_progress_value ( const char * pbarId,
                                  const int key,  // 0: hide; 1: show;
                                                  // 2: set range;
                                                  // 3: set value
                                  const int value // either range or value
                                );


  /*  ===================
        FORMS and INPUTS
      ===================  */

  void rvapi_add_form ( const char * formId,
                        const char * action,
                        const char * method,
                        const char * holderId,
                        const int    hrow,
                        const int    hcol,
                        const int    hrowSpan,
                        const int    hcolSpan
                      );

  void rvapi_add_file_upload   ( const char * inpId,
                                 const char * name,
                                 const char * value,
                                 const int    length,
                                 const bool   required,
                                 const char * formId,
                                 const int    hrow,
                                 const int    hcol,
                                 const int    hrowSpan,
                                 const int    hcolSpan
                               );
  void rvapi_add_line_edit     ( const char * inpId,
                                 const char * name,
                                 const char * text,
                                 const int    length,
                                 const char * formId,
                                 const int    hrow,
                                 const int    hcol,
                                 const int    hrowSpan,
                                 const int    hcolSpan
                               );
  void rvapi_add_hidden_text   ( const char * inpId,
                                 const char * name,
                                 const char * text,
                                 const char * formId,
                                 const int    hrow,
                                 const int    hcol,
                                 const int    hrowSpan,
                                 const int    hcolSpan
                               );
  void rvapi_add_submit_button ( const char * inpId,
                                 const char * title,
                                 const char * formAction,
                                 const char * formId,
                                 const int    hrow,
                                 const int    hcol,
                                 const int    hrowSpan,
                                 const int    hcolSpan
                               );
  void rvapi_add_button        ( const char * inpId,
                                 const char * title,
                                 const char * command,
                                 const char * data,
                                 const bool   rvOnly,
                                 const char * holderId,
                                 const int    hrow,
                                 const int    hcol,
                                 const int    hrowSpan,
                                 const int    hcolSpan
                               );
  void rvapi_add_checkbox      ( const char * inpId,
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
                               );
  void rvapi_add_combobox      ( const char * cbxId,
                                 const char * name,
                                 const char * onChange,
                                 const int    size,
                                 const char * holderId,
                                 const int    hrow,
                                 const int    hcol,
                                 const int    hrowSpan,
                                 const int    hcolSpan
                               );
  void rvapi_add_combobox_option ( const char * cbxId,
                                   const char * label,
                                   const char * value,
                                   const bool   selected
                                 );
  void rvapi_add_radio_button  ( const char * inpId,
                                 const char * title,
                                 const char * name,
                                 const char * value,
                                 const bool   checked,
                                 const char * holderId,
                                 const int    hrow,
                                 const int    hcol,
                                 const int    hrowSpan,
                                 const int    hcolSpan
                               );

  void rvapi_set_action_on_click  ( const char * inpId,
                                    const char * actId,
                                    const char * action,
                                    const char * onClick );

  void rvapi_set_action_on_change ( const char * inpId,
                                    const char * actId,
                                    const char * action,
                                    const char * onChange );

  void rvapi_disable_form         ( const char * formId,
                                    const bool   disable
                                  );

  void rvapi_disable_input        ( const char * inpId,
                                    const bool   disable
                                  );


  /*  ==========
        OTHER
      ==========  */

  /// \brief Removes widget with given Id from RVAPI document and output page.
  /// \param widgetId   Id of widget to be removed. Note that this also
  ///                   removes all children widgets. \b Important:
  ///                   use rvapi_flush() immediately before and
  ///                   immediately after removing a widget. If a set
  ///                   of widgets needs to be removed, use rvapi_flush()
  ///                   immediately before and after the set of
  ///                   consequitive rvapi_remove_widget() calls.
  void rvapi_remove_widget        ( const char * widgetId );


  /*  ==========
        OUTPUT
      ==========  */

  void rvapi_flush          ();
  void rvapi_set_time_quant ( const int quant );
  void rvapi_keep_polling   ( const bool on   );
  void rvapi_reset_task     ();


  /*  ==========
        UTILS
      ==========  */

  
  /// \brief RVAPI version number.
  /// \param version  buffer to accept a null-terminated version string
  ///                 in form "major.minor.micro", for example, "1.0.2"
  void rvapi_version          ( char * version );
  
  /// \brief Sets maximum size of task file.
  /// \param max_size  maximum size of task file allowed in kilobytes
  ///                  (50KB by default). Excessively large task files
  ///                  will jam the output page, and RVAPI will reset the
  ///                  task file and the whole page when this happens.
  ///                  On user side, this will look like a reload of
  ///                  the whole page. The default size is sufficiently
  ///                  large for most of reasonable-size reports, so
  ///                  that such reloads should never happen for them.
  void rvapi_set_max_task_size ( const int max_size );

  void rvapi_make_hard_spaces  ( char * dest, const char * source );
  bool rvapi_exists            ( const char * elementId           );
  void rvapi_url_encode        ( char * dest, const char * source );
  int  rvapi_error             ();

}

#endif // RVAPI_NODE_H
