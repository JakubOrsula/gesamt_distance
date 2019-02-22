//
//  =================================================================
//
//    04.08.17   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_data  <implementation>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Classes :  rvapi::Data - API Data class
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2013-2017
//
//  =================================================================
//

#include "rvapi_data.h"
#include "rvapi_tasks.h"

rvapi::Data::Data ( const char * datId, const char * dataTitle,
                    int hrow, int hcol, int hrowSpan, int hcolSpan  )
           : Node(datId,hrow,hcol,hrowSpan,hcolSpan)  {
  initData();
  title     = dataTitle;
  foldState = 1;
}

rvapi::Data::Data() : Node()  {
  initData();
}

rvapi::Data::~Data()  {
  freeData();
}

void rvapi::Data::initData()  {}

void rvapi::Data::freeData()  {
  dpath.clear();
  dtype.clear();
}

void rvapi::Data::addData ( std::string dataPath,
                            std::string dataType )  {
  dpath.push_back ( dataPath );
  dtype.push_back ( dataType );
}


int rvapi::Data::dindex ( const char * dataType )  {
int n = -1;
  for (int i=0;(i<(int)dtype.size()) && (n<0);i++)
    if (dtype.at(i)==dataType)
      n = i;
  return n;
}


void rvapi::Data::addFile ( std::string & task,
                            std::string   path )  {
std::string pos;
char        S[100];
size_t      n = path.rfind("/");
  if (n==std::string::npos)  n = path.rfind("\\");
  if (n==std::string::npos)  n = -1;
  sprintf ( S,"_dd_tbx%i",r );
  add_textbox_grid ( task,nodeId()+S,path.substr(n+1),nodeId(),
                     gridPosition(pos,r,0,1,1) );
  sprintf ( S,"_dd_path%i",r );
  add_button_grid ( task,nodeId()+S,"Export",
                    COMMAND_Export,path,
                    false,nodeId(),gridPosition(pos,r,1,1,1) );
  r++;
}


std::string rvapi::Data::makeUMolTitle ( std::string path )  {
std::string basename = path.substr(path.find_last_of("/\\") + 1);
  basename = basename.substr(0,basename.find_last_of('.'));
  if (title.find(basename)!=std::string::npos)
    return title + " ";
  return basename + " " + title + " ";
}


void rvapi::Data::addYXZMapFiles ( std::string & task,
                                   std::string   name,
                                   std::string   pathxyz,
                                   std::string   pathmtz,
                                   std::string   pathmap,
                                   std::string   pathdmap,
                                   std::string   pathlib )  {
std::string pos;

  add_dropdown     ( task,nodeId(),name,parent->nodeId(),
                     foldState,gridPosition(pos) );
  add_grid_compact ( task,nodeId() );

  if (pathxyz.length()>0)  {

    add_button ( task,nodeId()+"_coot","Coot",COMMAND_Coot,
                 "--pdb"  par_del + pathxyz + par_del
                 "--auto" par_del + pathmtz,
                 true,nodeId()+"-ext" );
  
    add_button ( task,nodeId()+"_ccp4mg","ccp4mg",COMMAND_CCP4MG,
                 pathxyz + par_del + pathmtz,
                 true,nodeId()+"-ext" );
    
    add_button ( task,nodeId()+"_uglymol","UglyMol",COMMAND_UglyMol,
                 makeUMolTitle(pathxyz) + item_del +
                 pathxyz + par_del + pathmap + par_del + pathdmap,
                 false,nodeId()+"-ext" );
  
    add_button ( task,nodeId()+"_viewhkl","ViewHKL",COMMAND_ViewHKL,
                 pathmtz, true,nodeId()+"-ext" );
  
    add_button ( task,nodeId()+"_display","Display",COMMAND_Display,
                 pathxyz, false,nodeId()+"-ext" );

  } else  {

    add_button ( task,nodeId()+"_coot","Coot",COMMAND_Coot,
                 "--map" par_del + pathmap,
                 true,nodeId()+"-ext" );
  
    add_button ( task,nodeId()+"_ccp4mg","ccp4mg",COMMAND_CCP4MG,
                 pathmtz,
                 true,nodeId()+"-ext" );
    
    add_button ( task,nodeId()+"_uglymol","UglyMol",COMMAND_UglyMol,
                 makeUMolTitle(pathxyz) + item_del +
                 pathxyz + par_del + pathmap + par_del + pathdmap,
                 false,nodeId()+"-ext" );
  
    add_button ( task,nodeId()+"_viewhkl","ViewHKL",COMMAND_ViewHKL,
                 pathmtz, true,nodeId()+"-ext" );
  
  }

  if (pathxyz .length()>0)  addFile ( task,pathxyz  );
  if (pathmap .length()>0)  addFile ( task,pathmtz  );
  if (pathmap .length()>0)  addFile ( task,pathmap  );
  if (pathdmap.length()>0)  addFile ( task,pathdmap );
  if (pathlib .length()>0)  addFile ( task,pathlib  );

}


void rvapi::Data::addYXZLibFiles ( std::string & task,
                                   std::string   name,
                                   std::string   pathxyz,
                                   std::string   pathlib )  {
std::string pos;

  add_dropdown     ( task,nodeId(),name,parent->nodeId(),
                     foldState,gridPosition(pos) );
  add_grid_compact ( task,nodeId() );

  add_button ( task,nodeId()+"_coot","Coot",COMMAND_Coot,
               "--pdb"  par_del + pathxyz,
               true,nodeId()+"-ext" );

  add_button ( task,nodeId()+"_ccp4mg","ccp4mg",COMMAND_CCP4MG,
               pathxyz,true,nodeId()+"-ext" );
  
  add_button ( task,nodeId()+"_uglymol","UglyMol",COMMAND_UglyMol,
               makeUMolTitle(pathxyz) + item_del + pathxyz,
               false,nodeId()+"-ext" );

  add_button ( task,nodeId()+"_display","Display",COMMAND_Display,
               pathlib, false,nodeId()+"-ext" );

  addFile ( task,pathxyz );
  addFile ( task,pathlib );

}



#define BTN_Coot     0x00000001
#define BTN_MG       0x00000002
#define BTN_UglyMol  0x00000004
#define BTN_ViewHKL  0x00000008
#define BTN_Display  0x00000010
#define BTN_Browser  0x00000020

void rvapi::Data::addDataFile ( std::string & task,
                                std::string   name,
                                std::string   path,
                                int           btnKey )  {
std::string pos;

  add_dropdown     ( task,nodeId(),name,parent->nodeId(),
                     foldState,gridPosition(pos) );
  add_grid_compact ( task,nodeId() );

  if (btnKey & BTN_Coot)
    add_button ( task,nodeId()+"_coot","Coot",COMMAND_Coot,
                 "--pdb" par_del + path,true,nodeId()+"-ext" );

  if (btnKey & BTN_MG)
    add_button ( task,nodeId()+"_ccp4mg","ccp4mg",COMMAND_CCP4MG,
                 path,true,nodeId()+"-ext" );

  if (btnKey & BTN_UglyMol)
    add_button ( task,nodeId()+"_uglymol","UglyMol",COMMAND_UglyMol,
                 makeUMolTitle(path) + item_del + path,
                 false,nodeId()+"-ext" );

  if (btnKey & BTN_ViewHKL)
    add_button ( task,nodeId()+"_viewhkl","ViewHKL",COMMAND_ViewHKL,
                 path, true,nodeId()+"-ext" );

  if (btnKey & BTN_Display)
    add_button ( task,nodeId()+"_display","Display",COMMAND_Display,
                 path, false,nodeId()+"-ext" );

  if (btnKey & BTN_Browser)
    add_button ( task,nodeId()+"_Browser","Show",COMMAND_Browser,
                 path, false,nodeId()+"-ext" );

  addFile ( task,path );

}


void rvapi::Data::write ( std::ofstream & s )  {
int n = dpath.size();
  swrite ( s,title     );
  swrite ( s,foldState );
  swrite ( s,n         );
  for (int i=0;i<n;i++)  {
    swrite ( s,dpath[i] );
    swrite ( s,dtype[i] );
  }
  Node::write ( s );
}

void rvapi::Data::read ( std::ifstream & s )  {
std::string t;
int         n;
  freeData();
  sread ( s,title     );
  sread ( s,foldState );
  sread ( s,n         );
  for (int i=0;i<n;i++)  {
    sread ( s,t );
    dpath.push_back ( t );
    sread ( s,t );
    dtype.push_back ( t );
  }
  Node::read ( s );
}


void rvapi::Data::flush_html ( std::string & outDir,
                               std::string & task )  {
std::string pathmap  = "";
std::string pathdmap = "";
std::string pathlib  = "";
int         ixyz,imtz,imap,idmap,ilib;

  if (wasCreated() || wasModified())  {

    if (wasCreated())  {
      r = 0;

      ixyz  = dindex ( FTYPE_XYZ           );
      imtz  = dindex ( FTYPE_HKL_MAP       );
      imap  = dindex ( FTYPE_HKL_CCP4_MAP  );
      idmap = dindex ( FTYPE_HKL_CCP4_DMAP );
      ilib  = dindex ( FTYPE_LIB           );
      if (imap>=0)   pathmap  = dpath.at(imap);
      if (idmap>=0)  pathdmap = dpath.at(idmap);
      if (ilib>=0)   pathlib  = dpath.at(ilib);

      if ((ixyz>=0) && (imtz>=0))
        addYXZMapFiles ( task,title,dpath.at(ixyz),dpath.at(imtz),
                         pathmap,pathdmap,pathlib );
      else if ((ixyz<0) && (imtz>=0) && (imap>=0))
        addYXZMapFiles ( task,title,"",dpath.at(imtz),
                         pathmap,pathdmap,pathlib );

      else if ((ixyz>=0) && (ilib>=0))
        addYXZLibFiles ( task,title,dpath.at(ixyz),pathlib );

      else if (ixyz>=0)
        addDataFile ( task,title,dpath.at(ixyz),
                           BTN_Coot | BTN_MG | BTN_UglyMol | BTN_Display );

      else if (dtype.at(0)==FTYPE_XYZ_MAP)
        addYXZMapFiles ( task,"Structure and electron density",
                         dpath.at(0)+".pdb",dpath.at(0)+".mtz",
                         pathmap,pathdmap,pathlib );

      else if (dtype.at(0)==FTYPE_XYZIN)
        addDataFile ( task,"Model structure",dpath.at(0),
                           BTN_Coot | BTN_MG | BTN_UglyMol | BTN_Display );
      else if (dtype.at(0)==FTYPE_XYZOUT)
        addDataFile ( task,"Structure",dpath.at(0),
                           BTN_Coot | BTN_MG | BTN_UglyMol | BTN_Display );
      else if (dtype.at(0)==FTYPE_LIB)
        addDataFile ( task,"Ligand structure",dpath.at(0),
                           BTN_Coot | BTN_MG | BTN_UglyMol | BTN_Display );
      else if (dtype.at(0)==FTYPE_HKL_HKL)
        addDataFile ( task,"Reflection data",dpath.at(0),BTN_ViewHKL );
      else if (dtype.at(0)==FTYPE_HKL_MAP)
        addDataFile ( task,"Electron density",dpath.at(0),BTN_ViewHKL );
      else if (dtype.at(0)!=FTYPE_SUMMARY)
        addDataFile ( task,title,dpath.at(0),0 );

    }

  }

  Node::flush_html ( outDir,task );

}

void rvapi::Data::make_xmli2_content ( std::string & tag,
                                       std::string & content )  {
  tag     = "data";
  content = "<title>" + title + "</title>\n";
  for (unsigned int i=0;i<dpath.size();i++)
    content.append ( "<path>" + dpath[i] + "</path><dtype>" +
                                dtype[i] + "</dtype>\n" );

  content.append ( "<open>" + bool2str(foldState>0) + "</open>\n" );

}
