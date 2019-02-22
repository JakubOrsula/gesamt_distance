//
//  =================================================================
//
//    06.03.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_radar  <implementation>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Classes :  rvapi::Radar - radar chart class
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2014
//
//  =================================================================
//

#include "rvapi_radar.h"
#include "rvapi_tasks.h"

rvapi::Radar::Radar ( const char * radarId, const char * radarTitle,
                      int hrow, int hcol, int hrowSpan, int hcolSpan )
            : Node ( radarId,hrow,hcol,hrowSpan,hcolSpan )  {
  initRadar();
  title = radarTitle;
}

rvapi::Radar::Radar() : Node()  {
  initRadar();
}

rvapi::Radar::~Radar()  {
  freeRadar();
}

void rvapi::Radar::initRadar()  {
  width       = 300;
  height      = 300;
  maxValue    = 1.0;
  nLevels     = 5;
  translateX  = 50;
  translateY  = 30;
  extraWidthX = 100;
  extraWidthY = 70;
  foldState   = 0;
}

void rvapi::Radar::freeRadar()  {
  names .clear();
  values.clear();
}

void rvapi::Radar::addProperty ( const char * name,
                                 const double value )  {
  names .push_back ( name  );
  values.push_back ( value );
  setModified();
}

void rvapi::Radar::write ( std::ofstream & s )  {
int n = names.size();

  swrite ( s,title       );
  swrite ( s,width       );
  swrite ( s,height      );
  swrite ( s,maxValue    );
  swrite ( s,nLevels     );
  swrite ( s,translateX  );
  swrite ( s,translateY  );
  swrite ( s,extraWidthX );
  swrite ( s,extraWidthY );
  swrite ( s,foldState   );
  swrite ( s,n           );

  for (int i=0;i<n;i++)  {
    swrite ( s,names [i] );
    swrite ( s,values[i] );
  }

  Node::write ( s );

}

void rvapi::Radar::read ( std::ifstream & s )  {
std::string name;
double       value;
int         n;

  freeRadar();

  sread ( s,title       );
  sread ( s,width       );
  sread ( s,height      );
  sread ( s,maxValue    );
  sread ( s,nLevels     );
  sread ( s,translateX  );
  sread ( s,translateY  );
  sread ( s,extraWidthX );
  sread ( s,extraWidthY );
  sread ( s,foldState   );
  sread ( s,n           );

  for (int i=0;i<n;i++)  {
    sread ( s,name  );
    sread ( s,value );
    names .push_back ( name  );
    values.push_back ( value );
  }

  Node::read ( s );

}

void rvapi::Radar::flush_html ( std::string & outDir,
                                std::string & task )  {

  if ((names.size()>0) && (wasCreated() || wasModified()))  {
    std::string data;
    std::string pos;
    char        N[200];

    if (wasCreated())
      add_dropdown ( task,nodeId(),title,parent->nodeId(),
                     foldState,gridPosition(pos) );

    for (unsigned int i=0;i<names.size();i++)  {
      if (data.empty())  {
        data = "[[{axis:\"";
      } else
        data.append ( ",{axis:\"" );
      data.append ( names[i] + "\",value:" );
      sprintf ( N,"%.7g",values[i] );
      data.append ( N );
      data.append ( "}" );
    }
    data.append ( "]]" );

    sprintf ( N,"{w:%i,h:%i,maxValue:%.7g,levels:%i,"
                "TranslateX:%i,TranslateY:%i,"
                "ExtraWidthX:%i,ExtraWidthY:%i}",
                width,height,maxValue,nLevels,
                translateX,translateY,
                extraWidthX,extraWidthY );

    add_radar ( task,data,N,nodeId() );

  }

  Node::flush_html ( outDir,task );

}

