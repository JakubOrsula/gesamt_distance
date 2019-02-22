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
//    30.05.16   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  GSMT_DomData <implementation>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::DomData
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2008-2016
//
// =================================================================
//

#include <string.h>

#include "gsmt_domdata.h"

// =================================================================

gsmt::DomData::DomData()  {
  initDomData();
}

gsmt::DomData::~DomData()  {
  freeMemory();
}

void gsmt::DomData::initDomData()  {
  nStructures =  0;    // number of structures with defined domains
  nDomains    =  0;    // number of domains defined
  sSize       =  NULL; // structure full length in residues
  SD          =  NULL; // [domNo][structNo] superposition data
  ddef        =  NULL; // [structNo][i] gives domains number
  Q           =  0.0;  // combined Q-score
  rmsd        = -1.0;  // combined rmsd
  seqId       =  0.0;  // combined sequence identity
  Nalign      =  0;    // combined alignment length
}

void gsmt::DomData::freeMemory()  {
int i;

  mmdb::FreeVectorMemory ( sSize,0 );

  if (SD)  {
    for (i=0;i<nDomains;i++)
      if (SD[i])
        delete SD[i];
    delete[] SD;
    SD = NULL;
  }
  
  mmdb::FreeMatrixMemory ( ddef,nStructures,0,0 );
  
  nStructures = 0;
  nDomains    = 0;

}


void gsmt::DomData::set_data_area ( PPStructure     S,
                                    PPSuperposition D,
                                    int             n_structures,
                                    int             n_domains )  {
// Allocates and initializes data structures for domains 
int i,j,nat;

  freeMemory();
  
  if ((n_structures>0) && (n_domains>0))  {

    nStructures = n_structures;
    nDomains    = n_domains;

    // sSize[i] keeps the length of ith structures as number of residues
    mmdb::GetVectorMemory ( sSize,nStructures,0 );
    for (i=0;i<nStructures;i++)
      sSize[i] = S[i]->getNCalphas();

    // SD[i] keeps superposition data for ith domain. Domain #0 is
    // global rigid-body alignment
    SD = new PMSuperposition[nDomains];
    for (i=0;i<nDomains;i++)
      SD[i] = new MSuperposition ( D[i] );
    
    // ddef[i][j] keeps domain number for jth residue of ith structure.
    // Domain #0 is global rigid-body alignment and it covers all
    // residues for all structures, therefore, it should never appear
    // in this array. 
    ddef = new mmdb::ivector[nStructures];
    for (i=0;i<nStructures;i++)  {
      mmdb::GetVectorMemory ( ddef[i],sSize[i],0 );
      nat = sSize[i];
      for (j=0;j<nat;j++)
        ddef[i][j] = 0;
    }

  }

}


void gsmt::DomData::lay_domains()  {
mmdb::ivector ddefi;
int           i,j,k,l,nat,dom1,dom2;
bool          fixed;

  // start with 1 because 0th domain corresponds to global alignment
  for (k=1;k<nDomains;k++)
    for (i=0;i<nStructures;i++)  {
      nat   = sSize[i];
      ddefi = ddef[i];
      for (j=0;j<nat;j++)
        if (SD[k]->c[i][j]>=0)
          ddefi[j] = k;
    }

/*
  printf ( " --- nDomains=%i\n",nDomains);
  for (i=0;i<nStructures;i++)  {
    for (k=0;k<sLen[i];k++)
      printf ( "%i",ddef[i][k] );
    printf ( "\n");
  }
*/

  // recover inter-domain gaps
  for (i=0;i<nStructures;i++)  {

    ddefi = ddef[i];
    dom1  = 0;
    nat   = sSize[i];
    j     = 0;

    while (j<nat)  {
      
      // Here we stay in the beginning of the assignment gap, and 'k'
      // points to 1st gap position. This may be untrue at k==0, but
      // this poses no harm to further workflow

      k = j;               // stand on the beginning of gap

      while (j<nat)        // find next assigned region
        if (ddefi[j]==0)   // skip all unassigned positions
             j++;
        else break;

      if (j<nat)  {        // assigned region found
        dom2 = ddefi[j];   // assigned domain number
        if (dom1==0)       // there were no assignments yet and found
          dom1 = dom2;     //   region will spread to the start of chain
      } else  {
        if (dom1==0)       // there were no assignment so
          dom1 = 1;        //   assign all chain as domain #1
        dom2 = dom1;       // assignment will run from both sides of gap
      }                  
      l = j-1;             // stand on last gap position
      while (k<=l)  {
        ddefi[k++] = dom1; // assign 'dom1' from start of gap
        ddefi[l--] = dom2; // assign 'dom2' from end of gap
      }

      while (j<nat)        // look for next gap
        if (ddefi[j]!=0)   // skip all assigned positions
             j++;
        else break;

      dom1 = ddefi[j-1];

    }

  }

  // remove short gap rattle
  for (k=1;k<nDomains;k++)  {
    do  {
      fixed = false;
      for (i=0;i<nStructures;i++)  {
        nat   = sSize[i]-1;
        ddefi = ddef[i];
        for (j=1;j<nat;j++)
          if ((ddefi[j-1]==k) && (ddefi[j]!=k) && (ddefi[j+1]==k))  {
            ddefi[j] = k;
            fixed = true;
          }
      }
    } while (fixed);
  }

/*
  printf ( " --- \n");
  for (i=0;i<nStructures;i++)  {
    for (k=0;k<sSize[i];k++)
      printf ( "%i",ddef[i][k] );
    printf ( "\n");
  }
*/

}



void gsmt::DomData::align_domains ( PPStructure S, PAligner A )  {
mmdb::PPAtom   atoms1,atoms2;
PSuperposition SDk;
mmdb::ivector  ddefi;
int            i,k,nat1,nat2,matchNo,j1,j2,i1,i2;

  Q      = 0.0;  // combined Q-score
  rmsd   = 0.0;  // combined rmsd
  seqId  = 0.0;  // combined sequence identity
  Nalign = 0;    // combined alignment length

  if (!A)          return;
  if (nDomains<1)  return;
  if (nDomains<2)  {
    Q      = SD[0]->Q;     // combined Q-score
    rmsd   = SD[0]->rmsd;  // combined rmsd
    seqId  = SD[0]->seqId; // combined sequence identity
    Nalign = SD[0]->Nalgn; // combined alignment length
    return;
  }

  // start with 1 because 0th domain corresponds to global alignment
  for (k=1;k<nDomains;k++)  {
    for (i=0;i<nStructures;i++)  {
      S[i]->selectSubStructure ( NULL );
      S[i]->getCalphas ( atoms1,nat1 );
      ddefi = ddef[i];
      for (j1=0;j1<nat1;j1++)
        if (ddefi[j1]!=k)
          S[i]->selectAtom ( atoms1[j1],mmdb::SKEY_CLR );
      S[i]->finishAtomSelection();
    }
    if (A->Align(S[0],S[1],false)==GSMT_Ok)  {
      A->getBestMatch ( SDk,matchNo );  // with local (in-domain) atom selection
      if (SDk)  {
        mmdb::Mat4Copy ( SDk->T,SD[k]->T[1] );
        SD[k]->rmsd  = SDk->rmsd;
        SD[k]->Q     = SDk->Q;
        SD[k]->seqId = SDk->seqId;
        SD[k]->Nalgn = SDk->Nalgn;
        S[0]->getCalphas ( atoms1,nat1 ); // atom serial numbers
        S[1]->getCalphas ( atoms2,nat2 ); //   provide global mapping
        for (i=0;i<nStructures;i++)
          for (j2=0;j2<sSize[i];j2++)  // full-size vectors
            SD[k]->c[i][j2] = -1;
        for (j1=0;j1<sSize[0];j1++)
          SD[k]->var[j1] = -1.0;
        for (j1=0;j1<nat1;j1++)  // local residue index
          if (SDk->c1[j1]>=0)  {
            i1 = atoms1[j1]->serNum;  // global residue index
            j2 = SDk->c1[j1];
            i2 = atoms2[j2]->serNum;
            SD[k]->c[0][i1] = i2;
            SD[k]->c[1][i2] = i1;
            SD[k]->var [i1] = SDk->dist1[j1];
            Nalign++;
            rmsd += SDk->dist1[j1]*SDk->dist1[j1];
            if (!strcmp(atoms1[j1]->GetResName(),
                        atoms2[j2]->GetResName()))
              seqId += 1.0;
          }
        SD[k]->dSize[0] = nat1;
        SD[k]->dSize[1] = nat2;
      }
    }
  }

  if (Nalign>0)  {
    Q      = A->getQR0();
    Q      = Nalign*Nalign/(sSize[0]*sSize[1]*(1.0+rmsd/(Nalign*Q*Q)));
    rmsd   = sqrt(rmsd/Nalign);
    seqId /= Nalign;
  }

}


void gsmt::DomData::setData ( PPStructure     S,
                              PPSuperposition D,
                              PAligner        A,
                              int             n_structures,
                              int             n_domains )  {
int i;

  for (i=0;i<n_structures;i++)
    S[i]->selectSubStructure ( NULL );

  set_data_area ( S,D,n_structures,n_domains );
  lay_domains   ();
  align_domains ( S,A );  

}


void gsmt::DomData::getCombinedAlignment ( mmdb::imatrix & c,
                                           mmdb::rvector & var )  {
int i,j,k,nat;

  nat = sSize[0];
  mmdb::GetVectorMemory ( var,nat,0 );
  for (j=0;j<nat;j++)
    var[j] = -1.0;

  for (k=1;k<nDomains;k++)
    for (j=0;j<nat;j++)
      if (SD[k]->c[0][j]>=0)
        var[j] = SD[k]->var[j];

  c = new mmdb::ivector[nStructures];
  for (i=0;i<nStructures;i++)  {
    nat = sSize[i];
    mmdb::GetVectorMemory ( c[i],nat,0 );
    for (j=0;j<nat;j++)
      c[i][j] = -1;
    for (k=1;k<nDomains;k++)
      for (j=0;j<nat;j++)
        if (SD[k]->c[i][j]>=0)
          c[i][j] = SD[k]->c[i][j];
  }

}

