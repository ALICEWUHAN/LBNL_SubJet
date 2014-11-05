//YonghongZhang

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
#include "AliEmcalJetTask.h"
#include "AliSubJetContainer.h"
#include "AliAnalysisManager.h"
#include "AliESDEvent.h"
#include "AliEmcalJetFinder.h"
#include "AliFJWrapper.h"
#include<iostream>
using namespace std;
ClassImp(AliSubJetContainer)


AliSubJetContainer::AliSubJetContainer():
AliJetContainer()
{
  cout<<"he"<<endl;

}
AliSubJetContainer::AliSubJetContainer(const char * name):
AliJetContainer()
{
  cout<<name<<endl;
}
Bool_t AliSubJetContainer::AcceptJet(AliEmcalJet *jet) const
{
  return kTRUE;
}
AliEmcalJet* AliSubJetContainer::GetAcceptJet(Int_t i) const
{
  AliEmcalJet *jet = GetJet(i);
  if(!AcceptJet(jet)) return 0;
  return jet;
}
AliEmcalJet*  AliSubJetContainer::GetNextAcceptJet(Int_t i)
{
  if (i>=0) fCurrentID = i;
  const Int_t njets = GetNEntries();
  AliEmcalJet *jet = 0;
  while (fCurrentID < njets && !jet) {
    jet = GetAcceptJet(fCurrentID);
    fCurrentID++;
  }
  return jet;
}


