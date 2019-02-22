//
//  =================================================================
//
//    04.12.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_tabledata  <interface>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Classes :  rvapi::TableData - table data class
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2013-2014
//
//  =================================================================
//

#ifndef RVAPI_TABLEDATA_H
#define RVAPI_TABLEDATA_H

#include <string>
#include <vector>
#include <fstream>

namespace rvapi  {

  class TableCell {

    public:
      std::string  text;
      std::string  tooltip;
      std::string  style;
      std::string  css;
      int          rowSpan;
      int          colSpan;

      TableCell ()  { rowSpan = 1;  colSpan = 1; }
      virtual ~TableCell()  {}

      std::string geti2XMLText();

      virtual void write ( std::ofstream & s );
      virtual void read  ( std::ifstream & s );

  };


  class TableData {

    public:
      std::vector<TableCell *>        horz_headers;
      std::vector<TableCell *>        vert_headers;
      std::string                              css;
      std::string                         td_style;
      std::vector<std::vector<TableCell *> > table;

      TableData ();
      virtual ~TableData();

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
      void putValue  ( int    v, int trow, int tcol );
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

      std::string & getHtml  ( std::string & t, std::string tableId,
                               bool compact );
      std::string & getXmli2 ( std::string & x );

      inline bool wasModified  ()  { return modified;  }
      inline void setUnmodified()  { modified = false; }

      virtual void write ( std::ofstream & s );
      virtual void read  ( std::ifstream & s );

  protected:
    bool modified;

    void initTableData();
    void freeTableData();

    std::string & formCell ( std::string & cell,
                             int cell_type, // 0,1,2 for th-h, th-v and td
                             TableCell * cell_data );

    void writeCells ( std::ofstream & s, std::vector<TableCell *> & v );
    void readCells  ( std::ifstream & s, std::vector<TableCell *> & v );

  };

}

#endif // RVAPI_TABLEDATA_H
