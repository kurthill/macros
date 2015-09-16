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


int Fun4All_G4_tpc_plus_vtx_Ups(
	int istate
)
{
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
// 	const string magfield = "/phenix/upgrades/decadal/fieldmaps/bPHENIX.dp.root";
	const string magfield = "1.5";
	
	Fun4AllServer *se = Fun4AllServer::instance();
	se->Verbosity(0);
	
	int uniqueseed = TRandom3(0).GetSeed();
	recoConsts *rc = recoConsts::instance();
	rc->set_IntFlag("RANDOMSEED", uniqueseed);
	
	
	PHG4ParticleGeneratorVectorMeson *gen = new PHG4ParticleGeneratorVectorMeson();
	gen->set_vtx_zrange(0.0, 0.0);
// 	gen->set_vtx_zrange(-10.0, +10.0);
	// Note: this rapidity range completely fills the acceptance of eta = +/- 1 unit
	gen->set_rapidity_range(-1.0, +1.0);
	gen->set_pt_range(0.0, 10.0);
	gen->set_decay_types("e+","e-");
	
	int nEvents = 0;
	
	if(istate==1)
	{
		// Upsilon(1S)
		// nEvents = 200;
		nEvents = 8770;
		gen->set_mass(9.46);
		gen->set_width(54.02e-6);
	}
	else if(istate==2)
	{
		// Upsilon(2S)
		nEvents = 2205;
		gen->set_mass(10.0233);
		gen->set_width(31.98e-6);
	}
	else
	{
		// Upsilon(3S)
		nEvents = 1155;
		gen->set_mass(10.3552);
		gen->set_width(20.32e-6);
	}
	se->registerSubsystem(gen);
	
	
	
	G4Setup(absorberactive, magfield);
	
	// ****************************************************************
	// SVX_CELL
	
	
	
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
	
	// ****************************************************************
	
	
	// ****************************************************************
	// SVX_TRACK
  
  PHG4SvtxDigitizer* digi = new PHG4SvtxDigitizer();
  digi->Verbosity(0);
  for(i=0;i<n_svx_layer;++i)
  {
    digi->set_adc_scale(i, 255, 1.0e-6);
  }
  for(i=n_svx_layer;i<62;++i)
  {
    digi->set_adc_scale(i, 10000, 1.0e0);
  }
  se->registerSubsystem( digi );
	
  PHG4SvtxDeadArea* deadarea = new PHG4SvtxDeadArea();
  deadarea->Verbosity(0);
  se->registerSubsystem( deadarea );
  
  PHG4SvtxThresholds* thresholds = new PHG4SvtxThresholds();
  thresholds->Verbosity(0);
  thresholds->set_threshold(0.0);
  thresholds->set_use_thickness_mip(0, true);
  se->registerSubsystem( thresholds );
  
	
  PHG4TPCClusterizer* clusterizer = new PHG4TPCClusterizer;
  se->registerSubsystem( clusterizer );

  PHG4HoughTransform* hough = new PHG4HoughTransform(62,58);
	hough->set_use_vertex(false);
	hough->setRemoveHits(true);
	hough->set_min_pT(0.2);
	
	hough->setBinScale(0.9);
	hough->setZBinScale(0.9);
	
	hough->Verbosity(10);
	for(int i=0;i<n_svx_layer;++i)
	{
		hough->set_material(0, 0.013);
		hough->set_material(1, 0.013);
	}
	hough->set_material(n_svx_layer, 0.01);
	for(int i=n_svx_layer+1;i<62;++i)
	{
    hough->set_material(i, 0.06/60.);
	}
	hough->setUseCellSize(true);
	
  
	
	se->registerSubsystem( hough );
	
	// ****************************************************************
	
	stringstream ss;ss.clear();ss.str("");ss<<"g4_eval_ups_"<<istate<<".root";
	
	SubsysReco* eval = new PHG4Evaluator("PHG4EVALUATOR", ss.str().c_str());
	eval->Verbosity(0);
	se->registerSubsystem( eval );
	
	Fun4AllInputManager *in = new Fun4AllDummyInputManager( "JADE");
	se->registerInputManager( in );

	
	se->run(nEvents);
	
	se->End();
	std::cout << "All done" << std::endl;
	delete se;
	gSystem->Exit(0);
	
}
