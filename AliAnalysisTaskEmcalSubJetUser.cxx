// Sub Jet task.
// Author: Yonghong ZHANG
#include <TClonesArray.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TH3F.h>
#include <TList.h>
#include <TLorentzVector.h>
#include "AliVCluster.h"
#include "AliAODCaloCluster.h"
#include "AliESDCaloCluster.h"
#include "AliVTrack.h"
#include "AliEmcalJet.h"
#include "AliRhoParameter.h"
#include "AliLog.h"
#include "AliJetContainer.h"
#include "AliParticleContainer.h"
#include "AliClusterContainer.h"
#include "AliPicoTrack.h"
#include "AliSubJetContainer.h"
#include "AliAnalysisTaskEmcalSubJetUser.h"

ClassImp(AliAnalysisTaskEmcalSubJetUser)

//________________________________________________________________________
AliAnalysisTaskEmcalSubJetUser::AliAnalysisTaskEmcalSubJetUser() : 
  AliAnalysisTaskEmcalSubJet("AliAnalysisTaskEmcalSubJetUser")
{
  // Default constructor.
  SetMakeGeneralHistograms(kTRUE);
}

//________________________________________________________________________
AliAnalysisTaskEmcalSubJetUser::AliAnalysisTaskEmcalSubJetUser(const char *name):
  AliAnalysisTaskEmcalSubJet(name)
{
  SetMakeGeneralHistograms(kTRUE);
}

//___________________________________________________________________________________
AliAnalysisTaskEmcalSubJetUser::AliAnalysisTaskEmcalSubJetUser(const char *name, Bool_t f) :
 AliAnalysisTaskEmcalSubJet(name,f)
{
 SetMakeGeneralHistograms(kTRUE);
}
//________________________________________________________________________
AliAnalysisTaskEmcalSubJetUser::~AliAnalysisTaskEmcalSubJetUser()
{
  // Destructor.
}

//________________________________________________________________________
void AliAnalysisTaskEmcalSubJetUser::UserCreateOutputObjects()
{
  AliAnalysisTaskEmcalSubJet::UserCreateOutputObjects();

  PostData(1, fOutput); // Post data for ALL output slots > 0 here.
}

//________________________________________________________________________
Bool_t AliAnalysisTaskEmcalSubJetUser::FillHistograms()
{
  AliAnalysisTaskEmcalSubJet::FillHistograms();

  CheckClusTrackMatching();
  return kTRUE;
}

//________________________________________________________________________
void AliAnalysisTaskEmcalSubJetUser::ExecOnce() {
  AliAnalysisTaskEmcalSubJet::ExecOnce();
}
//________________________________________________________________________
void AliAnalysisTaskEmcalSubJetUser::Ana(){
  AliAnalysisTaskEmcalSubJet::Ana();
  Yonghong2Xuwei(1);
}

void AliAnalysisTaskEmcalSubJetUser::Terminate(Option_t *) 
{
  // Called once at the end of the analysis.
}
