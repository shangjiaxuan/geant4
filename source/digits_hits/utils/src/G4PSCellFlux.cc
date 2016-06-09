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
// $Id: G4PSCellFlux.cc,v 1.4 2006/06/29 18:07:36 gunter Exp $
// GEANT4 tag $Name: geant4-08-01 $
//
// G4PSCellFlux
#include "G4PSCellFlux.hh"
#include "G4Track.hh"
#include "G4VSolid.hh"
#include "G4UnitsTable.hh"

///////////////////////////////////////////////////////////////////////////////
// (Description)
//   This is a primitive scorer class for scoring cell flux.
//   The Cell Flux is defined by  a sum of track length divided 
//   by the geometry volume, where all of the tracks in the geometry 
//   are taken into account. 
//
//   If you want to score only tracks passing through the geometry volume,
//  please use G4PSPassageCellFlux.
//
//
// Created: 2005-11-14  Tsukasa ASO, Akinori Kimura.
// 
///////////////////////////////////////////////////////////////////////////////

G4PSCellFlux::G4PSCellFlux(G4String name, G4int depth)
    :G4VPrimitiveScorer(name,depth),HCID(-1)
{;}

G4PSCellFlux::~G4PSCellFlux()
{;}

G4bool G4PSCellFlux::ProcessHits(G4Step* aStep,G4TouchableHistory*)
{
    G4double stepLength = aStep->GetStepLength();
    if ( stepLength == 0. ) return FALSE;
    G4double volume     = aStep->GetPreStepPoint()->GetPhysicalVolume()
                         ->GetLogicalVolume()->GetSolid()->GetCubicVolume();
    G4double CellFlux = stepLength / volume ;
    CellFlux *= aStep->GetPreStepPoint()->GetWeight(); 
    G4int index = GetIndex(aStep);
    EvtMap->add(index,CellFlux);

  return TRUE;
}

void G4PSCellFlux::Initialize(G4HCofThisEvent* HCE)
{
  EvtMap = new G4THitsMap<G4double>(detector->GetName(),
				    GetName());
  if ( HCID < 0 ) HCID = GetCollectionID(0);
  HCE->AddHitsCollection(HCID,EvtMap);
}

void G4PSCellFlux::EndOfEvent(G4HCofThisEvent*)
{;}

void G4PSCellFlux::clear(){
  EvtMap->clear();
}

void G4PSCellFlux::DrawAll()
{;}

void G4PSCellFlux::PrintAll()
{
  G4cout << " MultiFunctionalDet  " << detector->GetName() << G4endl;
  G4cout << " PrimitiveScorer " << GetName() <<G4endl; 
  G4cout << " Number of entries " << EvtMap->entries() << G4endl;
  std::map<G4int,G4double*>::iterator itr = EvtMap->GetMap()->begin();
  for(; itr != EvtMap->GetMap()->end(); itr++) {
    G4cout << "  copy no.: " << itr->first
	   << "  cell flux : " << *(itr->second)*cm*cm << " [cm^-2]"
	   << G4endl;
  }
}
