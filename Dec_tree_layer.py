import ROOT
from ROOT import TMVA

import array
ROOT.PyConfig.DisableRootLogon=True
ROOT.PyConfig.IgnoreCommandLineOptions=False

ntuple = ROOT.TNtuple("ntuple","ntuple","energy:layer:signal")

#open energy-peaks file
# keeps objects otherwise removed by garbage collected in a list
gcSaver = []

with open('Max_Energies.txt') as filein:

    
    for line in filein:
        #split line
        data = line.split(" ")
        #fill the ntuple
        ntuple.Fill(float(data[0]),#energy
                    int(data[1]),#layer
                    int(data[2]))#id
	                    


 
gcSaver.append(ROOT.TCanvas())

histo = ROOT.TH2F("histo","Layer vs. Energy",30,0,37,22,0,50)
histo.Draw()

# draw the signal events in red
ntuple.SetMarkerColor(ROOT.kRed)
ntuple.Draw("energy:layer"," signal < 0.5","same")

# draw the background events in blue
ntuple.SetMarkerColor(ROOT.kBlue)
ntuple.Draw("energy:layer","signal >= 0.5","same")

ROOT.TMVA.Tools.Instance()
fout = ROOT.TFile("test.root","RECREATE")

factory = ROOT.TMVA.Factory("TMVAClassification",fout, ":".join(["!V","!Silent","Color","DrawProgressBar","Transformations=I;D;P;G,D","AnalysisType=Classification"]))

dataloader = TMVA.DataLoader("dataset")
#add variables
dataloader.AddVariable("layer","F")
dataloader.AddVariable("energy","F")

dataloader.AddSignalTree(ntuple)
dataloader.AddBackgroundTree(ntuple)

#cuts defining signal and background
sigCut = ROOT.TCut("signal < 0.5")
bgCut = ROOT.TCut("signal >= 0.5")
dataloader.PrepareTrainingAndTestTree(sigCut, #signal
                                   bgCut, #background
                                   ":".join(["nTrain_Signal=0",
                                             "nTrain_background=0",
                                             "SplitMode=Random",
                                             "NormMode=NumEvents",
                                             "!V"
                                             ]))

method = factory.BookMethod(dataloader, ROOT.TMVA.Types.kBDT,"BDT",
                            ":".join([
                                "!H",
                                "!V",
                                "NTrees=300",
                                "MinNodeSize=0.75%",
                                "MaxDepth=3",
                                "BoostType=AdaBoost",
                                "SeparationType=GiniIndex",
                                "nCuts=20",
                                "PruneMethod=NoPruning"
                                ]))

factory.TrainAllMethods()
factory.TestAllMethods()
factory.EvaluateAllMethods()

print("Reader created")
reader = ROOT.TMVA.Reader()

print("Creating variable arrays and adding them to reader")
varx = array.array('f',[0]) ; reader.AddVariable("layer",varx)
vary = array.array('f',[0]) ; reader.AddVariable("energy",vary)

#read weights file
reader.BookMVA("BDT","dataset/weights/TMVAClassification_BDT.weights.xml")

# create a new 2D histogram with fine binning
histo2 = ROOT.TH2F("histo2","",40,0,37,60,0,50)
 
# loop over the bins of a 2D histogram
for i in range(1,histo2.GetNbinsX() + 1):
    for j in range(1,histo2.GetNbinsY() + 1):
         
        # find the bin center coordinates
        varx[0] = histo2.GetXaxis().GetBinCenter(i)
        vary[0] = histo2.GetYaxis().GetBinCenter(j)
         
        # calculate the value of the classifier
        # function at the given coordinate
        bdtOutput = reader.EvaluateMVA("BDT")
         
        # set the bin content equal to the classifier output
        histo2.SetBinContent(i,j,bdtOutput)

print("Drawing")
gcSaver.append(ROOT.TCanvas())
histo2.Draw("colz")

# fill histograms for signal and background from the test sample tree
ROOT.TestTree.Draw("BDT>>hSig(22,0.0,30.0)","classID == 0","goff")  # signal
ROOT.TestTree.Draw("BDT>>hBg(22,0.0,30.0)","classID == 1", "goff")  # background
 
ROOT.hSig.SetLineColor(ROOT.kRed); ROOT.hSig.SetLineWidth(2)  # signal histogram
ROOT.hBg.SetLineColor(ROOT.kBlue); ROOT.hBg.SetLineWidth(2)   # background histogram
 
# use a THStack to show both histograms
hs = ROOT.THStack("hs","")
hs.Add(ROOT.hSig)
hs.Add(ROOT.hBg)
 
# show the histograms
gcSaver.append(ROOT.TCanvas())
#hs.Draw()
