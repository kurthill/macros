#include <string>

using namespace std;

void
Fun4All_scan(int nEvents = 0,
    const char *input_file = "ndst.list",
    const char *output_file = "out.root")
{
  gSystem->Load("libfun4all");
  gSystem->Load("libPrototype2.so");
  gSystem->Load("/gpfs/mnt/gpfs02/sphenix/sim/sim01/sPHENIX/sunywrk/abhisek/analysis/Prototype2/HCAL/ShowerCalib/build/lib/libProto2ShowCalib.so");

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(Fun4AllServer::VERBOSITY_SOME);

  recoConsts *rc = recoConsts::instance();
  //rc->set_IntFlag("RUNNUMBER",0);

  Proto2ShowerCalib *calib = new Proto2ShowerCalib(output_file);
  //calib->fill_time_samples( true );
  se->registerSubsystem( calib );

  Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTin");
  //in->fileopen(input_file);
  in->AddListFile(input_file);
  se->registerInputManager(in);

  se->run(nEvents);

  se->End();

}
