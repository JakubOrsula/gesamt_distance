//
//  =================================================================
//
//    03.07.13   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_tasks  <interface>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Funstions: rvapi::Tasks - API task functions
//       ~~~~~~~~~~
//
//  (C) E. Krissinel 2013
//
//  =================================================================
//

#ifndef RVAPI_TASKS_H
#define RVAPI_TASKS_H

#include <string>

namespace rvapi  {

  void add_button        ( std::string & task,
                           std::string   btnId,
                           std::string   title,
                           std::string   command,
                           std::string   data,
                           bool          rvOnly,
                           std::string   holderId
                         );
  void add_submit_button ( std::string & task,
                           std::string   inpId,
                           std::string   title,
                           std::string   formAction,
                           std::string   holderId,
                           std::string   gridPosition
                         );
  void add_button_grid   ( std::string & task,
                           std::string   btnId,
                           std::string   title,
                           std::string   command,
                           std::string   data,
                           bool          rvOnly,
                           std::string   holderId,
                           std::string   gridPosition
                         );
  void add_checkbox_grid ( std::string & task,
                           std::string   cbxId,
                           std::string   title,
                           std::string   name,
                           std::string   value,
                           std::string   command,
                           std::string   data,
                           const bool    checked,
                           std::string   onChange,
                           std::string   holderId,
                           std::string   gridPosition
                         );
  void add_combobox_grid ( std::string & task,
                           std::string   cbxId,
                           std::string   name,
                           std::string   options,
                           std::string   onChange,
                           const int     size,
                           std::string   holderId,
                           std::string   gridPosition
                         );
  void add_dropdown      ( std::string & task,
                           std::string   ddnId,
                           std::string   legend,
                           std::string   holderId,
                           int           foldState,
                           std::string   gridPosition
                         );
  void add_file_upload   ( std::string & task,
                           std::string   inpId,
                           std::string   name,
                           std::string   value,
                           const int     size,
                           const bool    required,
                           std::string   onChange,
                           std::string   holderId,
                           std::string   gridPosition
                         );
  void add_form          ( std::string & task,
                           std::string   formId,
                           std::string   action,
                           std::string   method,
                           std::string   holderId,
                           std::string   gridPosition
                         );
  void add_graph         ( std::string & task,
                           std::string   gwdId,
                           std::string   holderId,
                           std::string   plotData,
                           std::string   gridPosition
                         );
  void add_grid          ( std::string & task,
                           std::string   holderId
                         );
  void add_grid_compact  ( std::string & task,
                           std::string   holderId
                         );
  void add_line_edit     ( std::string & task,
                           std::string   inpId,
                           std::string   name,
                           std::string   text,
                           const int     size,
                           std::string   holderId,
                           std::string   gridPosition
                         );
  void add_hidden_text   ( std::string & task,
                           std::string   inpId,
                           std::string   name,
                           std::string   text,
                           std::string   holderId,
                           std::string   gridPosition
                         );
  void add_html_grid     ( std::string & task,
                           std::string   html,
                           std::string   holderId,
                           std::string   gridPosition
                         );
  void add_log_graph     ( std::string & task,
                           std::string   gwdId,
                           std::string   holderId,
                           std::string   treeData,
                           std::string   gridPosition
                         );
  void add_panel         ( std::string & task,
                           std::string   panelId,
                           std::string   holderId,
                           std::string   gridPosition
                         );
  void add_fieldset      ( std::string & task,
                           std::string   panelId,
                           std::string   title,
                           std::string   holderId,
                           std::string   gridPosition
                         );
  void add_progress_bar  ( std::string & task,
                           std::string   pbarId,
                           int           range,
                           int           width,
                           std::string   holderId,
                           std::string   gridPosition
                         );
  void add_radar         ( std::string & task,
                           std::string   data,
                           std::string   options,
                           std::string   holderId
                         );
  void add_radio_button_grid ( std::string & task,
                               std::string   inpId,
                               std::string   title,
                               std::string   name,
                               std::string   value,
                               const bool    checked,
                               std::string   onChange,
                               std::string   holderId,
                               std::string   gridPosition
                             );

  void add_textbox_grid  ( std::string & task,
                           std::string   tbxId,
                           std::string   text,
                           std::string   holderId,
                           std::string   gridPosition
                         );
  void add_section       ( std::string & task,
                           std::string   secId,
                           std::string   name,
                           std::string   holderId,
                           bool          createOpen,
                           std::string   gridPosition
                         );
  void add_tree_widget   ( std::string & task,
                           std::string   treeId,
                           std::string   title,
                           std::string   holderId,
                           std::string   treeData,
                           std::string   gridPosition
                         );

  void nest_grid         ( std::string & task,
                           std::string   gridId,
                           std::string   holderId,
                           std::string   gridPosition
                         );
  void nest_grid_compact ( std::string & task,
                           std::string   gridId,
                           std::string   holderId,
                           std::string   gridPosition
                         );

  void put_task_stamp    ( std::string & task,
                           bool          reloadPage,
                           bool          resetCounter
                         );

  void remove_tab        ( std::string & task,
                           std::string   tabId
                         );

  void set_cell_stretch  ( std::string & task,
                           std::string   gridId,
                           int           width,
                           int           height,
                           int           row,
                           int           col
                         );

  void set_header        ( std::string & task,
                           std::string   header
                         );
  void set_html          ( std::string & task,
                           std::string   html,
                           std::string   holderId
                         );
  void set_html_grid     ( std::string & task,
                           std::string   html,
                           std::string   holderId,
                           std::string   gridPosition
                         );
  void set_html_tablesort( std::string & task,
                           std::string   html,
                           bool          paging,
                           std::string   holderId
                         );

  void set_progress_bar  ( std::string & task,
                           std::string   pbarId,
                           std::string   key,
                           int           value
                         );
  void set_section_state ( std::string & task,
                           std::string   secId,
                           bool          setOpen
                         );

  void load_content      ( std::string & task,
                           std::string   uri,
                           bool          watch,
                           std::string   subtaskUri,
                           std::string   holderId
                         );
  void load_content_grid ( std::string & task,
                           std::string   uri,
                           bool          watch,
                           std::string   subtaskUri,
                           std::string   holderId,
                           std::string   gridPosition
                         );
  void load_content_tablesort (
                           std::string & task,
                           std::string   uri,
                           bool          paging,
                           std::string   tableId,
                           std::string   holderId
                         );


}

#endif // RVAPI_TASKS_H
