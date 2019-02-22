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
//    14.05.16   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  GSMT_Input <implementation>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::Input
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2008-2016
//
// =================================================================
//

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifdef  _WIN32
# include <windows.h>
#endif

#include "mmdb2/mmdb_io_file.h"
#include "gesamt_input.h"
#include "gesamtlib/gsmt_archive.h"

// =================================================================

gsmt::Input::Input()  {
  init();
}

gsmt::Input::~Input()  {
  freeMemory();
}


int getNofSystemThreads()  {
#ifdef  _WIN32
SYSTEM_INFO sysinfo;
  GetSystemInfo ( &sysinfo );
  return sysinfo.dwNumberOfProcessors;
#else
  return sysconf ( _SC_NPROCESSORS_CONF );
#endif  
}


void gsmt::Input::init()  {
  taskCode     = TASK_None;           // task code
  fstruct      = NULL;                // structure file namess
  sel          = NULL;                // selection strings per structure
  scopSel      = NULL;                // scop/mmdb selection switches
  nStruct      = 0;                   // number of structures
  outFile      = NULL;                // output file
  jsonOutFile  = NULL;                // json-formatted output file
  rvapiRDir    = NULL;                // rvapi report directory
  rvapiJSUri   = NULL;                // rvapi JS Uri
  rvapiRDoc    = NULL;                // rvapi report document
  outSeqFile   = NULL;                // output sequence file
  inpSeqFile   = NULL;                // input sequence file
  csvFile      = NULL;                // output CSV file
  pdbDir       = NULL;                // PDB directory to screen
  archDir      = NULL;                // archive directory to screen
  inclFile     = NULL;                // file with inclusive entries
  exclFile     = NULL;                // file with exclusive entries
  QR0          = QR0_default;         // parameter of Q-score
  Qthresh      = -1.0;                // Q-threshold for sheafs
  sigma        = sigma_default;       // superposition sigma
  minMatch1    = 0.7;                 // minimal match fraction #1 (query) 
  minMatch2    = 0.7;                 // minimal match fraction #1 (target)
  trimSize     = 0.0;                 // size-based trim factor for scan results
  trimQ        = 0.0;                 // Q-based trim factor for scan results
  mode         = PERFORMANCE_Efficient; // efficiency mode
  sheafMode    = SHEAF_None;          // no sheaf by default
  subSeqLen    = 50;                  // subseq-ce length for model making
  minSeqMatch  = 0.2;                 // min seq. score for model making
  nthreads     = 1;                   // number of threads
  npacks       = 16*getNofSystemThreads();  // number of packs in archive
  verbosity    = 1;                   // verbosity level
  compressArch = true;                // making compressed gesamt archive
  out_multi    = 0;                   // output multi-files/single file
  out_units    = mmdb::STYPE_RESIDUE; // output selected residues/chains/models
  viewer       = NULL;                // viewer handler
  viewerData   = 0;                   // viewer data modifier
  IC           = INPUT_Ok;
  jsonCompact  = false;               // key to make compact json output

}

void gsmt::Input::freeMemory()  {

  for (int i=0;i<nStruct;i++)  {
    if (fstruct[i])  delete[] fstruct[i];
    if (sel    [i])  delete[] sel    [i];
  }

  if (fstruct)     delete[] fstruct;
  if (sel)         delete[] sel;
  if (scopSel)     delete[] scopSel;
  if (outFile)     delete[] outFile;
  if (jsonOutFile) delete[] jsonOutFile;
  if (rvapiRDir)   delete[] rvapiRDir;
  if (rvapiJSUri)  delete[] rvapiJSUri;
  if (rvapiRDoc)   delete[] rvapiRDoc;
  if (outSeqFile)  delete[] outSeqFile;
  if (inpSeqFile)  delete[] inpSeqFile;
  if (csvFile)     delete[] csvFile;
  if (pdbDir)      delete[] pdbDir;  
  if (archDir)     delete[] archDir;
  if (inclFile)    delete[] inclFile;
  if (exclFile)    delete[] exclFile;
  if (viewer)      delete viewer;
  
  init();

}


void gsmt::Input::addFName ( mmdb::cpstr fname )  {
mmdb::psvector fs,s;
mmdb::ovector  scop;
int            n = nStruct;

  nStruct++;
  fs   = new mmdb::pstr[nStruct];
  s    = new mmdb::pstr[nStruct];
  scop = new bool[nStruct];
  for (int i=0;i<n;i++)  {
    fs  [i] = fstruct[i];
    s   [i] = sel    [i];
    scop[i] = scopSel[i];
  }
  delete[] fstruct;
  delete[] sel;
  delete[] scopSel;

  fstruct    = fs;
  sel        = s;
  scopSel    = scop;
  fstruct[n] = NULL;
  sel    [n] = NULL;
  scopSel[n] = false;
  mmdb::CreateCopy ( fstruct[n],fname );
  mmdb::CreateCopy ( sel    [n],"*"   );

}

mmdb::realtype gsmt::Input::getReal ( mmdb::cpstr    argv,
                                      mmdb::cpstr    key,
                                      mmdb::realtype minValid,
                                      mmdb::realtype maxValid,
                                      INPUT_CODE     wrongFormat,
                                      INPUT_CODE     wrongValue )  {
mmdb::realtype  V;
char           *err_ptr;
int             m;
  m = strlen(key);
  V = strtod ( &(argv[m]),&err_ptr );
  if ((V==0.0) && (err_ptr==&(argv[m])))
    IC = wrongFormat;
  else if (((minValid>-mmdb::MaxReal) && (V<minValid)) ||
           ((maxValid<mmdb::MaxReal) && (V>maxValid)))
    IC = wrongValue;
  return V;
}


gsmt::INPUT_CODE gsmt::Input::parseCommandLine (
                                           int argc, char ** argv )  {
int   argNo,argc1;
bool  done;
  
  freeMemory();
  
  mmdb::CreateCopy ( rvapiJSUri,"jsrview" );
  
  taskCode = TASK_None;

  argNo = 1;
  argc1 = argc - 1;
  done  = false;
  while ((argNo<argc) && (!done))  {

    if (argv[argNo][0]!='-')  {
      // starts input file name possibly followed by selection string
      addFName ( argv[argNo++] );
      if (argNo>=argc)  {
        done = true;
      } else if (!strcasecmp(argv[argNo],"-s"))  {
        scopSel[nStruct-1] = false;
        argNo++;
        if (argNo<argc)  {
          mmdb::CreateCopy ( sel[nStruct-1],argv[argNo++] );
          if (!strcasecmp(sel[nStruct-1],"(all)"))
            strcpy ( sel[nStruct-1],"*" );
        }
      } else if (!strcasecmp(argv[argNo],"-d"))  {
        scopSel[nStruct-1] = true;
        argNo++;
        if (argNo<argc)
          mmdb::CreateCopy ( sel[nStruct-1],argv[argNo++] );
      } else if (argv[argNo][0]=='-')  {
        done = true;  // end of input of structures
      }
    } else
      done = true;  // end of input of structures

  }
  
  if (argNo<argc1)  {
    if (!strcasecmp(argv[argNo],"-input-list"))  {
      mmdb::io::File f;
      f.assign ( argv[++argNo],true,false );
      if (f.reset())  {
        char S[1000];
        while ((!f.FileEnd()) && (IC==INPUT_Ok))  {
          f.ReadLine ( S,sizeof(S)-1 );
          mmdb::CutSpaces ( S,mmdb::SCUTKEY_BEGEND ); 
          if (S[0] && (S[0]!='#'))  {
            mmdb::pstr p1,p2;
            if (S[0]=='"')  {
              p1 = &(S[1]);
              p2 = mmdb::FirstOccurence ( p1,'"' );
            } else  {
              p1 = &(S[0]);
              p2 = mmdb::FirstOccurence ( p1,' ' );
            }
            if (p2)  *p2 = char(0);
            addFName ( p1 );
            if (p2)  {
              p1 = p2 + 1;
              if (*p1)  {
                p2 = mmdb::FirstOccurence ( p1,strlen(p1),"-s ",3 );
                if (p2)
                  scopSel[nStruct-1] = false;
                else  {
                  p2 = mmdb::FirstOccurence ( p1,strlen(p1),"-d ",3 );
                  if (p2)
                    scopSel[nStruct-1] = true;
                }
                if (p2)  {
                  p1 = p2 + 3;
                  while (*p1==' ')  p1++;
                  if (*p1)  {
                    mmdb::CreateCopy ( sel[nStruct-1],p1 );
                    if (!strcasecmp(sel[nStruct-1],"(all)"))
                      strcpy ( sel[nStruct-1],"*" );
                  } else
                    IC = INPUT_InputListError;
                }
              }
            }
          }
        }
        f.shut();
      } else
        IC = INPUT_WrongCommandLine;
      argNo++;
    }
  }
  
  if (nStruct==2)      taskCode = TASK_PairwiseAlignment;
  else if (nStruct>2)  taskCode = TASK_MultipleAlignment;

  while (argNo<argc)  {

    if ((argNo<argc1) && (!strcasecmp(argv[argNo],"-pdb")))  {
      gsmt::copyDirPath ( argv[++argNo],pdbDir );
      if (nStruct==1)
        taskCode = TASK_PDBScan;
    } else if ((!strcasecmp(argv[argNo],"-archive")) &&
               (argNo<argc1)) {
      gsmt::copyDirPath ( argv[++argNo],archDir );
      if (nStruct==1)
        taskCode = TASK_ArchiveStructScan;
    } else if ((argNo<argc1) &&
               (!strcasecmp(argv[argNo],"--make-archive")))  {
      if (taskCode!=TASK_None)
        IC = INPUT_WrongCommandLine;
      else  {
        gsmt::copyDirPath ( argv[++argNo],archDir );
        taskCode = TASK_MakeArchive;
      }
    } else if ((argNo<argc1) &&
               (!strcasecmp(argv[argNo],"--scan-sequence")))  {
      mmdb::CreateCopy ( inpSeqFile,argv[++argNo] );
      if (taskCode!=TASK_None)
           IC  = INPUT_WrongCommandLine;
      else taskCode = TASK_ArchiveSeqScan;
    } else if ((argNo<argc1) &&
               (!strcasecmp(argv[argNo],"--make-model")))  {
      mmdb::CreateCopy ( inpSeqFile,argv[++argNo] );
      if (taskCode!=TASK_None)
           IC  = INPUT_WrongCommandLine;
      else taskCode = TASK_MakeModel;
    } else if ((argNo<argc1) &&
               (!strcasecmp(argv[argNo],"--update-archive")))  {
      if (taskCode!=TASK_None)
        IC = INPUT_WrongCommandLine;
      else  {
        gsmt::copyDirPath ( argv[++argNo],archDir );
        taskCode = TASK_UpdateArchive;
      }
    } else if (!strncmp(argv[argNo],"-view",5))  {
      if (!viewer)
        viewer = new gsmt::Viewer();
      readViewerSettings ( viewer,argv[0] );
      if (strlen(argv[argNo])>5)
        viewerData = atoi ( &(argv[argNo][5]) );
    } else if (!strcmp(argv[argNo],"-high"))  {
      mode = gsmt::PERFORMANCE_High;
    } else if (!strcmp(argv[argNo],"-normal"))  {
      mode = gsmt::PERFORMANCE_Efficient;
    } else if (!strcmp(argv[argNo],"-sheaf"))  {
      sheafMode = gsmt::SHEAF_Auto;
      if ((taskCode!=TASK_PairwiseAlignment) &&
          (taskCode!=TASK_MultipleAlignment))
            IC = INPUT_WrongCommandLine;
      else  taskCode  = TASK_SheafAlignment;
    } else if (!strcmp(argv[argNo],"-sheaf-atoms"))  {
      sheafMode = gsmt::SHEAF_Atoms;
      if ((taskCode!=TASK_PairwiseAlignment) &&
          (taskCode!=TASK_MultipleAlignment))
            IC = INPUT_WrongCommandLine;
      else  taskCode = TASK_SheafAlignment;
    } else if (!strcmp(argv[argNo],"-sheaf-chains"))  {
      sheafMode = gsmt::SHEAF_Chains;
      if ((taskCode!=TASK_PairwiseAlignment) &&
          (taskCode!=TASK_MultipleAlignment))
            IC = INPUT_WrongCommandLine;
      else  taskCode = TASK_SheafAlignment;
    } else if (!strcmp(argv[argNo],"-sheaf-all"))  {
      sheafMode = gsmt::SHEAF_All;
      if ((taskCode!=TASK_PairwiseAlignment) &&
          (taskCode!=TASK_MultipleAlignment))
            IC = INPUT_WrongCommandLine;
      else  taskCode = TASK_SheafAlignment;
    } else if (!strcmp(argv[argNo],"-sheaf-x"))  {
      sheafMode = gsmt::SHEAF_X;
      if ((taskCode!=TASK_PairwiseAlignment) &&
          (taskCode!=TASK_MultipleAlignment))
            IC = INPUT_WrongCommandLine;
      else  taskCode = TASK_SheafAlignment;
    } else if (!strcmp(argv[argNo],"-domains"))  {
      if ((taskCode!=TASK_PairwiseAlignment) &&
          (taskCode!=TASK_MultipleAlignment))
            IC = INPUT_WrongCommandLine;
      else  taskCode = TASK_Domains;
    } else if (!strncmp(argv[argNo],"-r0=",4))  {
      QR0 = getReal ( argv[argNo],"-r0=",
                      mmdb::MinReal,mmdb::MaxReal,
                      INPUT_WrongR0Format,INPUT_WrongR0Value );
    } else if (!strncmp(argv[argNo],"-sheaf-Q=",9))  {
      Qthresh = getReal ( argv[argNo],"-sheaf-Q=", -1.0,1.0,
                      INPUT_WrongQThreshFormat,INPUT_WrongQThreshValue );
    } else if (!strncmp(argv[argNo],"-sigma=",7))  {
      sigma = getReal ( argv[argNo],"-sigma=",
                        mmdb::MinReal,mmdb::MaxReal,
                        INPUT_WrongSigmaFormat,INPUT_WrongSigmaValue );
    } else if (!strncmp(argv[argNo],"-min1=",6))  {
      minMatch1 = getReal ( argv[argNo],"-min1=",0.0,1.0,
                            INPUT_WrongMin1Format,
                            INPUT_WrongMin1Value );
    } else if (!strncmp(argv[argNo],"-min2=",6))  {
      minMatch2 = getReal ( argv[argNo],"-min2=",0.0,1.0,
                            INPUT_WrongMin2Format,
                            INPUT_WrongMin2Value );
    } else if (!strncmp(argv[argNo],"-trim-size=",11))  {
      trimSize = getReal ( argv[argNo],"-trim-size=",0.0,1.0,
                             INPUT_WrongTrimSizeFormat,
                             INPUT_WrongTrimSizeValue );
    } else if (!strncmp(argv[argNo],"-trim-Q=",8))  {
      trimQ = getReal ( argv[argNo],"-trim-Q=",0.0,1.0,
                             INPUT_WrongTrimQFormat,
                             INPUT_WrongTrimQValue );
    } else if ((argNo<argc1) && (!strcmp(argv[argNo],"-incl-list")))  {
      mmdb::CreateCopy ( inclFile,argv[++argNo] );
    } else if ((argNo<argc1) && (!strcmp(argv[argNo],"-excl-list")))  {
      mmdb::CreateCopy ( exclFile,argv[++argNo] );
    } else if (!strcmp(argv[argNo],"-nthreads=auto"))  {
      nthreads = getNofSystemThreads();
    } else if (!strncmp(argv[argNo],"-nthreads=",10))  {
      nthreads = atoi ( &argv[argNo][10] );
      if (nthreads<=0)
        IC = INPUT_WrongNThreads;
    } else if (!strcmp(argv[argNo],"-npacks=auto"))  {
      npacks = 16*getNofSystemThreads();
    } else if (!strncmp(argv[argNo],"-npacks=",8))  {
      npacks = atoi ( &argv[argNo][8] );
      if (npacks<=0)
        IC = INPUT_WrongNPacks;
    } else if (!strncmp(argv[argNo],"-compression=",13))  {
      compressArch = (atoi(&argv[argNo][13])!=0);
    } else if (!strcmp(argv[argNo],"-v0"))  {
      verbosity = 0;
    } else if (!strcmp(argv[argNo],"-v1"))  {
      verbosity = 1;
    } else if (!strcmp(argv[argNo],"-v2"))  {
      verbosity = 2;
    } else if ((argNo<argc1) && (!strcmp(argv[argNo],"-o")))  {
      mmdb::CreateCopy ( outFile,argv[++argNo] );
      if (!out_multi)
        out_multi = 1;  // single file output
    } else if ((argNo<argc1) && (!strcmp(argv[argNo],"--json")))  {
      mmdb::CreateCopy ( jsonOutFile,argv[++argNo] );
      jsonCompact = false;
    } else if ((argNo<argc1) && (!strcmp(argv[argNo],"--json-compact")))  {
      mmdb::CreateCopy ( jsonOutFile,argv[++argNo] );
      jsonCompact = true;
    } else if ((argNo<argc1) && (!strcmp(argv[argNo],"--rvapi-rdir")))  {
      mmdb::CreateCopy ( rvapiRDir,argv[++argNo] );
      if (rvapiRDoc)  {
        delete[] rvapiRDoc;
        rvapiRDoc = NULL;
      }
    } else if ((argNo<argc1) && (!strcmp(argv[argNo],"--rvapi-jsuri")))  {
      mmdb::CreateCopy ( rvapiJSUri,argv[++argNo] );
    } else if ((argNo<argc1) && (!strcmp(argv[argNo],"--rvapi-rdoc")))  {
      mmdb::CreateCopy ( rvapiRDoc,argv[++argNo] );
      if (rvapiRDir)  {
        delete[] rvapiRDir;
        rvapiRDir = NULL;
      }
    } else if (!strncmp(argv[argNo],"-o-",3))  {
      if (mmdb::FirstOccurence(argv[argNo],'a'))
          out_units = mmdb::STYPE_ATOM;
      else if (mmdb::FirstOccurence(argv[argNo],'r'))
          out_units = mmdb::STYPE_RESIDUE;
      else if (mmdb::FirstOccurence(argv[argNo],'c'))
          out_units = mmdb::STYPE_CHAIN;
      else if (mmdb::FirstOccurence(argv[argNo],'m'))
          out_units = mmdb::STYPE_MODEL;
      else if (mmdb::FirstOccurence(argv[argNo],'*'))
          out_units = mmdb::STYPE_UNDEFINED;
      if (mmdb::FirstOccurence(argv[argNo],'s'))
          out_multi = 1;
      else if (mmdb::FirstOccurence(argv[argNo],'f'))
          out_multi = 2;
      else if (mmdb::FirstOccurence(argv[argNo],'d'))
          out_multi = 3;
      if (!out_multi)
        out_multi = 1;  // single file output
    } else if ((argNo<argc1) && (!strcmp(argv[argNo],"-a")))  {
      mmdb::CreateCopy ( outSeqFile,argv[++argNo] );
    } else if ((argNo<argc1) && (!strcmp(argv[argNo],"-csv")))  {
      mmdb::CreateCopy ( csvFile,argv[++argNo] );
    } else
      IC = INPUT_UnknownKeyword;
    argNo++;
  }
  
  if (inpSeqFile && (!archDir))
    return IC = INPUT_WrongCommandLine;

  if (pdbDir && archDir && (nStruct>0))
    return IC = INPUT_WrongCommandLine;

  if ((pdbDir || archDir) && (nStruct>1))
    return IC = INPUT_WrongCommandLine;
  
  if ((!pdbDir) &&  (!archDir) && (nStruct<2))
    return IC = INPUT_WrongCommandLine;

  if ((out_multi==1) && (!outFile))
    return IC = INPUT_NoOutputFile;

  if ((out_multi==3) && (!outFile))
    return IC = INPUT_NoOutputDirectory;
    
  return IC;

}


mmdb::cpstr gsmt::Input::errMessage ( INPUT_CODE icode )  {
  switch (icode)  {
    case INPUT_Ok:
      return "Ok";
    case INPUT_CantOpenInputList:
      return "cannot open input list file";
    case INPUT_WrongCommandLine:
      return "wrong or ambiguous command line";
    case INPUT_UnknownKeyword:
      return "unknown keyword encountered";
    case INPUT_WrongR0Value:
      return "r0 must be greater than zero";
    case INPUT_WrongR0Format:
      return "r0 must be a valid positive real number, no spaces";
    case INPUT_WrongQThreshValue:
      return "Q must be withing -1.0 and 1.0";
    case INPUT_WrongQThreshFormat:
      return "Q must be a valid real number, no spaces";
    case INPUT_WrongSigmaValue:
      return "sigma must be greater than zero";
    case INPUT_WrongSigmaFormat:
      return "sigma must be a valid positive real number, no spaces";
    case INPUT_WrongMin1Value:
      return "min1 must be within the range of [0..1]";
    case INPUT_WrongMin1Format:
      return "min1 must be a valid real number within the range of "
             "[0..1], no spaces";
    case INPUT_WrongMin2Value:
      return "min2 must be within the range of [0..1]";
    case INPUT_WrongMin2Format:
      return "min2 must be a valid real number within the range of  "
             "[0..1], no spaces";
    case INPUT_WrongTrimQValue:
      return "trim-Q must be within the range of [0..1]";
    case INPUT_WrongTrimQFormat:
      return "trim-Q must be a valid real number within the range of  "
             "[0..1], no spaces";
    case INPUT_WrongTrimSizeValue:
      return "trim-size must be within the range of [0..1]";
    case INPUT_WrongTrimSizeFormat:
      return "trim-size must be a valid real number within the range of  "
             "[0..1], no spaces";
    case INPUT_WrongNThreads:
      return "nthreads must be a valid positive integer number or "
             "'auto', no spaces";
    case INPUT_WrongNPacks:
      return "npacks must be a valid positive integer number or "
             "'auto', no spaces";
    case INPUT_NoOutputFile:
      return "'-o-s' option is not complemented by option '-o' with "
             "output file path";
    case INPUT_NoOutputDirectory:
      return "'-o-d' option is not complemented by option '-o' with "
             "path to output directory";
    default:
      return "unknown input code";
  }
}

