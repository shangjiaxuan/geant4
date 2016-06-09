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
// $Id: HadronPhysicsLHEP_BERT.cc,v 1.6 2006/06/29 17:56:07 gunter Exp $
// GEANT4 tag $Name: geant4-08-01 $
//
//---------------------------------------------------------------------------
//
// ClassName: HadronPhysicsLHEP_BERT
//
// Author: 2002 J.P. Wellisch
//
// Modified:
//  1.12.2005 G.Folger: migration to non static particles
// 08.06.2006 V.Ivanchenko: remove stopping
// 20.06.2006 G.Folger: Bertini applies to Kaons, i.e. use SetMinEnergy instead of SetMinPionEnergy
//
//----------------------------------------------------------------------------
//
#include "HadronPhysicsLHEP_BERT.hh"

#include "globals.hh"
#include "G4ios.hh"
#include <iomanip>   
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"

#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4ShortLivedConstructor.hh"

HadronPhysicsLHEP_BERT::HadronPhysicsLHEP_BERT(const G4String& name)
                    :  G4VPhysicsConstructor(name) 
{}

void HadronPhysicsLHEP_BERT::CreateModels()
{
  theNeutrons=new G4NeutronBuilder;

  theNeutrons->RegisterMe(theLHEPNeutron=new G4LHEPNeutronBuilder);
  theNeutrons->RegisterMe(theBertiniNeutron=new G4BertiniNeutronBuilder);
  theLHEPNeutron->SetMinInelasticEnergy(9.5*GeV);
  theBertiniNeutron->SetMaxEnergy(9.9*GeV);

  thePro=new G4ProtonBuilder;
  thePro->RegisterMe(theLHEPPro=new G4LHEPProtonBuilder);
  thePro->RegisterMe(theBertiniPro=new G4BertiniProtonBuilder);
  theLHEPPro->SetMinEnergy(9.5*GeV);
  theBertiniPro->SetMaxEnergy(9.9*GeV);
  
  thePiK=new G4PiKBuilder;
  thePiK->RegisterMe(theLHEPPiK=new G4LHEPPiKBuilder);
  thePiK->RegisterMe(theBertiniPiK=new G4BertiniPiKBuilder);
  theLHEPPiK->SetMinEnergy(9.5*GeV);
  theBertiniPiK->SetMaxEnergy(9.9*GeV);

  theMiscLHEP=new G4MiscLHEPBuilder;
}

HadronPhysicsLHEP_BERT::~HadronPhysicsLHEP_BERT()
{
    delete theNeutrons;
    delete theLHEPNeutron;
    delete theBertiniNeutron;
    
    delete thePiK;
    delete theLHEPPiK;
    delete theBertiniPiK;
    
    delete thePro;
    delete theLHEPPro;
    delete theBertiniPro;
    
    delete theMiscLHEP;
}

void HadronPhysicsLHEP_BERT::ConstructParticle()
{
  G4MesonConstructor pMesonConstructor;
  pMesonConstructor.ConstructParticle();

  G4BaryonConstructor pBaryonConstructor;
  pBaryonConstructor.ConstructParticle();

  G4ShortLivedConstructor pShortLivedConstructor;
  pShortLivedConstructor.ConstructParticle();  
}

#include "G4ProcessManager.hh"
void HadronPhysicsLHEP_BERT::ConstructProcess()
{
  CreateModels();
  theNeutrons->Build();
  thePro->Build();
  thePiK->Build();
  theMiscLHEP->Build();
}
