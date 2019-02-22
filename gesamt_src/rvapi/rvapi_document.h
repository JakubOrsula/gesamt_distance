//
//  =================================================================
//
//    15.0y.17   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_document  <interface>
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


#ifndef RVAPI_DOCUMENT_H
#define RVAPI_DOCUMENT_H

#include "rvapi_node.h"

namespace rvapi  {

  class Tab;
  class Section;
  class Fieldset;
  class Grid;
  class Table;
  class Data;
  class LogGraph;
  class Graph;
  class Radar;
  class TreeWidget;
  class Input;

  class Document : public Node  {

    public:

      Document ();
      ~Document();

      virtual NODE_TYPE type() { return NTYPE_Document; }

      // ====================  INITIALISATION  ======================

      /// \brief Configurator.
      /// \param[in] docId   document ID.
      ///
      /// If docId is given, then it will be used as a dash-prepended
      /// namespace for all children ids, e.g. "docId-tabId". When
      /// children are retrieved, the same rule will apply. However,
      /// by unsetting docId, full namespaced Ids will need to be
      /// used for retrieval. This makes it possible to use the same
      /// Document for collecting data from several runs of the same
      /// application, or even several different applications, without
      /// conflict on their data ids. Consider that docId may be passed
      /// through program's command-line arguments.
      void configure  ( const char * docId,
                        const char * outDir,
                        const char * windowTitle,
                        OUTPUT_MODE  outMode = MODE_Html,
                        LAYOUT_MODE  layMode = LAYOUT_Full,
                        const char * jsUri   = NULL,
                        const char * helpFN  = NULL, // no help button
                        const char * htmlFN  = "index.html",
                        const char * taskFN  = "task.tsk",
                        const char * xmli2FN = "i2.xml",
                        const bool   initClean = true
                      );
      
      /// \brief Putting optional metadata, which can be passed between
      /// processes sharing the same document
      void putMeta ( const char * metastring );

      /// \brief Getting optional metadata, which can be passed between
      /// processes sharing the same document
      const char * getMeta();

      // ==================  FORMATION OF PAGE  =====================

      void addHeader  ( const char * htmlString );

      void addTab     ( const char * tabId,
                        const char * tabName,
                        bool         open
                      );

      void insertTab  ( const char * tabId,
                        const char * tabName,
                        const char * beforeTabId,
                        bool         open
                      );

      void removeTab  ( const char * tabId );

      Tab * findTab   ( const char *tabId );

      void addSection ( const char * secId,
                        const char * secTitle,
                        const char * holderId,
                        int          hrow     = -1,
                        int          hcol     = 0,
                        int          hrowSpan = 1,
                        int          hcolSpan = 1,
                        bool         open     = true
                      );

      void setSectionState ( const char * secId,
                             bool         open
                           );
      Section * findSection ( const char *secId );

      void addFieldset ( const char * gridId,
                         const char * title,
                         const char * holderId,
                         int          hrow     = -1,
                         int          hcol     = 0,
                         int          hrowSpan = 1,
                         int          hcolSpan = 1
                       );
      Fieldset * findFieldset ( const char *gridId );

      void addGrid    ( const char * gridId,
                        bool         filling,
                        const char * holderId,
                        int          hrow     = -1,
                        int          hcol     = 0,
                        int          hrowSpan = 1,
                        int          hcolSpan = 1
                      );
      Grid * findGrid ( const char *gridId );

      void addPanel   ( const char * panelId,
                        const char * holderId,
                        int          hrow     = -1,
                        int          hcol     = 0,
                        int          hrowSpan = 1,
                        int          hcolSpan = 1
                      );

      void setCellStretch ( const char * gridWidgetId,
                            const int    width,
                            const int    height,
                            const int    row,
                            const int    col
                          );


      // ====================  TEXT WIDGET  =========================

      void setText    ( const char * labelId,
                        const char * textString,
                        const char * holderId,
                        int          hrow     = -1,
                        int          hcol     = 0,
                        int          hrowSpan = 1,
                        int          hcolSpan = 1
                      );

      void addText    ( const char * labelId,
                        const char * textString,
                        const char * holderId,
                        int          hrow     = -1,
                        int          hcol     = 0,
                        int          hrowSpan = 1,
                        int          hcolSpan = 1
                      );
                      
      void resetText  ( const char * labelId,
                        const char * newText );

      // ===================  CONTENT WIDGET  =======================

      void addContent ( const char * uri,
                        bool         watch,
                        const char * holderId,
                        int          hrow     = -1,
                        int          hcol     = 0,
                        int          hrowSpan = 1,
                        int          hcolSpan = 1
                      );

      // ========================  TABLES  ==========================

      void addTable   ( const char * tblId,
                        const char * tblTitle,
                        const char * holderId,
                        int          hrow      = -1,
                        int          hcol      = 0,
                        int          hrowSpan  = 1,
                        int          hcolSpan  = 1,
                        int          foldState = 1
                      );

      Table * findTable ( const char * tblId );


     // ===================  DATA BLOCKS (FILES)  ===================

      void addData      ( const char * datId,
                          const char * datTitle,
                          const char * uri,
                          const char * type,
                          const char * holderId,
                          int          hrow      = -1,
                          int          hcol      = 0,
                          int          hrowSpan  = 1,
                          int          hcolSpan  = 1,
                          int          foldState = 1 );

      Data * findData ( const char * datId );


      // =======================  GRAPHS  ===========================

      void addLogGraph ( const char * gphId,
                         const char * holderId,
                         int          hrow     = -1,
                         int          hcol     = 0,
                         int          hrowSpan = 1,
                         int          hcolSpan = 1 );

      LogGraph * findLogGraph ( const char * gphId );

      void addGraph ( const char * graphId,
                      const char * holderId,
                      int          hrow     = -1,
                      int          hcol     = 0,
                      int          hrowSpan = 1,
                      int          hcolSpan = 1 );

      Graph * findGraph ( const char * graphId );


      void addRadar  ( const char * radarId,
                       const char * radarTitle,
                       const char * holderId,
                       int          hrow      = -1,
                       int          hcol      = 0,
                       int          hrowSpan  = 1,
                       int          hcolSpan  = 1,
                       int          foldState = 0
                     );

      Radar * findRadar ( const char * radarId );


      // =======================  TREES  ============================

      void addTreeWidget ( const char * treeId,
                           const char * title,
                           const char * holderId,
                           const int    hrow     = -1,
                           const int    hcol     = 0,
                           const int    hrowSpan = 1,
                           const int    hcolSpan = 1 );

      TreeWidget * findTreeWidget ( const char * treeId );

      void setTreeNode ( const char * treeId,
                         const char * leafId,
                         const char * title,
                         const char * openState,
                         const char * parentId
                       );

      // ===================  PROGRESS BAR  =========================

      void setToolbarProgress ( int key, int value );

      void addProgressBar     ( const char * pbarId,
                                const char * holderId,
                                const int    hrow     = -1,
                                const int    hcol     = 0,
                                const int    hrowSpan = 1,
                                const int    hcolSpan = 1 );

      void setProgressValue ( const char * pbarId, int key, int value );

      // ==================  FORMS AND INPUTS  ======================

      void addForm ( const char * formId,
                     const char * action,
                     const char * method,
                     const char * holderId,
                     const int    hrow     = -1,
                     const int    hcol     = 0,
                     const int    hrowSpan = 1,
                     const int    hcolSpan = 1
                   );

      void addFileUpload   ( const char * inpId,
                             const char * name,
                             const char * value,
                             const int    length,
                             const bool   required,
                             const char * formId,
                             const int    hrow     = -1,
                             const int    hcol     = 0,
                             const int    hrowSpan = 1,
                             const int    hcolSpan = 1
                           );
      void addLineEdit     ( const char * inpId,
                             const char * name,
                             const char * text,
                             const int    length,
                             const char * formId,
                             const int    hrow     = -1,
                             const int    hcol     = 0,
                             const int    hrowSpan = 1,
                             const int    hcolSpan = 1
                           );
      void addHiddenText   ( const char * inpId,
                             const char * name,
                             const char * text,
                             const char * formId,
                             const int    hrow     = -1,
                             const int    hcol     = 0,
                             const int    hrowSpan = 1,
                             const int    hcolSpan = 1
                           );
      void addSubmitButton ( const char * inpId,
                             const char * title,
                             const char * formAction,
                             const char * formId,
                             const int    hrow     = -1,
                             const int    hcol     = 0,
                             const int    hrowSpan = 1,
                             const int    hcolSpan = 1
                           );
      void addButton       ( const char * inpId,
                             const char * title,
                             const char * command,
                             const char * data,
                             const bool   rvOnly,
                             const char * holderId,
                             const int    hrow     = -1,
                             const int    hcol     = 0,
                             const int    hrowSpan = 1,
                             const int    hcolSpan = 1
                           );
      void addCheckbox     ( const char * inpId,
                             const char * title,
                             const char * name,
                             const char * value,
                             const char * command,
                             const char * data,
                             const bool   checked,
                             const char * holderId,
                             const int    hrow     = -1,
                             const int    hcol     = 0,
                             const int    hrowSpan = 1,
                             const int    hcolSpan = 1
                           );
      void addCombobox     ( const char * cbxId,
                             const char * name,
                             const char * onChange,
                             const int    size,
                             const char * holderId,
                             const int    hrow     = -1,
                             const int    hcol     = 0,
                             const int    hrowSpan = 1,
                             const int    hcolSpan = 1
                           );
      void addComboboxOption ( const char * cbxId,
                               const char * label,
                               const char * value,
                               const bool   selected
                             );
      void addRadioButton  ( const char * inpId,
                             const char * title,
                             const char * name,
                             const char * value,
                             const bool   checked,
                             const char * holderId,
                             const int    hrow     = -1,
                             const int    hcol     = 0,
                             const int    hrowSpan = 1,
                             const int    hcolSpan = 1
                           );

      Input * findInput ( const char * inpId );

      void disableForm  ( const char * formId,
                          const bool   disable );

      void disableInput ( const char * inpId,
                          const bool   disable );

      // not for tabs and sections
      void removeWidget ( const char * widgetId );

      // =======================  OUTPUT  ===========================

      void setDocFileName ( const char *fname )  { docFName = fname; }

      // this pair of functions requires configuration of the document
      // and document file must be in the report directory; some
      // settings (particularly docuent Id) may be changed before
      // restore().
      void store          ( const char *fname );
      void restore        ( const char *fname );

      // this pair of functions does not require configuration of
      // the document, and document file may be placed anywhere;
      // all settings will be restored exactly and cannot be changed
      void store2         ( const char *fpath );
      void restore2       ( const char *fpath );

      void flush      ();
      void setPolling ( bool on );  // off by default
      void resetTask  ();           // resets task counter
      
      void setMaxTaskFileSize ( const long max_size );
      
      void setTimeQuant ( const int quant )  { timeQuant = quant; }

      // use only for external queries! (@-namespacing)
      const char * makeId ( std::string & nsId, const char * itemId );

    protected:

      OUTPUT_MODE  outputMode;    //!< output mode
      LAYOUT_MODE  layoutMode;    //!< layout mode
      std::string  outputDir;     //!< output directory (slash-terminated)
      std::string  documentId;    //!< unique document Id
      std::string  winTitle;      //!< window title
      std::string  docFName;      //!< document file name
      std::string  helpFName;     //!< default help (html) file name
      std::string  htmlFName;     //!< html file name
      std::string  taskFName;     //!< task file name
      std::string  xmli2FName;    //!< xmli2 file name
      std::string  tasks;         //!< special task list

      std::string  jsrviewUri;    //!< uri to js support directory
      std::string  docUri;        //!< uri to documentation
      std::vector<std::string>  uri_css;  //!< list of css modules
      std::vector<std::string>  uri_js;   //!< list of js modules
      std::string  jqPlotEx_js;   //!< jqPlot extra canvas URI for < IE9
 
      std::string  meta;          //!< optional metadata

      int          timeQuant;     //!< time quant for browser to update, ms
      int          stop_poll;     //!< flag to stop polling server
      int          reset_cnt;     //!< resets web page counter for task file

      void initDocument();
      void freeDocument();

      void makeJSLinks ();

      void   writeNode ( std::ofstream & s, Node * node );
      Node * readNode  ( std::ifstream & s );

      virtual void write ( std::ofstream & s );
      virtual void read  ( std::ifstream & s );

      void flush_file ( std::string & data,
                        const bool  timeStamp,
                        const bool  shrinkFile,
                        std::string fname);

    private:
      long  task_file_size, max_task_file_size;

  };

}

#endif // RVAPI_DOCUMENT_H
