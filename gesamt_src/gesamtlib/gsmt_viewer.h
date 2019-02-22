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
//  **** Module  :  GSMT_Viewer <interface>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::Viewer
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2008-2015
//
// =================================================================
//

#ifndef __GESAMT_Viewer__
#define __GESAMT_Viewer__

#include "gsmt_structure.h"
#include "gsmt_superposition.h"


// =================================================================

namespace gsmt {

  enum VIEWER_KEY  {
    VIEWER_Rasmol,
    VIEWER_Jmol,
    VIEWER_CCP4MG,
    VIEWER_NGL
  };

  enum VIEWER_CODE  {
    VIEWER_Ok,
    VIEWER_cantWriteFile
  };

  DefineClass(Viewer);

  class Viewer  {

    public :

      Viewer ();
      ~Viewer();

      void setJMolPath       ( mmdb::cpstr   jmol_path );
      void setRasmolPath     ( mmdb::cpstr rasmol_path );
      void setCCP4MGPath     ( mmdb::cpstr ccp4mg_path );
      void setNGLPath        ( mmdb::cpstr    ngl_path );
      void setViewerType     ( VIEWER_KEY  viewer_type );
      void setBackgroundMode ( bool      in_background );
      void setStyle          ( mmdb::cpstr start_style );

      VIEWER_CODE View       ( PStructure     s1,
                               PStructure     s2,
                               PSuperposition SD,
                                mmdb::ivector c10,
                                mmdb::ivector c20 );

      VIEWER_CODE Download   ( PStructure     s1,
                               PStructure     s2,
                               PSuperposition SD,
                               mmdb::cpstr    fileName );

      void InitClass ();
      void FreeMemory();

    protected :
      mmdb::pstr rasmol;  //!< path to rasmol
      mmdb::pstr jmol;    //!< path to jmol
      mmdb::pstr ccp4mg;  //!< path to ccp4mg
      mmdb::pstr ngl;     //!< path to ngl
      mmdb::pstr bckg;    //!< ampersend for launching in background
      mmdb::pstr style;   //!< initial style (cartoon, backbone etc.)
      VIEWER_KEY viewer;

      void launchViewer         ();
      void makeNGLSelection     ( mmdb::pstr    & sBase,
                                  mmdb::pstr    & sMatch,
                                  mmdb::cpstr     chID,
                                  mmdb::PPResidue r,
                                  mmdb::ivector   c,
                                  int             nRes,
                                  mmdb::cpstr     baseCol,
                                  mmdb::cpstr     matchCol
                                );
      void writeSuperposition   ( mmdb::io::RFile  f,
                                  PStructure      s1,
                                  PStructure      s2,
                                  PSuperposition  SD,
                                  mmdb::ivector  c10,
                                  mmdb::ivector  c20 );
     VIEWER_CODE prepare_rasmol ( PStructure     s1,
                                  PStructure     s2,
                                  PSuperposition SD,
                                  mmdb::ivector  c10,
                                  mmdb::ivector  c20 );
     VIEWER_CODE prepare_ccp4mg ( PStructure     s1,
                                  PStructure     s2,
                                  PSuperposition SD,
                                  mmdb::ivector  c10,
                                  mmdb::ivector  c20 );
     VIEWER_CODE prepare_ngl    ( PStructure     s1,
                                  PStructure     s2,
                                  PSuperposition SD,
                                  mmdb::ivector  c10,
                                  mmdb::ivector  c20 );

  };

}

extern void printViewerOptTemplate ( mmdb::cpstr argv0 );
extern void readViewerSettings     ( gsmt::PViewer Viewer,
                                     mmdb::cpstr argv0 );

#endif

