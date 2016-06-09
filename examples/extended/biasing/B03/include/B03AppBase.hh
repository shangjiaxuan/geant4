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
// $Id: B03AppBase.hh,v 1.2 2006/06/29 16:35:01 gunter Exp $
// GEANT4 tag $Name: geant4-08-01 $
//
// --------------------------------------------------------------------
#ifndef B03AppBase_hh
#define B03AppBase_hh B03AppBase_hh

class G4RunManager;
class B03PrimaryGeneratorAction;
class B03PhysicsList;
class B03DetectorConstruction;

class B03AppBase {
public:
  ~B03AppBase();
  static B03AppBase &GetB03AppBase();
  G4RunManager &GetRunManager(){return *frunMgr;}
  
private:
  B03AppBase();
  static B03AppBase *fB03AppBase;
  G4RunManager *frunMgr;

  B03DetectorConstruction *fDetector;
  B03PrimaryGeneratorAction *fPrimary;
  B03PhysicsList *fPhysics;
};

// B03AppBase *GetB03AppBase();



#endif









