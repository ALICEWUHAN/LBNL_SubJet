#ifndef ALISUBJETCONTAINER_H
#define ALISUBJETCONTAINER_H

class AliEmcalJetTask;
#include <iostream>
using namespace std;

class TClonesArray;
class TString;

class AliSubJetContainer : public AliJetContainer {

  public:

    AliSubJetContainer();
    AliSubJetContainer(const char * name);
    virtual ~AliSubJetContainer(){};
//    AliEmcalJet * GetNextAcceptJet(Int_t i=-1);
//
    Bool_t        AcceptJet(AliEmcalJet *jet) const;
    AliEmcalJet*  GetAcceptJet(Int_t i) const;
    AliEmcalJet*  GetNextAcceptJet(Int_t i=-1);

  protected:


   ClassDef(AliSubJetContainer, 1)
};
#endif

