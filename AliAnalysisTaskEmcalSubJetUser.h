#ifndef ALIANALYSISTASKEMCALSUBJETUSER_H
#define ALIANALYSISTASKEMCALSUBJETUSER_H

// $Id$
class TH1;
class TH2;
class TH3;
class AliJetContainer;
class AliParticleContainer;
class AliClusterContainer;
class AliEmcalJetFinder;
class AliSubJetContainer;
#include "AliAnalysisTaskEmcalJet.h"
#include "AliAnalysisTaskEmcalSubJet.h"
#include "AliEmcalJetFinder.h"
#include <iostream>
using namespace std;
class AliAnalysisTaskEmcalSubJetUser : public AliAnalysisTaskEmcalSubJet {
 public:

  AliAnalysisTaskEmcalSubJetUser();
  AliAnalysisTaskEmcalSubJetUser(const char *name);
  AliAnalysisTaskEmcalSubJetUser(const char *name, Bool_t f);

  virtual ~AliAnalysisTaskEmcalSubJetUser();

  void                        UserCreateOutputObjects();
  void                        Terminate(Option_t *option);

  virtual void   Yonghong2Xuwei(int a) {  cout << "Mo bai 2 Xue wei" << endl; }
//  virtual void   UserExec(Option_t *opt) { /*AliAnalysisTaskEmcalSubJet::UserExec(opt);*/ ;  }

 protected:

  void                        ExecOnce();
  Bool_t                      FillHistograms()   ;
  void                        Ana();
  AliSubJetContainer          *fSubJetsCont;            //! SubJets!

//  Bool_t                      Run()   { AliAnalysisTaskEmcalJet::Run(); /*AliAnalysisTaskEmcalSubJet::Run();*/ this->Ana(); return kTRUE;}

 private:
  AliAnalysisTaskEmcalSubJetUser(const AliAnalysisTaskEmcalSubJetUser&);            // not implemented
  AliAnalysisTaskEmcalSubJetUser &operator=(const AliAnalysisTaskEmcalSubJetUser&); // not implemented

  ClassDef(AliAnalysisTaskEmcalSubJetUser, 1) // jet sample analysis task
};
#endif
