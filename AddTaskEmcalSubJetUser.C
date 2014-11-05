AliAnalysisTaskEmcalSubJetUser* AddTaskEmcalSubJetUser(
  const UInt_t subjettype         = AliAnalysisTaskEmcalSubJet::kAKT | AliAnalysisTaskEmcalSubJet::kFullJet,
  Bool_t       isMC               = kTRUE, 
  Bool_t       isesd              = kTRUE,
  Int_t        year               = 2012,
  const char   *ntracks           = "Tracks",
  const char   *nclusters         = "CaloClusters",
  const char   *njets             = "Jets",
  const char   *nsubjets          = "SubJets",
  const char   *nrho              = "Rho",
  Double_t     jetradius          = 0.4,
  Double_t     subjetradius       = 0.05,
  const Int_t  subrecombScheme    = 1,
  Double_t     jetMatchR          = 0.25,
  Double_t     jetptcut           = 5,
  Double_t     jetareacut         = 0.557,
  const char   *type              = "EMCAL",
  Int_t        leadhadtype        = 0,
  const char   *taskname          = "AliEmcalSubJet",
)
{  
  // Get the pointer to the existing analysis manager via the static access method.
  //==============================================================================
  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  if (!mgr)
  {
    ::Error("AddTaskFullJetPro", "No analysis manager to connect to.");
    return NULL;
  }    
  // Check the analysis type using the event handlers connected to the analysis manager.
  //==============================================================================
  if (!mgr->GetInputEventHandler())
  {
    ::Error("AddTaskFullJetPro", "This task requires an input event handler");
    return NULL;
  }
  
  TString name(taskname);
  if (strcmp(njets,"")) {
    name += "_";
    name += njets;
  }
  if (strcmp(nrho,"")) {
    name += "_";
    name += nrho;
  }
/*
  char recombSchemeString[200];
  if(recombScheme==0)
    sprintf(recombSchemeString,"%s","E_scheme");
  else if(recombScheme==1)
    sprintf(recombSchemeString,"%s","pt_scheme");
  else if(recombScheme==2)
    sprintf(recombSchemeString,"%s","pt2_scheme");
  else if(recombScheme==3)
    sprintf(recombSchemeString,"%s","Et_scheme");
  else if(recombScheme==4)
    sprintf(recombSchemeString,"%s","Et2_scheme");
  else if(recombScheme==5)
    sprintf(recombSchemeString,"%s","BIpt_scheme");
  else if(recombScheme==6)
    sprintf(recombSchemeString,"%s","BIpt2_scheme");
  else if(recombScheme==99)
    sprintf(recombSchemeString,"%s","ext_scheme");
  else {
    ::Error("AddTaskAliEmcalJet", "Recombination scheme not recognized.");
    return NULL;
  }
*/
  AliAnalysisTaskEmcalSubJetUser * jetTask = new AliAnalysisTaskEmcalSubJetUser(name,kTRUE);
  jetTask->SetCentRange(0.,100.);
  jetTask->SetSubJetRadius(0.1);
  jetTask->SetSubJetRadius(subjetradius);
  jetTask->SetSubRecombScheme(subrecombScheme);
  AliParticleContainer *trackCont  = jetTask->AddParticleContainer(ntracks);
  trackCont->SetClassName("AliVTrack");
  AliClusterContainer *clusterCont = jetTask->AddClusterContainer(nclusters);

  TString strType(type);

  AliJetContainer *jetCont    = jetTask->AddJetContainer(njets,strType,jetradius);
  AliJetContainer *subjetCont = jetTask->AddSubJetContainer(strType,subjetradius);  //due to the we find subjet here so we will not need the subjet name.

  if(jetCont) {
    jetCont->SetRhoName(nrho);
    jetCont->ConnectParticleContainer(trackCont);
    jetCont->ConnectClusterContainer(clusterCont);
    jetCont->SetZLeadingCut(0.98,0.98);
    jetCont->SetPercAreaCut(0.6);
    jetCont->SetJetPtCut(jetptcut);
    jetCont->SetLeadingHadronType(leadhadtype);
  }
  if(subjetCont){
    subjetCont->ConnectParticleContainer(trackCont);
    subjetCont->ConnectClusterContainer(clusterCont);
    subjetCont->SetZLeadingCut(0.98,0.98);
    subjetCont->SetPercAreaCut(0.6);
    subjetCont->SetJetPtCut(0.15);
  }
 
  mgr->AddTask(jetTask);

  AliAnalysisDataContainer *cinput1  = mgr->GetCommonInputContainer()  ;
  TString contname(name);
  contname += "_histos";
  AliAnalysisDataContainer *coutput1 = mgr->CreateContainer(contname.Data(), 
							    TList::Class(),AliAnalysisManager::kOutputContainer,
							    Form("%s", AliAnalysisManager::GetCommonFileName()));
  mgr->ConnectInput  (jetTask, 0,  cinput1 );
  mgr->ConnectOutput (jetTask, 1, coutput1 );
  
  return jetTask;
}
