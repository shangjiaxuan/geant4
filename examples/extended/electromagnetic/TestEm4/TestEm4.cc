// This code implementation is the intellectual property of
// the GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: TestEm4.cc,v 1.4 2000/03/09 11:27:37 gracia Exp $
// GEANT4 tag $Name: geant4-01-01 $
//
// 
// --------------------------------------------------------------
//      GEANT 4 - example Em4 
//
//      For information related to this code contact:
//      CERN, IT Division, ASD Group
// --------------------------------------------------------------
// Comments
//
// 
// --------------------------------------------------------------

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "Randomize.hh"

#ifdef G4VIS_USE
#include "Em4VisManager.hh"
#endif

#include "Em4DetectorConstruction.hh"
#include "Em4PhysicsList.hh"
#include "Em4PrimaryGeneratorAction.hh"
#include "Em4RunAction.hh"
#include "Em4EventAction.hh"
#include "Em4SteppingAction.hh"
#include "Em4SteppingVerbose.hh"

int main(int argc,char** argv) {
 
  //choose the Random engine
  HepRandom::setTheEngine(new RanecuEngine);

  //my Verbose output class
  G4VSteppingVerbose::SetInstance(new Em4SteppingVerbose);
  
  // Construct the default run manager
  G4RunManager * runManager = new G4RunManager;

  // set mandatory initialization classes
  runManager->SetUserInitialization(new Em4DetectorConstruction);
  runManager->SetUserInitialization(new Em4PhysicsList);
  
#ifdef G4VIS_USE
  // visualization manager
  G4VisManager* visManager = new Em4VisManager;
  visManager->Initialize();
#endif
    
  // set user action classes
  runManager->SetUserAction(new Em4PrimaryGeneratorAction);
  Em4RunAction* RunAct     = new Em4RunAction;
  runManager->SetUserAction(RunAct);
  Em4EventAction* EvAct    = new Em4EventAction(RunAct);  
  runManager->SetUserAction(EvAct);
  runManager->SetUserAction(new Em4SteppingAction(EvAct));
  
  //Initialize G4 kernel
  runManager->Initialize();
    
  // get the pointer to the User Interface manager 
    G4UImanager* UI = G4UImanager::GetUIpointer();  

  if (argc==1)   // Define UI terminal for interactive mode  
    { 
     G4UIsession * session = new G4UIterminal;
     UI->ApplyCommand("/control/execute init.mac");    
     session->SessionStart();
     delete session;
    }
  else           // Batch mode
    { 
     G4String command = "/control/execute ";
     G4String fileName = argv[1];
     UI->ApplyCommand(command+fileName);
    }

  // job termination
#ifdef G4VIS_USE
  delete visManager;
#endif
  delete runManager;

  return 0;
}
