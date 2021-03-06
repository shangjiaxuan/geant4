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

// Author: Ivana Hrivnacova, 20/07/2017 (ivana@ipno.in2p3.fr)

#include "G4Hdf5AnalysisManager.hh"
#include "G4Hdf5FileManager.hh"
#include "G4Hdf5NtupleFileManager.hh"
#include "G4AnalysisManagerState.hh"
#include "G4AnalysisUtilities.hh"
#include "G4Threading.hh"
#include "G4AutoLock.hh"

using namespace G4Analysis;

// mutex in a file scope

namespace {
  //Mutex to lock master manager when opening a file
  G4Mutex openFileMutex = G4MUTEX_INITIALIZER;
  //Mutex to lock master manager when merging H1 histograms 
  G4Mutex mergeH1Mutex = G4MUTEX_INITIALIZER;
  //Mutex to lock master manager when merging H1 histograms 
  G4Mutex mergeH2Mutex = G4MUTEX_INITIALIZER;
  //Mutex to lock master manager when merging H1 histograms 
  G4Mutex mergeH3Mutex = G4MUTEX_INITIALIZER;
  //Mutex to lock master manager when merging P1 profiles
  G4Mutex mergeP1Mutex = G4MUTEX_INITIALIZER;
  //Mutex to lock master manager when merging P2 profiles
  G4Mutex mergeP2Mutex = G4MUTEX_INITIALIZER;
  //Mutex to lock master manager when closing a file
  G4Mutex closeFileMutex = G4MUTEX_INITIALIZER;
}  

G4Hdf5AnalysisManager* G4Hdf5AnalysisManager::fgMasterInstance = nullptr;
G4ThreadLocal G4Hdf5AnalysisManager* G4Hdf5AnalysisManager::fgInstance = nullptr;

//_____________________________________________________________________________
G4Hdf5AnalysisManager* G4Hdf5AnalysisManager::Instance()
{
  if ( fgInstance == nullptr ) {
    G4bool isMaster = ! G4Threading::IsWorkerThread();
    fgInstance = new G4Hdf5AnalysisManager(isMaster);
  }
  
  return fgInstance;
}

//_____________________________________________________________________________
G4bool G4Hdf5AnalysisManager::IsInstance()
{
  return ( fgInstance != 0 );
}    

//_____________________________________________________________________________
G4Hdf5AnalysisManager::G4Hdf5AnalysisManager(G4bool isMaster)
 : G4ToolsAnalysisManager("Hdf5", isMaster),
   fNtupleFileManager(nullptr),
   fFileManager(nullptr)
{
#ifdef G4MULTITHREADED
#ifndef H5_HAVE_THREADSAFE
    G4ExceptionDescription message;
    message 
      << "Your HDF5 lib is not built with H5_HAVE_THREADSAFE.";
    G4Exception("G4Hdf5AnalysisManager::G4Hdf5AnalysisManager",
                "Analysis_F002", FatalException, message);
#endif
#endif

  if ( ( isMaster && fgMasterInstance ) || ( fgInstance ) ) {
    G4ExceptionDescription description;
    description 
      << "      " 
      << "G4Hdf5AnalysisManager already exists." 
      << "Cannot create another instance.";
    G4Exception("G4Hdf5AnalysisManager::G4Hdf5AnalysisManager",
                "Analysis_F001", FatalException, description);
  }              
  if ( isMaster ) fgMasterInstance = this;
  fgInstance = this;
  
  // File manager
  fFileManager = std::make_shared<G4Hdf5FileManager>(fState);
  SetFileManager(fFileManager);
  fFileManager->SetBasketSize(fgkDefaultBasketSize);

  // Ntuple file manager
  fNtupleFileManager = std::make_shared<G4Hdf5NtupleFileManager>(fState);
  fNtupleFileManager->SetFileManager(fFileManager);
  fNtupleFileManager->SetBookingManager(fNtupleBookingManager);
}

//_____________________________________________________________________________
G4Hdf5AnalysisManager::~G4Hdf5AnalysisManager()
{  
  if ( fState.GetIsMaster() ) fgMasterInstance = nullptr;
  fgInstance = nullptr;
}

// 
// private methods
//

//_____________________________________________________________________________
G4bool G4Hdf5AnalysisManager::WriteH1()
{
  auto h1Vector = fH1Manager->GetH1Vector();
  auto hnVector = fH1Manager->GetHnVector();

  if ( ! h1Vector.size() ) return true;

  auto result = true;

  if ( ! G4Threading::IsWorkerThread() )  {
    result = WriteHn(h1Vector, hnVector, "h1");
  }  
  else {
    // The worker manager just adds its histograms to the master
    // This operation needs a lock
    G4AutoLock lH1(&mergeH1Mutex);
    fgMasterInstance->fH1Manager->AddH1Vector(h1Vector);
    lH1.unlock();
  }  
  
  return result;
}
 
//_____________________________________________________________________________
G4bool G4Hdf5AnalysisManager::WriteH2()
{
  auto h2Vector = fH2Manager->GetH2Vector();
  auto hnVector = fH2Manager->GetHnVector();

  if ( ! h2Vector.size() ) return true;

  auto result = true;
  
  if ( ! G4Threading::IsWorkerThread() )  {
    result = WriteHn(h2Vector, hnVector, "h2");
  }  
  else {
    // The worker manager just adds its histograms to the master
    // This operation needs a lock
    G4AutoLock lH2(&mergeH2Mutex);
    fgMasterInstance->fH2Manager->AddH2Vector(h2Vector);
    lH2.unlock();
  }  
  
  return result;
}

//_____________________________________________________________________________
G4bool G4Hdf5AnalysisManager::WriteH3()
{
  auto h3Vector = fH3Manager->GetH3Vector();
  auto hnVector = fH3Manager->GetHnVector();

  if ( ! h3Vector.size() ) return true;

  auto result = true;
  
  if ( ! G4Threading::IsWorkerThread() )  {
    result = WriteHn(h3Vector, hnVector, "h3");
  }  
  else {
    // The worker manager just adds its histograms to the master
    // This operation needs a lock
    G4AutoLock lH3(&mergeH3Mutex);
    fgMasterInstance->fH3Manager->AddH3Vector(h3Vector);
    lH3.unlock();
  }  
  
  return result;
}

//_____________________________________________________________________________
G4bool G4Hdf5AnalysisManager::WriteP1()
{
  auto p1Vector = fP1Manager->GetP1Vector();
  auto hnVector = fP1Manager->GetHnVector();

  if ( ! p1Vector.size() ) return true;

  auto result = true;
  
  if ( ! G4Threading::IsWorkerThread() )  {
    result = WritePn(p1Vector, hnVector, "p1");
  }  
  else {
    // The worker manager just adds its profiles to the master
    // This operation needs a lock
    G4AutoLock lP1(&mergeP1Mutex);
    fgMasterInstance->fP1Manager->AddP1Vector(p1Vector);
    lP1.unlock();
  }  
  
  return result;
}
    
//_____________________________________________________________________________
G4bool G4Hdf5AnalysisManager::WriteP2()
{
  auto p2Vector = fP2Manager->GetP2Vector();
  auto hnVector = fP2Manager->GetHnVector();

  if ( ! p2Vector.size() ) return true;

  auto result = true;
  
  if ( ! G4Threading::IsWorkerThread() )  {
    result = WritePn(p2Vector, hnVector, "p2");
  }  
  else {
    // The worker manager just adds its profiles to the master
    // This operation needs a lock
    G4AutoLock lP2(&mergeP2Mutex);
    fgMasterInstance->fP2Manager->AddP2Vector(p2Vector);
    lP2.unlock();
  }  
  
  return result;
}

//_____________________________________________________________________________
G4bool G4Hdf5AnalysisManager::Reset()
{
// Reset histograms and ntuple

  auto finalResult = true;

  auto result = G4ToolsAnalysisManager::Reset();
  finalResult = finalResult && result;
  
  result = fNtupleFileManager->Reset();
  finalResult = finalResult && result;
  
  return finalResult;
}  
 
// 
// protected methods
//

//_____________________________________________________________________________
G4bool G4Hdf5AnalysisManager::OpenFileImpl(const G4String& fileName)
{
  // Create ntuple manager(s)
  // and set it to base class which takes then their ownership
  SetNtupleManager(fNtupleFileManager->CreateNtupleManager());

  auto finalResult = true;

  G4AutoLock lock(&openFileMutex);
  auto result = fFileManager->OpenFile(fileName);
  finalResult = finalResult && result;

  result = fNtupleFileManager->ActionAtOpenFile(fFileManager->GetFullFileName());
  finalResult = finalResult && result;
  lock.unlock();

  return finalResult;
}  

//_____________________________________________________________________________
G4bool G4Hdf5AnalysisManager::WriteImpl() 
{
  auto finalResult = true;

#ifdef G4VERBOSE
  if ( fState.GetVerboseL4() ) {
    fState.GetVerboseL4()->Message("write", "files", "");
  }
#endif

  auto result = WriteH1();
  finalResult = finalResult && result;

  // H2
  result = WriteH2();
  finalResult = finalResult && result;
  
  // H3
  result = WriteH3();
  finalResult = finalResult && result;
  
  // P1
  result = WriteP1();
  finalResult = finalResult && result;
  
  // P2
  result = WriteP2();
  finalResult = finalResult && result;
  
  // Write ASCII if activated
  if ( IsAscii() ) {
    result = WriteAscii(fFileManager->GetFileName());
    finalResult = finalResult && result;
  }   

#ifdef G4VERBOSE
  if ( fState.GetVerboseL2() ) {
    fState.GetVerboseL2()->Message("write", "files", "", finalResult);
  }
#endif

  return finalResult;
}

//_____________________________________________________________________________
G4bool G4Hdf5AnalysisManager::CloseFileImpl(G4bool reset)
{
  auto finalResult = true;

  G4AutoLock lock(&closeFileMutex);
  auto result = fFileManager->CloseFiles();
  finalResult = finalResult && result;

  if ( reset ) {
    // reset data
    result = Reset();
    if ( ! result ) {
      G4ExceptionDescription description;
      description << "      " << "Resetting data failed";
      G4Exception("G4Hdf5AnalysisManager::CloseFile()",
                "Analysis_W021", JustWarning, description);
    }
  }
  finalResult = finalResult && result;

  result = fNtupleFileManager->ActionAtCloseFile(reset);
  finalResult = finalResult && result;

  lock.unlock();

  return finalResult; 
}
