#include <ctime>
#include "TGrid.h"
#include "iostream.h"
#include "TSystem.h"
#include "TROOT.h"
using namespace std;
const char *   ROOTV     ="v5-34-08-6";
const char *   AliRootV  ="vAN-20141007";
const char *   fastjetV  ="2"; // 2 means the 2.0.4 3 means 3



void SetupPar(char* pararchivename);
AliAnalysisGrid* CreateAlienHandler(const char* uniqueName,const char* period, const char* gridDir, const char* gridMode, const char* runNumbers, const char* pattern, TString additionalCode, TString additionalHeaders, Int_t maxFilesPerWorker,  Int_t workerTTL, Bool_t isMC);

void LoadLibs();                    
//__________________________________________________________________________________________________________
void runEmcalSubJet(
         const char*    dataType            = "ESD",                 // analysis type, AOD, ESD or sESD

         const char*    collisionType       = "pp",

   //     Bool_t         useGrid             = kTRUE,                // local or grid

         Bool_t         useGrid             = kFALSE,                // local or grid


         const char*    gridMode            = "full",               

         const char*    pattern             = "*/*AliESDs.root",     

         const char*    gridDir             = "/alice/sim/2012/LHC12a15a",   

         const char*    runNumbers          = "146805",              // considered run numbers 

         UInt_t         numLocalFiles       = 1,                     // number of files analyzed locally  

         const char*    runPeriod           = "LHC12a15a",       // set the run period (used on grid)

         const char*    uniqueName          = "Jet_Track_v2",     // sets base string for the name 

         Int_t          year                = 2011,
 
         UInt_t         pSel                = AliVEvent::kMB,        // used event

         Bool_t         useTender           = kTRUE,                 // trigger, tender task should be used

         Bool_t         isMC                = kTRUE,                 // trigger,  MC handler should be used

         Bool_t         PhySelMC            = kTRUE,      //  do physical selection for MC event or not

         Bool_t         isDeltaPt           = kTRUE,

         Bool_t         isResponse          = kTRUE,

         const char*    addCXXs             = "AliAnalysisTaskEmcalSubJet.cxx",
     
         const char*    addHs               = "AliAnalysisTaskEmcalSubJet.h",

         Int_t          maxFilesPerWorker   = 100,

         Int_t          workerTTL           = 7200

         )
{
  enum AlgoType {kKT, kANTIKT};
  enum JetType  {kFULLJETS, kCHARGEDJETS, kNEUTRALJETS};
  gSystem->SetFPEMask();
  gSystem->Setenv("ETRAIN_ROOT", ".");
  gSystem->Setenv("ETRAIN_PERIOD", runPeriod);
  TString usedData(dataType);

  Int_t runtype=-1;     //2 Pb-Pb  3 p-p 0 7TeV pp 
  if(strstr(collisionType,"pp"  )!=NULL)  runtype=1;
  if(strstr(collisionType,"PbPb")!=NULL)  runtype=2;
  if(strstr(collisionType,"pPb" )!=NULL)  runtype=3;
 
  TString additionalCXXs(addCXXs);
  TString additionalHs(addHs);
  cout << dataType << " analysis chosen" << endl;
  if (useGrid)  
  {
    cout << "-- using AliEn grid.\n";
    if (usedData == "sESD") 
    {
      cout << "Skimmed ESD analysis not available on the grid!" << endl;
      return;
    }
  }
  else
    cout << "-- using local analysis.\n";
  LoadLibs();//Load necessary libraries
//  SetupPar("PWGJEEMCALJetTasks");


  AliAnalysisManager* mgr = new AliAnalysisManager(uniqueName);
  TString localFiles("-1");
  if(usedData == "AOD")
  {
    localFiles = "files_aod.txt";
    gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/macros/train/AddAODHandler.C");
    AliAODInputHandler* aodH = AddAODHandler();
  }
  else if((usedData == "ESD") || (usedData == "sESD"))
  {
    if (usedData == "ESD")
      if(isMC)
        localFiles = "files_mc_esd.txt";
      else
        localFiles = "files_esd.txt";
    else
      localFiles = "files_sesd.txt";    
    gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/macros/train/AddESDHandler.C");
    AliESDInputHandler* esdH = AddESDHandler();
  }
  else
  {
    cout << "Data type not recognized! You have to specify ESD, AOD, or sESD!\n";
  }
  if(!useGrid)
    cout << "Using " << localFiles.Data() << " as input file list.\n";
  if (isMC && (usedData != "AOD"))
  {
    AliMCEventHandler* mcH = new AliMCEventHandler();
    mcH->SetPreReadMode(AliMCEventHandler::kLmPreRead);
    mcH->SetReadTR(kTRUE);
    mgr->SetMCtruthEventHandler(mcH); 
  }


  if(usedData == "AOD"){
     cout<<"the AOD input do not need physical selection!\n";
     cout<<"IMPORTANT: You do not need to add the physics selection task! The information is already stored in the AOD.!\n ";
  }
  else if((usedData=="ESD")||(usedData == "sESD")){
     gROOT->LoadMacro("$ALICE_ROOT/PWG/EMCAL/macros/AddTaskEmcalPhysicsSelection.C");
     AliEmcalPhysicsSelectionTask  *physSelTask = AddTaskEmcalPhysicsSelection(kFALSE, kTRUE, pSel, 0.30, 0.15, 10, kFALSE, -1, -1, -1, -1);
     //physSelTask->SelectCollisionCandidates(pSel);
  //  if (!physSelTask) 
  //  {
  //    cout << "no physSelTask"; 
  //    return; 
   // }
  }
  else cout<< "Data type not recognized! You have to specify ESD, AOD, or sESD!\n";

  if (usedData == "ESD" && runtype!=1) {
    gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/macros/AddTaskCentrality.C");
    AliCentralitySelectionTask *centralityTask = AddTaskCentrality(kFALSE);
  }

  if (usedData == "sESD") {
    gROOT->LoadMacro("$ALICE_ROOT/PWG/EMCAL/macros/AddTaskEmcalCompat.C");
    AliEmcalCompatTask *comptask = AddTaskEmcalCompat();
  }

  gROOT->LoadMacro("$ALICE_ROOT/PWG/EMCAL/macros/AddTaskEmcalSetup.C");
  AliEmcalSetupTask *setupTask = AddTaskEmcalSetup();
  setupTask->SetGeoPath("$ALICE_ROOT/OADB/EMCAL");
  cout<<"everything is fine Loading Emcal"<<endl;

  if (useTender)
  {
    gROOT->LoadMacro("$ALICE_ROOT/PWG/EMCAL/macros/AddTaskEMCALTender.C");
    AliAnalysisTaskSE *tender = AddTaskEMCALTender();
    if (usedData != "AOD" && !useGrid) {
      AliTender *alitender = dynamic_cast<AliTender*>(tender);
      alitender->SetDefaultCDBStorage("local://$ALICE_ROOT/OCDB"); 
    }
  }

//  gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/macros/AddTaskPIDResponse.C");
//  AliAnalysisTaskSE *taskRespPID = AddTaskPIDResponse(isMC);

  gROOT->LoadMacro("$ALICE_ROOT/PWGJE/EMCALJetTasks/macros/AddTaskJetPreparation.C");
  AddTaskJetPreparation(runPeriod,"PicoTracks",isMC ? "MCParticlesSelected":"","CaloClusters","CaloClustersCorr",2.0,0.00,0.03,0.015,0.15,pSel,kFALSE,kTRUE, kTRUE,kFALSE,kFALSE);

  gROOT->LoadMacro("$ALICE_ROOT/PWGJE/EMCALJetTasks/macros/AddTaskEmcalJet.C");
  AliEmcalJetTask* jetFinderTask = AddTaskEmcalJet("PicoTracks", "CaloClusters", kANTIKT, 0.4, 1, 0.150, 0.300);

  gROOT->LoadMacro("AliSubJetContainer.cxx+g");
  gROOT->LoadMacro("AliAnalysisTaskEmcalSubJet.cxx+g");

  Bool_t IsESD=kFALSE;
  if(usedData == "ESD") IsESD=kTRUE;
//  AliAnalysisTaskEmcalSubJet *anaTask = AddTaskEmcalAnaSubJet(isMC,IsESD,year,"PicoTracks", "CaloClustersCorr",jetFinderTask->GetName());
  
  gROOT->LoadMacro("AliAnalysisTaskEmcalSubJetUser.cxx+g");
  gROOT->LoadMacro("AddTaskEmcalSubJetUser.C");

  AliAnalysisTaskEmcalSubJetUser *anaTask = AddTaskEmcalSubJetUser(AliAnalysisTaskEmcalSubJet::kAKT | AliAnalysisTaskEmcalSubJet::kFullJet,isMC,IsESD,year,"PicoTracks", "CaloClustersCorr",jetFinderTask->GetName());

  cout<<"is herits From ALiPhysicsSelection Task"<<endl;
  cout<<"what's wrong with you!"<<endl;

/*
  // This part is due to the Unfolding part with deltapt and detector_Res 
  if(isDeltaPt && runtype!=1){
    gROOT->LoadMacro("$ALICE_ROOT/PWGJE/EMCALJetTasks/macros/AddTaskDeltaPt.C");
    AliAnalysisTaskDeltaPt * anadeltaptTask =AddTaskDeltaPt("PicoTracks", "CaloClustersCorr",jetFinderTask->GetName(),"Rho");
  }
  if(isResponse && isMC){
    gROOT->LoadMacro("$ALICE_ROOT/PWGJE/EMCALJetTasks/macros/AddTaskJetResponseMaker.C");
    AliJetResponseMaker * anaresponseTask = AddTaskJetResponseMaker( "PicoTracks", "CaloClustersCorr",jetFinderTask->GetName(),"Rho",0.4,"MCParticlesSelected", "CaloClustersCorr",jetFinderTaskMC->GetName(),"Rho",0.4);
  }   
*/
  TObjArray *toptasks = mgr->GetTasks();
  for (Int_t i=0; i<toptasks->GetEntries(); ++i) 
  {
    AliAnalysisTaskSE *task = dynamic_cast<AliAnalysisTaskSE*>(toptasks->At(i));
    if (!task)
      continue;
    if (task->InheritsFrom("AliPhysicsSelectionTask"))
      continue;
    ::Info("setPSel", "Set physics selection for %s (%s)", task->GetName(), task->ClassName());
    task->SelectCollisionCandidates(pSel);
  }
//  mgr->SetUseProgressBar(2, 25);        
  if (!mgr->InitAnalysis()) 
    return;
  mgr->PrintStatus();
  if (useGrid) 
  {  // GRID CALCULATION
    AliAnalysisGrid *plugin = CreateAlienHandler(uniqueName,runPeriod,gridDir, gridMode, runNumbers, pattern, additionalCXXs, additionalHs, maxFilesPerWorker, workerTTL, isMC);
    mgr->SetGridHandler(plugin);
    cout << "Starting GRID Analysis...";
    mgr->SetDebugLevel(1);
    mgr->StartAnalysis("grid");
  }
  else
  {  // LOCAL CALCULATION
    TChain* chain = 0;
    if (usedData == "AOD") 
    {
      gROOT->LoadMacro("$ALICE_ROOT/PWG/EMCAL/macros/CreateAODChain.C");
      chain = CreateAODChain(localFiles.Data(), numLocalFiles);
    }
    else
    {  // ESD or skimmed ESD
      gROOT->LoadMacro("$ALICE_ROOT/PWG/EMCAL/macros/CreateESDChain.C");
      chain = CreateESDChain(localFiles.Data(), numLocalFiles);
    } 
    cout << "Starting LOCAL Analysis...";
    mgr->SetDebugLevel(1);
    mgr->StartAnalysis("local", chain);
  }
}
//______________________________________________________________________________
void LoadLibs()
{ 
  gSystem->Load("libTree");
  gSystem->Load("libVMC");
  gSystem->Load("libGeom");
  gSystem->Load("libGui");
  gSystem->Load("libXMLParser");
  gSystem->Load("libMinuit");
  gSystem->Load("libMinuit2");
  gSystem->Load("libProof");
  gSystem->Load("libPhysics");
  gSystem->Load("libSTEERBase");
  gSystem->Load("libESD");
  gSystem->Load("libAOD");
  gSystem->Load("libOADB");
  gSystem->Load("libANALYSIS");
  gSystem->Load("libCDB");
  gSystem->Load("libRAWDatabase");
  //gSystem->Load("libSTEER");
  gSystem->Load("libEVGEN");
  gSystem->Load("libANALYSISalice");
  gSystem->Load("libCORRFW");
  gSystem->Load("libTOFbase");
  gSystem->Load("libRAWDatabase.so");
  gSystem->Load("libRAWDatarec.so");
  gSystem->Load("libTPCbase.so");
  gSystem->Load("libTPCrec.so");
  gSystem->Load("libITSbase.so");
  gSystem->Load("libITSrec.so");
  gSystem->Load("libTRDbase.so");
  gSystem->Load("libSTAT.so");
  gSystem->Load("libTRDrec.so");
  gSystem->Load("libHMPIDbase.so");  
  gSystem->Load("libPWGTools");
  //gSystem->Load("libPWGPP.so");
  gSystem->Load("libPWGHFbase");
  gSystem->Load("libPWGDQdielectron");
  gSystem->Load("libPWGHFhfe");
  gSystem->Load("libPHOSUtils");
  gSystem->Load("libPWGCaloTrackCorrBase");
  gSystem->Load("libVZEROrec");
  gSystem->Load("libTENDER");
  gSystem->Load("libTENDERSupplies");
  gSystem->Load("libEMCALUtils");
  gSystem->Load("libEMCALraw");
  gSystem->Load("libEMCALbase");
  gSystem->Load("libPWGEMCAL");
//  gSystem->Load("libPWGGAEMCALTasks");
  gSystem->Load("libESDfilter");
  gSystem->Load("libPWGCFCorrelationsBase");
  gSystem->Load("libPWGCFCorrelationsDPhi");
  //gSystem->Load("libCGAL");
  gSystem->Load("libfastjet");
  if(strstr(fastjetV,"2" )!=NULL){
    gSystem->Load("libSISConePlugin");
    gSystem->Load("libCDFConesPlugin");
  }
  else if(strstr(fastjetV,"3" )!=NULL){
   gSystem->Load("libsiscone");
   gSystem->Load("libsiscone_spherical");
   gSystem->Load("libfastjetplugins");
  }
  else cout<<"the fastjet version is out of control"<<endl;
  gSystem->Load("libJETAN");
  gSystem->Load("libFASTJETAN");
  gSystem->Load("libPWGJE.so");
  gSystem->Load("libPWGJEEMCALJetTasks");
  
  //SetupPar("PWGJEEMCALJetTasks");
  gSystem->AddIncludePath("-Wno-deprecated");
  gSystem->AddIncludePath("-I$ALICE_ROOT -I$ALICE_ROOT/include -I$ALICE_ROOT/EMCAL -I$ALICE_ROOT/PWG/EMCAL -I$ALICE_ROOT/PWGJE/EMCALJetTasks");
  gSystem->AddIncludePath(" -I$ALICE_ROOT/PWGHF/ -I$ALICE_ROOT/PWGHF/base -I$ALICE_ROOT/PWGHF/vertexingHF -I$ALICE_ROOT/PWGJE/FlavourJetTasks");
  gSystem->AddIncludePath("-I$ALICE_ROOT/PWGDQ/dielectron -I$ALICE_ROOT/PWGHF/hfe");
  gSystem->AddIncludePath("-I$ALICE_ROOT/JETAN -I$ALICE_ROOT/JETAN/fastjet");
}

AliAnalysisGrid* CreateAlienHandler(const char* uniqueName,const char* period,const char* gridDir, const char* gridMode, const char* runNumbers, 
                                     const char* pattern, TString additionalCode, TString additionalHeaders, Int_t maxFilesPerWorker, 
                                     Int_t workerTTL, Bool_t isMC)
{
  TDatime currentTime;
  TString tmpName(uniqueName);
  TString Runperiod(period);

  TString tmpAdditionalLibs("");
  tmpAdditionalLibs = Form("libTree.so libVMC.so libGeom.so libGui.so libXMLParser.so libMinuit.so libMinuit2.so libProof.so libPhysics.so libSTEERBase.so libESD.so libAOD.so libOADB.so libANALYSIS.so libCDB.so libRAWDatabase.so libSTEER.so libEVGEN.so libANALYSISalice.so libCORRFW.so libTOFbase.so libRAWDatabase.so libRAWDatarec.so libTPCbase.so libTPCrec.so libITSbase.so libITSrec.so libTRDbase.so libTENDER.so libSTAT.so libTRDrec.so libHMPIDbase.so libPWGTools.so  libPWGHFbase.so libPWGDQdielectron.so libPWGHFhfe.so libEMCALUtils.so libPHOSUtils.so libPWGCaloTrackCorrBase.so libEMCALraw.so libEMCALbase.so  libEMCALrec.so  libVZERObase.so libVZEROrec.so  libTENDERSupplies.so  libPWGEMCAL.so libESDfilter.so  libPWGCFCorrelationsBase.so libPWGCFCorrelationsDPhi.so  libCGAL.so libJETAN.so libfastjet.so  libSISConePlugin.so libCDFConesPlugin.so  libFASTJETAN.so  libPWGJE.so  libPWGJEEMCALJetTasks.so %s %s",additionalCode.Data(),additionalHeaders.Data());

  TString macroName("");
  TString execName("");
  TString jdlName("");
  macroName = Form("%s.C", tmpName.Data());
  execName = Form("%s.sh", tmpName.Data());
  jdlName = Form("%s.jdl", tmpName.Data());

  AliAnalysisAlien *plugin = new AliAnalysisAlien();
  plugin->SetOverwriteMode();
  plugin->SetRunMode(gridMode);   

  plugin->SetAPIVersion("V1.1x");
  plugin->SetROOTVersion(ROOTV);
  plugin->SetAliROOTVersion(AliRootV);

  plugin->SetGridDataDir(gridDir);          // e.g. "/alice/sim/LHC10a6"
  plugin->SetDataPattern(pattern);          //dir structure in run directory
  if (!isMC)
   plugin->SetRunPrefix("000");
  plugin->AddRunList(runNumbers);
  plugin->SetGridWorkingDir(Form("%s/%s",Runperiod.Data(),tmpName.Data()));
  plugin->SetGridOutputDir("output"); // In this case will be $HOME/work/output
  plugin->SetAnalysisSource(additionalCode.Data());
  plugin->SetAdditionalLibs(tmpAdditionalLibs.Data());
  plugin->AddExternalPackage("boost::v1_43_0");
  plugin->AddExternalPackage("cgal::v3.6");
  plugin->AddExternalPackage("fastjet::v2.4.2");
  plugin->SetDefaultOutputs(kTRUE);
  plugin->SetAnalysisMacro(macroName.Data());
  plugin->SetSplitMaxInputFileNumber(maxFilesPerWorker);
  plugin->SetExecutable(execName.Data());
  plugin->SetTTL(workerTTL);
  plugin->SetInputFormat("xml-single");
  plugin->SetJDLName(jdlName.Data());
  plugin->SetPrice(1);      
  plugin->SetSplitMode("se");
  return plugin;
}

void SetupPar(char* pararchivename)
{ 
  TString cdir(Form("%s", gSystem->WorkingDirectory() )) ;
  TString parpar(Form("%s.par", pararchivename)) ;

  if ( gSystem->AccessPathName(pararchivename) ) {
    TString processline = Form(".! tar xvzf %s",parpar.Data()) ;
    gROOT->ProcessLine(processline.Data());
  }

  TString ocwd = gSystem->WorkingDirectory();
  gSystem->ChangeDirectory(pararchivename);
  if (!gSystem->AccessPathName("PROOF-INF/BUILD.sh")) {
    printf("*******************************\n");
    printf("*** Building PAR archive    ***\n");
    cout<<pararchivename<<endl;
    printf("*******************************\n");

    if (gSystem->Exec("PROOF-INF/BUILD.sh")) {
      Error("runProcess","Cannot Build the PAR Archive! - Abort!");
      return -1;
    }
  }
 if (!gSystem->AccessPathName("PROOF-INF/SETUP.C")) {
    printf("*******************************\n");
    printf("*** Setup PAR archive       ***\n");
    cout<<pararchivename<<endl;
    printf("*******************************\n");
    gROOT->Macro("PROOF-INF/SETUP.C");
  }

  gSystem->ChangeDirectory(ocwd.Data());
  printf("Current dir: %s\n", ocwd.Data());
}




