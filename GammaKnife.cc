//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file GammaKnife.cc
/// \brief Main program of the GammaKnife example
#include "G4Types.hh"

#include "GammaKnifeDetectorConstruction.hh"
#include "GammaKnifePhysicsList.hh"
#include "GammaKnifeActionInitialization.hh"
#include "G4SDManager.hh"
#include "GammaKnifeRunAction.hh"
#include "GammaKnifeEventAction.hh"

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "G4UImanager.hh"
#include "G4UIcommand.hh"

#include "Randomize.hh"


#include "Randomize.hh"
#include "G4ScoringManager.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

namespace {
  void PrintUsage() {
    G4cerr << " Usage: " << G4endl;
    G4cerr << " GammaKnife [-m macro ] [-u UIsession] [-t nThreads]" << G4endl;
    G4cerr << "   note: -t option is available only for multi-threaded mode."
           << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv)
{
  // Evaluate arguments
  //
  if ( argc > 7 ) {
    PrintUsage();
    return 1;
  }
  G4String nps;
  G4String macro;
  G4String session;
#ifdef G4MULTITHREADED
  G4int nThreads = 0;
#endif
  for ( G4int i=1; i<argc; i=i+2 ) {
    if      ( G4String(argv[i]) == "-m" ) macro = argv[i+1];
    else if ( G4String(argv[i]) == "-u" ) session = argv[i+1];
    else if (G4String(argv[i]) == "-nps") nps = argv[i + 1];
#ifdef G4MULTITHREADED
    else if ( G4String(argv[i]) == "-t" ) {
      nThreads = G4UIcommand::ConvertToInt(argv[i+1]);
  
    }
#endif
    else {
      PrintUsage();
      return 1;
    }
  }  
  
  // Detect interactive mode (if no macro provided) and define UI session
  //
  G4UIExecutive* ui = 0;
  if ( ! macro.size() ) {
    ui = new G4UIExecutive(argc, argv, session);
  }

  // Optionally: choose a different Random engine...
  //
  // G4Random::setTheEngine(new CLHEP::MTwistEngine);
  
  // Construct the default run manager
  //
#ifdef G4MULTITHREADED
  G4MTRunManager * runManager = new G4MTRunManager;
  if ( nThreads > 0 ) { 
    runManager->SetNumberOfThreads(nThreads);
  }  
#else
  G4RunManager * runManager = new G4RunManager;
#endif


  // Activate UI-command base scorer
  G4ScoringManager* scManager = G4ScoringManager::GetScoringManager();
  scManager->SetVerboseLevel(1);
 ////set mandatory initialization classes

  GammaKnifeDetectorConstruction* det = new GammaKnifeDetectorConstruction();
  runManager->SetUserInitialization(det);

  GammaKnifePhysicsList* phys = new GammaKnifePhysicsList;
  runManager->SetUserInitialization(phys);

  runManager->SetUserInitialization(new GammaKnifeActionInitialization());
  GammaKnifeRunAction *pRunAction = new GammaKnifeRunAction();
  runManager->SetUserAction(pRunAction);

  //initialize visualization
  G4VisManager* visManager = nullptr;
  visManager = new G4VisExecutive;
  visManager->Initialize();
  
  // Get the pointer to the User Interface manager
  auto UImanager = G4UImanager::GetUIpointer();
 // G4cout << " Ok. Checking!2" << G4endl; /// !!!!! vi tri 1

  // Process macro or start UI session
  //
  if ( macro.size() ) {
    // batch mode
    //G4String command0 = "/ run/numberOfThreads ";
    //UImanager->ApplyCommand(command0 + nThreads);
    G4String command = "/control/execute ";
    UImanager->ApplyCommand(command+macro);
    G4String command1 = "/run/beamOn ";
    UImanager->ApplyCommand(command1 + nps);
  }
  else  {  
    // interactive mode : define UI session
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    ui->SessionStart();
    delete ui;
  }

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not be deleted 
  // in the main() program !
  delete visManager;
  delete runManager;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
