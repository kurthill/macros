int Min_cemc_layer = -1;
int Max_cemc_layer = -1;
int Min_si_layer = -1;
int Max_si_layer = -1;
int Cemc_slats_per_cell = 72; // make it 2*2*2*3*3 so we can try other combinations

int Fun4All_G4_tpc_plus_vtx_reco(
	int nEvents,
	int job_id,
	const char* inputfile,
	const char* outputfile
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
	
	
	
	// G4Setup(absorberactive, magfield);
	
	
	PHG4HoughTransform* hough = new PHG4HoughTransform(62,58);
	hough->set_use_vertex(true);
	hough->setRemoveHits(true);
	hough->set_min_pT(0.2);
	hough->set_chi2_cut_full( 3.0 );
	hough->set_chi2_cut_init( 3.0 );

	hough->setBinScale(0.9);
	hough->setZBinScale(0.9);

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
    hough->setVoteErrorScale(i, 0.1);
  }
  
	
	se->registerSubsystem( hough );
	
	
	stringstream ss;ss.clear();ss.str("");ss<<"g4_eval_"<<job_id<<".root";
	

	// SubsysReco* eval = new SvtxEvaluator("SVTXEVALUATOR", ss.str().c_str());
	//   eval->Verbosity(100);
	//   se->registerSubsystem( eval );


	Fun4AllInputManager *in = new Fun4AllDstInputManager( "DSTIN");
  	se->registerInputManager( in );
  	se->fileopen( in->Name(), inputfile );

  	Fun4AllDstOutputManager *out = new Fun4AllDstOutputManager("DSTOUT", outputfile);
  	se->registerOutputManager(out);
	
	se->run(nEvents);
	
	se->End();
	std::cout << "All done" << std::endl;
	delete se;
	gSystem->Exit(0);
	
}
