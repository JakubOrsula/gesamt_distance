//
//  =================================================================
//
//    15.07.17   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_tasks  <implementation>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Funstions: rvapi::Tasks - API task functions
//       ~~~~~~~~~~
//
//  (C) E. Krissinel 2013-2017
//
//  =================================================================
//

#include <stdio.h>
#include <time.h>

#include "rvapi_tasks.h"
#include "rvapi_defs.h"


std::string b2s ( bool b )  {
  if (b)  return "true";
  return "false";
}


std::string i2s ( int i )  {
  if (i>0)  return "true";
  if (i<0)  return "false";
  return "none";
}

std::string to_string ( int i )  {
char N[100];
  sprintf ( N,"%i",i );
  return std::string(N);
}

void rvapi::add_button ( std::string & task,
                         std::string   btnId,
                         std::string   title,
                         std::string   command,
                         std::string   data,
                         bool          rvOnly,
                         std::string   holderId
                       )  {
  task.append ( add_button_key key_del +
                btnId         + key_del +
                title         + key_del +
                command       + key_del +
                data          + key_del +
                b2s(rvOnly)   + key_del +
                holderId      + key_ter );
}

void rvapi::add_submit_button ( std::string & task,
                                std::string   inpId,
                                std::string   title,
                                std::string   formAction,
                                std::string   holderId,
                                std::string   gridPosition
                              )  {
  task.append ( add_submit_button_key key_del +
                                inpId + key_del +
                                title + key_del );
  if (!formAction.empty())  task.append ( formAction );
                      else  task.append ( "*" );
  task.append (                         key_del +
                             holderId + key_del +
                         gridPosition + key_ter );
}


void rvapi::add_button_grid ( std::string & task,
                              std::string   btnId,
                              std::string   title,
                              std::string   command,
                              std::string   data,
                              bool          rvOnly,
                              std::string   holderId,
                              std::string   gridPosition
                            )  {
  task.append ( add_button_grid_key key_del +
                      btnId        + key_del +
                      title        + key_del +
                      command      + key_del +
                      data         + key_del +
                      b2s(rvOnly)  + key_del +
                      holderId     + key_del +
                      gridPosition + key_ter );
}

void rvapi::add_checkbox_grid ( std::string & task,
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
                              )  {
  task.append ( add_checkbox_grid_key key_del +
                        cbxId        + key_del +
                        title        + key_del +
                        name         + key_del +
                        value        + key_del +
                        command      + key_del +
                        data         + key_del +
                        b2s(checked) + key_del +
                        onChange     + key_del +
                        holderId     + key_del +
                        gridPosition + key_ter );
}

void rvapi::add_combobox_grid ( std::string & task,
                                std::string   cbxId,
                                std::string   name,
                                std::string   options,
                                std::string   onChange,
                                const int     size,
                                std::string   holderId,
                                std::string   gridPosition
                              )  {
  task.append ( add_combobox_grid_key key_del +
                               cbxId + key_del +
                                name + key_del +
                             options + key_del +
                            onChange + key_del +
                     to_string(size) + key_del +
                            holderId + key_del +
                        gridPosition + key_ter );
}

void rvapi::add_dropdown ( std::string & task,
                           std::string   ddnId,
                           std::string   legend,
                           std::string   holderId,
                           int           foldState,
                           std::string   gridPosition
                         )  {
std::string fstate;

  if (legend.empty())
    legend = " ";

  switch (foldState)  {
    case  -2: fstate = "_folded-wide";  break;
    case  -1: fstate = "_folded";       break;
    default:
    case   0: fstate = "none";          break;
    case 100: fstate = "none-wide";     break;
    case   1: fstate = "unfolded";      break;
    case   2: fstate = "unfolded-wide"; break;
  }

  task.append ( add_dropdown_key key_del +
                ddnId           + key_del +
                legend          + key_del +
                holderId        + key_del +
                gridPosition    + key_del +
                fstate          + key_ter );
}

void rvapi::add_file_upload ( std::string & task,
                              std::string   inpId,
                              std::string   name,
                              std::string   value,
                              const int     size,
                              const bool    required,
                              std::string   onChange,
                              std::string   holderId,
                              std::string   gridPosition
                            )  {
  task.append ( add_file_upload_key key_del +
                             inpId + key_del +
                              name + key_del +
                       value + " " + key_del +
                   to_string(size) + key_del +
                     b2s(required) + key_del +
                          onChange + key_del +
                          holderId + key_del +
                      gridPosition + key_ter );
}

void rvapi::add_form ( std::string & task,
                       std::string   formId,
                       std::string   action,
                       std::string   method,
                       std::string   holderId,
                       std::string   gridPosition
                     )  {
  task.append ( add_form_key key_del +
                     formId + key_del +
                     action + key_del +
                     method + key_del +
                   holderId + key_del +
               gridPosition + key_ter );
}

void rvapi::add_graph ( std::string & task,
                        std::string   gwdId,
                        std::string   holderId,
                        std::string   plotData,
                        std::string   gridPosition
                      )  {
  task.append ( add_graph_key key_del +
                       gwdId + key_del +
                    holderId + key_del +
                    plotData + key_del +
                gridPosition + key_ter );
}

void rvapi::add_grid ( std::string & task,
                       std::string   holderId
                     )  {
  task.append ( add_grid_key key_del +
                holderId + key_ter );
}

void rvapi::add_grid_compact ( std::string & task,
                               std::string   holderId
                             )  {
  task.append ( add_grid_compact_key key_del +
                           holderId + key_ter );
}

void rvapi::add_line_edit ( std::string & task,
                            std::string   inpId,
                            std::string   name,
                            std::string   text,
                            const int     size,
                            std::string   holderId,
                            std::string   gridPosition
                          )  {
  task.append ( add_line_edit_key key_del +
                           inpId + key_del +
                            name + key_del +
                            text + key_del +
                 to_string(size) + key_del +
                        holderId + key_del +
                    gridPosition + key_ter );
}

void rvapi::add_hidden_text ( std::string & task,
                              std::string   inpId,
                              std::string   name,
                              std::string   text,
                              std::string   holderId,
                              std::string   gridPosition
                            )  {
  task.append ( add_hidden_text_key key_del +
                             inpId + key_del +
                              name + key_del +
                              text + key_del +
                          holderId + key_del +
                      gridPosition + key_ter );
}


void rvapi::add_html_grid ( std::string & task,
                            std::string   html,
                            std::string   holderId,
                            std::string   gridPosition
                          )  {
  task.append ( add_html_grid_key key_del +
                            html + key_del +
                        holderId + key_del +
                    gridPosition + key_ter );
}


void rvapi::add_log_graph ( std::string & task,
                            std::string   gwdId,
                            std::string   holderId,
                            std::string   treeData,
                            std::string   gridPosition
                          )  {
  task.append ( add_log_graph_key key_del +
                           gwdId + key_del +
                        holderId + key_del +
                        treeData + key_del +
                    gridPosition + key_ter );
}

void rvapi::add_panel ( std::string & task,
                        std::string   panelId,
                        std::string   holderId,
                        std::string   gridPosition
                      )  {
  task.append ( add_panel_key key_del +
                     panelId + key_del +
                    holderId + key_del +
                gridPosition + key_ter );
}

void rvapi::add_fieldset ( std::string & task,
                           std::string   fsetId,
                           std::string   title,
                           std::string   holderId,
                           std::string   gridPosition
                         )  {
  task.append ( add_fieldset_key key_del +
                      fsetId + key_del +
                       title + key_del +
                    holderId + key_del +
                gridPosition + key_ter );
}

void rvapi::add_progress_bar ( std::string & task,
                               std::string   pbarId,
                               int           range,
                               int           width,
                               std::string   holderId,
                               std::string   gridPosition
                             )  {
  task.append ( add_progress_bar_key key_del +
                             pbarId + key_del +
                   to_string(range) + key_del +
                   to_string(width) + key_del +
                           holderId + key_del +
                       gridPosition + key_ter );
}

void rvapi::add_radar ( std::string & task,
                        std::string   data,
                        std::string   options,
                        std::string   holderId
                      )  {
  task.append ( add_radar_key key_del +
                        data + key_del +
                     options + key_del +
                    holderId + key_ter );
}

void rvapi::add_radio_button_grid ( std::string & task,
                                    std::string   inpId,
                                    std::string   title,
                                    std::string   name,
                                    std::string   value,
                                    const bool    checked,
                                    std::string   onChange,
                                    std::string   holderId,
                                    std::string   gridPosition
                                  )  {
  task.append ( add_radio_button_key key_del +
                              inpId + key_del +
                              title + key_del +
                               name + key_del +
                              value + key_del +
                       b2s(checked) + key_del +
                           onChange + key_del +
                           holderId + key_del +
                       gridPosition + key_ter );
}


void rvapi::add_section ( std::string & task,
                          std::string   secId,
                          std::string   name,
                          std::string   holderId,
                          bool          createOpen,
                          std::string   gridPosition
                        )  {
  task.append ( add_section_key key_del +
                         secId + key_del +
                          name + key_del +
                      holderId + key_del +
                  gridPosition + key_del +
               b2s(createOpen) + key_ter );
}

void rvapi::add_textbox_grid ( std::string & task,
                               std::string   tbxId,
                               std::string   text,
                               std::string   holderId,
                               std::string   gridPosition
                             )  {
  task.append ( add_textbox_grid_key key_del +
                              tbxId + key_del +
                               text + key_del +
                           holderId + key_del +
                       gridPosition + key_ter );
}

void rvapi::add_tree_widget ( std::string & task,
                              std::string   treeId,
                              std::string   title,
                              std::string   holderId,
                              std::string   treeData,
                              std::string   gridPosition
                            )  {
  task.append ( add_tree_widget_key key_del +
                             treeId + key_del +
                              title + key_del +
                           holderId + key_del +
                           treeData + key_del +
                       gridPosition + key_ter );
}


void rvapi::nest_grid ( std::string & task,
                        std::string   gridId,
                        std::string   holderId,
                        std::string   gridPosition
                      )  {
   task.append ( nest_grid_key key_del +
                       gridId + key_del +
                     holderId + key_del +
                 gridPosition + key_ter );
}

void rvapi::nest_grid_compact ( std::string & task,
                                std::string   gridId,
                                std::string   holderId,
                                std::string   gridPosition
                              )  {
  task.append ( nest_grid_compact_key key_del +
                              gridId + key_del +
                            holderId + key_del +
                        gridPosition + key_ter );
}

void rvapi::put_task_stamp ( std::string & task,
                             bool          reloadPage,
                             bool          resetCounter )  {
  task.append ( task_stamp_key key_del +
        to_string(time(NULL)) + key_del +
           to_string(clock()) + key_del );
  if (reloadPage)
        task.append ( "RELOAD" key_ter );
  else if (resetCounter)
        task.append ( "RESET" key_ter );
  else  task.append ( "PASS" key_ter  );
}

void rvapi::remove_tab ( std::string & task,
                         std::string   tabId
                       )  {
  task.append ( remove_tab_key key_del + tabId + key_ter );                       
}

void rvapi::set_cell_stretch ( std::string & task,
                               std::string   gridId,
                               int           width,
                               int           height,
                               int           row,
                               int           col
                             )  {
std::string w,h;
  if (width>0)   w = to_string(width)   + "%";
          else   w = to_string(-width)  + "px";
  if (height>0)  h = to_string(height)  + "%";
           else  h = to_string(-height) + "px";
  task.append ( set_cell_stretch_key key_del +
                             gridId + key_del +
                                  w + key_del +
                                  h + key_del +
                     to_string(row) + key_del +
                     to_string(col) + key_ter );
}

void rvapi::set_header ( std::string & task,
                         std::string   header )  {
  task.append ( set_header_key key_del +
                       header + key_ter );
}

void rvapi::set_html ( std::string & task,
                       std::string   html,
                       std::string   holderId )  {
  task.append ( set_html_key key_del  +
                    holderId + key_del +
                        html + key_ter );
}

void rvapi::set_html_grid ( std::string & task,
                            std::string   html,
                            std::string   holderId,
                            std::string   gridPosition
                          )  {
  task.append ( set_html_grid_key key_del +
                            html + key_del +
                        holderId + key_del +
                    gridPosition + key_ter );
}

void rvapi::set_html_tablesort ( std::string & task,
                                 std::string   html,
                                 bool          paging,
                                 std::string   holderId )  {
  task.append ( set_html_tablesort_key key_del  +
                          b2s(paging) + key_del +
                             holderId + key_del +
                                 html + key_ter );
}



void rvapi::set_progress_bar ( std::string & task,
                               std::string   pbarId,
                               std::string   key,
                               int           value
                             )  {
  task.append ( set_progress_bar_key key_del +
                             pbarId + key_del +
                                key + key_del +
                   to_string(value) + key_ter );
}


void rvapi::set_section_state ( std::string & task,
                                std::string   secId,
                                bool          setOpen
                              )  {
  task.append ( set_section_state_key key_del +
                               secId + key_del +
                        b2s(setOpen) + key_ter );
}


void rvapi::load_content ( std::string & task,
                           std::string   uri,
                           bool          watch,
                           std::string   subtaskUri,
                           std::string   holderId )  {
  task.append ( load_content_key key_del  +
                       holderId + key_del +
                            uri + key_del +
                     b2s(watch) + key_del +
                     subtaskUri + key_ter );
}

void rvapi::load_content_grid ( std::string & task,
                                std::string   uri,
                                bool          watch,
                                std::string   subtaskUri,
                                std::string   holderId,
                                std::string   gridPosition
                              )  {
  task.append ( load_content_grid_key key_del +
                                 uri + key_del +
                          b2s(watch) + key_del +
                          subtaskUri + key_del +
                            holderId + key_del +
                        gridPosition + key_ter );
}

void rvapi::load_content_tablesort ( std::string & task,
                                     std::string   uri,
                                     bool          paging,
                                     std::string   tableId,
                                     std::string   holderId )  {
  task.append ( load_content_tablesort_key key_del  +
                                      uri + key_del +
                              b2s(paging) + key_del +
                                  tableId + key_del +
                                 holderId + key_ter );
}
