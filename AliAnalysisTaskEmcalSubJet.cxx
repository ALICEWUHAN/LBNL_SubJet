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
#include "AliFJWrapper.h"
#include "AliEmcalJetTask.h"
#include "AliSubJetContainer.h"
#include "AliAnalysisTaskEmcalSubJet.h"

ClassImp(AliAnalysisTaskEmcalSubJet)

//________________________________________________________________________
AliAnalysisTaskEmcalSubJet::AliAnalysisTaskEmcalSubJet(Bool_t fhist) : 
  AliAnalysisTaskEmcalJet("AliAnalysisTaskEmcalSubJet", kTRUE),
  fJetsCont(0),
  fTracksCont(0),
  fCaloClustersCont(0),
  fSubJetArray(0),
  fSubJetCollArray(),
  fSubJetGhost(0.005),
  fSubJetType(kNone),
  fSubJetRadius(0.1),
  fSubJetRecombScheme(fastjet::pt_scheme), 
  fSubJetsName("SubJets"),
  fSubJets(0), 
  fTrimming(0),
  fPurning(0),
  fHistoFlag(fhist),
  fSubJetsCont(0)
{
  fSubJetCollArray.SetOwner(kTRUE);
  // Default constructor.
  SetMakeGeneralHistograms(kTRUE);
}

//________________________________________________________________________
AliAnalysisTaskEmcalSubJet::AliAnalysisTaskEmcalSubJet(const char *name,Bool_t fhist) : 
  AliAnalysisTaskEmcalJet(name, kTRUE),
  fJetsCont(0),
  fTracksCont(0),
  fCaloClustersCont(0), 
  fSubJetArray(0),
  fSubJetCollArray(),
  fSubJetGhost(0.005),
  fSubJetType(kAKT|kFullJet|kRX1Jet),
  fSubJetRadius(0.1),
  fSubJetRecombScheme(fastjet::pt_scheme),
  fSubJetsName("SubJets"),
  fSubJets(0),
  fTrimming(0),
  fPurning(0),
  fHistoFlag(fhist),
  fSubJetsCont(0)
{
  fSubJetCollArray.SetOwner(kTRUE);
  // Standard constructor.
  SetMakeGeneralHistograms(kTRUE);
}

//________________________________________________________________________
AliAnalysisTaskEmcalSubJet::~AliAnalysisTaskEmcalSubJet()
{
  // Destructor.
}

//________________________________________________________________________
void AliAnalysisTaskEmcalSubJet::UserCreateOutputObjects()
{
  // Create user output.

  AliAnalysisTaskEmcalJet::UserCreateOutputObjects();
  fSubJets = new TClonesArray("AliEmcalJet");
  fSubJets->SetName(fSubJetsName);

  fJetsCont           = GetJetContainer(0);
  if(fJetsCont) { //get particles and clusters connected to jets
    fTracksCont       = fJetsCont->GetParticleContainer();
    fCaloClustersCont = fJetsCont->GetClusterContainer();
  } else {        //no jets, just analysis tracks and clusters
    fTracksCont       = GetParticleContainer(0);
    fCaloClustersCont = GetClusterContainer(0);
  }
  fTracksCont->SetClassName("AliVTrack");
  fCaloClustersCont->SetClassName("AliVCluster");

//  fSubJetsCont  = GetSubJetContainer(0);

  TString histname;
//*******************************************************
  if(fHistoFlag){
    fHSubJetSpectrum= new TH1D("subjet","",100,0,100);
    fOutput->Add(fHSubJetSpectrum);
    fHSubJetSpectrumEtaPhi=new TH2D("subjetetaphi","",200,-1,1,360,0,TMath::TwoPi());
    fOutput->Add(fHSubJetSpectrumEtaPhi);
    
    fHNumberOfJets= new TH2D("numberofSubjets","",200,0,200,50,0,50);
    fOutput->Add(fHNumberOfJets);
    fHZeta= new TH1D("z_distr","",100,0,1);
    fOutput->Add(fHZeta);
    fHDeltaR= new TH2D("DeltaR","",200,0,200,60,0,0.6);
    fOutput->Add(fHDeltaR);
  }
//*************************************************
  PostData(1, fOutput); // Post data for ALL output slots > 0 here.
}
//________________________________________________________________________
Bool_t AliAnalysisTaskEmcalSubJet::FillHistograms()
{
  if (fTracksCont) {
    AliVTrack *track = static_cast<AliVTrack*>(fTracksCont->GetNextAcceptParticle(0)); 
    while(track) {
      track = static_cast<AliVTrack*>(fTracksCont->GetNextAcceptParticle());
    }
  }
  
  if (fCaloClustersCont) {
    AliVCluster *cluster = fCaloClustersCont->GetNextAcceptCluster(0); 
    while(cluster) {
      TLorentzVector nPart;
      cluster->GetMomentum(nPart, fVertex);

      cluster = fCaloClustersCont->GetNextAcceptCluster();
    }
  }

  if (fJetsCont && fJetsCont->GetNJets()) {
    AliEmcalJet *jet = fJetsCont->GetNextAcceptJet(0); 
    while(jet) {
      if(jet->GetMotherIndex()==-2){
         fHNumberOfJets->Fill(jet->Pt(),jet->GetNumberOfSubJets());           
      }
      jet = fJetsCont->GetNextAcceptJet();
    }    
  }

  if(fHistoFlag){
    if(fSubJetsCont && fSubJetsCont->GetNJets()){
      AliEmcalJet *subjet = fSubJetsCont->GetNextAcceptJet(0);
      while(subjet){          
        fHSubJetSpectrum->Fill(subjet->Pt());
        fHSubJetSpectrumEtaPhi->Fill(subjet->Eta(),subjet->Phi());
        AliEmcalJet *motjet = dynamic_cast<AliEmcalJet*>(fJetsCont->GetJet(subjet->GetMotherIndex()));
        Double_t    z       = subjet->Pt()/motjet->Pt();
        Double_t    deltaR  = DeltaR(subjet->Eta(),subjet->Phi(),motjet->Eta(),motjet->Phi()); 
        fHDeltaR->Fill(motjet->Pt(),deltaR);
        fHZeta->Fill(z);
        subjet =fSubJetsCont->GetNextAcceptJet();
      }
    }
  } 
  CheckClusTrackMatching();

  return kTRUE;
}

//________________________________________________________________________
void AliAnalysisTaskEmcalSubJet::CheckClusTrackMatching()
{
  
  if(!fTracksCont || !fCaloClustersCont)
    return;
  Double_t deta = 999;
  Double_t dphi = 999;

  AliVTrack *track = static_cast<AliVTrack*>(fTracksCont->GetNextAcceptParticle(0)); 
  while(track) {
    //Get matched cluster
    Int_t emc1 = track->GetEMCALcluster();
    if(fCaloClustersCont && emc1>=0) {
      AliVCluster *clusMatch = fCaloClustersCont->GetCluster(emc1);
      if(clusMatch) {
	AliPicoTrack::GetEtaPhiDiff(track, clusMatch, dphi, deta);
      }
    }
    track = static_cast<AliVTrack*>(fTracksCont->GetNextAcceptParticle());
  }
  
  //Get closest track to cluster
  AliVCluster *cluster = fCaloClustersCont->GetNextAcceptCluster(0); 
  while(cluster) {
    TLorentzVector nPart;
    cluster->GetMomentum(nPart, fVertex);
    
    //Get matched track
    AliVTrack *mt = NULL;      
    AliAODCaloCluster *acl = dynamic_cast<AliAODCaloCluster*>(cluster);
    if(acl) {
      if(acl->GetNTracksMatched()>1)
	mt = static_cast<AliVTrack*>(acl->GetTrackMatched(0));
    }
    else {
      AliESDCaloCluster *ecl = dynamic_cast<AliESDCaloCluster*>(cluster);
      Int_t im = ecl->GetTrackMatchedIndex();
      if(fTracksCont && im>=0) {
	mt = static_cast<AliVTrack*>(fTracksCont->GetParticle(im));
      }
    }
    if(mt) {
      AliPicoTrack::GetEtaPhiDiff(mt, cluster, dphi, deta);
      
      /* //debugging
	 if(mt->IsEMCAL()) {
	 Int_t emc1 = mt->GetEMCALcluster();
	 Printf("current id: %d  emc1: %d",fCaloClustersCont->GetCurrentID(),emc1);
	 AliVCluster *clm = fCaloClustersCont->GetCluster(emc1);
	 AliPicoTrack::GetEtaPhiDiff(mt, clm, dphi, deta);
	 Printf("deta: %f dphi: %f",deta,dphi);
	 }
      */
    }
    cluster = fCaloClustersCont->GetNextAcceptCluster();
  }
}

//________________________________________________________________________
void AliAnalysisTaskEmcalSubJet::ExecOnce() {
  AliAnalysisTaskEmcalJet::ExecOnce();
  if(!AddSubJetToBranch()) { cout <<"Add SubJet Branch Failed"<<endl;}
  for(Int_t i =0; i<fSubJetCollArray.GetEntriesFast(); i++) {
    AliJetContainer *cont = static_cast<AliJetContainer*>(fSubJetCollArray.At(i));
    cont->SetRunNumber(InputEvent()->GetRunNumber());
    cont->SetEMCALGeometry();
    cont->SetArray(InputEvent());
  }
  if (fJetsCont && fJetsCont->GetArray() == 0)       fJetsCont = 0;
  if (fTracksCont && fTracksCont->GetArray() == 0)   fTracksCont = 0;
  if (fCaloClustersCont && fCaloClustersCont->GetArray() == 0) fCaloClustersCont = 0;
  if(InputEvent()->GetPrimaryVertex()) InputEvent()->GetPrimaryVertex()->GetXYZ(fvertex);
}
//________________________________________________________________________

Bool_t AliAnalysisTaskEmcalSubJet::Run()
{
  FindSubJets(fJetsCont);
  Ana();
  return kTRUE;  // If return kFALSE FillHistogram() will NOT be executed.
}
//______________________________________________________________________________________

AliSubJetContainer* AliAnalysisTaskEmcalSubJet::GetSubJetContainer(Int_t i) const{
  if(i<0 || i>=fSubJetCollArray.GetEntriesFast()) return 0;
  AliSubJetContainer *cont = static_cast<AliSubJetContainer*>(fSubJetCollArray.At(i));
  return cont;
}
//_______________________________________________________________________________________

AliSubJetContainer* AliAnalysisTaskEmcalSubJet::GetSubJetContainer(const char* name) const{
  AliSubJetContainer *cont = static_cast<AliSubJetContainer*>(fSubJetCollArray.FindObject(name));
  return cont;
}
//_______________________________________________________________________________________

AliSubJetContainer* AliAnalysisTaskEmcalSubJet::AddSubJetContainer( TString defaultCutType, Float_t jetRadius) {
  TString tmp = TString(fSubJetsName);
  if(tmp.IsNull()) return 0;

  AliSubJetContainer *cont = 0x0;
  cont = new AliSubJetContainer();
  cont->SetArrayName(fSubJetsName);
  cont->SetJetRadius(jetRadius);

  defaultCutType.ToUpper();

  if(!defaultCutType.IsNull() && !defaultCutType.EqualTo("USER")) {
    if(defaultCutType.EqualTo("TPC"))
      cont->SetJetAcceptanceType(AliJetContainer::kTPC);
    else if(defaultCutType.EqualTo("EMCAL"))
      cont->SetJetAcceptanceType(AliJetContainer::kEMCAL);
    else
      AliWarning(Form("%s: default cut type %s not recognized. Not setting cuts.",GetName(),defaultCutType.Data()));
  } else
    cont->SetJetAcceptanceType(AliJetContainer::kUser);
  fSubJetCollArray.Add(cont);
  return cont;
}
//________________________________________________________________________

void AliAnalysisTaskEmcalSubJet::Terminate(Option_t *) 
{
  // Called once at the end of the analysis.
}
//____________________________________________________________________________

void AliAnalysisTaskEmcalSubJet::FindSubJets(AliJetContainer *jetcon){ 
  Int_t jetCount=0;
  Int_t jetIndex=0;
  
  TString name("subjet_fjwrapper"); 
  AliFJWrapper fjw(name, name);
  SetSubFJWrapper(fjw);

  if(!jetcon){ cout<<"cant get the Jets Continer"<<endl;
  }else{
     AliEmcalJet *jet = jetcon->GetNextAcceptJet(0); 
     while(jet) {
       if(CheckDoSubJet(jet))continue;
       jetIndex=jetcon->GetCurrentID();       
       fjw.Clear();
       FindSubJets(fjw,jet,jetIndex,jetCount); 
       jet->SetMotherIndex(-2);   // Here Set The DoSubJetFlag
       jetCount+= jet->GetNumberOfSubJets(); 
       jet = jetcon->GetNextAcceptJet();
    }
    if(!ConnectSubJetConstituents()) cout<<"Connect SubJets Constituents Failed"<<endl;
  }
}
//_________________________________________________________________________________-
Bool_t  AliAnalysisTaskEmcalSubJet::CheckDoSubJet(AliEmcalJet *jet){
  if(jet->GetMotherIndex()!=-2) return kFALSE;
  else {return kTRUE;}
}
//_________________________________________________________________________________
void AliAnalysisTaskEmcalSubJet::FindSubJets(AliFJWrapper &fjw, AliEmcalJet*jet, Int_t index, Int_t jetCount){  
  if(!jet||jet->GetMotherIndex()==-2) return;
  if((fSubJetType & kFullJet)||(fSubJetType & kChargedJet) ){
    for(Int_t i=0;i<jet->GetNumberOfTracks();i++){
       AliVParticle *t = jet->TrackAt(i, fTracks);  //asume the jet->TrackAt the golbel track it       
       if(!t) continue;
       fjw.AddInputVector(t->Px(), t->Py(), t->Pz(), t->E(), jet->TrackAt(i) + 100);
    }
  }
  if((fSubJetType & kFullJet) || (fSubJetType & kNeutralJet)){
    for(Int_t i=0;i<jet->GetNumberOfClusters();i++){
       AliVCluster *cl =jet->ClusterAt(i,fCaloClusters);
       if(!cl)continue;
       TLorentzVector nP;
       cl->GetMomentum(nP, fvertex);
       Double_t  cPx  = nP.Px();
       Double_t  cPy  = nP.Py();
       Double_t  cPz  = nP.Pz();
       Double_t    e  = TMath::Sqrt(cPx*cPx+cPy*cPy+cPz*cPz);
       fjw.AddInputVector(cPx, cPy, cPz, e, -(jet->ClusterAt(i) + 100));
    }
  }
  fjw.Run();
  vector<fastjet::PseudoJet> subjets_incl = fjw.GetInclusiveJets();
  Int_t ns=0;
  jet->SetNumberOfSubJets(subjets_incl.size());
  for(UInt_t i=0;i<subjets_incl.size();i++){
     if(subjets_incl[i].perp()<0.1) continue;  // exclude  the ghost jet
     AliEmcalJet *subjet = new ((*fSubJets)[jetCount])AliEmcalJet(subjets_incl[i].perp(), subjets_incl[i].eta(), subjets_incl[i].phi(), subjets_incl[i].m());
     subjet->SetLabel(jetCount);
     jet->AddSubJetAt(jetCount, ns);           // <========== jet =>subjet
     ns++;
     subjet->SetMotherIndex(index);             // <========== subjet =>Jet

     vector<fastjet::PseudoJet> constituents(fjw.GetJetConstituents(i));
     fastjet::PseudoJet area(fjw.GetJetAreaVector(i)); 
     SetSubJetInfo(constituents,subjet,area,fvertex,jetCount);
     jetCount++;
   }
   jet->SetNumberOfSubJets(ns);
}
//_______________________________________________________________________________________________________
void AliAnalysisTaskEmcalSubJet::FindSubJets(AliEmcalJet *jet, Int_t index, Int_t  jetCount) //jetCount which use to set the index in the subjetArray
{

  TString name("kt");
  fastjet::JetAlgorithm jalgo(fastjet::kt_algorithm);
  if ((fSubJetType & kAKT) != 0) {
    name  = "antikt";
    jalgo = fastjet::antikt_algorithm;
    AliDebug(1,"Using AKT algorithm");
  }
  else {
    AliDebug(1,"Using KT algorithm");
  }

  if ((fSubJetType & kR002Jet) != 0)
    fSubJetRadius = 0.02;
  else if ((fSubJetType & kR005Jet) != 0)
    fSubJetRadius = 0.05;
  else if ((fSubJetType & kR010Jet) != 0)
    fSubJetRadius = 0.1;
 
  if(!jet||jet->GetMotherIndex()==-2) return;
  AliFJWrapper fjw("anti-kt", "anti-kt");
  fjw.SetAreaType(fastjet::active_area_explicit_ghosts);
  fjw.SetGhostArea(fSubJetGhost);
  fjw.SetR(fSubJetRadius);
  fjw.SetAlgorithm(jalgo);
  fjw.SetRecombScheme(static_cast<fastjet::RecombinationScheme>(fastjet::pt_scheme));
  fjw.SetMaxRap(0.5);
  fjw.Clear();

  if(!jet||jet->GetMotherIndex()==-2) return;
   for(Int_t i=0;i<jet->GetNumberOfTracks();i++){
     AliVParticle *t = jet->TrackAt(i, fTracks);  //asume the jet->TrackAt the golbel track it       
     if(!t) continue;
      fjw.AddInputVector(t->Px(), t->Py(), t->Pz(), t->E(), jet->TrackAt(i) + 100);
   }
   fjw.Run();
   vector<fastjet::PseudoJet> subjets_incl = fjw.GetInclusiveJets();
   Int_t ns=0;
   jet->SetNumberOfSubJets(subjets_incl.size());
   for(UInt_t i=0;i<subjets_incl.size();i++){ 
      if(subjets_incl[i].perp()<0.1) continue;  // exclude  the ghost jet
      AliEmcalJet *subjet = new ((*fSubJets)[jetCount])AliEmcalJet(subjets_incl[i].perp(), subjets_incl[i].eta(), subjets_incl[i].phi(), subjets_incl[i].m());
      subjet->SetLabel(jetCount);
      jet->AddSubJetAt(jetCount, ns);           // <========== jet =>subjet
      ns++;
      subjet->SetMotherIndex(index);             // <========== subjet =>Jet
      vector<fastjet::PseudoJet> constituents(fjw.GetJetConstituents(i));
      subjet->SetNumberOfTracks(constituents.size());
      subjet->SetNumberOfClusters(constituents.size());

      Int_t nt            = 0;
      Int_t nc            = 0;
      Double_t neutralE   = 0;
      Double_t maxCh      = 0;
      Double_t maxNe      = 0;
      Int_t gall          = 0;
      Int_t gemc          = 0;
      Int_t cemc          = 0;
      Int_t ncharged      = 0;
      Int_t nneutral      = 0;
      Double_t mcpt       = 0;
      Double_t emcpt      = 0;

      AliEmcalJetTask c1;
      c1.FillJetConstituents(constituents,subjet,fvertex,jetCount,nt,nc,maxCh,maxNe,ncharged,nneutral,neutralE,mcpt,cemc,emcpt,gall,gemc);

      subjet->SetNumberOfTracks(nt);
      subjet->SetNumberOfClusters(nc);
      subjet->SortConstituents();
      subjet->SetMaxNeutralPt(maxNe);
      subjet->SetMaxChargedPt(maxCh);
      subjet->SetNEF(neutralE / subjet->E());
      fastjet::PseudoJet area(fjw.GetJetAreaVector(i));
      subjet->SetArea(area.perp());
      subjet->SetAreaEta(area.eta());
      subjet->SetAreaPhi(area.phi());
      subjet->SetNumberOfCharged(ncharged);
      subjet->SetNumberOfNeutrals(nneutral);
      subjet->SetMCPt(mcpt);
      subjet->SetNEmc(cemc);
      subjet->SetPtEmc(emcpt);
      jetCount++;
   }
   jet->SetNumberOfSubJets(ns); 
}
//________________________________________________________________________
Bool_t AliAnalysisTaskEmcalSubJet::AddSubJetToBranch(){

  if(!InputEvent()){
    AliError(Form("%s: Could not retrieve event! Returning", GetName()));
    return 0;
  }
  if (!(InputEvent()->FindListObject(fSubJetsName))){
     InputEvent()->AddObject(fSubJets);
     return 1;
   }
   else {
     AliError(Form("%s: Object with name %s already in event! Returning", GetName(), fSubJetsName.Data()));
     return 0;
   }
}
void AliAnalysisTaskEmcalSubJet::SelectSubJets()
{
}

void AliAnalysisTaskEmcalSubJet::Ana()
{ 
   if(fSubJets){
     ;
 //    cout<<"this is Ana!"<<endl;
  /*   for(int i=0;i<fSubJets->GetEntries();i++){
        AliEmcalJet *subjet = (AliEmcalJet*)fSubJets->At(i);
        if(!subjet) cout<<"can't get the the individual subjet"<<endl;
        fHSubJetSpectrum->Fill(subjet->Pt());
        if (subjet->Pt()>0.1) {
        fHSubJetSpectrumEtaPhi->Fill(subjet->Eta(),subjet->Phi());
        }
     }
   */
  }
}

//___________________________________________________________________________________________________________

AliEmcalJet  * AliAnalysisTaskEmcalSubJet::Trimming(AliEmcalJet *jet,Int_t index){
  if(!jet) return 0;  
//  Filter trimmerSoftb(subjDef, fastjet::SelectorPtFractionMin(dSubjTriPt));
//  AliEmcalJet *trimjet= trimmerSoftb(sortedSoftJets[j]);
//  return trimjet;
  //  std::vector<fastjet::PseudoJet> trimmdSoftSubja = trimmdSoftJeta.pieces();
  //  std::vector<fastjet::PseudoJet> sortedSoftSubja = fastjet::sorted_by_pt(trimmdSoftSubja);
  return 0;
}
AliEmcalJet *     AliAnalysisTaskEmcalSubJet::Purning(AliEmcalJet *jet,Int_t index){
  if(!jet) return 0;
  return 0;


}
//_____________________________________________________________________________
Bool_t  AliAnalysisTaskEmcalSubJet::ConnectSubJetConstituents()
{
  fSubJetsCont  = GetSubJetContainer(0);
//  fSubJetsCont->SetArray(InputEvent());   
//  fSubJetsCont  = GetSubJetContainer(0);
  if(!fSubJetsCont)  {       
    cout<<"not found subjetsCont"<<endl;
    return kFALSE;
  }
  else{
    fTracksCont       = fSubJetsCont->GetParticleContainer();
    fCaloClustersCont = fSubJetsCont->GetClusterContainer();
    return kTRUE;
  }   
}
//_______________________________________________________________
void  AliAnalysisTaskEmcalSubJet::SetSubFJWrapper(AliFJWrapper &fjw){

  fastjet::JetAlgorithm jalgo(fastjet::kt_algorithm);
  if ((fSubJetType & kAKT) != 0) {
    jalgo = fastjet::antikt_algorithm;
    AliDebug(1,"Using AKT algorithm");
  }
  else {
    AliDebug(1,"Using KT algorithm");
  }

  if ((fSubJetType & kR002Jet) != 0)
    fSubJetRadius = 0.02;
  else if ((fSubJetType & kR005Jet) != 0)
    fSubJetRadius = 0.05;
  else if ((fSubJetType & kR010Jet) != 0)
    fSubJetRadius = 0.1;

  fjw.SetAreaType(fastjet::active_area_explicit_ghosts);
  fjw.SetGhostArea(fSubJetGhost);
  fjw.SetR(fSubJetRadius);
  fjw.SetAlgorithm(jalgo);
  fjw.SetRecombScheme(static_cast<fastjet::RecombinationScheme>(fSubJetRecombScheme));
  fjw.SetMaxRap(0.5);
}
//__________________________________________________________________________________
void  AliAnalysisTaskEmcalSubJet::SetSubJetInfo(vector<fastjet::PseudoJet> & constituents,AliEmcalJet *subjet,fastjet::PseudoJet area,Double_t vertex[3],Int_t jetCount){  
  subjet->SetNumberOfTracks(constituents.size());
  subjet->SetNumberOfClusters(constituents.size());
  Int_t nt            = 0;
  Int_t nc            = 0;
  Double_t neutralE   = 0;
  Double_t maxCh      = 0;
  Double_t maxNe      = 0;
  Int_t gall          = 0;
  Int_t gemc          = 0;
  Int_t cemc          = 0;
  Int_t ncharged      = 0;
  Int_t nneutral      = 0;
  Double_t mcpt       = 0;
  Double_t emcpt      = 0;

  AliEmcalJetTask c1;
  c1.FillJetConstituents(constituents,subjet,vertex,jetCount,nt,nc,maxCh,maxNe,ncharged,nneutral,neutralE,mcpt,cemc,emcpt,gall,gemc);

  subjet->SetNumberOfTracks(nt);
  subjet->SetNumberOfClusters(nc);
  subjet->SortConstituents();
  subjet->SetMaxNeutralPt(maxNe);
  subjet->SetMaxChargedPt(maxCh);
  subjet->SetNEF(neutralE / subjet->E());
  subjet->SetArea(area.perp());
  subjet->SetAreaEta(area.eta());
  subjet->SetAreaPhi(area.phi());
  subjet->SetNumberOfCharged(ncharged);
  subjet->SetNumberOfNeutrals(nneutral);
  subjet->SetMCPt(mcpt);
  subjet->SetNEmc(cemc);
  subjet->SetPtEmc(emcpt);
}

