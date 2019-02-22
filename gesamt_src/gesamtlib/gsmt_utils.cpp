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
//    14.06.16   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  GSMT_Utils <interface>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2008-2016
//
// =================================================================
//

#include <string.h>
#include <errno.h>

#include "gsmt_utils.h"

// =================================================================

void gsmt::getHours ( mmdb::realtype secs,
                      int & ihour, int & imin, int & isec )  {
  ihour = int(secs/3600.0);
  imin  = int((secs-ihour*3600)/60);
  isec  = int(secs-ihour*3600-imin*60);
}

void gsmt::getOutFilePath ( mmdb::pstr  & fpath,
                            mmdb::cpstr   orgFName,
                            mmdb::cpstr   outFName,
                            int           serNo,
                            int           out_key )  {
mmdb::pstr fname,p;
char       fext[100],suffix[100];
int        n;

  fname = NULL;
  sprintf ( suffix,"_%i",serNo );
  if (out_key==2)  {

    if (outFName)  mmdb::CreateCopy ( fname,outFName );
             else  mmdb::CreateCopy ( fname,orgFName );
    strcpy ( fext,mmdb::io::GetFExt(fname) );
    p = mmdb::LastOccurence ( fname,'.' );
    if (p)  *p = char(0);
    mmdb::CreateCopCat ( fpath,fname,suffix,fext );

  } else  {
    
    mmdb::CreateCopy ( fpath,outFName );
    n = strlen(fpath)-1;
    if ((fpath[n]!='/') && (fpath[n]!='\\'))
      mmdb::CreateConcat ( fpath,mmdb::io::_dir_sep );
    mmdb::CreateCopy ( fname,
                  mmdb::io::GetFName(orgFName,mmdb::io::syskey_all) );
    strcpy ( fext,mmdb::io::GetFExt(fname) );
    p = mmdb::LastOccurence ( fname,'.' );
    if (p)  *p = char(0);
    mmdb::CreateConcat ( fpath,fname,suffix,fext );
    
  }
  
  if (fname)  delete[] fname;
  
}

bool gsmt::writeSuperposed ( const PPStructure    s,
                             const mmdb::mat44   *T,
                             int                  nStruct,
                             mmdb::cpstr          outFName,
                             mmdb::SELECTION_TYPE selType,
                             int                  out_multi )  {
mmdb::PManager M0,M;
mmdb::PPModel  models;
mmdb::pstr     fpath;
char           note[200];
int            nModels,nm,n,i;

  if (!out_multi)  return false;
  if (nStruct<=0)  return false;
  if ((out_multi==1) && (!outFName))  return false;
  
  if (out_multi==1)  {

    M0 = s[0]->getSelectedStructure ( selType );
    if (!M0)  return false;
    M0->ApplyTransform ( T[0] );
    
    nm = M0->GetNumberOfModels();
    if (mmdb::isMat4Unit(T[0],1.0e-5,false))
          sprintf ( note,"models 1-%i, unchanged" ,nm );
    else  sprintf ( note,"models 1-%i, superposed",nm );
    s[0]->setOutFile ( outFName,note );
    
    for (n=1;n<nStruct;n++)  {
      M = s[n]->getSelectedStructure ( selType );
      if (M)  {
        M->ApplyTransform ( T[n] ); 
        M->GetModelTable ( models,nModels );
        for (i=0;i<nModels;i++)
          M0->AddModel ( models[i] );
        if (mmdb::isMat4Unit(T[n],1.0e-5,false))
             sprintf ( note,"models %i-%i, unchanged" ,nm+1,nm+nModels );
        else sprintf ( note,"models %i-%i, superposed",nm+1,nm+nModels );
        s[n]->setOutFile ( outFName,note );
        nm += nModels;
        delete M;
      }
    }
    
    if (!strcasecmp(mmdb::io::GetFExt(outFName),".cif"))
          M0->WriteCIFASCII ( outFName );
    else  M0->WritePDBASCII ( outFName );
    
    delete M0;
    
  } else  {
    
    fpath = NULL;
    
    for (n=0;n<nStruct;n++)  {
      M = s[n]->getSelectedStructure ( selType );  
      M->ApplyTransform ( T[n] );
      getOutFilePath ( fpath,s[n]->getFilePath(),outFName,n+1,out_multi );
      if (!strcasecmp(mmdb::io::GetFExt(fpath),".cif"))
            M->WriteCIFASCII ( fpath );
      else  M->WritePDBASCII ( fpath );
      if (mmdb::isMat4Unit(T[n],1.0e-5,false))
            s[n]->setOutFile ( fpath,"original position" );
      else  s[n]->setOutFile ( fpath,"superposed" );
      delete M;
    }
    
    if (fpath) delete[] fpath;
  
  }

  return true;
    
}


#ifdef  _WIN32
bool gsmt::mkdirp ( mmdb::cpstr path )  {
#else
bool gsmt::mkdirp ( mmdb::cpstr path, mode_t mode )  {
#endif
// const cast for hack
mmdb::pstr p = const_cast<char*>(path);

  // Do mkdir for each slash until end of string or error
  while (*p!='\0') {
    // Skip first character
    p++;

    // Find first slash or end
    while ((*p!='\0') && (*p!='/'))
      p++;

    // Remember value from p
    char v = *p;

    // Write end of string at p
    *p = '\0';

    // Create folder from path to '\0' inserted at p
#ifdef  _WIN32
    if ((mkdir(path)==-1) && (errno!=EEXIST))  {
#else
    if ((mkdir(path,mode)==-1) && (errno!=EEXIST))  {
#endif
      *p = v;
      return false;
    }

    // Restore path to it's former glory
    *p = v;

  }

  return true;
}
