#define MySelector_cxx
// The class definition in MySelector.h has been generated automatically
// by the ROOT utility TTree::MakeSelector(). This class is derived
// from the ROOT class TSelector. For more information on the TSelector
// framework see $ROOTSYS/README/README.SELECTOR or the ROOT User Manual.


// The following methods are defined in this file:
//    Begin():        called every time a loop on the tree starts,
//                    a convenient place to create your histograms.
//    SlaveBegin():   called after Begin(), when on PROOF called only on the
//                    slave servers.
//    Process():      called for each event, in this function you decide what
//                    to read and fill your histograms.
//    SlaveTerminate: called at the end of the loop on the tree, when on PROOF
//                    called only on the slave servers.
//    Terminate():    called at the end of the loop on the tree,
//                    a convenient place to draw/fit your histograms.
//
// To use this file, try the following session on your Tree T:
//
// root> T->Process("MySelector.C")
// root> T->Process("MySelector.C","some options")
// root> T->Process("MySelector.C+")
//


#include "MySelector.h"
#include <TH2.h>
#include <TStyle.h>
#include <TH1F.h>
#include <iostream>
#include <fstream>

void MySelector::Begin(TTree * /*tree*/)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();
}

void MySelector::SlaveBegin(TTree * /*tree*/)
{
   // The SlaveBegin() function is called after the Begin() function.
   // When running with PROOF SlaveBegin() is called on each slave server.
   // The tree argument is deprecated (on PROOF 0 is passed).

   
   myFile.open("Max_Energies.txt",ios::out);
}

Bool_t MySelector::Process(Long64_t entry)
{
   // The Process() function is called for each entry in the tree (or possibly
   // keyed object in the case of PROOF) to be processed. The entry argument
   // specifies which entry in the currently loaded tree is to be processed.
   // When processing keyed objects with PROOF, the object is already loaded
   // and is available via the fObject pointer.
   //
   // This function should contain the \"body\" of the analysis. It can contain
   // simple or elaborate selection criteria, run algorithms on the data
   // of the event and typically fill histograms.
   //
   // The processing can be stopped by calling Abort().
   //
   // Use fStatus to set the return value of TTree::Process().
   //
   // The return value is currently not used.

   fReader.SetLocalEntry(entry);

   // Loop over electrons
      for (int igen = 0, ngen =  gen_energy.GetSize(); igen < ngen; ++igen)
       {
        const double electronmass = 0.000511;
	const float photonmass = 0.0;
	TLorentzVector TLVElectron;
	//2-D array to store hits of a candidate electron
	//int candidateHits [rechit_layer.GetSize()][rechit_energy.GetSize()];
	int id;
	//int initLayer;
	//	int viablerechits=0;//counter for viable rechits
	if(gen_pdgid[igen]==-11||gen_pdgid[igen]==11)
	  {
	     TLVElectron.SetPtEtaPhiM(gen_pt[igen],gen_eta[igen],gen_phi[igen],photonmass);
	     id = 0;
	  }else{
	     TLVElectron.SetPtEtaPhiM(gen_pt[igen],gen_eta[igen],gen_phi[igen],electronmass);
	     id=1;
	}
        
        // Loop over HGCRecHits
        double MaxE=0;
	int maxLayer=0;

        //int i=0;
	//double initEnergy;
        for (int irc = 0, nrc =  rechit_energy.GetSize(); irc < nrc; ++irc)//loop over rechits
        {
         // these kind of if statements will help you filter your data, depending on your research question.
	  //Event cutoff: greater than 0.4 GeV
	  
         if (rechit_layer[irc]<29)
        {//start if
          TLorentzVector TLVRecHit;
	  if (rechit_energy[irc]>0.4)
	    {
	      double rechit_pt=rechit_energy[irc]/cosh(rechit_eta[irc]); //p=E for rechits
	      TLVRecHit.SetPtEtaPhiE(rechit_pt,rechit_eta[irc],rechit_phi[irc],rechit_energy[irc]);
	      double dR=TLVRecHit.DeltaR(TLVElectron);
	      if (dR<0.3){
	    
		if (TLVRecHit.E()>MaxE)
		  {
		    MaxE=TLVRecHit.E();
		    maxLayer=rechit_layer[irc];
		  }
	      }
	    }
          
        }//end if
        }
        //Write max energy and layer to energy-peaks.txt
	myFile << MaxE << " ";
	myFile << maxLayer << " ";
	myFile << id << "\n";
        } // Loop over electrons ends

       // Event loop ends

   return kTRUE;
}

void MySelector::SlaveTerminate()
{
   // The SlaveTerminate() function is called after all entries or objects
   // have been processed. When running with PROOF SlaveTerminate() is called
   // on each slave server.
   myFile.close();
}

void MySelector::Terminate()
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.

}
