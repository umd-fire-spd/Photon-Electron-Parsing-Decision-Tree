#include <iostream>

void ChainRunJob()
{
TChain chain("ana/hgc");
chain.Add("./test_photon_1_TeV_1000_E.root");
chain.Add("./hgcalNtuple_1000_1.root");
chain.Process("MySelector.C");
}
