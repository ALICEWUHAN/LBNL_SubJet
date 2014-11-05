#ifndef ALIANALYSISTASKEMCALSUBJET_H
#define ALIANALYSISTASKEMCALSUBJET_H

// $Id$
#include "AliAnalysisTaskEmcalJet.h"
#include "AliEmcalJetFinder.h"
#include <iostream>
using namespace std;

class TH1;
class TH2;
class TH3;
class AliJetContainer;
class AliParticleContainer;
class AliClusterContainer;
class AliEmcalJetFinder;
class AliSubJetContainer;
class AliFJWrapper;

Double_t DeltaR(Double_t et1,Double_t ph1,Double_t et2,Double_t ph2)
{
  Double_t eta=TMath::Abs(et1-et2);
  Double_t phi=TMath::Abs(ph1-ph2);
  if(phi>=TMath::TwoPi())phi = phi-TMath::TwoPi();
  if(phi>TMath::Pi())phi = phi-TMath::Pi();
  return TMath::Sqrt(eta*eta+phi*phi);
};
class AliAnalysisTaskEmcalSubJet : public AliAnalysisTaskEmcalJet { public:
  enum SubJetType{
    kNone=0,
    kKT=1<<0,
    kAKT=1<<1,
    kFullJet=1<<2,
    kChargedJet=1<<3,
    kNeutralJet=1<<4,
    kR002Jet=1<<5,
    kR005Jet=1<<6,
    kR010Jet=1<<7,
    kRX1Jet=1<<8,  // user defined radii, use SetRadius(Double_t)
    kRX2Jet=1<<9,
    kRX3Jet=1<<10
  };

  AliAnalysisTaskEmcalSubJet(Bool_t fhist);
  AliAnalysisTaskEmcalSubJet(const char *name,Bool_t fhist);
  virtual ~AliAnalysisTaskEmcalSubJet();

  void                        UserCreateOutputObjects();
  void                        Terminate(Option_t *option);
  void                        SetSubJetsName(char * name) {   fSubJetsName=name;      } 
  void                        SetSubJetType(UInt_t t)     {   fSubJetType =t;         }
  void                        SetSubJetRadius(Double_t r) {   fSubJetRadius = r;    if ((fSubJetType & (kRX1Jet|kRX2Jet|kRX3Jet)) == 0) AliWarning("Radius value will be ignored if jet type is not set to a user defined radius (kRX1Jet,kRX2Jet,kRX3Jet).");   }
  void                        SetSubJetAlgo(Int_t a)      {   if(a==0) fSubJetType|= kAKT;else if(a==1) fSubJetType|= kKT;} 
  void                        SetType(Int_t t)            {   if(t==0) fSubJetType|= kFullJet;  else if (t==1) fSubJetType |= kChargedJet; else if (t==2) fSubJetType |= kNeutralJet;}
  void                        SetSubJetGhost(Double_t gr) {   fSubJetGhost=gr;           } 
  void                        SetSubRecombScheme(Int_t s) {   fSubJetRecombScheme=s;     } 
  void                        SetTrimming(Bool_t k)       {   fTrimming=k;               }
  void                        SetPurning(Bool_t p)        {   fPurning=p;                }
  void                        SetHistos(Bool_t m)         {   fHistoFlag=m;              }
  void                        SetSubFJWrapper(AliFJWrapper &fjw);
  void                        SetSubJetInfo(vector<fastjet::PseudoJet> &constituents, AliEmcalJet *jet,fastjet::PseudoJet area,Double_t vertex[3],Int_t jetCount);
  Double_t                    GetSubJetRadius()           {   return fSubJetRadius;      }
  Double_t                    GetSubJetGhost()            {   return fSubJetGhost;       }
  TString                     GetSubJetName()             {   return fSubJetsName;       }
  Int_t                       GetSubRecombScheme()        {   return fSubJetRecombScheme;}
  UInt_t                      GetSubJetType()             {   return fSubJetType;        }
  AliSubJetContainer          *AddSubJetContainer(TString defaultCutType = "", Float_t jetRadius = 0.4);   
  AliSubJetContainer          *GetSubJetContainer(Int_t i) const;
  AliSubJetContainer          *GetSubJetContainer(const char* name)   const;
  Bool_t                      AddSubJetToBranch();                                        //! Add The SubJet to the Event
  Bool_t                      ConnectSubJetConstituents();                                //! Connect the SubJet to Clusters and Tracks
  void                        FindSubJets( AliJetContainer *jetcon);                      //! Fill the SubJet Into the fSubJets
  void                        FindSubJets( AliFJWrapper &fjw, AliEmcalJet *jet, Int_t index, Int_t jetCount); //! find subjet in each jet
  void                        FindSubJets(AliEmcalJet *jet, Int_t index, Int_t jetCount); //! find subjet in each jet
  void                        SelectSubJets();                                            //! Select Subjet  
  Bool_t                      CheckDoSubJet(AliEmcalJet *jet);                            //! Check the jet ever do the Subjet find or not
  virtual void                Ana(); 
  virtual void                FillSubJets()        {;}
  virtual void                FillSubJetsTrack( )  {;}
  virtual void                FillSubJetsCluster() {;} 
  virtual AliEmcalJet *       Trimming(AliEmcalJet *jet,Int_t index);
  virtual AliEmcalJet *       Purning(AliEmcalJet *jet,Int_t index);
  virtual void                Yonghong2Xuwei(int)  {;}
 protected:
  void                        ExecOnce();
  Bool_t                      FillHistograms()   ;
  Bool_t                      Run()              ;   
  void                        CheckClusTrackMatching();
  // General histograms
  TH1D  *fHJetSpectrum;
  TH2D  *fHJetSpectrumEtaPhi;
  TH1D  *fHFJetSpectrum;             //!FillHisto
  TH2D  *fHFJetSpectrumEtaPhi;       //!
  TH1D  *fHSubJetSpectrum;
  TH2D  *fHSubJetSpectrumEtaPhi;
  TH1D  *fHCSubJetSpectrum;         //!Check Mapping
  TH2D  *fHCSubJetSpectrumEtaPhi;   //!Check Mapping

  TH2D  *fHNumberOfJets;
  TH1D  *fHZeta;
  TH2D  *fHDeltaR;

  AliJetContainer            *fJetsCont;                  //!Jets
  AliParticleContainer       *fTracksCont;                //!Tracks
  AliClusterContainer        *fCaloClustersCont;          //!Clusters  
  vector<AliEmcalJet*>       fSubJetArray;                // Internal array for the subjets 
  TObjArray                  fSubJetCollArray;            // zyh subjet collection array
  Double_t                   fSubJetGhost;                //!subjet ghost area;
  UInt_t                     fSubJetType;                 //!subjet Type will used as AliEmcalTask
  Double_t                   fSubJetRadius;               //!subjet radius;
  Int_t                      fSubJetRecombScheme;         //!subjet recombine Scheme;
  TString                    fSubJetsName;                //!subjet name 
  TClonesArray               *fSubJets;                   //!subjet collection which will add to branch
  Bool_t                     fTrimming;                   //! flag for Trimming;
  Bool_t                     fPurning;                    //! flag for Purning
  Bool_t                     fHistoFlag;                  //! flag for histrogram;
  AliSubJetContainer         *fSubJetsCont;               //! SubJets!
  Double_t                   fvertex[3];

 private:
  AliAnalysisTaskEmcalSubJet(const AliAnalysisTaskEmcalSubJet&);            // not implemented
  AliAnalysisTaskEmcalSubJet &operator=(const AliAnalysisTaskEmcalSubJet&); // not implemented
  ClassDef(AliAnalysisTaskEmcalSubJet, 1) // jet sample analysis task
};
#endif
