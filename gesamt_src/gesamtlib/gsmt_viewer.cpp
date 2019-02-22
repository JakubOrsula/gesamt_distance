//
// =================================================================
//  This code is distributed under the terms and conditions of the
//  CCP4 Program Suite Licence Agreement as 'Part 2' (Annex 2)
//  software. A copy of the CCP4 licence can be obtained by writing
//  to CCP4, Research Complex at Harwell, Rutherford Appleton
//  Laboratory, Didcot OX11 0FA, UK, or from
//  http://www.ccp4.ac.uk/ccp4license.php.
// =================================================================
//
//    10.02.13   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  GSMT_Viewer <implementation>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::Viewer
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2008-2013
//
// =================================================================
//

#include <stdlib.h>
#include <string.h>

#include "gsmt_viewer.h"

//  Temporary viewer files

#define  rasmol_script        "__rasmol.script"
#define  ngl_script           "__ngl.script"
#define  structure_file       "__view_structure.pdb"

//  Viewer colours

#define  COLOUR_unmatched_1     "[150,150,150]"
#define  COLOUR_unmatched_2     "[230,230,230]"
#define  COLOUR_matched_1       "[0,150,150]"
#define  COLOUR_matched_2       "[0,255,255]"
#define  COLOUR_bug             "[255,0,255]"
#define  COLOUR_cluster_1       "[255,0,0]"
#define  COLOUR_cluster_2       "[0,255,0]"



// =================================================================

gsmt::Viewer::Viewer()  {
  InitClass();
}

gsmt::Viewer::~Viewer()  {
  FreeMemory();
}

void gsmt::Viewer::InitClass()  {
  rasmol = NULL;
  jmol   = NULL;
  ccp4mg = NULL;
  ngl    = NULL;
  mmdb::CreateCopy ( rasmol,"/home/eugene/Applications/Rasmol/rasmol" );
  mmdb::CreateCopy ( jmol  ,"/Programs/Jmol/jmol" );
  mmdb::CreateCopy ( ccp4mg,"/Programs/ccp4/bin/ccp4mg" );
  mmdb::CreateCopy ( ccp4mg,"/Programs/NGL/nglviewer" );
  bckg   = NULL;
  style  = NULL;   //!< initial style (cartoon, backbone etc.)
  mmdb::CreateCopy ( style,"cartoon" );
  viewer = VIEWER_Jmol;
}

void gsmt::Viewer::setJMolPath ( mmdb::cpstr jmol_path )  {
  mmdb::CreateCopy ( jmol,jmol_path );
}

void gsmt::Viewer::setRasmolPath ( mmdb::cpstr rasmol_path )  {
  mmdb::CreateCopy ( rasmol,rasmol_path );
}

void gsmt::Viewer::setCCP4MGPath ( mmdb::cpstr ccp4mg_path )  {
  mmdb::CreateCopy ( ccp4mg,ccp4mg_path );
}

void gsmt::Viewer::setNGLPath ( mmdb::cpstr ngl_path )  {
  mmdb::CreateCopy ( ngl,ngl_path );
}

void gsmt::Viewer::setViewerType ( VIEWER_KEY viewer_type )  {
  viewer = viewer_type;
}

void gsmt::Viewer::setBackgroundMode ( bool in_background )  {
  if (in_background)
    mmdb::CreateCopy ( bckg," &" );
  else if (bckg)  {
    delete[] bckg;
    bckg = NULL;
  }
}

void gsmt::Viewer::setStyle ( mmdb::cpstr start_style )  {
  mmdb::CreateCopy ( style,start_style );
}


void gsmt::Viewer::FreeMemory()  {
  if (rasmol)  {
    delete[] rasmol;
    rasmol = NULL;
  }
  if (jmol)  {
    delete[] jmol;
    jmol = NULL;
  }
  if (ccp4mg)  {
    delete[] ccp4mg;
    ccp4mg = NULL;
  }
  if (ngl)  {
    delete[] ngl;
    ngl = NULL;
  }
  if (bckg)  {
    delete[] bckg;
    bckg = NULL;
  }
  if (style)  {
    delete[] style;
    style = NULL;
  }
}

void gsmt::Viewer::launchViewer()  {
mmdb::pstr S;

  S = NULL;
  switch (viewer)  {
    default:
    case VIEWER_Rasmol: mmdb::CreateCopCat ( S,rasmol," -script ",
                                             rasmol_script," ",
                                             structure_file );
                     break;
    case VIEWER_Jmol  : mmdb::CreateCopCat ( S,jmol," --script ",
                                             rasmol_script," ",
                                             structure_file );
                     break;
    case VIEWER_CCP4MG: mmdb::CreateCopCat ( S,ccp4mg," -pict ",
                                             structure_file );
                     break;
    case VIEWER_NGL   : mmdb::CreateCopCat ( S,ngl," -cfg ",
                                             ngl_script," ",
                                             structure_file );
  }
  if (bckg)
    mmdb::CreateConcat ( S,bckg );

  system ( S );

  delete[] S;

}

gsmt::VIEWER_CODE gsmt::Viewer::Download ( PStructure     s1,
                                           PStructure     s2,
                                           PSuperposition SD,
                                           mmdb::cpstr    fileName ) {
mmdb::io::File f;

  f.assign ( fileName,true,false,mmdb::io::GZM_CHECK );
  if (!f.rewrite())
    return VIEWER_cantWriteFile;

  writeSuperposition ( f,s1,s2,SD,NULL,NULL );

  f.shut();

  return VIEWER_Ok;

}


gsmt::VIEWER_CODE gsmt::Viewer::View ( PStructure     s1,
                                       PStructure     s2,
                                       PSuperposition SD,
                                       mmdb::ivector  c10,
                                       mmdb::ivector  c20 ) {
VIEWER_CODE rc;

  switch (viewer)  {

    default :
    case VIEWER_Rasmol:
    case VIEWER_Jmol  : rc = prepare_rasmol ( s1,s2,SD,c10,c20 ); break;
    case VIEWER_CCP4MG: rc = prepare_ccp4mg ( s1,s2,SD,c10,c20 ); break;
    case VIEWER_NGL   : rc = prepare_ngl    ( s1,s2,SD,c10,c20 );

  }
    
  if (rc==VIEWER_Ok)
    launchViewer();

  return rc;

}


gsmt::VIEWER_CODE gsmt::Viewer::prepare_rasmol ( PStructure     s1,
                                                 PStructure     s2,
                                                 PSuperposition SD,
                                                 mmdb::ivector  c10,
                                                 mmdb::ivector  c20 ) {
mmdb::io::File f;

  f.assign ( rasmol_script,true );
  if (!f.rewrite())
    return VIEWER_cantWriteFile;

  f.Write ( "select all\n"
            "wireframe off\n"
            "spacefill off\n"
            "cartoons on\n"
            "select atomno=1\n"
            "colour " COLOUR_unmatched_1 "\n"
            "select atomno=2\n"
            "colour " COLOUR_unmatched_2 "\n"
            "select atomno=3\n"
            "colour " COLOUR_matched_1 "\n"
            "select atomno=4\n"
            "colour " COLOUR_matched_2 "\n" );

  if (c10)
    f.Write ( "select atomno=5\n"
              "colour " COLOUR_cluster_1 "\n" );
  if (c20)
    f.Write ( "select atomno=6\n"
              "colour " COLOUR_cluster_2 "\n" );

  f.Write ( "select atomno=7\n"
            "colour " COLOUR_bug "\n"
            "select all\n"
          );

  f.shut();

  f.assign ( structure_file,true );
  if (!f.rewrite())
    return VIEWER_cantWriteFile;

  writeSuperposition ( f,s1,s2,SD,c10,c20 );

  f.shut();

  return VIEWER_Ok;

}

#define ccp4mg_molname  "ccp4mg_molname"

gsmt::VIEWER_CODE gsmt::Viewer::prepare_ccp4mg ( PStructure     s1,
                                                 PStructure     s2,
                                                 PSuperposition SD,
                                                 mmdb::ivector  c10,
                                                 mmdb::ivector  c20 ) {
mmdb::io::File f;

  f.assign ( structure_file,true );
  if (!f.rewrite())
    return VIEWER_cantWriteFile;

  f.Write ( "MolData (\n"
            "    filename = ['INLINE', '" );
  f.Write ( ccp4mg_molname );
  f.Write ( "', '']\n"
            ")\n\n" );
        
  f.Write ( "MolDisp (\n"
            "    selection_parameters =  { 'select' : 'all'},\n"
            "    style_parameters =  { 'style_mode' : 'SPLINE'  },\n"
            "    colour_parameters =  { 'colour_mode' : 'bychain'}\n"
            ")\n\n"
          );


/*

SelectionScheme (
          name = 'interesting_residues',
          context = '1df7',
          selection = '/1/A/10-20 or //500(NDP) or //501(MTX)',
          )
*/
//ColourScheme (
//          name = 'c_grey',
//          context = 'generic',
//          colours = [['green', 'all'],
//                     ['grey', '/*/*/*/*[C]:*'],
//                     ['red', '/*/*/*/*[O]:*'],
//                     ['blue', '/*/*/*/*[N]:*'],
//                     ['yellow', '/*/*/*/*[S]:*'],
//                     ['tan', '/*/* /*/*[H]:*'],
//                     ['magenta', '/*/* /*/*[P]:*']] )
/*
MolDisp (
          selection_parameters = {
                             'select' : 'selection_scheme',
                             'selection_scheme' : 'interesting_residues' },
          style_parameters =  { 'style_mode' : 'SPLINE'  },
          colour_parameters =  {
                         'colour_mode' : 'rules',
                         'user_scheme' : ['c_grey','1df7' ] }
        )

*/

  f.Write ( "\n"
            "Inline (name =  '" );
  f.Write ( ccp4mg_molname );
  f.Write ( "' , data = '''\n" );

  writeSuperposition ( f,s1,s2,SD,c10,c20 );

  f.Write ( "''' )\n" );
  f.shut();

  f.shut();

  return VIEWER_Ok;

}


void nglResID ( mmdb::PResidue r, mmdb::pstr L )  {
mmdb::InsCode ic;
  strcpy ( ic,r->GetInsCode() );
  if (ic[0])  sprintf ( L,"%i^%s",r->GetSeqNum(),ic );
        else  sprintf ( L,"%i",r->GetSeqNum() );
}

void gsmt::Viewer::makeNGLSelection ( mmdb::pstr    & sBase,
                                      mmdb::pstr    & sMatch,
                                      mmdb::cpstr     chID,
                                      mmdb::PPResidue r,
                                      mmdb::ivector   c,
                                      int             nRes,
                                      mmdb::cpstr     baseCol,
                                      mmdb::cpstr     matchCol
                                    )  {
char L[100],L2[100];
int  i,i1,i2;

  mmdb::CreateCopy ( sBase ,"" );
  mmdb::CreateCopy ( sMatch,"" );

  i2 = 0;
  i  = 0;
  while (i<nRes)  {

    while (i<nRes)
      if (c[i]<0) i++;
             else break;
    i1 = i;

    if (i1>i2)  {

      if (!sBase[0])  {
        sprintf ( L,"[ '%s', ':%s and (",baseCol,chID );
        mmdb::CreateCopy ( sBase,L );
      } else
        mmdb::CreateConcat ( sBase," or " );

      nglResID ( r[i2],L );
      if (i1==i2+1)
        mmdb::CreateConcat ( sBase,L );
      else  {
        nglResID ( r[i1-1],L2 );
        mmdb::CreateConcat ( sBase,L,"-",L2 );
      }

    }

    while (i<nRes)
      if (c[i]>=0) i++;
              else break;
    i2 = i;

    if (i1<i2)  {
      if (!sMatch[0])  {
        sprintf ( L,"[ '%s', ':%s and (",matchCol,chID );
        mmdb::CreateCopy ( sMatch,L );
      } else
        mmdb::CreateConcat ( sMatch," or " );

      nglResID ( r[i1],L );
      if (i2==i1+1)
        mmdb::CreateConcat ( sMatch,L );
      else  {
        nglResID ( r[i2-1],L2 );
        mmdb::CreateConcat ( sMatch,L,"-",L2 );
      }

    }
  
  }

  if (sBase [0])  mmdb::CreateConcat ( sBase ,")' ]" );
  if (sMatch[0])  mmdb::CreateConcat ( sMatch,")' ]" );

}


gsmt::VIEWER_CODE gsmt::Viewer::prepare_ngl ( PStructure     s1,
                                              PStructure     s2,
                                              PSuperposition SD,
                                              mmdb::ivector  c10,
                                              mmdb::ivector  c20 ) {
mmdb::io::File  f;
mmdb::PPResidue r;
mmdb::pstr      sBase;
mmdb::pstr      sMatch;
int             nRes;
bool            sep;

  f.assign ( ngl_script,true );
  if (!f.rewrite())
    return VIEWER_cantWriteFile;

  f.Write (
    "windowTitle         = 'No Title';\n"
    "initialStyle        = '" );
  f.Write ( style );
  f.Write ( "';\n"
    "initialBckgColor    = 'black';\n"
    "initialColorScheme  = 'alignment';\n"
    "predefinedColorName = 'alignment';\n"
    "quality             = 'high';\n"
    "colorSelection      = [\n"
  );

  sBase  = NULL;
  sMatch = NULL;
  s1->getChainRes    ( r,nRes  );
  makeNGLSelection   ( sBase,sMatch,"A",r,SD->c1,nRes,
                       "lightgray","cyan" );
  sep = false;
  if (sBase[0])  {
    f.Write ( sBase );
    sep = true;
  }
  if (sMatch[0])  {
    if (sep)
      f.Write ( ",\n" );
    f.Write ( sMatch );
    sep = true;
  }

  s2->getChainRes    ( r,nRes  );
  makeNGLSelection   ( sBase,sMatch,"B",r,SD->c2,nRes,
                       "dimgray","darkcyan" );
  if (sBase[0])  {
    if (sep)
      f.Write ( ",\n" );
    f.Write ( sBase );
    sep = true;
  }
  if (sMatch[0])  {
    if (sep)
      f.Write ( ",\n" );
    f.Write ( sMatch );
  }
  f.Write ( "\n];\n" );
  f.shut();
  
  delete[] sBase;
  delete[] sMatch;

  f.assign ( structure_file,true );
  if (!f.rewrite())
    return VIEWER_cantWriteFile;

  writeSuperposition ( f,s1,s2,SD,c10,c20 );

  f.shut();

  return VIEWER_Ok;

}


void gsmt::Viewer::writeSuperposition ( mmdb::io::RFile f,
                                        PStructure     s1,
                                        PStructure     s2,
                                        PSuperposition SD,
                                        mmdb::ivector c10,
                                        mmdb::ivector c20 )  {
mmdb::PPResidue Res;
mmdb::PPAtom    A;
mmdb::ChainID   chID,chID0;
mmdb::realtype  x,y,z;
int             i,j,colour,sN, nRes,nAtoms;

  strcpy ( chID,"A" );

  s1->getChainRes ( Res,nRes );

  for (i=0;i<nRes;i++)
    if (Res[i])  {
      strcpy ( chID0,Res[i]->GetChainID() );
      Res[i]->SetChainID ( chID );
      if (SD->c1[i]<0)  colour = 1;
                  else  colour = 3;
      if (c10)  {
        if (c10[i]>=0)  colour = 5;
      }
      Res[i]->GetAtomTable ( A,nAtoms );
      for (j=0;j<nAtoms;j++)
        if (A[j])  {
          sN = A[j]->serNum;
          A[j]->serNum = colour;
          A[j]->PDBASCIIDump ( f );
          A[j]->serNum = sN;
        }
      Res[i]->SetChainID ( chID0 );
    }


  strcpy ( chID,"B" );

  s2->getChainRes ( Res,nRes );

  for (i=0;i<nRes;i++)
    if (Res[i])  {
      strcpy ( chID0,Res[i]->GetChainID() );
      Res[i]->SetChainID ( chID );
      if (SD->c2[i]<0)  colour = 2;
                  else  colour = 4;
      if (c20)  {
        if (c20[i]>=0)  colour = 6;
      }
      Res[i]->GetAtomTable ( A,nAtoms );
      for (j=0;j<nAtoms;j++)
        if (A[j])  {
          sN = A[j]->serNum;
          A[j]->serNum = colour;
          x = A[j]->x;
          y = A[j]->y;
          z = A[j]->z;
          A[j]->Transform ( SD->T );
          A[j]->PDBASCIIDump ( f );
          A[j]->x = x;
          A[j]->y = y;
          A[j]->z = z;
          A[j]->serNum = sN;
        }
      Res[i]->SetChainID ( chID0 );
    }

}



void printViewerOptTemplate ( mmdb::cpstr argv0 )  {
  printf ( "\nEdit this file and copy it to %s.opt\n\n"
           "--------------------- file begins  ------------------\n"
           "RASMOL_PATH    /Programs/Rasmol/rasmol\n"
           "JMOL_PATH      /Programs/Jmol/jmol\n"
           "CCP4MG_PATH    /Programs/ccp4/bin/ccp4mg\n"
           "NGL_PATH       /Programs/ngl/nglviewer\n"
           "VIEWER         JMol\n"
           "STYLE          backbone"
           "IN_BACKGROUND  Yes\n"
           "--------------------- file ends  --------------------\n",
           argv0 );
}


void readViewerSettings ( gsmt::PViewer Viewer, mmdb::cpstr argv0 )  {
mmdb::io::File f;
mmdb::pstr     optFName,p;
char           S[200];

  optFName = NULL;
  mmdb::CreateCopy ( optFName,argv0 );
  p = mmdb::LastOccurence ( optFName,'/' );
  if (!p)
    p = mmdb::FirstOccurence ( optFName,'\\' );  // Windows?

  if (p)  {
    p[1] = char(0);
    mmdb::CreateConcat ( optFName,"gesamt.opt" );
  } else
    mmdb::CreateCopy ( optFName,"gesamt.opt" );

  f.assign ( optFName,true );
  if (f.reset(true))  {
    while (!f.FileEnd())  {
      f.ReadLine ( S,sizeof(S) );
      mmdb::CutSpaces ( S,mmdb::SCUTKEY_BEGEND );
      p = mmdb::FirstOccurence ( S,' ' );
      if (p)  {
        p[0] = char(0);
        p++;
        while (*p==' ')  p++;
        if (!strcasecmp(S,"RASMOL_PATH"))
          Viewer->setRasmolPath ( p );
        else if (!strcasecmp(S,"JMOL_PATH"))
          Viewer->setJMolPath ( p );
        else if (!strcasecmp(S,"CCP4MG_PATH"))
          Viewer->setCCP4MGPath ( p );
        else if (!strcasecmp(S,"NGL_PATH"))
          Viewer->setNGLPath ( p );
        else if (!strcasecmp(S,"VIEWER"))  {
          if (!strcasecmp(p,"JMol"))
            Viewer->setViewerType ( gsmt::VIEWER_Jmol   );
          else if (!strcasecmp(p,"Rasmol"))
            Viewer->setViewerType ( gsmt::VIEWER_Rasmol );
          else if (!strcasecmp(p,"CCP4MG"))
            Viewer->setViewerType ( gsmt::VIEWER_CCP4MG );
          else if (!strcasecmp(p,"NGL"))
            Viewer->setViewerType ( gsmt::VIEWER_NGL    );
        } else if (!strcasecmp(S,"IN_BACKGROUND"))  {
          if (!strcasecmp(p,"Yes"))
            Viewer->setBackgroundMode ( true );
          else if (!strcasecmp(p,"No"))
            Viewer->setBackgroundMode ( false );
        } else if (!strcasecmp(S,"STYLE"))
          Viewer->setStyle ( p );
      }
    }
    f.shut();
  }

  if (optFName)  delete[] optFName;

}
