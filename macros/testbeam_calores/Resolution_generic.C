#include "/phenix/u/abhisek/tools/SetOKStyle.C"

void Resolution_generic()
{
 SetOKStyle();
 OKStyle->SetPadGridX(0);
 OKStyle->SetPadGridY(0);
 OKStyle->SetOptTitle(0);
 OKStyle->SetOptStat(0);
 OKStyle->SetPadRightMargin(0.01);
 double en[] = {2, 4, 6, 8, 12, 16, 20, 30, 40, 50, 60};
  int nfiles = 11;
  char *files[] = { 
	"outputdir_set2/Prototype_pionp_2.root",
	"outputdir_set2/Prototype_pionp_4.root",
	"outputdir_set2/Prototype_pionp_6.root",
	"outputdir_set2/Prototype_pionp_8.root",
	"outputdir_set2/Prototype_pionp_12.root",
	"outputdir_set2/Prototype_pionp_16.root",
	"outputdir_set2/Prototype_pionp_20.root",
	"outputdir_set2/Prototype_pionp_30.root",
	"outputdir_set2/Prototype_pionp_40.root",
	"outputdir_set2/Prototype_pionp_50.root",
	"outputdir_set2/Prototype_pionp_60.root"
                  };
 

 /*double en[] = {2, 4, 6, 8, 12, 16, 24, 32};
 int nfiles = 8;
 char *files[] = { 
        "outputdir_set2/Prototype_pionn_2.root",
        "outputdir_set2/Prototype_pionn_4.root",
        "outputdir_set2/Prototype_pionn_6.root",
        "outputdir_set2/Prototype_pionn_8.root",
        "outputdir_set2/Prototype_pionn_12.root",
        "outputdir_set2/Prototype_pionn_16.root",
        "outputdir_set2/Prototype_pionn_24.root",
        "outputdir_set2/Prototype_pionn_32.root",
                  };
 */
 TFile *fout = TFile::Open("out.root","RECREATE");
 TCanvas *can = new TCanvas("can","can",600,600);
 can->Divide(4,3);
 TString name;
 TH1F *hsignal;
 
 TH1F *res = new TH1F("res","res",280,0,70);
 res->SetMarkerStyle(20); res->SetMarkerSize(1.2);
 res->GetYaxis()->SetTitle("Resolution");
 res->GetXaxis()->SetTitle("Beam Energy");

 TH1F *linearity = new TH1F("linearity","linearity",280,0,70);
 linearity->SetMarkerStyle(20);
 linearity->SetMarkerSize(1.5);
 linearity->GetYaxis()->SetTitle("Reconstructed Energy");
 linearity->GetXaxis()->SetTitle("Beam Energy");
 linearity->SetMarkerColor(2);

 float emcal_weight[] = {0.5, 0.499, 0.674, 0.702, 0.711, 0.686, 0.692, 0.668};

 for(int ifile=2; ifile<nfiles; ifile++)
 {
  cout << "Reading " << files[ifile] << endl;
  TFile *fin = TFile::Open(files[ifile],"READ");
  if(!fin)
  { 
    cout << "file not found " << files[ifile] << endl;
    continue;
  }
  name = Form("hsignal_%i", (int) en[ifile]);
  hsignal = new TH1F(name.Data(),name.Data(),500,0,100);
  //hsignal = new TH1F(name.Data(),name.Data(),400,0,80);
  hsignal->Sumw2();
  hsignal->SetMarkerSize(1.2);
  hsignal->GetXaxis()->SetTitle("Beam Energy");
  TTree *T = (TTree*) fin->Get("T");
  if(!T) continue;
  T->SetAlias("hcalin_e_new", "2.0*hcalin_e" );
  T->SetAlias("hcal_e", "hcalin_e_new+hcalout_e");
  T->SetAlias("ahcal", "(hcalin_e_new-hcalout_e)/(hcalin_e_new+hcalout_e)");
  T->SetAlias("aemcal", "(cemc_e-hcal_e)/(cemc_e+hcal_e)");
  T->SetAlias("Good_h","abs(C2_sum)<20");
  //hcalout
  //T->Draw(Form("cemc_e+hcalin_e+hcalout_e>>%s",name.Data()),Form("cemc_e<%f && hcalin_e<%f && good_h", 0.05*en[ifile], 0.05*en[ifile]), "goff");

  //full hcal
  //T->Draw(Form("hcalin_e_new+hcalout_e>>%s",name.Data()),Form("ahcal>-1 && ahcal<1 && Good_h"), "goff"); 
  T->Draw(Form("hcalin_e_new+hcalout_e>>%s",name.Data()),"Good_h", "goff"); 

  //Full
  //T->Draw(Form("%f*cemc_e+hcalin_e_new+hcalout_e>>%s",2*emcal_weight[ifile],name.Data()),"good_h && aemcal<0.8", "goff");
  if(hsignal->GetEntries()<5) continue;
  can->cd(ifile+1);
  hsignal->SetAxisRange(0,hsignal->GetMean()+4*hsignal->GetRMS());
  hsignal->Draw();
  TText *txt = new TText(0.6,0.7,Form("%2.0f GeV",en[ifile]) );
  txt->SetNDC();
  txt->SetTextSize(2*txt->GetTextSize());
  txt->Draw("Same");
  //double min_range = hsignal->GetMean()-0.8*hsignal->GetRMS();
  //double max_range = hsignal->GetMean()+1.2*hsignal->GetRMS();
  //if(min_range<4) min_range = 3; //2.5;

  //for hcalout
  double min_range = hsignal->GetMean()-1.2*hsignal->GetRMS();
  double max_range = hsignal->GetMean()+1.4*hsignal->GetRMS();
  
 //if(en[ifile]<5){
 // min_range = 1.66;
 // max_range = hsignal->GetMean()+0.7*hsignal->GetRMS();
 // }
 if(en[ifile]==6) min_range = 2.8;
 if(en[ifile]==8) min_range = 3.5;

  //for full
  //double min_range = hsignal->GetMean()-1.6*hsignal->GetRMS();
  //double max_range = hsignal->GetMean()+1.6*hsignal->GetRMS();

  hsignal->Fit("gaus","","",min_range,max_range);
  TF1 *gaus = (TF1*) hsignal->GetFunction("gaus");
  can->Update();
  fout->cd();
  hsignal->Write();
  //cout << gaus->GetParameter(2)/gaus->GetParameter(1) << endl;
  if(en[ifile]<3) continue;
  res->SetBinContent(res->FindBin(en[ifile]), gaus->GetParameter(2)/gaus->GetParameter(1));
  double rel_err1 = gaus->GetParError(1)/gaus->GetParameter(1);
  double rel_err2 = gaus->GetParError(2)/gaus->GetParameter(2);
  double tot_err = sqrt(rel_err1*rel_err1 + rel_err2*rel_err2);
  res->SetBinError( res->FindBin(en[ifile]), tot_err*res->GetBinContent(res->FindBin(en[ifile])) );
  //res->SetBinError( res->FindBin(en[ifile]), gaus->GetParError(2)/gaus->GetParameter(1));

  linearity->SetBinContent(linearity->FindBin(en[ifile]), gaus->GetParameter(1));
  linearity->SetBinError(linearity->FindBin(en[ifile]), gaus->GetParError(1));
 }

 TLegend * legend = new TLegend( 0.2, 0.7, 0.8, 0.88, "", "NDC" );
 legend->SetFillColor(0);
 legend->SetBorderSize(0);
 legend->SetTextSize( .05 );
 TCanvas *can2 = new TCanvas("can2","can2",800,600);
 res->SetMaximum(1.2);
 res->Draw("P");
 TF1 * f_calo_r = new TF1("f_calo_r", "sqrt(2*2+[0]*[0]+[1]*[1]/x)/100", 0.3, 65);
 f_calo_r->SetParLimits(0,0,30);
 f_calo_r->SetParLimits(1,0,200);
 res->Fit(f_calo_r, "RM", "", 0.3, 65);
 //legend->AddEntry(res,"T-1044, HCAL cosmic calib.","P");
 //f_calo_r->SetLineColor(1);
 //f_calo_r->DrawCopy("same");
 name  = Form("#DeltaE/E = 2%% #oplus %.1f%% #oplus %.1f%%/#sqrt{E}",
          f_calo_r->GetParameter(0), f_calo_r->GetParameter(1));
 legend->AddEntry( f_calo_r,  name.Data(), "L" );
 legend->Draw();
 can2->Update();

 TCanvas *c2 = new TCanvas("c2","c2",800,600);
 TF1 *fa = new TF1(Form("fa_%i",ifile),"([0]*x-[1]*x*x)",-0.1,65.);
  fa->SetLineColor(ifile+1);
 linearity->Fit(fa,"RM0","goff",-0.1,65);
 linearity->Draw();
 fa->Draw("Same");

 fout->cd();
 res->Write();
 f_calo_r->Write();
 linearity->Write();
}
