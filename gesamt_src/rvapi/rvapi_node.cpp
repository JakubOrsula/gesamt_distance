//
//  =================================================================
//
//    04.12.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_node  <implementation>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Classes :  rvapi::Node - API base node class
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2013-2014
//
//  =================================================================
//

#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include "rvapi_node.h"

int __rvapi_error = 0;

int  rvapi::getRVAPIError()  { return __rvapi_error; }
void rvapi::setRVAPIError ( int error_code )  { __rvapi_error = error_code; }


rvapi::Node::Node ( const char * nodeId,
                    int hrow,     int hcol,
                    int hrowSpan, int hcolSpan )  {

  initNode();

  if (!nodeId)          genId ( Id );
  else if (!nodeId[0])  genId ( Id );
                  else  Id = nodeId;

  row     = hrow;     // cell row in parent's grid
  col     = hcol;     // cell column in parent's grid
  rowSpan = hrowSpan; // row span in parent's grid
  colSpan = hcolSpan; // row span in parent's grid

  created = true;

}

rvapi::Node::Node()  {
  initNode();
}

rvapi::Node::~Node()  {
  freeNode();
}

int rvapi::Node::idcnt = 0;

void rvapi::Node::genId ( std::string & gId )  {
char S[200];
  sprintf ( S,"auto_id_%i",idcnt++ );
  gId = S;
}

void rvapi::Node::initNode()  {
  modified = true;
  parent   = NULL;
  row      = 0;     // cell row in parent's grid
  col      = 0;     // cell column in parent's grid
  rowSpan  = 1;     // row span in parent's grid
  colSpan  = 1;     // row span in parent's grid
}

void rvapi::Node::freeNode()  {
  for (int i=0;i<(int)nodes.size();i++)
    if (nodes[i])  delete nodes[i];
  nodes.resize(0);
}

void rvapi::Node::setPosition ( int hrow,     int hcol,
                                int hrowSpan, int hcolSpan )  {
  row      = hrow;     // cell row in parent's grid
  col      = hcol;     // cell column in parent's grid
  rowSpan  = hrowSpan; // row span in parent's grid
  colSpan  = hcolSpan; // row span in parent's grid
}

void rvapi::Node::addNode ( Node *node )  {
  node->setParent  ( this );
  nodes.push_back  ( node );
}

bool rvapi::Node::treeWasModified()  {
  if (modified)  return true;
  for (unsigned int i=0;i<nodes.size();i++)
    if (nodes[i]->treeWasModified())
      return true;
  return false;
}

void rvapi::Node::setTreeModified()  {
  created  = true;
  for (unsigned int i=0;i<nodes.size();i++)
    nodes[i]->setTreeModified();
}

bool rvapi::Node::treeWasCreated()  {
  if (created)  return true;
  for (unsigned int i=0;i<nodes.size();i++)
    if (nodes[i]->treeWasCreated())
      return true;
  return false;
}

void rvapi::Node::setTreeCreated()  {
  modified = true;
  created  = true;
  for (unsigned int i=0;i<nodes.size();i++)
    nodes[i]->setTreeCreated();
}

void rvapi::Node::setTreeFinished()  {
  modified = false;
  created  = false;
  for (unsigned int i=0;i<nodes.size();i++)
    nodes[i]->setTreeFinished();
}


rvapi::Node * rvapi::Node::addNode ( Node * node,
                                     std::string & holderId )  {
Node * holder = findNode ( holderId );
  if ((!holder) && (holderId=="body"))
    holder = this;
  if (holder)  {
    holder->addNode ( node );
    if (node->getRow()<0)  {
      int maxRow = -1;
      for (int i=0;i<holder->n_nodes();i++)
        maxRow = std::max ( maxRow,holder->node(i)->getRow() );
      node->setPosition ( maxRow+1,0,1,1 );
    }
  }
  return holder;
}

rvapi::Node * rvapi::Node::addNode ( Node * node,
                                     const char * holderId )  {
std::string hid = holderId;
  return addNode ( node,hid );
}

void rvapi::Node::removeNode ( Node * node )  {
unsigned int i,j;
  if (node->parent)  {
    j = 0;
    for (i=0;i<node->parent->nodes.size();i++)
      if (node->parent->nodes[i]!=node)  {
        if (i!=j)
          node->parent->nodes[j] = node->parent->nodes[i];
        j++;
      }
    if (j<node->parent->nodes.size())  {
      node->parent->nodes.resize(j);
      delete node;
    }
  }
}


std::string & rvapi::Node::gridPosition ( std::string & pos )  {
char S[200];
  sprintf ( S,"%i" key_del "%i" key_del "%i" key_del "%i",
              row,col,rowSpan,colSpan );
  pos = S;
  return pos;
}

std::string & rvapi::Node::gridPosition_xmli2 ( std::string & pos )  {
char S[200];

  if (row>=0)  {
    sprintf ( S," row=\"%i\" col=\"%i\" rowSpan=\"%i\" colSpan=\"%i\"",
              row,col,rowSpan,colSpan );
    pos = S;
  } else
    pos = "";

  return pos;

}


std::string & rvapi::Node::gridPosition ( std::string & pos,
                                          int hrow,     int hcol,
                                          int hrowSpan, int hcolSpan )  {
char S[200];
  sprintf ( S,"%i" key_del "%i" key_del "%i" key_del "%i",
              hrow,hcol,hrowSpan,hcolSpan );
  pos = S;
  return pos;
}

std::string & rvapi::Node::gridPosition_xmli2 ( std::string & pos,
                                          int hrow,     int hcol,
                                          int hrowSpan, int hcolSpan )  {
char S[200];

  if (hrow>=0)  {
    sprintf ( S," row=\"%i\" col=\"%i\" rowSpan=\"%i\" colSpan=\"%i\"",
                hrow,hcol,hrowSpan,hcolSpan );
    pos = S;
  } else
    pos = "";

  return pos;

}

std::string & rvapi::Node::getNodeFName ( std::string & fname,
                                          std::string  prefix,
                                          const char * suffix )  {
char S[200];
  sprintf ( S,".%i_%i.%i_%i.%s",row,col,rowSpan,colSpan,suffix );
  fname = prefix + S;
  return fname;
}

rvapi::Node * rvapi::Node::findNode ( std::string & nodeId )  {
// full tree search
Node *n = NULL;

  if (Id==nodeId)  return this;

  for (int i=0;(i<(int)nodes.size()) && (!n);i++)
    if (nodes[i])
      n = nodes[i]->findNode ( nodeId );

  return n;

}

rvapi::Node * rvapi::Node::findNode ( const char * nodeId )  {
std::string nid = nodeId;
  return findNode ( nid );
}

rvapi::Node * rvapi::Node::findNode ( const char * nodeId,
                                      NODE_TYPE t )  {
Node * node = findNode ( nodeId );
  if (node)  {
    if (node->type()==t)
      return node;
  }
  return NULL;
}


std::string & rvapi::Node::makeContent ( std::string & fname,
                                         std::string & data,
                                         std::string & outDir,
                                         const char  * contentName )  {
// note that fname gets moodified here, and this is important
std::string fpath = outDir + getNodeFName ( fname,Id,contentName );

  if (!data.empty())  {
    std::ofstream s;
    std::string   fpath0 = fpath;
    struct stat   buffer;
    bool          fexists = (stat(fpath.c_str(),&buffer)==0);

    if (fexists)  {
      char S[100];
      sprintf ( S,"%li",time(NULL) );
      fpath0.append ( S );
    }

    s.open ( fpath0.c_str() );
    if (s.good())
      s << data;
    s.close();

    if (fexists)  {
      remove ( fpath.c_str() );
      rename ( fpath0.c_str(),fpath.c_str() );
    }

  }

  return fname;

}

std::string & rvapi::Node::makeContent ( std::string & fname,
                                         std::string & data,
                                         std::string & outDir,
                                         std::string   contentName )  {
  return makeContent ( fname,data,outDir,contentName.c_str() );
}

std::string & rvapi::Node::addContent ( std::string & fname,
                                        std::string & data,
                                        std::string & outDir,
                                        const char  * contentName )  {

  if (!data.empty())  {
    std::ofstream s;
    std::string   taskfn  = outDir + getNodeFName ( fname,Id,contentName );
    std::string   taskfn0 = taskfn;
    struct stat   buffer;
    bool          fexists = (stat(taskfn.c_str(),&buffer)==0);

    if (fexists)  {
      char S[100];
      sprintf ( S,"%li",time(NULL) );
      taskfn0.append ( S );
      std::ifstream  src(taskfn .c_str(),std::ios::binary);
      std::ofstream  dst(taskfn0.c_str(),std::ios::binary);
      dst << src.rdbuf();
    }

    s.open ( taskfn0.c_str(),std::ios::out | std::ios::app );
    if (s.good())
      s << data;
    s.close();

    if (fexists)  {
      remove ( taskfn.c_str() );
      rename ( taskfn0.c_str(),taskfn.c_str() );
    }

  }

  return fname;

}

std::string & rvapi::Node::addContent ( std::string & fname,
                                        std::string & data,
                                        std::string & outDir,
                                        std::string   contentName )  {
  return addContent ( fname,data,outDir,contentName.c_str() );
}


void rvapi::Node::flush_html ( std::string & outDir,
                               std::string & task )  {

  created  = false;
  modified = false;

  for (int i=0;i<(int)nodes.size();i++)
    if (nodes[i])
      nodes[i]->flush_html ( outDir,task );

}

void rvapi::Node::make_xmli2_content ( std::string & tag,
                                       std::string & content )  {
  tag = "node";
  content.clear();
}

void rvapi::Node::flush_xmli2 ( std::string & outDir,
                                std::string & xml )  {
std::string tag;
std::string content;
std::string nid;
std::string pos;

  make_xmli2_content ( tag,content );

  if (!content.empty())  {
    nid = nodeId();
    if (nid.find("auto_id_")!=std::string::npos)  nid = parent->nodeId();
    gridPosition_xmli2 ( pos );
    xml.append ( "<" + tag + " id=\"" + nid + "\"" + pos + ">\n" + content );
  }

  for (int i=0;i<(int)nodes.size();i++)
    if (nodes[i])
      nodes[i]->flush_xmli2 ( outDir,xml );

  if (!content.empty())
    xml.append ( "</" + tag + ">\n" );

}

void rvapi::Node::write ( std::ofstream & s )  {
  swrite ( s,created  );
  swrite ( s,modified );
  swrite ( s,Id       );
  swrite ( s,beforeId );
  swrite ( s,row      );
  swrite ( s,col      );
  swrite ( s,rowSpan  );
  swrite ( s,colSpan  );
}

void rvapi::Node::read  ( std::ifstream & s )  {
//  created  = true;
//  modified = true;
  sread ( s,created  );
  sread ( s,modified );
  sread ( s,Id       );
  sread ( s,beforeId );
  sread ( s,row      );
  sread ( s,col      );
  sread ( s,rowSpan  );
  sread ( s,colSpan  );
}

void rvapi::swrite ( std::ofstream & s, int n )  {
  s.write ( (char*)(&n),sizeof(n) );
}

void rvapi::swrite ( std::ofstream & s, double v )  {
  s.write ( (char*)(&v),sizeof(v) );
}

void rvapi::swrite ( std::ofstream & s, bool b )  {
  s.write ( (char*)(&b),sizeof(b) );
}

void rvapi::swrite ( std::ofstream & s, std::string text )  {
int n = text.size();
  s.write ( (char*)(&n),sizeof(n) );
  s.write ( text.c_str(),n );
}


void rvapi::sread ( std::ifstream & s, int & n )  {
  s.read ( (char*)(&n),sizeof(n) );
}

void rvapi::sread ( std::ifstream & s, double & v )  {
  s.read ( (char*)(&v),sizeof(v) );
}

void rvapi::sread ( std::ifstream & s, bool & b )  {
  s.read ( (char*)(&b),sizeof(b) );
}

void rvapi::sread ( std::ifstream & s, std::string & text )  {
char *b;
int   n;
  s.read ( (char*)(&n),sizeof(n) );
  b = new char[n+10];
  s.read ( b,n );
  b[n] = char(0);
  text = b;
  delete[] b;
}

std::string rvapi::bool2str ( bool b )  {
  if (b)  return "true";
    else  return "false";
}

std::string rvapi::double2str ( double d, const char *fmt )  {
char S[200];
  if (fmt)  {
    if (fmt[0])  sprintf ( S,fmt,d );
           else  sprintf ( S,"%g",d );
  } else
    sprintf ( S,"%g",d );
  return S;
}
