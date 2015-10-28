int Min_cemc_layer = -1;
int Max_cemc_layer = -1;
int Min_si_layer = -1;
int Max_si_layer = -1;
int Cemc_slats_per_cell = 72; // make it 2*2*2*3*3 so we can try other combinations

double inner_cage_radius = 0.;
double diffusion = 0.0057;
double electrons_per_kev = 38.;

// tpc_cell_x is the TPC pad size.  The actual hit resolution depends not only on this pad size but also on the diffusion in the gas and amplification step
double tpc_cell_x = 0.12;
// tpc_cell_y is the z "bin" size.  It is approximately the z resolution * sqrt(12)
// eventually this will be replaced with an actual simulation of timing amplitude.
double tpc_cell_y = 0.17;

int n_svx_layer = 2;
double vtx_cell_x[2] = {0.005, 0.005};
double vtx_cell_y[2] = {0.0425, 0.0425};

int Fun4All_G4_tpc_hijing_full(
  int nEvents=1
)
{
  char inputFile[500];
  sprintf(inputFile,"/direct/phenix+hhj/frawley/simulation/sHijing/sHijing-11-13fm.dat");
 // sprintf(inputFile,"/direct/phenix+hhj/frawley/simulation/sHijing/sHijing-0-4fm_%i.dat",process);
  cout << "Using sHijing file " << inputFile << endl;
  
  
  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4detectors.so");
  gSystem->Load("libphhepmc.so");
  gSystem->Load("libg4testbench.so");
  gSystem->Load("libg4hough.so");
  gSystem->Load("libcemc.so");
  gSystem->Load("libg4eval.so");
  gROOT->LoadMacro("G4Setup_tpc_plus_vtx.C");
  
  G4Init();
  int absorberactive = 1;
  const string magfield = "1.5";
  
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);
  
  // ************************************
  // read hepmc
  HepMCNodeReader *hr = new HepMCNodeReader();
  se->registerSubsystem(hr);
  // ************************************
  
  
  G4Setup(absorberactive, magfield);
  
  
  // ************************************
  // make "cells", digitize and clusterize
  
  
    PHG4CylinderCellTPCReco *svtx_cells = new PHG4CylinderCellTPCReco();
    svtx_cells->setDiffusion(diffusion);
    svtx_cells->setElectronsPerKeV(electrons_per_kev);
    svtx_cells->Detector("SVTX");
    for(int i=0;i<n_svx_layer;++i)
    {
      svtx_cells->cellsize(i, vtx_cell_x[i], vtx_cell_y[i]);
    }
    for(int i=n_svx_layer;i<62;++i)
    {
      svtx_cells->cellsize(i, tpc_cell_x, tpc_cell_y);
    }
    
    se->registerSubsystem(svtx_cells);
  
  PHG4SvtxDigitizer* digi = new PHG4SvtxDigitizer();
  digi->Verbosity(0);
  for(i=0;i<n_svx_layer;++i)
  {
    digi->set_adc_scale(i, 255, 1.0e-6);
  }
  for(i=n_svx_layer;i<62;++i)
  {
    digi->set_adc_scale(i, 10000, 1.0e-1);
  }
  se->registerSubsystem( digi );
  
  
  
  PHG4TPCClusterizer* clusterizer = new PHG4TPCClusterizer("PHG4SvtxClusterizer",4,1);;
  se->registerSubsystem( clusterizer );


  // ************************************
  
  
  PHG4HoughTransformTPC* hough = new PHG4HoughTransformTPC(62,56);
  hough->set_use_vertex(true);
  hough->setRemoveHits(true);
  hough->setRejectGhosts(true);
  hough->set_min_pT(0.2);
  hough->set_chi2_cut_full( 3.0 );
  hough->set_chi2_cut_init( 3.0 );

  hough->setBinScale(1.0);
  hough->setZBinScale(1.0);

  hough->Verbosity(10);
  double mat_scale = 1.0;
  hough->set_material(0, mat_scale*0.013);
  hough->set_material(1, mat_scale*0.013);
  hough->set_material(2, mat_scale*0.01);
  for(int i=3;i<62;++i)
  {
    hough->set_material(i, mat_scale*0.06/60.);
  }
  hough->setUseCellSize(false);
  
  for(int i=2;i<62;++i)
  {
    hough->setFitErrorScale(i, 1./sqrt(12.));
  }
  for(int i=2;i<62;++i)
  {
    hough->setVoteErrorScale(i, 0.2);
  }

  se->registerSubsystem( hough );

    stringstream ss;ss.clear();ss.str("");ss<<"hij_eval_.root";
    

  SvtxEvaluator* eval = new SvtxEvaluator("SVTXEVALUATOR", ss.str().c_str());
  eval->do_cluster_eval(false);
  eval->do_g4hit_eval(false);
  eval->do_hit_eval(false);
  eval->do_gpoint_eval(false);
  eval->Verbosity(0);
  se->registerSubsystem( eval );

    // MomentumEvaluator* eval = new MomentumEvaluator(ss.str(),0.1,0.2,62,2,56,10.,80.);
    // se->registerSubsystem( eval );
  
  
  // ************************************
  // tell Fun4All the hepmc file to read
  Fun4AllInputManager *in = new Fun4AllHepMCInputManager( "DSTIN");
  se->registerInputManager( in );
  se->fileopen( in->Name(), inputFile );
  // ************************************
  
  se->run(nEvents);
  se->End();
  std::cout << "All done" << std::endl;
  delete se;
  gSystem->Exit(0);
}
