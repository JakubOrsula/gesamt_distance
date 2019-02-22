//
//  =================================================================
//
//    04.12.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_table  <interface>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Classes :  rvapi::Table - API Table class
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2013-2014
//
//  =================================================================
//

#ifndef RVAPI_TABLE_H
#define RVAPI_TABLE_H

#include "rvapi_node.h"

namespace rvapi  {

  class TableData;

  class Table : public Node  {

    public:
      Table ( const char * tblId,
              const char * tblTitle,
              int  hrow     = -1,
              int  hcol     = 0,
              int  hrowSpan = 1,
              int  hcolSpan = 1
            );
      Table ();
      ~Table();

      virtual NODE_TYPE type() { return NTYPE_Table; }

      inline void setFoldState ( int state )  { foldState = state; }

      void setStyle     ( const char * css, const char * td_style );
      void setTableType ( bool sorting, bool pages );

      void putHorzHeader   ( const char * header,
                             const char * tooltip,
                             int          tcol
                           );
      void shapeHorzHeader ( int          tcol,
                             const char * cell_style,
                             const char * cell_css,
                             int          rowSpan,
                             int          colSpan
                           );
      void putVertHeader   ( const char * header,
                             const char * tooltip,
                             int          trow
                           );
      void shapeVertHeader ( int          trow,
                             const char * cell_style,
                             const char * cell_css,
                             int          rowSpan,
                             int          colSpan
                           );
      void putValue  ( const char * v, int trow, int tcol );
      void putValue  ( int   v, int trow, int tcol );
      void putValue  ( double v, const char *fmt, int trow, int tcol );
      void shapeCell ( int          trow,
                       int          tcol,
                       const char * tooltip,
                       const char * cell_style,
                       const char * cell_css,
                       int          rowSpan,
                       int          colSpan
                     );

      void resetData ( int trow );


      virtual void write ( std::ofstream & s );
      virtual void read  ( std::ifstream & s );

      virtual void flush_html ( std::string & outDir,
                                std::string & task );

    protected:
      std::string     legend;  //!< to appear at the table
      TableData   *tableData;  //!< table data
      int          foldState;  //!< -1,0,+1 for initialy folded,
                               /// not foldable and initially unfolded
      bool          sortable;  //!< true to use the tablesorter plugin
      bool          paging;    //!< true to use the tablesorter pager plugin

      void initTable();
      void freeTable();

      virtual void make_xmli2_content  ( std::string & tag,
                                         std::string & content );

  };

}

#endif // RVAPI_TABLE_H
