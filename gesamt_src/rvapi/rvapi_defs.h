//
//  =================================================================
//
//    02.08.17   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_defs  <interface>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Content :  RVAPI internal definitions
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2013-2017
//
//  =================================================================
//


#ifndef RVAPI_DEFS_H
#define RVAPI_DEFS_H


namespace rvapi  {

  enum  RVAPI_VERSION  {
    MAJOR_VERSION = 1,  //!< RVAPI major version
    MINOR_VERSION = 0,  //!< RVAPI minor version
    MICRO_VERSION = 11  //!< RVAPI micro version
  };
  
  #define RVAPI_VERSION_Date  "02-08-2017"

  enum NODE_TYPE {
    NTYPE_None,
    NTYPE_Button,
    NTYPE_Content,
    NTYPE_Checkbox,
    NTYPE_Combobox,
    NTYPE_Data,
    NTYPE_DocHeader,
    NTYPE_Document,
    NTYPE_Fieldset,
    NTYPE_FileUpload,
    NTYPE_Form,
    NTYPE_Graph,
    NTYPE_Grid,
    NTYPE_Hidden,
    NTYPE_Input,
    NTYPE_LineEdit,
    NTYPE_LogGraph,
    NTYPE_Panel,
    NTYPE_ProgressBar,
    NTYPE_Radar,
    NTYPE_RadioButton,
    NTYPE_Section,
    NTYPE_SubmitButton,
    NTYPE_Tab,
    NTYPE_Table,
    NTYPE_Text,
    NTYPE_TreeWidget
  };

  enum OUTPUT_MODE  {
    MODE_Bin   = 0x00,
    MODE_Html  = 0x01,
    MODE_Xmli2 = 0x02
  };

  enum LAYOUT_MODE  {
    LAYOUT_Header     = 0x01,
    LAYOUT_Toolbar    = 0x02,
    LAYOUT_Tabs       = 0x04,
    LAYOUT_Full       = 0x07,
    LAYOUT_SlimHeader = 0x08
  };

}

#define  inline_size_threshold   600

#define  toolbar_progressbar_id  "_progressBar"

#define  key_del                    ":::"
#define  key_ter                    ";;;\n"
#define  par_del                    "*"
#define  item_del                   ">>>"
#define  add_button_key             "ADD_BUTTON"
#define  add_button_grid_key        "ADD_BUTTON_GRID"
#define  add_checkbox_grid_key      "ADD_CHECKBOX_GRID"
#define  add_combobox_grid_key      "ADD_COMBOBOX_GRID"
#define  add_dropdown_key           "ADD_DROPDOWN"
#define  add_line_edit_key          "ADD_LINE_EDIT"
#define  add_file_upload_key        "ADD_FILE_UPLOAD"
#define  add_form_key               "ADD_FORM"
#define  add_submit_button_key      "ADD_SUBMIT_BUTTON"
#define  add_graph_key              "ADD_GRAPH"
#define  add_log_graph_key          "ADD_LOG_GRAPH"
#define  add_grid_key               "ADD_GRID"
#define  add_grid_compact_key       "ADD_GRID_COMPACT"
#define  add_hidden_text_key        "ADD_HIDDEN_TEXT"
#define  add_html_grid_key          "ADD_HTML_GRID"
#define  add_panel_key              "ADD_PANEL"
#define  add_fieldset_key           "ADD_FIELDSET"
#define  add_progress_bar_key       "ADD_PROGRESS_BAR"
#define  add_radar_key              "ADD_RADAR"
#define  add_radio_button_key       "ADD_RADIO_BUTTON"
#define  add_section_key            "ADD_SECTION"
#define  add_tab_key                "ADD_TAB"
#define  add_textbox_grid_key       "ADD_TEXTBOX_GRID"
#define  add_tree_widget_key        "ADD_TREE_WIDGET"
#define  disable_form_key           "DISABLE_FORM"
#define  disable_input_key          "DISABLE_ELEMENT"
#define  load_content_grid_key      "LOAD_CONTENT_GRID"
#define  load_content_key           "LOAD_CONTENT"
#define  load_content_tablesort_key "LOAD_CONTENT_TABLESORT"
#define  nest_grid_key              "NEST_GRID"
#define  nest_grid_compact_key      "NEST_GRID_COMPACT"
#define  insert_tab_key             "INSERT_TAB"
#define  remove_tab_key             "REMOVE_TAB"
#define  remove_widget_key          "REMOVE_WIDGET"
#define  set_cell_stretch_key       "SET_CELL_STRETCH"
#define  set_header_key             "SET_HEADER"
#define  set_html_key               "SET_HTML"
#define  set_html_grid_key          "SET_HTML_GRID"
#define  set_html_tablesort_key     "SET_HTML_TABLESORT"
#define  set_progress_bar_key       "SET_PROGRESS_BAR"
#define  set_section_state_key      "SET_SECTION_STATE"
#define  set_text_grid_key          "SET_TEXT_GRID"
#define  task_stamp_key             "TASK_STAMP"
#define  set_time_quant_key         "SET_TIME_QUANT"
#define  stop_poll_key              "STOP_POLL"

#define  FTYPE_XYZ                  "xyz"
#define  FTYPE_XYZIN                "XYZIN"
#define  FTYPE_XYZOUT               "XYZOUT"
#define  FTYPE_LIB                  "LIB"
#define  FTYPE_XYZ_MAP              "xyz:map"
#define  FTYPE_SUMMARY              "summary"
#define  FTYPE_HKL_HKL              "hkl:hkl"
#define  FTYPE_HKL_MAP              "hkl:map"
#define  FTYPE_HKL_CCP4_MAP         "hkl:ccp4_map"
#define  FTYPE_HKL_CCP4_DMAP        "hkl:ccp4_dmap"
#define  FTYPE_TEXT                 "text"
#define  FTYPE_HTML                 "html"
#define  FTYPE_OTHER                "other"

#define  COMMAND_Export             "{export}"
#define  COMMAND_Coot               "{coot}"
#define  COMMAND_CCP4MG             "{ccp4mg}"
#define  COMMAND_UglyMol            "{uglymol}"
#define  COMMAND_ViewHKL            "{viewhkl}"
#define  COMMAND_Display            "{display}"
#define  COMMAND_Browser            "{browser}"

#endif // RVAPI_DEFS_H
