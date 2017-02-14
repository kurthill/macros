#include <cmath>
#include <TFile.h>
#include <TString.h>
#include <TLine.h>
#include <TTree.h>
#include <TLatex.h>
#include <TGraphErrors.h>
#include <cassert>
#include "SaveCanvas.C"
#include "SetOKStyle.C"
using namespace std;

TFile * _file0 = NULL;
TTree * T = NULL;
TString cuts = "";
TText *t;
TH1F *h;

int DrawPrototype2HCAL(const char *particle = "not_set",
		       const double momentum = -1,
		       //const TString infile =  "ALL.root"
		       const TString infile =  "/gpfs/mnt/gpfs04/sphenix/user/abhisek/production/outputdir/beam_00003285-0000_DSTReader.root"
			)
{
  cout << "Particle " << particle << endl;
  cout << "Momentum " << momentum << endl;
  cout << "Infile " << infile << endl;
  double emcal_calibration = 1.; 

  SetOKStyle();
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1111);
  TVirtualFitter::SetDefaultFitter("Minuit2");
  gSystem->Load("libg4eval.so");
  gSystem->Load("libqa_modules.so");
  //gSystem->Load("/gpfs/mnt/gpfs02/phenix/scratch/abhisek/coresoftware/offline/packages/Prototype2/build/lib/libPrototype2.so");
  gSystem->Load("libPrototype3.so");

  if (!_file0)
    {
      TString chian_str = infile;
      chian_str.ReplaceAll("ALL", "*DSTReader");

      TChain * t = new TChain("T");
      const int n = t->Add(chian_str);

      cout << "Loaded " << n << " root files with " << chian_str << endl;
      if(n==0) exit(1);
      assert(n > 0);

      T = t;

      _file0 = new TFile;
      _file0->SetName(infile);
    }

   assert(_file0);
   TCut event_sel = "1*1";
   cuts = "_all_event";

  cout << "Build event selection of " << (const char *) event_sel << endl;
   T->Draw(">>EventList", event_sel);
  TEventList * elist = gDirectory->GetObjectChecked("EventList", "TEventList");
  cout << elist->GetN() << " / " << T->GetEntriesFast() << " events selected"
      << endl;
  T->SetEventList(elist);  
  //Dump();
  HCALDistribution("HCALIN", true);
  HCALDistribution("HCALOUT", true);
}

void HCALDistribution(TString det = "HCALIN",bool log_scale = false)
{
  //TString gain = "CALIB";
  TString gain = "RAW";
  TString hilo = "LG";
  TText * t;
  TCanvas *c1 = new TCanvas(
      det + gain + TString(log_scale ? "_Log" : "") + cuts,
      det + gain + TString(log_scale ? "_Log" : "") + cuts,
      800, 800);
  c1->Divide(4, 4, 0., 0.01);
  int idx = 1;
  TPad * p;
  
   for (int iphi = 4 - 1; iphi >= 0; iphi--)
    {
      for (int ieta = 0; ieta < 4; ieta++)
        {
          p = (TPad *) c1->cd(idx++);
          c1->Update();

          if (log_scale)
            {
              p->SetLogy();
            }
          p->SetGridx(0);
          p->SetGridy(0);

          TString hname = Form("hEnergy_ieta%d_iphi%d", ieta, iphi)
              + det;

          TH1 * h = NULL;

          //h = new TH2F(hname,
          //      Form(";Tower Energy Sum (ADC);Count / bin"), 24, -.5, 23.5, 550, 0, 2050);
          h = new TH1F(hname,
                Form(";Tower Energy Sum (ADC);Count / bin"), 550, 0, 2050);
          h->SetLineWidth(0);
          h->SetLineColor(kBlue + 3);
          h->SetFillColor(kBlue + 3);
          h->GetXaxis()->SetTitleSize(.09);
          h->GetXaxis()->SetLabelSize(.08);
          h->GetYaxis()->SetLabelSize(.08);

          TString sdraw = "TMath::Abs(TOWER_" + gain
              + "_" + hilo + "_"+ det +"[].get_energy())>>" + hname;

         TString scut =
              Form(
                  "TOWER_%s_%s_%s[].get_bineta()==%d && TOWER_%s_%s_%s[].get_binphi()==%d",
                  gain.Data(), hilo.Data(), det.Data(),ieta, gain.Data(), hilo.Data(), det.Data(), iphi);
         //cout << "T->Draw(\"" << sdraw << "\",\"" << scut << "\");" << endl;
         T->Draw(sdraw, scut, "colz");
         t = new TText(0.2, 0.92, Form("E=%0.2f GeV", h->GetIntegral()));
         t->Draw("same");
       }
      }

   SaveCanvas(c1, TString(c1->GetName()), kTRUE);
}

void Dump()
{
 TString name = "output";
 //name += cuts;
 name += ".txt";
 T->Process("Prototype2_DSTReader.C+",name.Data());
}

