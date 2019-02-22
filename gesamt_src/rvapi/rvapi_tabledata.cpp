//
//  =================================================================
//
//    14.10.15   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_tabledata  <implementation>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Classes :  rvapi::TableData - table data class
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2013-2015
//
//  =================================================================
//

#include <stdio.h>

#include "rvapi_tabledata.h"
#include "rvapi_global.h"
#include "rvapi_node.h"

std::string rvapi::TableCell::geti2XMLText()  {
std::string s = text;
  s.erase ( s.find_last_not_of(" \n\r\t")+1 );
  if (s.empty())  s = ".";
  return s;
}

void rvapi::TableCell::write ( std::ofstream & s )  {
  swrite ( s,text    );
  swrite ( s,tooltip );
  swrite ( s,style   );
  swrite ( s,css     );
  swrite ( s,rowSpan );
  swrite ( s,colSpan );
}

void rvapi::TableCell::read ( std::ifstream & s )  {
  sread ( s,text    );
  sread ( s,tooltip );
  sread ( s,style   );
  sread ( s,css     );
  sread ( s,rowSpan );
  sread ( s,colSpan );
}


rvapi::TableData::TableData()  {
  initTableData();
}

rvapi::TableData::~TableData()  {
  freeTableData();
}

void rvapi::TableData::initTableData()  {
  modified = true;
  css      = RVAPI_CSS_TABLE_Blue;
  td_style = RVAPI_STYLE_Right;
}

void rvapi::TableData::freeTableData()  {
unsigned int i,j;

  for (i=0;i<horz_headers.size();i++)
    if (horz_headers[i])  delete horz_headers[i];
  horz_headers.clear();

  for (i=0;i<vert_headers.size();i++)
    if (vert_headers[i])  delete vert_headers[i];
  vert_headers.clear();

  for (i=0;i<table.size();i++)  {
    for (j=0;j<table[i].size();j++)
      if (table[i][j])  delete table[i][j];
    table[i].clear();
  }
  table.clear();

  modified = true;

}

void rvapi::TableData::putHorzHeader ( const char * header,
                                       const char * tooltip,
                                       int tcol )  {
  while ((int)horz_headers.size()<=tcol)
    horz_headers.push_back ( new TableCell() );
  horz_headers[tcol]->text    = header;
  horz_headers[tcol]->tooltip = tooltip;
  modified = true;
}

void rvapi::TableData::shapeHorzHeader ( int          tcol,
                                         const char * cell_style,
                                         const char * cell_css,
                                         int          rowSpan,
                                         int          colSpan )  {
  if ((int)horz_headers.size()<=tcol)
    putHorzHeader ( "","",tcol );
  horz_headers[tcol]->style   = cell_style;
  horz_headers[tcol]->css     = cell_css;
  horz_headers[tcol]->rowSpan = rowSpan;
  horz_headers[tcol]->colSpan = colSpan;
  modified = true;
}


void rvapi::TableData::putVertHeader ( const char * header,
                                       const char * tooltip,
                                       int trow )  {

  while ((int)vert_headers.size()<=trow)
    vert_headers .push_back ( new TableCell() );
  vert_headers[trow]->text    = header;
  vert_headers[trow]->tooltip = tooltip;

  if ((int)table.size()<=trow)  {
    std::vector<TableCell *> v;
    while ((int)table.size()<=trow)
      table.push_back(v);
  }

  modified = true;

}

void rvapi::TableData::shapeVertHeader ( int          trow,
                                         const char * cell_style,
                                         const char * cell_css,
                                         int          rowSpan,
                                         int          colSpan )  {
  if ((int)vert_headers.size()<=trow)
    putVertHeader ( "","",trow );
  vert_headers[trow]->style   = cell_style;
  vert_headers[trow]->css     = cell_css;
  vert_headers[trow]->rowSpan = rowSpan;
  vert_headers[trow]->colSpan = colSpan;
  modified = true;
}


void rvapi::TableData::putValue ( const char * v, int trow, int tcol ) {

  if ((int)table.size()<=trow)  {
    std::vector<TableCell *> v;
    while ((int)table.size()<=trow)
      table.push_back(v);
  }

  while ((int)table[trow].size()<=tcol)
    table[trow].push_back ( new TableCell() );

  table[trow][tcol]->text = v;

  modified = true;

}

void rvapi::TableData::putValue ( int v, int trow, int tcol )  {
char S[30];
  sprintf  ( S,"%i",v    );
  putValue ( S,trow,tcol );
}

void rvapi::TableData::putValue ( double v, const char *fmt,
                                  int trow, int tcol )  {
char S[50];
  if (fmt)  {
    if (fmt[0])  sprintf ( S,fmt,v );
           else  sprintf ( S,"%g",v );
  } else
    sprintf ( S,"%f",v );
  putValue ( S,trow,tcol );
}

void rvapi::TableData::resetData ( int trow ) {

  if ((int)table.size()<=trow)  {
    std::vector<TableCell *> v;
    while ((int)table.size()<=trow)
      table.push_back(v);
  } else  {
    for (unsigned int j=0;j<table[trow].size();j++)
      if (table[trow][j])  delete table[trow][j];
    table[trow].clear();
  }

  modified = true;

}


void rvapi::TableData::shapeCell ( int          trow,
                                   int          tcol,
                                   const char * tooltip,
                                   const char * cell_style,
                                   const char * cell_css,
                                   int          rowSpan,
                                   int          colSpan )  {
  if (((int)table.size()<=trow) ||
      ((int)table[trow].size()<=tcol))
    putValue ( "",trow,tcol );
  table[trow][tcol]->tooltip = tooltip;
  table[trow][tcol]->style   = cell_style;
  table[trow][tcol]->css     = cell_css;
  table[trow][tcol]->rowSpan = rowSpan;
  table[trow][tcol]->colSpan = colSpan;
  modified = true;
}


std::string & rvapi::TableData::formCell ( std::string & cell,
                                           int           cell_type,
                                           TableCell   * cell_data )  {
char  S[20];
int   ctype = cell_type;

  cell.clear();

  if (cell_data->css.rfind("-hh")==cell_data->css.size()-3)
    ctype = 0;
  else if (cell_data->css.rfind("-vh")==cell_data->css.size()-3)
    ctype = 1;
  else if (cell_data->css.rfind("-td")==cell_data->css.size()-3)
    ctype = 2;

  switch (ctype)  {
    case 0 :
    case 1 : cell = "<th style='white-space:nowrap;' class='";  break;
    default:
    case 2 : cell = "<td class='";  break;
  }

  if (cell_data->css!="")
    cell.append ( cell_data->css+"'" );
  else  {
    cell.append ( css );
    switch (ctype)  {
      case 0 : cell.append ( "-hh'" );  break;
      case 1 : cell.append ( "-vh'" );  break;
      default:
      case 2 : cell.append ( "-td'" );  break;
    }
  }

  if (cell_data->style!="")
    cell.append ( " style='"+cell_data->style+"'" );
  else if ((cell_type==2) && (td_style!=""))
    cell.append ( " style='"+td_style+"'" );

  if (cell_data->tooltip!="")
    cell.append ( " title='" + cell_data->tooltip + "'" );

  if (cell_data->rowSpan>1)  {
    sprintf ( S," rowspan='%i'",cell_data->rowSpan );
    cell.append ( S );
  }

  if (cell_data->colSpan>1)  {
    sprintf ( S," colspan='%i'",cell_data->colSpan );
    cell.append ( S );
  }

  cell.append ( ">"+cell_data->text );
  switch (ctype)  {
    case 0 :
    case 1 : cell.append ( "</th>\n" );  break;
    default:
    case 2 : cell.append ( "</td>\n" );  break;
  }

  return cell;

}

std::string & rvapi::TableData::getHtml ( std::string & t,
                                          std::string tableId,
                                          bool        compact )  {
std::string cell;
TableCell   phony;
int         i,j;

  t.clear();

  if (table.size()>0)  {

    phony.text = "&nbsp;";

    t = "<table ";
    if (!tableId.empty())
      t.append ( "id='" + tableId + "-grid' " );
    if (compact)
           t.append ( "style='width:auto' " );
     else  t.append ( "style='width:100%' " );
    t.append ( "class='" + css + "'>\n" );
    /*
    if (tableId.empty())
          t = "<table class='" + css + "'>\n";
    else  t = "<table id='"+tableId+"-grid' class='" + css + "'>\n";
    */

    if (horz_headers.size()>0)  {
      t.append ( " <thead>\n   <tr>\n" );
      if (vert_headers.size()>0)
        t.append ( formCell(cell,0,&phony) );
      for (j=0;j<(int)horz_headers.size();j++)
        t.append ( formCell(cell,0,horz_headers[j]) );
      t.append ( "  </tr>\n </thead>\n <tbody>\n" );
    }

    for (i=0;i<(int)table.size();i++)  {
      t.append ( "  <tr>\n" );
      if (!vert_headers.empty())  {
        if (i<(int)vert_headers.size())
              t.append ( formCell(cell,1,vert_headers[i]) );
        else  t.append ( formCell(cell,1,&phony) );
      }
      for (j=0;j<(int)table[i].size();j++)
        t.append ( formCell(cell,2,table[i][j]) );
      t.append ( "  </tr>\n" );
    }

    if (horz_headers.size()>0)
      t.append ( " </tbody>\n" );

    t.append ( "</table>\n" );

  }

  return t;

}

std::string & rvapi::TableData::getXmli2 ( std::string & x )  {
size_t nrows;

  if (horz_headers.size()>0)  {
    x.append ( "<headers separator=\"?\">\n" );
    for (unsigned int i=0;i<horz_headers.size();i++)  {
      if (i>0)  x.append ( "?" );
      x.append ( horz_headers[i]->text );
    }
    x.append ( "\n</headers>\n" );
  }
  x.append ( "<data>\n" );
  nrows = 0;
  for (unsigned long j=0;j<table.size();j++)
    nrows = std::max ( nrows,table[j].size() );    
  for (unsigned int i=0;i<table.size();i++)  {
    for (unsigned int j=0;j<nrows;j++)
       if (j<table[i].size())
             x.append ( "   " + table[i][j]->geti2XMLText() );
       else  x.append ( "   ." );
    x.append ( "\n" );
  }
  x.append ( "</data>\n" );

  return x;

}


void rvapi::TableData::writeCells ( std::ofstream & s,
                                    std::vector<TableCell *> & v )  {
int n = v.size();
int key,i;
  swrite ( s,n );
  for (i=0;i<n;i++)
    if (v.at(i))  {
      key = 1;
      swrite ( s,key );
      v.at(i)->write ( s );
    } else  {
      key = 0;
      swrite ( s,key );
    }
}

void rvapi::TableData::readCells ( std::ifstream & s,
                                   std::vector<TableCell *> & v )  {
TableCell *cell;
int n,key,i;
  sread ( s,n );
  for (i=0;i<n;i++)  {
    cell = new TableCell();
    sread ( s,key );
    if (key>0)
      cell->read ( s );
    v.push_back ( cell );
  }
}

void rvapi::TableData::write ( std::ofstream & s )  {
int nr = table.size();
int i;

  swrite ( s,modified );
  swrite ( s,css      );
  swrite ( s,td_style );
  swrite ( s,nr       );
  writeCells ( s,horz_headers );
  writeCells ( s,vert_headers );
  for (i=0;i<nr;i++)
    writeCells ( s,table[i] );

}

void rvapi::TableData::read ( std::ifstream & s )  {
int nr;
int i;

  freeTableData();

  sread ( s,modified );
  sread ( s,css      );
  sread ( s,td_style );
  sread ( s,nr       );
  readCells ( s,horz_headers );
  readCells ( s,vert_headers );
  for (i=0;i<nr;i++)  {
    std::vector<TableCell *> v;
    readCells ( s,v );
    table.push_back ( v );
  }

//  modified = true;

}
