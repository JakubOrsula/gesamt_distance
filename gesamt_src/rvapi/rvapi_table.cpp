//
//  =================================================================
//
//    04.12.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_table  <implementation>
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

#include <stdlib.h>

#include "rvapi_table.h"
#include "rvapi_tabledata.h"
#include "rvapi_tasks.h"

rvapi::Table::Table ( const char * tblId,
                      const char * tblTitle,
                      int hrow, int hcol,
                      int hrowSpan, int hcolSpan  )
            : Node(tblId,hrow,hcol,hrowSpan,hcolSpan)  {

  tableData = NULL;

  initTable();

  legend    = tblTitle;
  foldState = 1;

}

rvapi::Table::Table() : Node()  {
  initTable();
}

rvapi::Table::~Table()  {
  freeTable();
}

void rvapi::Table::initTable()  {
  tableData = new TableData();
  sortable  = false;  // true to use the tablesorter plugin
  paging    = false;  // true to use the tablesorter pager plugin
}

void rvapi::Table::freeTable()  {
  if (tableData)  {
    delete tableData;
    tableData = NULL;
  }
}

void rvapi::Table::setStyle ( const char * css,
                              const char * td_style )  {
  tableData->css      = css;
  tableData->td_style = td_style;
}

void rvapi::Table::setTableType ( bool sorting, bool pages )  {
  sortable = sorting;
  paging   = pages;
}


void rvapi::Table::putHorzHeader ( const char * header,
                                   const char * tooltip,
                                   int tcol )  {
  if (tableData)  {
    tableData->putHorzHeader ( header,tooltip,tcol );
    setModified();
  }
}

void rvapi::Table::shapeHorzHeader ( int          tcol,
                                     const char * cell_style,
                                     const char * cell_css,
                                     int          rowSpan,
                                     int          colSpan )  {
  if (tableData)  {
    tableData->shapeHorzHeader ( tcol,cell_style,cell_css,
                                 rowSpan,colSpan );
    setModified();
  }
}

void rvapi::Table::putVertHeader ( const char * header,
                                   const char * tooltip,
                                   int trow )  {
  if (tableData)  {
    tableData->putVertHeader ( header,tooltip,trow );
    setModified();
  }
}

void rvapi::Table::shapeVertHeader ( int          trow,
                                     const char * cell_style,
                                     const char * cell_css,
                                     int          rowSpan,
                                     int          colSpan )  {
  if (tableData)  {
    tableData->shapeVertHeader ( trow,cell_style,cell_css,
                                 rowSpan,colSpan );
    setModified();
  }
}

void rvapi::Table::putValue ( const char * v, int trow, int tcol )  {
  if (tableData)  {
    tableData->putValue ( v,trow,tcol );
    setModified();
  }
}

void rvapi::Table::putValue ( int v, int trow, int tcol )  {
  if (tableData)  {
    tableData->putValue ( v,trow,tcol );
    setModified();
  }
}

void rvapi::Table::putValue ( double v, const char * fmt,
                              int trow, int tcol )  {
  if (tableData)  {
    tableData->putValue ( v,fmt,trow,tcol );
    setModified();
  }
}

void rvapi::Table::resetData ( int trow )  {
  if (tableData)  {
    tableData->resetData ( trow );
    setModified();
  }
}

void rvapi::Table::shapeCell ( int          trow,
                               int          tcol,
                               const char * tooltip,
                               const char * cell_style,
                               const char * cell_css,
                               int          rowSpan,
                               int          colSpan )  {
  if (tableData)  {
    tableData->shapeCell ( trow,tcol,tooltip,cell_style,cell_css,
                           rowSpan,colSpan );
    setModified();
  }
}

void rvapi::Table::write ( std::ofstream & s )  {
int key;
  swrite ( s,legend    );
  swrite ( s,foldState );
  swrite ( s,sortable  );
  swrite ( s,paging    );
  if (tableData)  {
    key = 1;
    swrite ( s,key );
    tableData->write ( s );
  } else  {
    key = 0;
    swrite ( s,key );
  }
  Node::write ( s );
}

void rvapi::Table::read ( std::ifstream & s )  {
int key;
  freeTable();
  sread ( s,legend    );
  sread ( s,foldState );
  sread ( s,sortable  );
  sread ( s,paging    );
  sread ( s,key       );
  if (key>0)  {
    if (!tableData)
      tableData = new TableData();
    tableData->read ( s );
  }
  Node::read ( s );
}

void rvapi::Table::flush_html ( std::string & outDir,
                                std::string & task )  {

  if (wasCreated() || wasModified())  {
    std::string tddId = nodeId() + "_table";
    std::string pos;

    if (wasCreated())
      add_dropdown ( task,tddId,legend,parent->nodeId(),
                     foldState,gridPosition(pos) );

    if (tableData)  {

      std::string t;
      tableData->getHtml ( t,nodeId(),(abs(foldState)<2) );

      if ((t.find(key_ter)!=std::string::npos) ||
          (t.find(key_del)!=std::string::npos) ||
          (t.size()>inline_size_threshold))  {
        std::string subtask;
        std::string fname;
        Node::flush_html ( outDir,subtask );
        if (!subtask.empty())
             subtask = makeContent ( fname,subtask,outDir,tddId+".tsk" );
        else subtask = " ";
        if (sortable)
          load_content_tablesort ( task,makeContent(fname,t,outDir,"table"),
                                   paging,nodeId()+"-grid",tddId );
        else
          load_content ( task,makeContent(fname,t,outDir,"table"),
                         false,subtask,tddId );
      } else  {
        if (sortable)
              set_html_tablesort ( task,t,paging,tddId );
        else  set_html ( task,t,tddId );
        Node::flush_html ( outDir,task );
      }

      tableData->setUnmodified();

    }

  }

}

void rvapi::Table::make_xmli2_content ( std::string & tag,
                                        std::string & content )  {

  tag     = "table";
  content = "<legend>" + legend + "</legend>\n";
  tableData->getXmli2 ( content );

}
