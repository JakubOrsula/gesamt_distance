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
//    25.01.17   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  Gesamt usage <implementation>
//       ~~~~~~~~~
//  **** Project :  GESAMT_Usage
//       ~~~~~~~~~
//  **** Functions: printInstructions
//       ~~~~~~~~~~
//
//  GESAMT: General and Efficient Structural Alignment of
//          Macromoecular Targets
//
//  (C) E. Krissinel, 2008-2017
//
// =================================================================
//

#include <stdio.h>

#include "gesamt_input.h"

// =================================================================


void printInstructions ( char * argv0 )  {
  
  printf (
 "\n"
#ifdef compile_for_ccp4
   "$TEXT:Warning: $$ Wrong or no input $$\n"
#endif
"\n\
 SYNOPSIS:\n\
 ~~~~~~~~~\n\
\n\
 (1) 3D alignment and superposition of N>1 structures:\n\
\n\
     %s {struct1} {struct2} .. {structN} [options] [outputs]\n\
\n\n\
 (2) Find structural homologues in a vanilla PDB archive (slow):\n\
\n\
     %s {struct} -pdb /path-to-pdb-archive [options] [outputs]\n\
\n\n\
 (3) Make GESAMT archive for fast 3D searches:\n\
\n\
     %s --make-archive /path-to-gesamt-archive -pdb /path-to-pdb-archive [options]\n\
\n\n\
 (4) Update GESAMT archive (much faster than making it anew):\n\
\n\
     %s --update-archive /path-to-gesamt-archive -pdb /path-to-pdb-archive [options]\n\
\n\n\
 (5) Find structural homologues in GESAMT archive (fast):\n\
\n\
     %s {struct} -archive /path-to-gesamt-archive [options] [outputs]\n\
\n\n\
 (6) Find sequence homologues in GESAMT archive (fast):\n\
\n\
     %s --scan-sequence {seq} -archive /path-to-gesamt-archive [options] [outputs]\n\
\n\n\
 where curly brackets {} denote a group of parameters with predefined\n\
 order within the group, and square brackets [] denote optional parameters.\n\
 Note that general templates of command lines are predefined, such that,\n\
 e.g., placing [options] in the beginning of command line is not\n\
 possible.\n\
\n\
\n\
 INPUT STRUCTURES\n\
 ~~~~~~~~~~~~~~~~\n\
\n\
 Each {structN} represents one of the following constructs:\n\
\n\
   file_path\n\
   file_path -s CID\n\
   file_path -d SCOP\n\
\n\
 where:\n\
\n\
   file_path  relative or absolute path to file with coordinate data.\n\
              The file should be either in PDB or mmCIF formats (*.pdb,\n\
              *.ent or *.cif), and may be also gzipped. If no selection\n\
              is specified with either of -s or -d keys, all content\n\
              of (possibly multi-chain) file is taken for processing.\n\
              In case of multi-model NMR files, only 1st model is\n\
              taken by default, however, this may be changed with '-s'\n\
              selection (see below).\n\
\n\
   CID        (sub-)structure selection in CCP4 notation (the CCP4\n\
              Coordinate ID, cf. APPENDIX 1).\n\
\n\
   SCOP       (sub-)structure selection in SCOP notation (cf. APPENDIX 2).\n\
\n\
 All structure input may be specified in a separate file, which helps to\n\
 keep the command line short in case of excessively high number of input\n\
 structures. The file should list structure specifications exactly like\n\
 in command line, one structure per line, for example:\n\
\n\
 --------------- File structures.dat ----------------\n\
 #   File path     Selection key    Selecton\n\
   1ADZ.pdb           -s            /1/A\n\
   1ADZ.pdb           -s            /2/A\n\
   1ADZ.pdb           -s            /3/A\n\
   1XYZ.pdb\n\
 ----------------------------------------------------\n\
\n\
 Once the file is prepared, use it with key '-input-list' in the\n\
 beginning of command line, e.g.:\n\
     %s -input-list structures.dat [options] [outputs]\n\
\n\
 INPUT SEQUENCES\n\
 ~~~~~~~~~~~~~~~~\n\
\n\
 {seq} parameters represent paths to sequence files in FASTA format.\n\
\n\
\n\
 OPTIONS\n\
 ~~~~~~~\n\
\n\
 All options have default values, which are good in most cases, but may\n\
 be adjsuted in individual cases. None of these options should be used\n\
 in order to reproduce results in publication referenced below.\n\
\n\
   -normal    default mode, corresponding to robust performance with high\n\
              selectivity and sensitivity. Applicable in (1,2,5).\n\
\n\
   -high      high mode, where extra (in most cases marginal) selectivity\n\
              and sensitivity is achieved at the expense of a noticeable\n\
              slow-down (cf. details from the publication referenced).\n\
              Applicable in (1,2,5).\n\
\n\
   -sheaf     sheaf mode of structure matching, which assumes strictly\n\
              one-to-one correspondence between atoms and that all\n\
              input structures are of the same length. In this mode,\n\
              matching always starts with first atoms in each chain,\n\
              and longer chains will be truncated to the length of the\n\
              shortest chain, if necessary. In simple sheaf mode, atom\n\
              contacts and matched structures are chosen such as to\n\
              maximize the overall Q-score (i.e. not all atoms and\n\
              structures may be matched). Applicable in (1).\n\
\n\
   -sheaf-atoms  sheaf mode, where all atom contacts are forced even\n\
              if doing so decreases the overall score. Gesamt will\n\
              choose though which structures to include in the resulting\n\
              cluster. Applicable in (1).\n\
\n\
   -sheaf-chains sheaf mode, where all structures are forcibly matched\n\
              even if doing so decreases the overall score. Gesamt will\n\
              choose though which atom contacts to select in order to\n\
              achieve the maximum score. Applicable in (1).\n\
\n\
   -sheaf-all sheaf mode, where no selection of atom contacts and\n\
              structures is done, so that all atom contacts and all\n\
              structures are included. Applicable in (1).\n\
\n\
   -sheaf-Q=Q threshold value of Q-score used for chosing chains to be\n\
              included in sheafs. By default, Q<0 and the threshold\n\
              value is chosen automatically. Setting Q=0 is equivalent\n\
              to using -sheaf-chains mode. Applicable in (1).\n\
\n\
   -sheaf-x   calculates only pairwise rmsds between all input structures\n\
              in '-sheaf-atoms' mode. Applicable in (1).\n\
\n\
   -domains   performs domain analysis, i.e. identifies conserved\n\
              substructures in the set of given structures.\n\
              Applicable in (1).\n\
\n\
   -r0=X      sets R0 parameter of Q-score (default is X=3.0 angstrom).\n\
              This parameter balances the alignment length (the number of\n\
              aligned residue pairs) and r.m.s.d. Cf. details of Q-score\n\
              from the publication referenced). Applicable in (1,2,5).\n\
\n\
   -sigma=X   sets Sigma parameter (in angstrom) used for weighted\n\
              superposition. The lower Sigma, the more the superposition\n\
              algorithm is focused on short atom contacts. If Sigma is\n\
              not set, weighted superposition is not used. Applicable in\n\
              (1,2,5).\n\
\n\
   -min1=X    sets the minimal part of query (fixed) structure in (2,5,6),\n\
              which should be found in target (moving) structure from the\n\
              PDB or GESAMT archive. This means that all target structures\n\
              of size less than L=X*size(query), as well as all alignments\n\
              of length knowingly less than L matched atoms pairs, will be\n\
              ignored without exploration. This makes archive searches\n\
              considerably faster and leaves more relevant matches in the\n\
              resulting list. X should be in range of [0..1]. By default,\n\
              min1=0.7.\n\
\n\
   -min2=X    sets the minimal part of target (moving) structure from the\n\
              PDB or GESAMT archive in (2,5,6), which should be found in query\n\
              (fixed) structure given in the command line. This means that\n\
              all target structures of size less than size(query)/X, as well\n\
              as all alignments of length knowingly less than X*size(target)\n\
              matched atom pairs will be ignored without exploration. This\n\
              makes archive searches considerably faster and leaves more\n\
              relevant matches in the resulting list. X should be in range\n\
              of [0..1]. By default, min2=0.7.\n\
\n\
   -trim-Q=X  sets the Q-score-based trim factor for scan results, in\n\
              range [0..1]. All scan results with Q-score lower than X\n\
              will be filtered out. By default, trim-Q=0. Applicable\n\
              in (2,5).\n\
\n\
   -trim-size=X  sets the size-based trim factor for scan results, in\n\
              range [0..1]. The size of matched substructures, specified\n\
              with '-min1' and '-min2' keys, makes GESAMT to disregard\n\
              structural pairs where this size cannot be achieved in\n\
              principle, but this does not guarantee that the resulted\n\
              matches will always satisfy the requirement. '-trim-size'\n\
              option allows one to remove all (X=1) or some (X<1) results\n\
              that do not obey the min1/min2 restriction. Specifically,\n\
              only matches of size N such as N>=X*min1 and N>=X*min2 will\n\
              be left in the result list. By default, trim-size=0.\n\
              Applicable in (2,5,6).\n\
\n\
   -excl-list specifies that next command line item is path to file with\n\
              the list of PDB chains to be excluded from matching against\n\
              Gesamt Archive (5,6). The file should list one chains per\n\
              line as below:\n\
 --------------- File structures.dat ----------------\n\
   1SAR:A\n\
   1E94:B\n\
   3GCB:A\n\
   1XYZ:*\n\
 ----------------------------------------------------\n\
              where the wildcard '*' means all chains in given PDB entry.\n\
\n\
   -incl-list specifies that next command line item is path to file with\n\
              the list of PDB chains to be included from matching against\n\
              Gesamt Archive (5,6). The file is formed identically to the\n\
              above described.\n\
\n\
   -nthreads=N  sets the number of threads to use. By default, N=1 and\n\
              only the application's main thread is used. '-nthreads=auto'\n\
              will set the optimal number of threads for the system used\n\
              (%i for the system where you've run GESAMT to produce this\n\
              text). This option is applicable in (1) if N>2 and in\n\
              (2,5). Multi-threading has a tangible effect in (2,5) and\n\
              is beneficial for multiple alignment ((1), N>2) of many\n\
              (50+) structures or if aligned structures are unusually\n\
              big. DO NOT run multiple treads in (2) if your PDB archive\n\
              is made of gzipped files.\n\
\n\
   -npacks=N  sets the number of packs in GESAMT archive (used only in\n\
              (3)). Number of packs should be not less than the maximal\n\
              number of threads the system can efficiently run in parallel.\n\
              For better load balancing, number of packs should exceed the\n\
              maximal number of threads by an order of magnitude, however,\n\
              an excessive number of packs (1000+) may cause a slowdown\n\
              in archive searches. By default, N=16xM, where M is the\n\
              optimal number of threads for a given system, which can be\n\
              set up also with '-npacks=auto'.\n\
\n\
   -compression=N  sets the compression flag for making Gesamt Archive\n\
              (only in (3)). N=0 means no compression, N=1 means fast\n\
              compression. By default, N=1, which is recommended in\n\
              all instances.\n\
\n\
   -v0        sets verbosity level 0 in (2,5), limited to only basic\n\
              and error messages.\n\
\n\
   -v1        sets verbosity level 1 in (2,5), which, in adition to\n\
              level 0, includes a progress indicator.\n\
\n\
   -v2        sets verbosity level 2 in (2,5), which, in adition to\n\
              level 0, includes a brief reporting on each protein chain\n\
              processed.\n\
\n\
\n\
 OUTPUTS\n\
 ~~~~~~~\n\
\n\
 A few output options is available:\n\
\n\
   -view        loads superposed structures into a graphical viewer\n\
                (not applicable in the archive scanning mode). For this\n\
                to work, the 'gesamt.opt' configuration file should be\n\
                placed to directory containing the gesamt executable.\n\
                Template 'gesamt.opt' file is produced with command\n\
\n\
                %s --print-opt\n\
\n\
   -viewN       same as '-view' but used for the visulaisation of Nth\n\
                domain when '-domains' option is used\n\
\n\
   -o out_file  name of output file, receiving:\n\
                 (o) in case of pairwise ((1) with N=2) alignment:\n\
                     'struct2' superimposed over 'struct1', in format\n\
                     defined by file extension (*.pdb, *.ent or *.cif)\n\
                 (o) in case of multiple ((1) with N>2) alignment:\n\
                     all input structures in best mutual superposition,\n\
                     superimposed over 1st structure. The structures\n\
                     are represented as NMR models. The file format\n\
                     is defined by file extension (*.pdb, *.ent or *.cif).\n\
                 (o) in case of archive scanning (2,5):\n\
                     structural hits, found in the archive given, ordered\n\
                     by decreasing the Q-score (cf. publication cited).\n\
                     This is a plain-text space-separated-values file.\n\
                Note that in case (1), output file name may be auto-\n\
                generated by option '-o-f', described below, in which\n\
                case option '-o' should not be used.\n\
\n\
   -o-[a|r|c|m|*][s|f]  output specificator, applicable in (1). The\n\
                specificator is made of two groups, both optional and\n\
                both containing one of few possible specifiers:\n\
                   Group 1: content of output files:\n\
                     a   selected atoms\n\
                     r   selected residues\n\
                     c   selected chains\n\
                     m   selected models\n\
                     *   full content of the corresponding input file\n\
                   Group 2: single-file or multi-files mode:\n\
                     s   all superposed structures will be placed in\n\
                         one file, each structure represented by a\n\
                         model. This output is unsuitable if inpput\n\
                         structure selection contains more than 1 model.\n\
                         Note that '-o' option must be used for this\n\
                         specificator to take an effect.\n\
                     f   a series of output files, one per superposed\n\
                         input structure, will be produced. If '-o'\n\
                         option is not used, then output files will\n\
                         have the corresponding input file names\n\
                         appended by number of their order in the\n\
                         command line. If output file name is given with\n\
                         the '-o' option, then that name will be used\n\
                         with the structure number appended.\n\
                     d   same as 'f' but it treats file name given by\n\
                         '-o' option, as directory name where GESAMT\n\
                         will place superposed file with their oriignal\n\
                         names appended by the number of their order in\n\
                         the common line.\n\
                Default specificator:  -o-cs. Note that output file will\n\
                not be created with this specificator unless '-o' is\n\
                used\n\
\n\
   -a seq_file  output file receiving sequence alignment inferred from\n\
                structural alignment, in FASTA format.\n\
\n\
   -csv csv_file output file receiving details of structural alignments\n\
                in CSV format. Applicable in (1,2,5).\n\
\n\
\n\
 EXAMPLES\n\
 ~~~~~~~~\n\
\n\
 (o) Align chain A from file 1xyz.pdb and chain B from file 2xyz.cif,\n\
     output both 2xyz.cif:B superimposed over 1xyz.pdb:A in CIF format\n\
     in single file, as well as the corresponding sequence alignment:\n\
\n\
     %s 1xyz.pdb -s A 2xyz.cif -d 'b:' -o 2xyz_superposed.cif -a align.seq\n\
\n\
 (o) Multiply align chains A from 6 first NMR models in file 1nmr.pdb,\n\
     making alignments tighter than with default parameters, and output\n\
     both the superposed cluster of selected chains as individual CIF\n\
     files and the corresponding sequence alignment in FASTA format:\n\
\n\
     %s \\\n\
        1nmr.pdb -s '/1/A' 1nmr.pdb -s '/2/A' 1nmr.pdb -s '/3/A' \\\n\
        1nmr.pdb -s '/4/A' 1nmr.pdb -s '/5/A' 1nmr.pdb -s '/6/A' \\\n\
        -r0=1.5 -sigma=1.0 -o 1nmr_cluster.cif -o-cf -a align.seq\n\
\n\
     The superposed chains will be output in a series of files named\n\
     1nmr_cluster_N.cif, where N=1,2,3,4,5,6 is the order number of the\n\
     corresponding structure in the command line. Note that, in this\n\
     particular case, the order number coinsides with the NMR model\n\
     number, but in general, there is no connection between them.\n\
\n\
 (o) Find structural homologues to sub-chain formed by residues 22a to 112\n\
     of chain C from file 1xyz.pdb, in custom structural archive /share/pdb,\n\
     using high accuracy mode:\n\
\n\
     %s 1xyz.pdb -d 'c:22a-112' -pdb /share/pdb -high -o homologues.txt\n\
\n\
\n\
 APPENDIX 1. CCP4 Coordinate ID\n\
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\
\n\
 CCP4 Coordinate ID (CID) represents hierarchy of models, chains, residues\n\
 and atoms, represented as a file path in UNIX systems (all whitespaces\n\
 are ignored):\n\
\n\
     /mdl/chn/s1(res1).i1-s2(res2).i2/at[el].aloc\n\
 where:\n\
 \n\
    mdl   - the model's serial number or 0 or '*' for any model\n\
            (default)\n\
    chn   - the chain ID or list of chains like 'A,B,C' or '*'\n\
            for any chain (default)\n\
    s1,s2 - the starting and ending residue sequence numbers\n\
            or '*' for any sequence number (default)\n\
    i1,i2 - the residues insertion codes or '*' for any insertion\n\
            code. If sequence number other than '*' is specified,\n\
            then insertion code defaults to \"\" (no insertion\n\
            code), otherwise the default is '*'\n\
    res   - residue name or list of residue names like 'ARG,VAL,TRP'\n\
            or '*' (default)\n\
    at    - atom name or list of atom names like 'CA,N1,O' or '*'\n\
            for any atom name (default)\n\
    el    - chemical element name or list of chemical element\n\
            names like 'C,N,O' or '*' for any chemical element\n\
            name (default)\n\
    aloc  - the alternative location indicator or '*' for any\n\
            alternate location. If the atom name and chemical\n\
            element name is specified (both may be '*'), then\n\
            the alternative location indicator defaults to \"\"\n\
            (no alternate location), otherwise the default is\n\
            '*'\n\
\n\
 If CID starts with a slash, the absolute 'path' starting from model\n\
 number is assumed, otherwise, the path is decoded according to the\n\
 first symbol (letter for chain, number for residue etc.). This is not\n\
 completely unambiguous, in difference of the absolute 'path'\n\
 specification. Below are most frequently used working examples:\n\
\n\
    /mdl/chn/s1.i1-s2.i2/at[el]:aloc\n\
    /mdl/chn/*(res).ic/at[el]:aloc\n\
    chn/*(res).ic/at[el]:aloc\n\
    s1.i1-s2.i2/at[el]:aloc\n\
    s1.i1/at[el]:aloc\n\
    /mdl\n\
    chn\n\
    s1.i1-s2.i2\n\
    (res)\n\
    at[el]:aloc\n\
    chn//[el]\n\
\n\
 In GESAMT context, most useful exemplary CIDs are:\n\
\n\
   A                - take whole chain A from 1st model\n\
   /2/B             - take whole chain B from 2nd NMR model\n\
   C/12-122         - take residues with sequence numbers 12 to 122\n\
                      from chain C of 1st model\n\
   /3/D/12.A-122.B  - take residues in range of 12.A to 122.B from\n\
                      chain D of 3rd NMR model\n\
\n\
\n\
 APPENDIX 2. SCOP convention for substructure (domain) selections\n\
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\
\n\
 Provides an easy selection of chains and range of residues within\n\
 chains. First model in the file is always assumed. Examples:\n\
\n\
   \"*\", \"(all)\"           - take all file\n\
   \"-\"                    - take chain without chain ID\n\
   \"a:Ni-Mj,b:Kp-Lq,...\"  - take chain 'a' residue number N\n\
                            insertion code i to residue number M\n\
                            insertion code j plus chain 'b'\n\
                            residue number K insertion code p to\n\
                            residue number L insertion code q and\n\
                            so on.\n\
   \"a:,b:...\"             - take whole chains 'a' and 'b' and so on\n\
   \"a:,b:Kp-Lq,...\"       - any combination of the above.\n\
\n\
\n\
 REFERENCE\n\
 ~~~~~~~~~\n\
 E. Krissinel (2012). Enhanced fold recognition using efficient\n\
 short fragment clustering. J. Mol. Biochem. 1(2) 76-85.\n"
#ifdef compile_for_ccp4
"$$\n"
#endif
"\n",
argv0,argv0,argv0,argv0,argv0,argv0,argv0,
getNofSystemThreads(),
argv0,argv0,argv0,argv0
        );

}
