// these are defined in the Fun4All macro, here we just override the values
// with what is used in this macro
Min_si_layer = 0;
Max_si_layer = 62;
double inner_cage_radius = 30.;
string tpcgas = "G4_Ar";
double vtx_radius[2] = { 2.71, 4.63 };
double vtx_length[2] = { 20., 20. };
double n_rad_length_vtx_silicon[2] = { 0.21e-02, 0.21e-02 };
double n_rad_length_vtx_copper[2] = {1.07e-02, 1.07e-02};

void SvtxInit()
{
	Min_si_layer = 0;
	Max_si_layer = 62;
}

double Svtx(PHG4Reco* g4Reco, double radius, const int absorberactive = 0)
{
	gSystem->Load("libg4detectors.so");
	gSystem->Load("libg4testbench.so");
	
	PHG4CylinderSubsystem *cyl;
	
	double silicon_radiation_length = 9.36;
	double copper_radiation_length = 1.43;
	double n_rad_length_vtx_silicon = 0.21e-02;
	double n_rad_length_vtx_copper = 1.07e-02;
	
	radius = 0.;
	for(int ilayer=0;ilayer<n_svx_layer;++ilayer)
	{
		cyl = new PHG4CylinderSubsystem("SVTX", ilayer);
		radius = vtx_radius[ilayer];
		cyl->SetRadius(radius);
		cyl->SetLength( vtx_length[ilayer] );
		cyl->SetMaterial("G4_Si");
		cyl->SetThickness(  silicon_radiation_length * n_rad_length_vtx_silicon[ilayer]  );
		cyl->SetActive();
		cyl->SuperDetector("SVTX");
		g4Reco->registerSubsystem( cyl );
		
		radius += silicon_radiation_length * n_rad_length_vtx_silicon[ilayer] + no_overlapp;
		
		cyl = new PHG4CylinderSubsystem("SVTXSUPPORT", ilayer);
		cyl->SetRadius(radius);
		cyl->SetLength(vtx_length[ilayer]);
		cyl->SetMaterial("G4_Cu");
		cyl->SetThickness( copper_radiation_length * n_rad_length_vtx_copper[ilayer] );
		cyl->SuperDetector("SVTXSUPPORT");
		g4Reco->registerSubsystem( cyl );
	}
	
	double n_rad_length_cage = 1.0e-02;
	double cage_length = 400.;
	cyl = new PHG4CylinderSubsystem("SVTXSUPPORT", 3);
	cyl->SetRadius(radius);
	cyl->SetLength(cage_length);
	cyl->SetMaterial("G4_Cu");
	cyl->SetThickness( copper_radiation_length * n_rad_length_cage );
	cyl->SuperDetector("SVTXSUPPORT");
	g4Reco->registerSubsystem( cyl );
	
	double outer_radius = 80.;
	int npoints = Max_si_layer - n_svx_layer;
	double delta_radius =  ( outer_radius - inner_cage_radius )/( (double)npoints );
	radius = inner_cage_radius + delta_radius;
	
  
	for(int ilayer=n_svx_layer;ilayer<(2+npoints);++ilayer)
  {
    cyl = new PHG4CylinderSubsystem("SVTX", ilayer);
    cyl->SetRadius(radius);
    cyl->SetLength( cage_length );
    cyl->SetMaterial(tpcgas.c_str());
    cyl->SetThickness(  delta_radius - 0.01 );
    cyl->SetActive();
    cyl->SuperDetector("SVTX");
    g4Reco->registerSubsystem( cyl );
    
    radius += delta_radius;
  }
	
	
	
	
	return radius;
}
