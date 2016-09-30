//////////////////////////////////////////////////////////////////////
///
/// Step crawling code used by TrajClusterAlg
///
/// Bruce Baller, baller@fnal.gov
///
///
////////////////////////////////////////////////////////////////////////

#include "larreco/RecoAlg/TrajClusterAlg.h"
#include "larreco/RecoAlg/TCAlg/DebugStruct.h"


// TEMP for FillTrajTruth
#include "larsim/MCCheater/BackTracker.h"

class TH1F;
class TH2F;
class TProfile;

struct SortEntry{
  unsigned int index;
  float length;
};

bool greaterThan (SortEntry c1, SortEntry c2) { return (c1.length > c2.length);}
bool lessThan (SortEntry c1, SortEntry c2) { return (c1.length < c2.length);}


namespace tca {

  //------------------------------------------------------------------------------
  TrajClusterAlg::TrajClusterAlg(fhicl::ParameterSet const& pset)
  {
    reconfigure(pset);
    
/*
    // define some histograms
    art::ServiceHandle<art::TFileService> tfs;
    fMCSMom_KE_e = tfs->make<TH2F>("MCSMom_KE_e","MCSMom vs tru KE - e", 50, 0 , 1000, 50, 0, 1000);
    fMCSMom_KE_mu = tfs->make<TH2F>("MCSMom_KE_mu","MCSMom vs tru KE - mu", 50, 0 , 1000, 50, 0, 1000);
    fMCSMom_KE_pi = tfs->make<TH2F>("MCSMom_KE_pi","MCSMom vs tru KE - pi", 50, 0 , 1000, 50, 0, 1000);
    fMCSMom_KE_p = tfs->make<TH2F>("MCSMom_KE_p","MCSMom vs tru KE - p", 50, 0 , 1000, 50, 0, 1000);

    art::ServiceHandle<art::TFileService> tfs;
    
    fnHitsPerTP[0] = tfs->make<TH1F>("nHitsPerTP0","nHits / TP Pln 0", 10, 0, 10);
    fnHitsPerTP[1] = tfs->make<TH1F>("nHitsPerTP1","nHits / TP Pln 1", 10, 0, 10);
    fnHitsPerTP[2] = tfs->make<TH1F>("nHitsPerTP2","nHits / TP Pln 2", 10, 0, 10);
    
    fDelta[0] = tfs->make<TH1F>("Delta0","Delta Pln 0", 100, 0, 2);
    fDelta[1] = tfs->make<TH1F>("Delta1","Delta Pln 1", 100, 0, 2);
    fDelta[2] = tfs->make<TH1F>("Delta2","Delta Pln 2", 100, 0, 2);
    
    fDeltaN[0] = tfs->make<TH1F>("DeltaN0","Normalized Delta Pln 0", 50, 0, 4);
    fDeltaN[1] = tfs->make<TH1F>("DeltaN1","Normalized Delta Pln 1", 50, 0, 4);
    fDeltaN[2] = tfs->make<TH1F>("DeltaN2","Normalized Delta Pln 2", 50, 0, 4);
    
    fCharge[0] = tfs->make<TH1F>("Charge0","Charge/Pt Pln 0", 100, 0, 500);
    fCharge[1] = tfs->make<TH1F>("Charge1","Charge/Pt Pln 1", 100, 0, 500);
    fCharge[2] = tfs->make<TH1F>("Charge2","Charge/Pt Pln 2", 100, 0, 500);
    
    fnHitsPerTP_Angle[0] = tfs->make<TH2F>("nhtpertp_angle0","Hits/TP vs Angle Pln 0", 10, 0 , M_PI/2, 9, 1, 10);
    fnHitsPerTP_Angle[1] = tfs->make<TH2F>("nhtpertp_angle1","Hits/TP vs Angle Pln 1", 10, 0 , M_PI/2, 9, 1, 10);
    fnHitsPerTP_Angle[2] = tfs->make<TH2F>("nhtpertp_angle2","Hits/TP vs Angle Pln 2", 10, 0 , M_PI/2, 9, 1, 10);
    
    fnHitsPerTP_AngleP[0] = tfs->make<TProfile>("nhtpertp_anglep0","Hits/TP vs Angle Pln 0", 10, 0 , M_PI/2, "S");
    fnHitsPerTP_AngleP[1] = tfs->make<TProfile>("nhtpertp_anglep1","Hits/TP vs Angle Pln 1", 10, 0 , M_PI/2, "S");
    fnHitsPerTP_AngleP[2] = tfs->make<TProfile>("nhtpertp_anglep2","Hits/TP vs Angle Pln 2", 10, 0 , M_PI/2, "S");
    
    fPrEP = tfs->make<TH1F>("PrEP"," Proton EP", 40, 0, 1);
    fMuPiEP = tfs->make<TH1F>("MuPiEP"," Muon, Pion EP", 40, 0, 1);
    fPrEP->Sumw2();
    fMuPiEP->Sumw2();

 */
    art::ServiceHandle<art::TFileService> tfs;
    
    fDeltaN[0] = tfs->make<TH1F>("DeltaN0","Normalized Delta Pln 0", 50, 0, 4);
    fDeltaN[1] = tfs->make<TH1F>("DeltaN1","Normalized Delta Pln 1", 50, 0, 4);
    fDeltaN[2] = tfs->make<TH1F>("DeltaN2","Normalized Delta Pln 2", 50, 0, 4);

    fHitRMS[0] = tfs->make<TH1F>("hitrms0","Hit RMS Pln 0", 80, 0, 20);
    fHitRMS[1] = tfs->make<TH1F>("hitrms1","Hit RMS Pln 1", 80, 0, 20);
    fHitRMS[2] = tfs->make<TH1F>("hitrms2","Hit RMS Pln 2", 80, 0, 20);

    fTPWidth_Angle[0] = tfs->make<TH2F>("tpwidth_angle0","TP hit width vs Angle Pln 0", 20, 0, M_PI/2, 20, 0, 200);
    fTPWidth_Angle[1] = tfs->make<TH2F>("tpwidth_angle1","TP hit width vs Angle Pln 1", 20, 0, M_PI/2, 20, 0, 200);
    fTPWidth_Angle[2] = tfs->make<TH2F>("tpwidth_angle2","TP hit width vs Angle Pln 2", 20, 0, M_PI/2, 20, 0, 200);

    fTPWidth_AngleP[0] = tfs->make<TProfile>("tpwidth_anglep0","TP hit width vs Angle Pln 0", 10, 0, M_PI/2, "S");
    fTPWidth_AngleP[1] = tfs->make<TProfile>("tpwidth_anglep1","TP hit width vs Angle Pln 1", 10, 0, M_PI/2, "S");
    fTPWidth_AngleP[2] = tfs->make<TProfile>("tpwidth_anglep2","TP hit width vs Angle Pln 2", 10, 0, M_PI/2, "S");

    fExpect_Angle[0] = tfs->make<TProfile>("expect_angle0","Expected width vs Angle Pln 0", 11, 0, M_PI/2, "S");
    fExpect_Angle[1] = tfs->make<TProfile>("expect_angle1","Expected width vs Angle Pln 1", 11, 0, M_PI/2, "S");
    fExpect_Angle[2] = tfs->make<TProfile>("expect_angle2","Expected width vs Angle Pln 2", 11, 0, M_PI/2, "S");

    PrSum = 0;
    nPr = 0;
    MuPiSum = 0;
    nMuPi = 0;
    fEventsProcessed = 0;
//    if(fStudyMode) outFile.open("quality.txt");
    
  }
  
  bool TrajClusterAlg::SortByMultiplet(art::Ptr<recob::Hit> const& a, art::Ptr<recob::Hit> const& b)
  {
    // compare the wire IDs first:
    int cmp_res = a->WireID().cmp(b->WireID());
    if (cmp_res != 0) return cmp_res < 0; // order is decided, unless equal
    // decide by start time
    if (a->StartTick() != b->StartTick()) return a->StartTick() < b->StartTick();
    // if still undecided, resolve by local index
    return a->LocalIndex() < b->LocalIndex(); // if still unresolved, it's a bug!
  } // ClusterCrawlerAlg::SortByMultiplet()

  //------------------------------------------------------------------------------
  void TrajClusterAlg::reconfigure(fhicl::ParameterSet const& pset)
  {
 
    bool badinput = false;
    fHitFinderModuleLabel = pset.get<art::InputTag>("HitFinderModuleLabel");
    fMode                 = pset.get< short >("Mode", 0); // Default is don't use it
    fHitErrFac            = pset.get< float >("HitErrFac", 0.4);
    fMinAmp               = pset.get< float >("MinAmp", 5);
    fAngleRanges          = pset.get< std::vector<float>>("AngleRanges");
    fNPtsAve              = pset.get< short >("NPtsAve", 20);
    fMinPtsFit            = pset.get< std::vector<unsigned short >>("MinPtsFit");
    fMinPts               = pset.get< std::vector<unsigned short >>("MinPts");
    fMaxAngleRange        = pset.get< std::vector<unsigned short>>("MaxAngleRange");
    fMaxChi               = pset.get< float >("MaxChi", 10);
    fChgPullCut           = pset.get< float >("ChgPullCut", 3);
    fMultHitSep           = pset.get< float >("MultHitSep", 2.5);
    fKinkAngCut           = pset.get< float >("KinkAngCut", 0.4);
    fMaxWireSkipNoSignal  = pset.get< float >("MaxWireSkipNoSignal", 1);
    fMaxWireSkipWithSignal= pset.get< float >("MaxWireSkipWithSignal", 100);
    fProjectionErrFactor  = pset.get< float >("ProjectionErrFactor", 2);
    fJTMaxHitSep2         = pset.get< float >("JTMaxHitSep", 2);
    
    std::vector<std::string> skipAlgsVec = pset.get< std::vector<std::string>  >("SkipAlgs");
    
    fStudyMode            = pset.get< bool  >("StudyMode", false);
    fTagAllTraj           = pset.get< bool  >("TagAllTraj", false);
    fFillTruth            = pset.get< short >("FillTruth", 0);
    fDeltaRayTag          = pset.get< std::vector<short>>("DeltaRayTag", {-1, -1, -1});
    fMuonTag              = pset.get< std::vector<short>>("MuonTag", {-1, -1, -1, - 1});
    fShowerTag            = pset.get< std::vector<short>>("ShowerTag", {-1, -1});
    fMaxTrajSep           = pset.get< float >("MaxTrajSep", 4);
    fVertex2DCuts         = pset.get< std::vector<float >>("Vertex2DCuts", {-1, -1, -1, -1, -1});
    fVertex3DChiCut       = pset.get< float >("Vertex3DChiCut", -1);
    fMaxVertexTrajSep     = pset.get< std::vector<float>>("MaxVertexTrajSep");
    
    debug.Plane           = pset.get< int >("DebugPlane", -1);
    debug.Wire            = pset.get< int >("DebugWire", -1);
    debug.Tick            = pset.get< int >("DebugTick", -1);
    debug.WorkID          = pset.get< short>("DebugWorkID", 0);
    
    // check the angle ranges and convert from degrees to radians
    if(fAngleRanges.back() < 90) {
      std::cout<<"Last element of AngleRange != 90 degrees. Fixing it\n";
      fAngleRanges.back() = 90;
    }
    for(auto& range : fAngleRanges) {
      if(range < 0 || range > 90) throw art::Exception(art::errors::Configuration)<< "Invalid angle range "<<range<<" Must be 0 - 90 degrees";
      range *= M_PI / 180;
    } // range
    
    // convert the max traj separation into a separation^2
    fMaxTrajSep *= fMaxTrajSep;
    if(fJTMaxHitSep2 > 0) fJTMaxHitSep2 *= fJTMaxHitSep2;
    
    if(fMinPtsFit.size() != fMinPts.size()) badinput = true;
    if(fMaxVertexTrajSep.size() != fMinPts.size()) badinput = true;
    if(fMaxAngleRange.size() != fMinPts.size()) badinput = true;
    if(badinput) throw art::Exception(art::errors::Configuration)<< "Bad input from fcl file. Vector lengths are not the same";
    
    if(fVertex2DCuts.size() < 5) throw art::Exception(art::errors::Configuration)<<"Vertex2DCuts must be size 5";
    
    if(fMuonTag.size() < 4) throw art::Exception(art::errors::Configuration)<<"MuonTag must be size 4 [minPtsFit, minMCSMom, maxWireSkipNoSignal, minDeltaLen]";
    if(fDeltaRayTag.size() < 3) throw art::Exception(art::errors::Configuration)<<"DeltaRayTag must be size 3 [max endpoint sep, min MCSMom, max MCSMom]";
    if(fShowerTag.size() < 3) throw art::Exception(art::errors::Configuration)<< "ShowerTag must be size 3";
    
    if(kAlgBitSize != AlgBitNames.size())
      throw art::Exception(art::errors::Configuration)<<"kAlgBitSize "<<kAlgBitSize<<" != AlgBitNames size "<<AlgBitNames.size();
    fAlgModCount.resize(kAlgBitSize);
    
    unsigned short ib;
    bool gotit, printHelp = false;
    for(ib = 0; ib < AlgBitNames.size(); ++ib) fUseAlg[ib] = true;
    for(auto strng : skipAlgsVec) {
      gotit = false;
      if(strng == "All") {
        // turn everything off
        for(ib = 0; ib < AlgBitNames.size(); ++ib) fUseAlg[ib] = false;
        std::cout<<"Turning all algs off\n";
        gotit = true;
        break;
      } // All off
      for(ib = 0; ib < AlgBitNames.size(); ++ib) {
        if(strng == AlgBitNames[ib]) {
          fUseAlg[ib] = false;
          gotit = true;
          break;
        }
      } // ib
      if(!gotit) {
        std::cout<<"Unknown SkipAlgs input string '"<<strng<<"'\n";
        printHelp = true;
      }
    } // strng
    if(printHelp) {
      std::cout<<"Valid AlgNames:";
      for(auto strng : AlgBitNames) std::cout<<" "<<strng;
      std::cout<<"\n";
      std::cout<<"Or specify All to turn all algs off\n";
    }
    // Change the polarity of ChkInTraj
    if(fUseAlg[kChkInTraj]) { fUseAlg[kChkInTraj] = false; } else { fUseAlg[kChkInTraj] = true; std::cout<<"Note: ChkInTraj will be slow...\n"; }
   
  } // reconfigure
  
  ////////////////////////////////////////////////
  void TrajClusterAlg::ClearResults() {
    // clear everything in reverse order in which it appears in tjs DataStructs.h
    // so that vectors that appear later in the list are not unnecessarily shifted
    // before they are cleared.
    tjs.tjphs.clear();
    tjs.inTrialVtx3.clear();
    tjs.vtx3.clear();
    tjs.vtx.clear();
    tjs.tcl.clear();
    tjs.inClus.clear();
    tjs.WireHitRange.clear();
    tjs.newHits.clear();
    tjs.inTraj.clear();
    tjs.fHits.clear();
    tjs.allTraj.clear();
  } // ClearResults()

  ////////////////////////////////////////////////
  void TrajClusterAlg::RunTrajClusterAlg(art::Event & evt, bool fMakeNewHits)
  {

    if(fMode == 0) return;
    
    // Get the hits
    art::ValidHandle< std::vector<recob::Hit>> hitVecHandle = evt.getValidHandle<std::vector<recob::Hit>>(fHitFinderModuleLabel);

    ++fEventsProcessed;
    if(hitVecHandle->size() < 3) return;
    
    // a gratuitous clearing of everything before we start
    ClearResults();
    
    tjs.fHits.resize(hitVecHandle->size());
 
    larprop = lar::providerFrom<detinfo::LArPropertiesService>();
    detprop = lar::providerFrom<detinfo::DetectorPropertiesService>();
    
    for (unsigned int iht = 0; iht < tjs.fHits.size(); ++iht) tjs.fHits[iht] = art::Ptr< recob::Hit>(hitVecHandle, iht);
    
    // sort it as needed;
    // that is, sorted by wire ID number,
    // then by start of the region of interest in time, then by the multiplet
    std::sort(tjs.fHits.begin(), tjs.fHits.end(), &SortByMultiplet);
    
    // check for debugging mode triggered by Plane, Wire, Tick
    if(debug.Plane >= 0 && debug.Plane < 3 && debug.WorkID >= 0 && debug.Wire > 0 && debug.Tick > 0) {
      std::cout<<"Looking for debug hit "<<debug.Plane<<":"<<debug.Wire<<":"<<debug.Tick;
      for(unsigned int iht = 0; iht < tjs.fHits.size(); ++iht) {
        if((int)tjs.fHits[iht]->WireID().Plane != debug.Plane) continue;
        if((int)tjs.fHits[iht]->WireID().Wire != debug.Wire) continue;
        if(tjs.fHits[iht]->PeakTime() < debug.Tick - 5) continue;
        if(tjs.fHits[iht]->PeakTime() > debug.Tick + 5) continue;
        debug.Hit = iht;
        std::cout<<" iht "<<iht<<" "<<debug.Plane<<":"<<PrintHit(tjs.fHits[iht]);
        std::cout<<" Amp "<<(int)tjs.fHits[iht]->PeakAmplitude();
        std::cout<<" RMS "<<std::fixed<<std::setprecision(1)<<tjs.fHits[iht]->RMS();
        std::cout<<" Chisq "<<std::fixed<<std::setprecision(1)<<tjs.fHits[iht]->GoodnessOfFit();
        std::cout<<" Mult "<<tjs.fHits[iht]->Multiplicity();
        std::cout<<"\n";
        break;
      } // iht
      if(debug.Hit == UINT_MAX) std::cout<<" not found\n";
    } // debugging mode

    
    // set all hits to the available state
    tjs.inTraj.resize(tjs.fHits.size(), 0);
    
    fRun = evt.run();
    fSubRun  = evt.subRun();
    fEvent = evt.event();
    fWorkID = 0;
    
    // Set true if a truly bad situation occurs
    fQuitAlg = false;
    fIsRealData = evt.isRealData();
    didPrt = false;
    
    // Stepping directions for a number of trial reconstruction runs
    std::vector<short> sdirs;
    
    unsigned short nTrials = 1;
    if(fMode == -1) {
      // Step from DS to US
      sdirs.push_back(-1);
    } else if(fMode == 1) {
      // Step from US to DS
      sdirs.push_back(1);
    } else {
      // Step in both directions
      sdirs.push_back(-1);
      sdirs.push_back(1);
      nTrials = 2;
    }
    
    for(unsigned short itr = 0; itr < sdirs.size(); ++itr) {
      fStepDir = sdirs[itr];
      InitializeAllTraj();
      for (geo::TPCID const& tpcid: geom->IterateTPCIDs()) {
        geo::TPCGeo const& TPC = geom->TPC(tpcid);
        FillWireHitRange(tpcid);
        if(fQuitAlg) return;
        for(fPlane = 0; fPlane < TPC.Nplanes(); ++fPlane) {
          // no hits on this plane?
          if(tjs.FirstWire[fPlane] > tjs.LastWire[fPlane]) continue;
          // Set the CTP code to ensure objects are compared within the same plane
          fCTP = EncodeCTP(tpcid.Cryostat, tpcid.TPC, fPlane);
          fCstat = tpcid.Cryostat;
          fTpc = tpcid.TPC;
          // reconstruct all trajectories in the current plane
          ReconstructAllTraj();
          if(fQuitAlg) {
            std::cout<<"RunTrajCluster failed in ReconstructAllTraj Run "<<fRun<<" Event "<<fEvent<<" EventsProcessed "<<fEventsProcessed<<"\n";
            mf::LogVerbatim("TC")<<"RunTrajCluster failed after ReconstructAllTraj";
            ClearResults();
            return;
          }
        } // fPlane
        // No sense taking muon direction if delta ray tagging is disabled
        if(fDeltaRayTag[0] >= 0) TagMuonDirections(tjs, fMuonTag[3], debug.WorkID);
        if(fVertex3DChiCut > 0) Find3DVertices(tpcid);
        // stash allTraj, etc in the trials vector if more than one is planned
        if(nTrials > 1 && !tjs.allTraj.empty()) {
          tjs.trial.push_back(tjs.allTraj);
          tjs.inTrialTraj.push_back(tjs.inTraj);
          tjs.inTrialVtx.push_back(tjs.vtx);
          tjs.inTrialVtx3.push_back(tjs.vtx3);
        }
      } // tpcid
    } // itr
    
    if(nTrials > 1) {
      AnalyzeTrials();
//      AdjudicateTrials(reAnalyze);
      tjs.trial.clear();
      tjs.inTrialTraj.clear();
      tjs.inTrialVtx.clear();
      tjs.inTrialVtx3.clear();
    }
//    if(fTagAllTraj) TagAllTraj();

    //    bool reAnalyze = false;
    // put MC info into the trajectory struct
    FillTrajTruth();
 
    // Convert trajectories in allTraj into clusters
    MakeAllTrajClusters(fMakeNewHits);
    if(fQuitAlg) {
      std::cout<<"RunTrajCluster failed in MakeAllTrajClusters\n";
      ClearResults();
      return;
    }
    CheckHitClusterAssociations();
    if(fQuitAlg) {
      ClearResults();
      std::cout<<"RunTrajCluster failed in CheckHitClusterAssociations\n";
      return;
    }

    if(TJPrt > 0 || debug.Plane >= 0) {
      mf::LogVerbatim("TC")<<"Done in RunTrajClusterAlg";
      for(unsigned short itj = 0; itj < tjs.allTraj.size(); ++itj) {
        if(tjs.allTraj[itj].WorkID == TJPrt) {
          PrintAllTraj("DBG", tjs, debug, itj, USHRT_MAX);
          break;
        }
      } // itj
      // Print summary of all TJs
      PrintAllTraj("RTC", tjs, debug, USHRT_MAX, 0);
    }

    // temp
    if(fStudyMode) {
      for(unsigned int iht = 0; iht < tjs.fHits.size(); ++iht) {
        if(tjs.fHits[iht]->Multiplicity() != 1) continue;
        if(tjs.fHits[iht]->GoodnessOfFit() < 0) continue;
        if(tjs.fHits[iht]->GoodnessOfFit() > 50) continue;
        unsigned short ipl = tjs.fHits[iht]->WireID().Plane;
        fHitRMS[ipl]->Fill(tjs.fHits[iht]->RMS());
      } // iht
      for(unsigned short itj = 0; itj < tjs.allTraj.size(); ++itj) {
        Trajectory& tj = tjs.allTraj[itj];
        if(tj.AlgMod[kKilled]) continue;
        if(tj.MCSMom == 0) continue;
        // TP hit width plots
        unsigned short ipl = tj.CTP;
        for(unsigned short ipt = tj.EndPt[0]; ipt <= tj.EndPt[1]; ++ipt) {
          TrajPoint& tp = tj.Pts[ipt];
          if(tp.Chg == 0) continue;
          float dang = tp.Ang;
          if(dang > M_PI) dang = M_PI;
          if(dang < -M_PI) dang = M_PI;
          if(dang < 0) dang = -dang;
          if(dang > M_PI/2) dang = M_PI - dang;
          // width of all used hits in this tp
          float hitWid = TPHitsRMSTick(tjs, tp, true);
          fTPWidth_Angle[ipl]->Fill(dang, hitWid);
          fTPWidth_AngleP[ipl]->Fill(dang, hitWid);
          float expect = 0;
          if(std::abs(tp.Dir[0]) > 0.001) expect = fAveHitRMS[ipl] + std::abs(tp.Dir[1]/tp.Dir[0])/tjs.UnitsPerTick;
          fExpect_Angle[ipl]->Fill(dang, expect);
//          std::cout<<dang<<" "<<hitWid<<" "<<expect<<" Dir0 "<<tp.Dir[0]<<"\n";
          float dn = tp.Delta / fHitErrFac;
          if(dn > 0) fDeltaN[ipl]->Fill(dn);
        } // ipt
        if(tj.TruKE == 0) continue;
/*
        unsigned short pdg = std::abs(tj.TruPDG);
        double mass = 0.511;
        if(pdg == 13) mass = 105.7;
        if(pdg == 211) mass = 139.6;
        if(pdg == 2212) mass = 938.3;
        double tPlusM = tjs.allTraj[itj].TruKE + mass;
        double truMom = sqrt(tPlusM * tPlusM - mass * mass);
        std::cout<<tj.CTP<<":"<<PrintPos(tjs, tj.Pts[tj.EndPt[0]])<<"-"<<tj.CTP<<":"<<PrintPos(tjs, tj.Pts[tj.EndPt[1]]);
        std::cout<<" MCS TruKE "<<tj.TruKE<<" pdg "<<tj.TruPDG<<" MCSMom "<<(int)tj.MCSMom<<" EffPur "<<std::setprecision(2)<<tj.EffPur<<"\n";
        if(pdg == 11) fMCSMom_KE_e->Fill(tj.TruKE, tj.MCSMom);
        if(pdg == 13) fMCSMom_KE_mu->Fill(tj.TruKE, tj.MCSMom);
        if(pdg == 211) fMCSMom_KE_pi->Fill(tj.TruKE, tj.MCSMom);
        if(pdg == 2212) fMCSMom_KE_p->Fill(tj.TruKE, tj.MCSMom);
 */
      } // itj
/*
      for(unsigned short itj = 0; itj < tjs.allTraj.size(); ++itj) {
        if(tjs.allTraj[itj].AlgMod[kKilled]) continue;
        Trajectory& tj = tjs.allTraj[itj];
        unsigned short npwc = NumPtsWithCharge(tj, false);
        outFile<<tj.CTP<<" "<<npwc<<" "<<std::abs(tj.Pts[0].Ang)<<" "<<tj.MCSMom<<"\n";
        if(npwc < 20) continue;
        if(tj.MCSMom < 0.5) continue;
        std::cout<<"Bad npwc "<<npwc<<" Ang "<<std::abs(tj.Pts[0].Ang)<<" MCSMom "<<tj.MCSMom<<" WorkID "<<tj.WorkID;
        std::cout<<" "<<tj.CTP<<":"<<PrintPos(tjs, tj.Pts[0]);
        unsigned short endPt = tj.Pts.size()-1;
        std::cout<<" "<<tj.CTP<<":"<<PrintPos(tjs, tj.Pts[endPt]);
        std::cout<<"\n";
      } // itj
*/
    } // studymode

    // Print some reco-truth information to the screen
    if(fFillTruth > 0) {
      std::cout<<"Event "<<evt.event();
      double ave = -1;
      if(nMuPi > 0) ave = MuPiSum / (double)nMuPi;
      std::cout<<" nMuPi "<<nMuPi<<" MuPiEP "<<std::setprecision(2)<<ave;
      ave = -1;
      if(nPr > 0) ave = PrSum / (double)nPr;
      std::cout<<" nPr "<<nPr<<" PrEP "<<std::setprecision(2)<<ave;
      ave = -1;
      double sum = nPr + nMuPi;
      if(sum > 0) ave = (PrSum + MuPiSum) / sum;
      std::cout<<" combined "<<std::setprecision(2)<<ave;
      std::cout<<"\n";
    }

    // convert vertex time from WSE to ticks
    for(auto& avtx : tjs.vtx) avtx.Pos[1] /= tjs.UnitsPerTick;
    
    std::cout<<"RunTrajCluster success run "<<fRun<<" event "<<fEvent<<" allTraj size "<<tjs.allTraj.size()<<" events processed "<<fEventsProcessed<<"\n";
    
  } // RunTrajClusterAlg

  ////////////////////////////////////////////////
  void TrajClusterAlg::InitializeAllTraj()
  {
    tjs.allTraj.clear();
    std::fill(tjs.inTraj.begin(), tjs.inTraj.end(), 0);
    tjs.vtx.clear();
    tjs.vtx3.clear();
  } // InitializeAllTraj

  ////////////////////////////////////////////////
  void TrajClusterAlg::AnalyzeTrials()
  {
    // Analyze the Set of All Trajectories to construct a single set of trajectories
    // that is the best. The results are stored in tjs.allTraj.
    
    // This shouldn't happen but do it anyway
    if(tjs.trial.size() == 1) {
      tjs.allTraj = tjs.trial[0];
      return;
    }
    mf::LogVerbatim myprt("TC");

    unsigned short itrial, itj, jtrial, jtj, nSameHits = 0;
/*
    unsigned short nHitsInTraj;
    for(itrial = 0; itrial < tjs.trial.size(); ++itrial) {
      nHitsInTraj = 0;
      for(itj = 0; itj < tjs.trial[itrial].size(); ++itj) nHitsInTraj += tjs.trial[itrial][itj].NHits;
      myprt<<"Plane "<<fPlane<<" trial "<<itrial<<" NHits in all trajectories "<<nHitsInTraj<<"\n";
    } // is
*/
    std::vector<unsigned int> iHitVec, jHitVec;
    tjs.tjphs.clear();
    TjPairHitShare tmp;
    for(itrial = 0; itrial < tjs.trial.size() - 1; ++itrial) {
      for(itj = 0; itj < tjs.trial[itrial].size(); ++itj) {
        if(tjs.trial[itrial][itj].CTP != 1) continue;
        // ignore obsolete trajectories
        if(tjs.trial[itrial][itj].AlgMod[kKilled]) continue;
        // look at long trajectories for testing
        if(tjs.trial[itrial][itj].EndPt[1] < 5) continue;
        PutTrajHitsInVector(tjs.trial[itrial][itj], true, iHitVec);
        for(jtrial = itrial + 1; jtrial < tjs.trial.size(); ++jtrial) {
          for(jtj = 0; jtj < tjs.trial[jtrial].size(); ++jtj) {
            if(tjs.trial[jtrial][jtj].CTP != tjs.trial[itrial][itj].CTP) continue;
            if(tjs.trial[jtrial][jtj].AlgMod[kKilled]) continue;
            PutTrajHitsInVector(tjs.trial[jtrial][jtj], true, jHitVec);
            CountSameHits(iHitVec, jHitVec, nSameHits);
            if(nSameHits == 0) continue;
            tmp.iTrial = itrial; tmp.iTj = itj;
            tmp.jTrial = jtrial; tmp.jTj = jtj;
            tmp.nSameHits = nSameHits;
            tjs.tjphs.push_back(tmp);
            // keep track of the
          } // jtj
        } // jtr
      } // itj
    } // itr
    
    myprt<<"  itr  itj   Pts    from        to    jtr  jtj   Pts   from  to   nSameHits\n";
    unsigned short ept0, ept1, inpt, jnpt;
    for(auto& tmp : tjs.tjphs) {
      itrial = tmp.iTrial; jtrial = tmp.jTrial;
      itj = tmp.iTj; jtj = tmp.jTj;
      auto& iTj = tjs.trial[itrial][itj];
      inpt = NumPtsWithCharge(iTj, true);
      auto& jTj = tjs.trial[jtrial][jtj];
      jnpt = NumPtsWithCharge(jTj, true);
      // ignore if only a few hits are shared
      if(tmp.nSameHits < 5) continue;
      ept0 = iTj.EndPt[0];
      ept1 = iTj.EndPt[1];
      myprt<<std::setw(5)<<tmp.iTrial<<std::setw(5)<<itj<<" "<<inpt<<" "<<PrintPos(tjs, iTj.Pts[ept0])<<" - "<<PrintPos(tjs, iTj.Pts[ept1]);
      ept0 = jTj.EndPt[0];
      ept1 = jTj.EndPt[1];
      myprt<<std::setw(5)<<tmp.jTrial<<std::setw(5)<<jtj<<" "<<jnpt<<" "<<PrintPos(tjs, jTj.Pts[ept0])<<" - "<<PrintPos(tjs, jTj.Pts[ept1]);
      myprt<<std::setw(6)<<tmp.nSameHits<<"\n";
    }

  } // AnalyzeTrials

  ////////////////////////////////////////////////
  bool TrajClusterAlg::MergeAndStore(unsigned short itj1, unsigned short itj2)
  {
    // Merge the two trajectories and store them. Returns true if it was successfull.
    // Merging is done between the end of tj1 and the beginning of tj2
    // First check for major failures
    fQuitAlg = false;
    if(itj1 > tjs.allTraj.size() - 1) fQuitAlg = true;
    if(itj2 > tjs.allTraj.size() - 1) fQuitAlg = true;
    if(tjs.allTraj[itj1].AlgMod[kKilled] || tjs.allTraj[itj2].AlgMod[kKilled]) {
      mf::LogWarning("TC")<<"MergeAndStore: Trying to merge a killed trajectory. Here they are ";
      PrintAllTraj("tj1", tjs, debug, itj1, USHRT_MAX);
      PrintAllTraj("tj1", tjs, debug, itj2, USHRT_MAX);
      fQuitAlg = true;
    }
    
    if(fQuitAlg) {
      mf::LogError("TC")<<"Failed in MergeAndStore";
      return false;
    }
    
    // make a copy so they can be trimmed as needed
    // tj1 goes in work so we can use Storework
    work = tjs.allTraj[itj1];
    Trajectory tj2 = tjs.allTraj[itj2];
    
    // ensure that these are in the same step order
    if(work.StepDir != tj2.StepDir) return false;
    
    // assume that everything will succeed
    fQuitAlg = false;
    
    // remove any points at the end of tj1 that don't have used hits
    work.Pts.resize(work.EndPt[1] + 1);
    
    // determine if they overlap by finding the point on tj2 that is closest
    // to the end point of tj1.
    TrajPoint& endWorkTP = work.Pts[work.EndPt[1]];
    // Set minSep large so that dead wire regions are accounted for
    float minSep = 1000;
    unsigned short tj2ClosePt = 0;
    // Note that TrajPointTrajDOCA only considers TPs that have charge
    TrajPointTrajDOCA(tjs, endWorkTP, tj2, tj2ClosePt, minSep);
    // check for full overlap
    if(tj2ClosePt > tj2.EndPt[1]) return false;
    // check for the following possibilities, where - indicates a TP with charge
    // work:  --------------
    // tj2:                  -------------
    // Another possibility with overlap
    // work:  --------------
    // tj2:               ---------------
    
    // The approach is to append tj2 to tj1, store tj1 as a new trajectory,
    // and re-assign all hits to the new trajectory
    
    // First ensure that any hit will appear only once in the merged trajectory in the overlap region
    // whether it is used or unused. The point on tj2 where the merge will begin, tj2ClosePt, will be
    // increased until this condition is met.
    // Make a temporary vector of work hits in the end points for simpler searching
    std::vector<unsigned int> workHits;
    for(unsigned short ii = 0; ii < work.Pts.size(); ++ii) {
      // only go back a few points in work
      if(ii > 10) break;
      unsigned short ipt = work.Pts.size() - 1 - ii;
      workHits.insert(workHits.end(), work.Pts[ipt].Hits.begin(), work.Pts[ipt].Hits.end());
      if(ipt == 0) break;
    } // ii
    
    bool bumpedPt = true;
    while(bumpedPt) {
      bumpedPt = false;
      for(unsigned short ii = 0; ii < tj2.Pts[tj2ClosePt].Hits.size(); ++ii) {
        unsigned int iht = tj2.Pts[tj2ClosePt].Hits[ii];
        if(std::find(workHits.begin(), workHits.end(), iht) != workHits.end()) bumpedPt = true;
      } // ii
      if(bumpedPt && tj2ClosePt < tj2.EndPt[1]) {
        ++tj2ClosePt;
      } else {
        break;
      }
    } // bumpedPt
    
    // append tj2 hits to tj1
    work.Pts.insert(work.Pts.end(), tj2.Pts.begin() + tj2ClosePt, tj2.Pts.end());
    
    // A more exhaustive check that hits only appear once
    if(HasDuplicateHits(work)) return false;
/*
    // put ALL work hits in workHits
    workHits.clear();
    PutTrajHitsInVector(work, false, workHits);
    for(unsigned short ii = 0; ii < workHits.size() - 1; ++ii) {
      for(unsigned short jj = ii + 1; jj < workHits.size(); ++jj) if(workHits[ii] == workHits[jj]) return false;
     } // iht
*/
    // kill the original trajectories
    MakeTrajectoryObsolete(tjs, itj1);
    MakeTrajectoryObsolete(tjs, itj2);
    // Do this so that StoreWork keeps the correct WorkID (of itj1)
    work.ID = work.WorkID;
    StoreWork();
    return true;
    
  } // MergeAndStore

  ////////////////////////////////////////////////
  void TrajClusterAlg::CountSameHits(std::vector<unsigned int>& iHitVec, std::vector<unsigned int>& jHitVec, unsigned short& nSameHits)
  {
    // Counts the number of hits that are used in two different vectors of hits
    nSameHits = 0;
    for(unsigned short ii = 0; ii < iHitVec.size(); ++ii) {
      if(std::find(jHitVec.begin(), jHitVec.end(), iHitVec[ii]) != jHitVec.end()) ++nSameHits;
    }
  } // CountSameHits
  
  ////////////////////////////////////////////////
  void TrajClusterAlg::ReconstructAllTraj()
  {
    // Reconstruct clusters in fPlane and put them in allTraj
    
    unsigned int ii, iwire, jwire, iht, jht;
    
    if(fPlane > tjs.FirstWire.size() - 1) {
      mf::LogWarning("TC")<<"ReconstructAllTraj called with invalid fPlane "<<fPlane;
      fQuitAlg = true;
      return;
    }
    
    unsigned int nwires = tjs.LastWire[fPlane] - tjs.FirstWire[fPlane] - 1;
    std::vector<unsigned int> iHitsInMultiplet, jHitsInMultiplet;
    unsigned short ihtIndex, jhtIndex;
    
    // turn of trajectory printing
    TJPrt = 0;
    didPrt = false;
    
    // Make several passes through the hits with user-specified cuts for each
    // pass. In general these are to not reconstruct large angle trajectories on
    // the first pass
    float maxHitsRMS = 4 * fAveHitRMS[fPlane];
    for(unsigned short pass = 0; pass < fMinPtsFit.size(); ++pass) {
      fPass = pass;
      for(ii = 0; ii < nwires; ++ii) {
        // decide which way to step given the sign of fStepDir
        if(fStepDir > 0) {
          // step DS
          iwire = tjs.FirstWire[fPlane] + ii;
          jwire = iwire + 1;
        } else {
          // step US
          iwire = tjs.LastWire[fPlane] - ii - 1;
          jwire = iwire - 1;
        }
        // skip bad wires or no hits on the wire
        if(tjs.WireHitRange[fPlane][iwire].first < 0) continue;
        if(tjs.WireHitRange[fPlane][jwire].first < 0) continue;
        unsigned int ifirsthit = (unsigned int)tjs.WireHitRange[fPlane][iwire].first;
        unsigned int ilasthit = (unsigned int)tjs.WireHitRange[fPlane][iwire].second;
        unsigned int jfirsthit = (unsigned int)tjs.WireHitRange[fPlane][jwire].first;
        unsigned int jlasthit = (unsigned int)tjs.WireHitRange[fPlane][jwire].second;
        for(iht = ifirsthit; iht < ilasthit; ++iht) {
          // ignore below-threshold hits
          if(tjs.fHits[iht]->PeakAmplitude() < fMinAmp) continue;
          // clear out any leftover work tjs.inTraj's that weren't cleaned up properly
//          for(oht = ifirsthit; oht < ilasthit; ++oht) if(tjs.inTraj[oht] < 0) tjs.inTraj[oht] = 0;
          prt = (iht == debug.Hit);
          if(prt) didPrt = true;
          // Only consider hits that are available
          if(tjs.inTraj[iht] != 0) continue;
          // We hope to make a trajectory point at the hit position of iht in WSE units
          // with a direction pointing to jht
          unsigned int fromWire = tjs.fHits[iht]->WireID().Wire;
          float fromTick = tjs.fHits[iht]->PeakTime();
          float iqtot = tjs.fHits[iht]->Integral();
          float hitsRMSTick = tjs.fHits[iht]->RMS();
          GetHitMultiplet(iht, iHitsInMultiplet, ihtIndex);
//          if(iHitsInMultiplet.size() > 1) HitMultipletPosition(iht, fromTick, deltaRms, iqtot);
          if(iHitsInMultiplet.size() > 1) {
            fromTick = HitsPosTick(tjs, iHitsInMultiplet, iqtot);
            hitsRMSTick = HitsRMSTick(tjs, iHitsInMultiplet);
          }
          bool fatIHit = (hitsRMSTick > maxHitsRMS);
          if(prt) mf::LogVerbatim("TC")<<"+++++++ Pass "<<fPass<<" Found debug hit "<<fPlane<<":"<<PrintHit(tjs.fHits[iht])<<" tjs.inTraj "<<tjs.inTraj[iht]<<" RMS "<<tjs.fHits[iht]->RMS()<<" BB Multiplicity "<<iHitsInMultiplet.size()<<" AveHitRMS["<<fPlane<<"] "<<fAveHitRMS[fPlane]<<" HitsRMSTick "<<hitsRMSTick<<" fatIHit "<<fatIHit;
          for(jht = jfirsthit; jht < jlasthit; ++jht) {
            // Only consider hits that are available
            if(tjs.inTraj[iht] != 0) continue;
            if(tjs.inTraj[jht] != 0) continue;
            if(tjs.fHits[jht]->PeakAmplitude() < fMinAmp) continue;
            // clear out any leftover work tjs.inTraj's that weren't cleaned up properly
            for(unsigned short oht = jfirsthit; oht < jlasthit; ++oht) {
              if(tjs.inTraj[oht] < 0) {
                mf::LogVerbatim("TC")<<"Bad cleanup "<<PrintHit(tjs.fHits[oht])<<" "<<tjs.inTraj[oht]<<" events processed "<<fEventsProcessed;
                std::cout<<"Bad cleanup "<<PrintHit(tjs.fHits[oht])<<" "<<tjs.inTraj[oht]<<" events processed "<<fEventsProcessed<<" fWorkID "<<fWorkID<<"\n";
                tjs.inTraj[oht] = 0;
//                fQuitAlg = true;
//                return;
              }
            }
            unsigned int toWire = jwire;
            float toTick = tjs.fHits[jht]->PeakTime();
            float jqtot = tjs.fHits[jht]->Integral();
            if(jqtot < 1) continue;
            GetHitMultiplet(jht, jHitsInMultiplet, jhtIndex);
            hitsRMSTick = HitsRMSTick(tjs, jHitsInMultiplet);
            bool fatJHit = (hitsRMSTick > maxHitsRMS);
            if(fPass == 0) {
              // require both hits to be consistent
              if((fatIHit && !fatJHit) || (!fatIHit && fatJHit)) {
                if(prt) mf::LogVerbatim("TC")<<" jht fails "<<PrintHit(tjs.fHits[jht])<<" hit RMS "<<tjs.fHits[jht]->RMS()<<" mRMS "<<hitsRMSTick<<" fatJhit "<<fatJHit<<" max RMS "<<maxHitsRMS;
                continue;
              }
            } else {
              // pass > 0
              if(jHitsInMultiplet.size() > 1) toTick = HitsPosTick(tjs, jHitsInMultiplet, jqtot);
//              HitMultipletPosition(jht, toTick, deltaRms, jqtot);
            }
            if(prt) mf::LogVerbatim("TC")<<"+++++++ checking ClusterHitsOK with jht "<<fPlane<<":"<<PrintHit(tjs.fHits[jht])<<" BB Multiplicity "<<jHitsInMultiplet.size()<<" HitsRMSTick "<<HitsRMSTick(tjs, jHitsInMultiplet)<<" fatJhit "<<fatJHit<<" setting toTick to "<<(int)toTick;
            // Ensure that the hits StartTick and EndTick have the proper overlap
            if(!TrajHitsOK(iht, jht)) continue;
            // start a trajectory with direction from iht -> jht
            StartWork(fromWire, fromTick, toWire, toTick, fCTP);
            if(didPrt) TJPrt = work.WorkID;
            // check for a major failure
            if(fQuitAlg) return;
            if(work.Pts.empty()) {
              if(prt) mf::LogVerbatim("TC")<<"ReconstructAllTraj: StartWork failed";
              prt = false;
              ReleaseWorkHits();
              continue;
            }
            unsigned short angRange = AngleRange(work.Pts[0]);
            // check for a large angle crawl
            if(angRange > fMaxAngleRange[work.Pass]) {
              if(prt) mf::LogVerbatim("TC")<<"ReconstructAllTraj: Wrong angle range "<<angRange<<" for this pass "<<work.Pass;
              prt = false;
              ReleaseWorkHits();
              continue;
            }
            work.Pts[0].DeltaRMS = fHitErrFac * tjs.UnitsPerTick * hitsRMSTick;
            // don't include the charge of iht since it will be low if this
            // is a starting/ending track
            work.AveChg = jqtot;
            // try to add close hits
            bool sigOK;
            AddHits(work, 0, sigOK);
            // check for a major failure
            if(fQuitAlg) return;
            if(!sigOK || NumUsedHits(work.Pts[0]) == 0) {
              if(prt) mf::LogVerbatim("TC")<<" No hits at initial trajectory point ";
              prt = false;
              ReleaseWorkHits();
              continue;
            }
            // print the header and the first TP
            if(prt) PrintTrajectory("RAT", tjs, work, USHRT_MAX);
            // We can't update the trajectory yet because there is only one TP.
            work.EndPt[0] = 0;
            // now try stepping away
            StepCrawl();
            // check for a major failure
            if(fQuitAlg) return;
            if(prt) mf::LogVerbatim("TC")<<" After first StepCrawl. fGoodWork "<<fGoodWork<<" fTryWithNextPass "<<fTryWithNextPass;
            if(!fGoodWork && fTryWithNextPass) {
              StepCrawl();
              if(!fGoodWork || !fUpdateTrajOK) {
                if(prt) mf::LogVerbatim("TC")<<" xxxxxxx StepCrawl failed AGAIN. fTryWithNextPass "<<fTryWithNextPass;
                prt = false;
                ReleaseWorkHits();
                continue;
              } // Failed again
            }
            // Check the quality of the work trajectory
            CheckTraj(work);
            // check for a major failure
            if(fQuitAlg) return;
            if(prt) mf::LogVerbatim("TC")<<"ReconstructAllTraj: After CheckWork EndPt "<<work.EndPt[0]<<"-"<<work.EndPt[1]<<" fGoodWork "<<fGoodWork<<" fTryWithNextPass "<<fTryWithNextPass;
            if(fTryWithNextPass) {
              // Most likely, the first part of the trajectory was good but the latter part
              // had too many unused hits. The work vector was
              // truncated and fPass incremented, so give it another try
              work.AlgMod[kTryWithNextPass] = true;
              StepCrawl();
              // check for a major failure
              if(fQuitAlg) return;
              if(!fGoodWork) {
                if(prt) mf::LogVerbatim("TC")<<" xxxxxxx StepCrawl failed AGAIN after CheckWork";
                ReleaseWorkHits();
                continue;
              } // Failed again
            } // fTryWithNextPass
            if(prt) mf::LogVerbatim("TC")<<"StepCrawl done: fGoodWork "<<fGoodWork<<" NumPtsWithCharge "<<NumPtsWithCharge(work, true)<<" cut "<<fMinPts[work.Pass];
            // decide if the trajectory is long enough for this pass
            if(!fGoodWork || NumPtsWithCharge(work, true) < fMinPts[work.Pass]) {
              if(prt) mf::LogVerbatim("TC")<<" xxxxxxx Not enough points "<<NumPtsWithCharge(work, true)<<" minimum "<<fMinPts[work.Pass]<<" or !fGoodWork";
              ReleaseWorkHits();
              continue;
            }
            ReversePropagate(work);
            if(prt) mf::LogVerbatim("TC")<<"ReconstructAllTraj: calling StoreWork with npts "<<work.EndPt[1];
            StoreWork();
            // check for a major failure
            if(fQuitAlg) return;
            break;
          } // jht
          // THIS LOOKS WRONG
//          if(tjs.inTraj[iht] > 0) break;
        } // iht
      } // iwire
      EndMerge();
      if(fQuitAlg) return;
      TagDeltaRays(tjs, fCTP, fDeltaRayTag, debug.WorkID);
      Find2DVertices();
      if(fQuitAlg) return;
    } // fPass
    
    // Use unused hits in all trajectories
    UseUnusedHits();
    
    // make junk trajectories using nearby un-assigned hits
    if(fJTMaxHitSep2 > 0) {
      FindJunkTraj();
      if(fQuitAlg) return;
    }
    TagDeltaRays(tjs, fCTP, fDeltaRayTag, debug.WorkID);
    TagShowerTraj(tjs, fCTP, fShowerTag, debug.WorkID);
    Find2DVertices();
    // check for a major failure
    if(fQuitAlg) return;
    
    // last attempt to attach Tjs to vertices
    for(unsigned short ivx = 0; ivx < tjs.vtx.size(); ++ivx) if(tjs.vtx[ivx].NTraj > 0) AttachAnyTrajToVertex(ivx);
    
     work.Pts.clear();
    
//    if(didPrt) PrintAllTraj("RAT", tjs, debug, USHRT_MAX, USHRT_MAX);
    
  } // ReconstructAllTraj

  //////////////////////////////////////////
  void TrajClusterAlg::UseUnusedHits()
  {
    if(tjs.allTraj.size() == 0) return;
    if(!fUseAlg[kUseUnusedHits]) return;
    
    for(unsigned short itj = 0; itj < tjs.allTraj.size(); ++itj) {
      Trajectory& tj = tjs.allTraj[itj];
      if(tj.AlgMod[kKilled]) continue;
      for(unsigned short ipt = 0; ipt < tj.Pts.size(); ++ipt) {
        if(AngleRange(tj.Pts[ipt]) == 0) continue;
//        if(!IsLargeAngle(tj.Pts[ipt])) continue;
        bool hitsAdded = false;
        for(unsigned short ii = 0; ii < tj.Pts[ipt].Hits.size(); ++ii) {
          // hit is associated with this point and it is not used
          if(tj.Pts[ipt].UseHit[ii]) continue;
          unsigned int iht = tj.Pts[ipt].Hits[ii];
          // and it is not used in any other trajectory
          if(tjs.inTraj[iht] != 0) continue;
          tj.Pts[ipt].UseHit[ii] = true;
          tjs.inTraj[iht] = tj.ID;
          hitsAdded = true;
        } // ii
        if(hitsAdded) {
          DefineHitPos(tj.Pts[ipt]);
          tj.AlgMod[kUseUnusedHits] = true;
          if(prt) mf::LogVerbatim("TC")<<"UseUnusedHits: Using hits on ipt "<<ipt;
        }
      } // ipt
      if(tj.AlgMod[kUseUnusedHits]) SetEndPoints(tjs, tj);
    } // itj
    
  } // UseUnusedHits
  
  //////////////////////////////////////////
  void TrajClusterAlg::ReversePropagate(Trajectory& tj)
  {
    // Reverse the trajectory and propagate the fit from the end
    // to the beginning, possibly masking off hits that were associated
    // with TPs along the way
    
    if(!fUseAlg[kRevProp]) return;
    
    if(NumPtsWithCharge(tj, false) < 10) return;
    
    // Only consider trajectories that have had their beginning trajectory points
    // updated by FixTrajBegin
    if(!tj.AlgMod[kFixBegin]) return;
    
    // decide how complicated this should get. If the delta values at the beginning are not
    // too bad, we can just mask them off. If this is not the case, then the trajectory should
    // be truly reverse propagated, which necessitates re-fitting the trajectory points at the beginning


    if(prt) {
      mf::LogVerbatim("TC")<<"ReversePropagate: No charge compatability requirement";
    }
    
/*

    // Start at ipt = 0 to try to pick up any hits that may be been neglected at the beginning.
    // While we are doing this, count the number of points not used
    unsigned short nPtsNotUsed = 0;
    for(unsigned short ipt = 0; ipt < tj.EndPt[0] + 5; ++ipt) {
      float qIn = tj.Pts[ipt].Chg;
      UnsetUsedHits(tj.Pts[ipt]);
      float maxDelta = 5 * tj.Pts[tj.EndPt[1]].DeltaRMS;
      // be looser if there is only one hit
      if(tj.Pts[ipt].Hits.size() == 1) maxDelta *= 1.5;
      FindUseHits(tj, ipt, maxDelta, false);
      DefineHitPos(tj.Pts[ipt]);
      if(tj.Pts[ipt].Chg == 0) ++nPtsNotUsed;
      tj.Pts[ipt].Delta = PointTrajDOCA(tjs, tj.Pts[ipt].HitPos[0], tj.Pts[ipt].HitPos[1], tj.Pts[ipt]);
      if(tj.Pts[ipt].Chg != qIn) tj.AlgMod[kRevProp] = true;
      if(prt) PrintTrajectory("RP1", tjs, tj, ipt);
    } //ipt
    if(tj.AlgMod[kRevProp]) SetEndPoints(tjs, tj);
    
    // All the points were used so there is no reason to get more complicated
    if(nPtsNotUsed == 0) return;

    // save a copy in case something bad happens
    Trajectory TjCopy = tj;
    
    ReverseTraj(tj);
    
    // mask off the last 5 points from the fit (that may have just been added above)
    for(unsigned short ii = 0; ii < 5; ++ii) {
      unsigned short ipt = tj.EndPt[1] - ii;
      UnsetUsedHits(tj.Pts[ipt]);
    } // ii
    SetEndPoints(tjs, tj);
    
    if(prt) mf::LogVerbatim("TC")<<" Reverse propagate the trajectory - really";
    
    // Update the trajectory parameters w/o these points
    UpdateDeltaRMS(tj);
    // TODO Others?
    
    // Now add them back in and re-fit as we go. Start by refitting the
    // last point, which shouldn't really have any effect since we are fitting
    // the same number of points. The only difference is that the trajectory is flipped
    UpdateTraj(tj);
    if(!fUpdateTrajOK) {
      std::cout<<"ReversePropagate: failed on the first fit. This is really bad. Recovering";
      tj = TjCopy;
      return;
    }
    unsigned short startPt = tj.EndPt[1];
    for(unsigned short ipt = startPt; ipt < tj.Pts.size(); ++ipt) {
      float maxDelta = 5 * tj.Pts[tj.EndPt[1]].DeltaRMS;
      // be looser if there is only one hit
      if(tj.Pts[ipt].Hits.size() == 1) maxDelta *= 1.5;
      // Maybe we should use charge here...
      FindUseHits(tj, ipt, maxDelta, false);
      DefineHitPos(tj.Pts[ipt]);
      if(tj.Pts[ipt].Chg == 0) continue;
      UpdateTraj(tj);
      if(!fUpdateTrajOK) {
        std::cout<<"ReversePropagate: failed on point "<<ipt;
        tj = TjCopy;
        return;
      } //
      if(prt) PrintTrajectory("RP2", tjs, tj, ipt);
    } // ipt
*/
    if(prt) mf::LogVerbatim("TC")<<" Reverse propagate commented out for now";

  } // ReversePropagate
  
  //////////////////////////////////////////
  void TrajClusterAlg::FindJunkTraj()
  {
    // Makes junk trajectories using unassigned hits
    
    if(fPlane > tjs.FirstWire.size() - 1) {
      mf::LogWarning("TC")<<"FindJunkTraj called with invalid fPlane "<<fPlane;
      fQuitAlg = true;
      return;
    }
    
    unsigned int iwire, ifirsthit, ilasthit, iht;
    unsigned int jwire, jfirsthit, jlasthit, jht;
    unsigned int kwire, kfirsthit, klasthit, kht;
    unsigned int fromIndex;
    unsigned int loWire, hiWire;
    float loTime, hiTime;
    bool hitsAdded;
    unsigned short nit, tht, newTjIndex;
    
    // shouldn't have to do this but...
    for(iht = 0; iht < tjs.fHits.size(); ++iht) if(tjs.inTraj[iht] < 0) tjs.inTraj[iht] = 0;
    
    std::vector<unsigned int> tHits;
    for(iwire = tjs.FirstWire[fPlane]; iwire < tjs.LastWire[fPlane] - 1; ++iwire) {
      // skip bad wires or no hits on the wire
      if(tjs.WireHitRange[fPlane][iwire].first < 0) continue;
      jwire = iwire + 1;
      if(tjs.WireHitRange[fPlane][jwire].first < 0) continue;
      ifirsthit = (unsigned int)tjs.WireHitRange[fPlane][iwire].first;
      ilasthit = (unsigned int)tjs.WireHitRange[fPlane][iwire].second;
      jfirsthit = (unsigned int)tjs.WireHitRange[fPlane][jwire].first;
      jlasthit = (unsigned int)tjs.WireHitRange[fPlane][jwire].second;
      for(iht = ifirsthit; iht < ilasthit; ++iht) {
        prt = (iht == debug.Hit);
        if(prt) {
          mf::LogVerbatim("TC")<<"FindJunkTraj: Found debug hit "<<PrintHit(tjs.fHits[iht])<<" tjs.inTraj "<<tjs.inTraj[iht]<<" fJTMaxHitSep2 "<<fJTMaxHitSep2;
        }
        if(tjs.inTraj[iht] != 0) continue;
        if(tjs.fHits[iht]->PeakAmplitude() < fMinAmp) continue;
        for(jht = jfirsthit; jht < jlasthit; ++jht) {
          if(tjs.inTraj[jht] != 0) continue;
          if(prt && HitSep2(tjs, iht, jht) < 100) mf::LogVerbatim("TC")<<" use "<<PrintHit(tjs.fHits[jht])<<" HitSep2 "<<HitSep2(tjs, iht, jht);
          if(tjs.fHits[jht]->PeakAmplitude() < fMinAmp) continue;
          if(HitSep2(tjs, iht, jht) > fJTMaxHitSep2) continue;
          tHits.clear();
          // add all hits and flag them
          fromIndex = iht - tjs.fHits[iht]->LocalIndex();
          for(kht = fromIndex; kht < fromIndex + tjs.fHits[iht]->Multiplicity(); ++kht) {
            if(tjs.inTraj[kht] != 0) continue;
            tHits.push_back(kht);
            tjs.inTraj[kht] = -4;
          } // kht
          fromIndex = jht - tjs.fHits[jht]->LocalIndex();
          for(kht = fromIndex; kht < fromIndex + tjs.fHits[jht]->Multiplicity(); ++kht) {
            if(tjs.inTraj[kht] != 0) continue;
            tHits.push_back(kht);
            tjs.inTraj[kht] = -4;
          } // kht
          if(iwire != 0) { loWire = iwire - 1; } else { loWire = 0; }
          if(jwire < tjs.NumWires[fPlane] - 3) { hiWire = jwire + 2; } else { hiWire = tjs.NumWires[fPlane] - 1; }
          hitsAdded = true;
          nit = 0;
          while(hitsAdded && nit < 100) {
            hitsAdded = false;
            for(kwire = loWire; kwire < hiWire + 1; ++kwire) {
              if(tjs.WireHitRange[fPlane][kwire].first < 0) continue;
              kfirsthit = (unsigned int)tjs.WireHitRange[fPlane][kwire].first;
              klasthit = (unsigned int)tjs.WireHitRange[fPlane][kwire].second;
              for(kht = kfirsthit; kht < klasthit; ++kht) {
                if(tjs.inTraj[kht] != 0) continue;
                // this shouldn't be needed but do it anyway
                if(std::find(tHits.begin(), tHits.end(), kht) != tHits.end()) continue;
                // check w every hit in tHit
                for(tht = 0; tht < tHits.size(); ++tht) {
//                  if(prt && HitSep2(kht, tHits[tht]) < 100) mf::LogVerbatim("TC")<<" kht "<<PrintHit(tjs.fHits[kht])<<" tht "<<PrintHit(tjs.fHits[tHits[tht]])<<" HitSep2 "<<HitSep2(kht, tHits[tht])<<" cut "<<fJTMaxHitSep2;
                  if(HitSep2(tjs, kht, tHits[tht]) > fJTMaxHitSep2) continue;
                  hitsAdded = true;
                  tHits.push_back(kht);
                  tjs.inTraj[kht] = -4;
                  if(tHits.size() > 50) {
                    mf::LogWarning("TC")<<"FindJunkTraj: More than 50 hits found in junk trajectory. Stop looking";
                    break;
                  }
                  if(kwire > hiWire) hiWire = kwire;
                  if(kwire < loWire) loWire = kwire;
                  break;
                } // tht
              } // kht
//              if(prt) mf::LogVerbatim("TC")<<" kwire "<<kwire<<" thits size "<<tHits.size();
            } // kwire
            ++nit;
          } // hitsAdded && nit < 100
          loTime = 1E6; hiTime = 0;
          loWire = USHRT_MAX; hiWire = 0;
          for(tht = 0; tht < tHits.size(); ++tht) {
            if(tjs.fHits[tHits[tht]]->WireID().Wire < loWire) loWire = tjs.fHits[tHits[tht]]->WireID().Wire;
            if(tjs.fHits[tHits[tht]]->WireID().Wire > hiWire) hiWire = tjs.fHits[tHits[tht]]->WireID().Wire;
            if(tjs.fHits[tHits[tht]]->PeakTime() < loTime) loTime = tjs.fHits[tHits[tht]]->PeakTime();
            if(tjs.fHits[tHits[tht]]->PeakTime() > hiTime) hiTime = tjs.fHits[tHits[tht]]->PeakTime();
          }
          if(prt) {
            mf::LogVerbatim myprt("TC");
            myprt<<" tHits";
            for(auto tht : tHits) myprt<<" "<<PrintHit(tjs.fHits[tht]);
            myprt<<"\n";
          } // prt
          // See if this is a ghost trajectory
          unsigned short ofTraj = USHRT_MAX;
          if(IsGhost(tHits, ofTraj)) continue;
          MakeJunkTraj(tHits, newTjIndex);
          if(fQuitAlg) return;
          // release any hits that weren't included in a trajectory
          for(auto iht : tHits) if(tjs.inTraj[iht] == -4) tjs.inTraj[iht] = 0;
          if(hitsAdded) break;
        } // jht
      } // iht
    } // iwire
  } // FindJunkTraj

  //////////////////////////////////////////
  void TrajClusterAlg::MakeJunkTraj(std::vector<unsigned int> tHits, unsigned short& newTjIndex)
  {
    
     // Make a crummy trajectory using the provided hits
    newTjIndex = USHRT_MAX;
    
    if(tHits.size() < 2) return;

    std::vector<std::vector<unsigned int>> tpHits;
    unsigned short ii, iht, ipt;
    
    // Start the work trajectory using the first and last hits to
    // define a starting direction
    StartWork(tHits[0], tHits[tHits.size()-1]);
    if(work.ID == debug.WorkID) {
      mf::LogVerbatim("TC")<<" Turning on debug mode in MakeJunkTraj";
      prt = true;
    }
    
    // Do a more complicated specification of TP hits if there
    // are enough of them
    if(tHits.size() > 6) {
      // fit all of the hits to a line
      std::vector<float> x(tHits.size()), y(tHits.size()), yerr2(tHits.size());
      float intcpt, slope, intcpterr, slopeerr, chidof, qtot = 0;
      
      for(ii = 0; ii < tHits.size(); ++ii) {
        iht = tHits[ii];
        x[ii] = tjs.fHits[iht]->WireID().Wire;
        y[ii] = tjs.fHits[iht]->PeakTime() * tjs.UnitsPerTick;
        qtot += tjs.fHits[iht]->Integral();
        yerr2[ii] = tjs.fHits[iht]->Integral();
      } // ii
      fLinFitAlg.LinFit(x, y, yerr2, intcpt, slope, intcpterr, slopeerr, chidof);
      
      if(prt) mf::LogVerbatim("TC")<<" tHits line fit chidof "<<chidof<<" Angle "<<atan(slope);
      // return without making a junk trajectory
      if(chidof > 900) return;
      // A rough estimate of the trajectory angle
      work.Pts[0].Ang = atan(slope);
      // Rotate the hits into this coordinate system to find the start and end
      // points and general direction
      float cs = cos(-work.Pts[0].Ang);
      float sn = sin(-work.Pts[0].Ang);
      float tAlong, minAlong = 1E6, maxAlong = -1E6;
      float pointSize = 2.1;
      // sort the hits by the distance along the general direction
      std::vector<SortEntry> sortVec(tHits.size());
      SortEntry sortEntry;
      for(ii = 0; ii < x.size(); ++ii) {
        tAlong = cs * x[ii] - sn * y[ii];
        if(tAlong < minAlong) minAlong = tAlong;
        if(tAlong > maxAlong) maxAlong = tAlong;
        sortEntry.index = ii;
        sortEntry.length = tAlong;
        sortVec[ii] = sortEntry;
      } // ii
      std::sort(sortVec.begin(), sortVec.end(), lessThan);
      // make a temp vector
      std::vector<unsigned int> tmp(sortVec.size());
      // overwrite with the sorted values
      for(ii = 0; ii < sortVec.size(); ++ii) tmp[ii] = tHits[sortVec[ii].index];
      tHits = tmp;
      // create a trajectory point at each WSE unit (if there are hits at that point)
      unsigned short npts = (unsigned short)((maxAlong - minAlong) / pointSize);
      // rotate back into normal coordinate system
      if(prt) mf::LogVerbatim("TC")<<" minAlong "<<minAlong<<" maxAlong "<<maxAlong<<" work.Pts[0].Ang "<<work.Pts[0].Ang<<" npts "<<npts;
      if(npts < 2) npts = 2;
      tpHits.resize(npts);
      for(ii = 0; ii < tHits.size(); ++ii) {
        ipt = (unsigned short)((sortVec[ii].length - minAlong) / pointSize);
        if(ipt > npts - 1) ipt = npts - 1;
        if(prt) mf::LogVerbatim("TC")<<"tHit "<<PrintHit(tjs.fHits[tHits[ii]])<<" length "<<sortVec[ii].length<<" ipt "<<ipt<<" Chg "<<(int)tjs.fHits[tHits[ii]]->Integral();
        tpHits[ipt].push_back(tHits[ii]);
      }
    }  else {
      // just a few hits. Put each one at a TP in the order that
      // they were found
      tpHits.resize(tHits.size());
      for(ii = 0; ii < tHits.size(); ++ii) {
        tpHits[ii].push_back(tHits[ii]);
      }
    } // tHits.size()
    // make the TPs
    // work.Pts[0] is already defined but it needs hits added
    work.Pts[0].Hits = tpHits[0];
    work.Pts[0].UseHit.resize(work.Pts[0].Hits.size(), true);
    DefineHitPos(work.Pts[0]);
    work.Pts[0].Pos = work.Pts[0].HitPos;
    if(prt) PrintTrajectory("MJT", tjs, work, USHRT_MAX);
    // another TP to get the direction
    TrajPoint tpd;
    // make the rest of the TPs
    for(ipt = 1; ipt < tpHits.size(); ++ipt) {
      if(tpHits[ipt].empty()) continue;
      // Use the first TP as a starting point
      TrajPoint tp = work.Pts[0];
      tp.Step = ipt;
      tp.Hits = tpHits[ipt];
      // use all hits
      tp.UseHit.resize(tp.Hits.size(), true);
      DefineHitPos(tp);
      // Just use the hit position as the traj position
      tp.Pos = tp.HitPos;
      if(TrajPointSeparation(work.Pts[ipt-1], tp) < 0.5) continue;
      // define the direction
      MakeBareTrajPoint(tjs, work.Pts[ipt-1], tp, tpd);
      if(tpd.Pos[0] < 0) {
        // bad direction
        mf::LogError("TC")<<"Bad direction in MakeJunkTraj\n";
        fQuitAlg = true;
        return;
      }
      tp.Dir = tpd.Dir;
      tp.Ang = tpd.Ang;
      if(ipt == 1) {
        work.Pts[0].Dir = tpd.Dir;
        work.Pts[0].Ang = tpd.Ang;
      }
      work.Pts.push_back(tp);
      SetEndPoints(tjs, work);
    }
    if(prt) {
      PrintTrajectory("MJT", tjs, work, USHRT_MAX);
    }
    work.AlgMod[kJunkTj] = true;
    // See if this works or just does damage
    fGoodWork = true;
//    CheckTraj(work);
//    if(!fGoodWork) return;
    // Finally push it onto tjs.allTraj
    StoreWork();
    if(fQuitAlg) return;
    // return with a valid index for the new trajectory
    newTjIndex = tjs.allTraj.size() - 1;
  } // MakeJunkTraj

  //////////////////////////////////////////
  void TrajClusterAlg::FillTrajTruth()
  {
    
    if(fIsRealData) return;
    if(fFillTruth <= 0) return;
    
    art::ServiceHandle<cheat::BackTracker> bt;
    // list of all true particles
    sim::ParticleList const& plist = bt->ParticleList();
    // list of all true particles that will be considered
    std::vector<const simb::MCParticle*> plist2;
    // true (reconstructed) hits for each particle in plist2
    std::vector<std::vector<art::Ptr<recob::Hit>>> hlist2;
    // index of trajectory matched to each particle in plist2 in each plane
    std::vector<std::vector<short>> truToTj;
    // number of hits in each plist2 and plane
    std::vector<std::vector<unsigned short>> nTruInPlist2;
    // number of true hits in each trajectory and plane
    std::vector<std::vector<unsigned short>> nTruInTj;
    // total number of hits in each trajectory and plane
    std::vector<std::vector<unsigned short>> nRecInTj;

//    geo::TPCGeo const& TPC = geom->TPC(tpcid);
//    unsigned short Nplanes = TPC.Nplanes();
    unsigned short Nplanes = 3;

    int trackID;
//    int neutTrackID = -1;
    int pdg;
    float KE;
    std::vector<int> tidlist;
    bool isCharged;
    // initialize the true->(trajectory,plane) association
    std::vector<short> temp(Nplanes, -1);
    // initialize the true hit count
    std::vector<unsigned short> temp2(Nplanes, 0);
 
    for(sim::ParticleList::const_iterator ipart = plist.begin(); ipart != plist.end(); ++ipart) {
      const simb::MCParticle* part = (*ipart).second;
      assert(part != 0);
      trackID = part->TrackId();
      pdg = abs(part->PdgCode());
      art::Ptr<simb::MCTruth> theTruth = bt->TrackIDToMCTruth(trackID);
      if(fFillTruth < 2  && theTruth->Origin() == simb::kCosmicRay) continue;
//      if(theTruth->Origin() == simb::kBeamNeutrino) isNeutrino = true;
      isCharged = (pdg == 11) || (pdg == 13) || (pdg == 211) || (pdg == 321) || (pdg == 2212);
      if(!isCharged) continue;
      // KE in MeV
      KE = 1000 * (part->E() - part->Mass());
      if(KE < 10) continue;
      // ~2 cm range cut
      if(pdg == 11 && KE < 10) continue;
      if(pdg == 13 && KE < 12) continue;
      if(pdg == 211 && KE < 14) continue;
      if(pdg == 321 && KE < 14) continue;
      if(pdg == 2212 && KE < 30) continue;
      tidlist.push_back(trackID);
      plist2.push_back(part);
      truToTj.push_back(temp);
      nTruInTj.push_back(temp2);
      nTruInPlist2.push_back(temp2);
//      std::cout<<"TrackID "<<trackID<<" pdg "<<part->PdgCode()<<" E "<<part->E()<<" mass "<<part->Mass()<<" KE "<<(int)KE<<" MeV. Mother "<<part->Mother()<<" Proc "<<part->Process()<<"\n";
    }
    
    if(tidlist.empty()) return;
    
    // get the hits (in all planes) that are matched to the true tracks
    hlist2 = bt->TrackIDsToHits(tjs.fHits, tidlist);
    if(hlist2.size() != plist2.size()) {
      std::cout<<" Oops. hlist2 size "<<hlist2.size()<<" != plist2 size "<<plist2.size()<<"\n";
      return;
    }
    tidlist.clear();
    
    // vector of (mother, daughter) pairs
    std::vector<std::pair<unsigned short, unsigned short>> moda;
    // Deal with mother-daughter tracks
    // Assume that daughters appear later in the list. Step backwards
    // to accumulate all generations of daughters
    unsigned short dpl, ii, jj, jpl, kpl;
    for(ii = 0; ii < plist2.size(); ++ii) {
      dpl = plist2.size() - 1 - ii;
      // no mother
      if(plist2[dpl]->Mother() == 0) continue;
      // electron
      if(abs(plist2[dpl]->PdgCode()) == 11) continue;
      // the actual mother trackID is offset from the neutrino trackID
      int motherID = plist2[dpl]->Mother();
      // ensure that we are only looking at BeamNeutrino or single particle daughters
//      if(motherID != neutTrackID) continue;
      // count the number of daughters
      int ndtr = 0;
      for(kpl = 0; kpl < plist2.size(); ++kpl) {
        if(plist2[kpl]->Mother() == motherID) ++ndtr;
      }
      // require only one daughter
      if(ndtr > 1) continue;
      // find the mother in the list
      int mpl = -1;
      for(jj = 0; jj < plist2.size(); ++jj) {
        jpl = plist2.size() - 1 - jj;
        if(plist2[jpl]->TrackId() == motherID) {
          mpl = jpl;
          break;
        }
      } // jpl
      // mother not found for some reason
      if(mpl < 0) {
//        mf::LogVerbatim("TC")<<" mother of daughter "<<dpl<<" not found. mpl = "<<mpl;
        continue;
      }
      // ensure that PDG code for mother and daughter are the same
      if(plist2[dpl]->PdgCode() != plist2[mpl]->PdgCode()) continue;
      moda.push_back(std::make_pair(mpl, dpl));
 //     std::cout<<"moda "<<mpl<<" "<<dpl<<"\n";
    } //  dpl

    // count of the number of tj hits matched to each true particle in plist2
    unsigned short plane, iplist, imd, mom, itj;
    unsigned int iht;
    for(ii = 0; ii < hlist2.size(); ++ii) {
      // assume that this is the mother
      mom = ii;
      // see if mom is instead a daughter
      for(iplist = 0; iplist < plist2.size(); ++iplist) {
        for(imd = 0; imd < moda.size(); ++imd) if(mom == moda[imd].second) mom = moda[imd].first;
      }
      for(auto& hit : hlist2[ii]) {
        plane = hit->WireID().Plane;
        ++nTruInPlist2[mom][plane];
      } // hit
    } // ii
/*
    for(ii = 0; ii < nTruInPlist2.size(); ++ii) {
      std::cout<<ii<<" nTruInPlist2 ";
      for(plane = 0; plane < 3; ++plane) std::cout<<" "<<nTruInPlist2[ii][plane];
      std::cout<<"\n";
    }
*/
    // Now fill the reconstructed information. First size the vectors
    imd = tjs.allTraj.size();
    nRecInTj.resize(imd);
    for(ii = 0; ii < nRecInTj.size(); ++ii) {
      nRecInTj[ii].resize(Nplanes);
    } // ii

    unsigned short ipt, nTruHit, imtru;
    // temp vector for counting the number of trajectory hits that
    // are assigned to each true particle in one plane
    std::vector<unsigned short> nHitInPlist2(plist2.size());
    for(itj = 0; itj < tjs.allTraj.size(); ++itj) {
      if(tjs.allTraj[itj].AlgMod[kKilled]) continue;
      Trajectory& tj = tjs.allTraj[itj];
      geo::PlaneID planeID = DecodeCTP(tj.CTP);
      plane = planeID.Plane;
      for(ii = 0; ii < nHitInPlist2.size(); ++ii) nHitInPlist2[ii] = 0;
      for(ipt = 0; ipt < tj.Pts.size(); ++ipt) {
        if(tj.Pts[ipt].Chg == 0) continue;
        for(ii = 0; ii < tj.Pts[ipt].Hits.size(); ++ii) {
          if(!tj.Pts[ipt].UseHit[ii]) continue;
//          std::cout<<itj<<" "<<ipt<<" "<<ii<<" size "<<tj.Pts[ipt].Hits.size();
          iht = tj.Pts[ipt].Hits[ii];
//          std::cout<<" "<<iht<<"\n";
          ++nRecInTj[itj][plane];
          // look for this hit in hlist2
          for(jj = 0; jj < hlist2.size(); ++jj) {
            if(std::find(hlist2[jj].begin(), hlist2[jj].end(), tjs.fHits[iht]) != hlist2[jj].end()) {
              mom = jj;
              // check for the real mother
              for(iplist = 0; iplist < plist2.size(); ++iplist) {
                for(imd = 0; imd < moda.size(); ++imd) if(mom == moda[imd].second) mom = moda[imd].first;
              }
              ++nHitInPlist2[mom];
              break;
            }
          } // jj
        } // ii
      } // ipt
      // Associate the trajectory with the truth particle that has the highest number of hits in this plane
      nTruHit = 0;
      imtru = USHRT_MAX;
//      std::cout<<itj<<" nHitInPlist2 ";
      for(iplist = 0; iplist < nHitInPlist2.size(); ++iplist) {
//        std::cout<<" "<<nHitInPlist2[iplist];
        if(nHitInPlist2[iplist] > nTruHit) {
          nTruHit = nHitInPlist2[iplist];
          imtru = iplist;
        }
      } // iplist
//      std::cout<<" in plane "<<plane<<" nRecInTj "<<nRecInTj[itj][plane]<<"\n";
      if(imtru == USHRT_MAX) continue;
      // now see if this trajectory has the most hits associated with imtru
      if(nTruHit > nTruInTj[imtru][plane]) {
        truToTj[imtru][plane] = itj;
        nTruInTj[imtru][plane] = nTruHit;
      }
//      std::cout<<itj<<" nTruHit "<<nTruHit<<" imtru "<<imtru<<" nTruInTj "<<nTruInTj[imtru][plane]<<" truToTj "<<truToTj[imtru][plane]<<"\n";
    } // itj
    
//    bool badEvent = false;
    // now we can calulate efficiency and purity
    float nRecHits, nTruRecHits, nTruHits, eff, pur, ep;
    for(iplist = 0; iplist < plist2.size(); ++iplist) {
      if(hlist2[iplist].empty()) continue;
      float KE = 1000 * (plist2[iplist]->E() - plist2[iplist]->Mass());
      for(plane = 0; plane < Nplanes; ++plane) {
        // ignore not-reconstructable particles
        if(nTruInPlist2[iplist][plane] < 2) continue;
        eff = 0; pur = 0;
        if(truToTj[iplist][plane] < 0) {
          nRecHits = 0;
          nTruRecHits = 0;
          itj = USHRT_MAX;
        } else {
          itj = truToTj[iplist][plane];
          nRecHits = nRecInTj[itj][plane];
          nTruRecHits = nTruInTj[iplist][plane];
        }
        nTruHits = nTruInPlist2[iplist][plane];
        if(nTruHits > 0) eff = nTruRecHits / nTruHits;
        if(nRecHits > 0) pur = nTruRecHits / nRecHits;
        ep = eff * pur;
        if(ep == 0) ep = 1E-3;
        if(ep == 1) ep = 0.99999;
//        std::cout<<iplist<<" PDG "<<plist2[iplist]->PdgCode()<<" KE "<<KE<<" plane "<<plane<<" itj "<<truToTj[iplist][plane]<<" nTruHits "<<(int)nTruHits<<" nRecHits "<<nRecHits<<" nTruRecHits "<<nTruRecHits<<" ep "<<ep<<"\n";
        if(ep > 0.3) {
          itj = truToTj[iplist][plane];
          tjs.allTraj[itj].TruPDG = plist2[iplist]->PdgCode();
          tjs.allTraj[itj].TruKE = KE;
          tjs.allTraj[itj].EffPur = ep;
        }
        pdg = abs(plist2[iplist]->PdgCode());
        if(pdg == 2212) {
          ++nPr;
          PrSum += ep;
//          fPrEP->Fill(ep);
        } else if(pdg == 13 || pdg == 211) {
          ++nMuPi;
          MuPiSum += ep;
//          fMuPiEP->Fill(ep);
        }
        if(ep < 0.3 && KE > 100) {
          // print out the location of this bad boy
          unsigned int loW = 9999;
          unsigned int loT = 0;
          unsigned int hiW = 0;
          unsigned int hiT = 0;
          for(unsigned short ii = 0; ii < hlist2[iplist].size(); ++ii) {
            art::Ptr<recob::Hit> theHit = hlist2[iplist][ii];
            if(theHit->WireID().Plane != plane) continue;
            if(theHit->WireID().Wire < loW) {
              loW = theHit->WireID().Wire;
              loT = theHit->PeakTime();
            }
            if(theHit->WireID().Wire > hiW) {
              hiW = theHit->WireID().Wire;
              hiT = theHit->PeakTime();
            }
          } // ii
          mf::LogVerbatim myprt("TC");
          myprt<<"Evt "<<fEvent<<" nskip "<<fEventsProcessed-1<<" BadEP "<<std::setprecision(2)<<ep<<" KE "<<(int)KE<<" "<<plane<<":"<<loW<<":"<<loT<<"-"<<plane<<":"<<hiW<<":"<<hiT;
          if(itj != USHRT_MAX) {
            myprt<<" tjID "<<tjs.allTraj[itj].ID;
            for(unsigned short ib = 0; ib < AlgBitNames.size(); ++ib) if(tjs.allTraj[itj].AlgMod[ib]) myprt<<" "<<AlgBitNames[ib];
          }
//          badEvent = true;
        } // bad ep
      } // plane
    } // iplist
    
//    if(badEvent) outFile<<fRun<<" "<<fSubRun<<" "<<fEvent<<"\n";
   
  } // FillTrajTruth
/*
  //////////////////////////////////////////
  void TrajClusterAlg::TagPhotons()
  {
    // look for the signature of a photon conversion trajectory.
    // 1) Reasonably long trajectory
    // 2) 2x ratio of charge between beginning and end
    // 3) Multiplicity 1 TPs except in the middle where the electrons separate
    // 4) Significant angle change where the electrons separate
    // 5) It is probable that the electron trajectories will be incomplete so
    //    some merging may be needed
    // We will also know the direction the photon was heading using charge
    
    float bchg, echg, chgrat, dang;
//    float aveChg;
    unsigned short ipt, bPt, ePt, photonConvEnd, electronSepPt = USHRT_MAX;
    unsigned short photonConvEndPt;
    for(unsigned short itj = 0; itj < tjs.allTraj.size(); ++itj) {
      Trajectory& tj1 = tjs.allTraj[itj];
      if(tj1.AlgMod[kKilled]) continue;
      if(tj1.CTP != fCTP) continue;
      bPt = tj1.EndPt[0];
      ePt = tj1.EndPt[1];
      // length
      if(tj1.Pts.size() < 10) continue;
      // significant angle difference
      dang = DeltaAngle(tj1.Pts[bPt].Ang, tj1.Pts[ePt].Ang);
      if(dang < 0.2) continue;
      bchg = tj1.Pts[bPt].AveChg;
      if(bchg == 0) continue;
      echg = tj1.Pts[ePt].AveChg;
      if(echg == 0) continue;
      if(bchg > echg) {
        chgrat = bchg / echg;
        // photon converted at the beginning of the traj
        photonConvEnd = 0;
        photonConvEndPt = bPt;
      } else {
        chgrat = echg / bchg;
        // photon converted at the end of the traj
        photonConvEnd = 1;
        photonConvEndPt = ePt;
      }
      // 2x charge ratio
      if(chgrat < 1.5 || chgrat > 2.5) continue;
      // put a 2D vertex at what we think is the beginning
      VtxStore aVtx;
      aVtx.Wire = tj1.Pts[photonConvEndPt].Pos[0];
      aVtx.Time = tj1.Pts[photonConvEndPt].Pos[1];
      aVtx.NTraj = 1;
      aVtx.CTP = tj1.CTP; aVtx.Topo = 7;
      tjs.vtx.push_back(aVtx);
      tj1.Vtx[photonConvEnd] = tjs.vtx.size() - 1;
      // unresolved e+e- pair
      tj1.PDGCode = 24;
      if(prt) mf::LogVerbatim("TC")<<"chgrat "<<tjs.allTraj[itj].ID<<" "<<chgrat<<" photonConvEnd "<<photonConvEnd<<" bchg "<<(int)bchg<<" echg "<<(int)echg;
//      PrintAllTraj(tjs, debug, itj, USHRT_MAX);
      // find the separation point of the electrons, use a local average charge
      for(ipt = bPt + 1; ipt < ePt - 1; ++ipt) {
        if(tj1.Pts[ipt-1].Chg == 0) continue;
        if(tj1.Pts[ipt].Chg == 0) continue;
        if(tj1.Pts[ipt+1].Chg == 0) continue;
        if(electronSepPt == USHRT_MAX && tj1.Pts[ipt].Hits.size() > 1) electronSepPt = ipt;
//        aveChg = (tj1.Pts[ipt-1].Chg + tj1.Pts[ipt].Chg + tj1.Pts[ipt+1].Chg) / 3;
//        mf::LogVerbatim("TC")<<"ipt "<<ipt<<" aveChg "<<(int)aveChg<<" mult "<<tj1.Pts[ipt].Hits.size();
      }
    } // itj
    
  } // TagPhotons

  //////////////////////////////////////////
  void TrajClusterAlg::TagAllTraj()
  {
    // try to tag as shower-like or track-like
    
    if(tjs.allTraj.size() < 2) return;
    
    shPrt = (fShowerPrtPlane == (short)fPlane);
    if(shPrt) didPrt = true;
    
    TagPhotons();
    trjint.clear();
    TrjInt aTrjInt;
    ClsOfTrj.clear();
    
    // maximum separation^2
    float maxSep2 = fMaxTrajSep;
    float minSep2;
    float dang, vw, vt;
    std::vector<std::array<unsigned short, 3>> nCloseEnd(tjs.allTraj.size());
    unsigned short i1, i2, ipt1, ipt2;
    unsigned short endPt1, endPt2;
    unsigned short bin1, bin2;
    
    for(i1 = 0; i1 < tjs.allTraj.size() - 1; ++i1) {
      Trajectory& tj1 = tjs.allTraj[i1];
      if(tj1.AlgMod[kKilled]) continue;
      if(tj1.CTP != fCTP) continue;
      if(tj1.PDGCode == 12) continue;
      for(i2 = i1 + 1; i2 < tjs.allTraj.size(); ++i2) {
        Trajectory& tj2 = tjs.allTraj[i2];
        if(tj2.AlgMod[kKilled]) continue;
        if(tj2.CTP != tj1.CTP) continue;
        if(tj2.PDGCode == 12) continue;
        // find the closest approach
        minSep2 = maxSep2;
        TrajTrajDOCA(tj1, tj2, ipt1, ipt2, minSep2);
        if(minSep2 == maxSep2) continue;
        // Count the number at each end and in the middle
        bin1 = (unsigned short)(3 * (float)ipt1 / (float)tj1.EndPt[1]);
        if(bin1 > 2) bin1 = 2;
        // only count if this end doesn't have a vertex
        if(tj1.Vtx[bin1] < 0) ++nCloseEnd[i1][bin1];
        bin2 = (unsigned short)(3 * (float)ipt2 / (float)tj2.EndPt[1]);
        if(bin2 > 2) bin2 = 2;
        if(tj2.Vtx[bin2] < 0) ++nCloseEnd[i2][bin2];
        // find the angle between the TPs at the intersection
        dang = DeltaAngle(tj1.Pts[ipt1].Ang, tj2.Pts[ipt2].Ang);
        if(bin1 != 1  && bin2 != 1) {
          // the DOCA point is at the ends of the two TJs.
          // Find the intersection using the appropriate end points
          endPt1 = tjs.allTraj[i1].EndPt[0];
          if(bin1 == 2) endPt1 = tjs.allTraj[i1].EndPt[1];
          endPt2 = tjs.allTraj[i2].EndPt[0];
          if(bin2 == 2) endPt2 = tjs.allTraj[i2].EndPt[1];
          TrajIntersection(tjs.allTraj[i1].Pts[endPt1], tjs.allTraj[i2].Pts[endPt2], vw, vt);
//          if(shPrt) mf::LogVerbatim("TC")<<"TI check i1 "<<i1<<" endPt1 "<<endPt1<<" Ang "<<allTraj[i1].Pts[endPt1].Ang<<" i2 "<<i2<<" endPt2 "<<endPt2<<" Ang "<<allTraj[i2].Pts[endPt2].Ang<<" W:T "<<(int)vw<<":"<<(int)vt/tjs.UnitsPerTick;
        } else {
          vw = -1;
          vt = -1;
        }
        float dangErr = tjs.allTraj[i1].Pts[ipt1].AngErr;
        if(tjs.allTraj[i2].Pts[ipt2].AngErr > dangErr) dangErr = tjs.allTraj[i2].Pts[ipt2].AngErr;
//        float dangSig = dang / dangErr;
//        if(shPrt) mf::LogVerbatim("TC")<<"Close "<<i1<<" bin1 "<<bin1<<" i2 "<<i2<<" bin2 "<<bin2<<" minSep2 "<<minSep2<<" dang "<<dang<<" fKinkAngCut "<<fKinkAngCut<<" dangSig "<<dangSig<<" dvtx2 "<<dvtx2;
        // save it
        aTrjInt.itj1 = i1;
        aTrjInt.ipt1 = ipt1;
        aTrjInt.itj2 = i2;
        aTrjInt.ipt2 = ipt2;
        aTrjInt.sep2 = minSep2;
        aTrjInt.dang = dang;
        aTrjInt.vw = vw;
        aTrjInt.vt = vt;
        trjint.push_back(aTrjInt);
      } // jj
    } // ii
    if(trjint.empty()) return;
    if(shPrt) {
      for(i1 = 0; i1 < trjint.size(); ++i1) {
        mf::LogVerbatim("TC")<<i1<<" trjint "<<" "<<trjint[i1].itj1<<" "<<trjint[i1].itj2<<" sep2 "<<trjint[i1].sep2<<" dang "<<trjint[i1].dang<<" tjs.vtx "<<fPlane<<":"<<(int)trjint[i1].vw<<":"<<(int)(trjint[i1].vt / tjs.UnitsPerTick);
      }
    }
    
    std::vector<std::vector<unsigned short>> trjintIndices;
    FindClustersOfTrajectories(trjintIndices);
    
    unsigned short icot;
    // Deal with each COT (Note that ClsOfTrj and trjintIndices have the same size)
    for(icot = 0; icot < ClsOfTrj.size(); ++icot) {
      // try fitting the DOCA points to a line to get the shower axis if there are enough points
      if(trjintIndices[icot].size() > 3) {
        DefineShowerTraj(icot, trjintIndices);
      } // trjintIndices[icot].size() > 3
      else {
        mf::LogVerbatim("TC")<<" no shower angle and endpoint information available... Write some code";
      }
    } // icot
    
    // clean up
    trjint.clear();
    ClsOfTrj.clear();
    
    KillVerticesInShowers();
  } // TagAllTraj

  //////////////////////////////////////////
  void TrajClusterAlg::KillVerticesInShowers()
  {
    // Kill all vertices between shower-like trajectories
    unsigned short ivx, itj, nsh;
    std::vector<unsigned short> tjIndices;
    for(ivx = 0; ivx < tjs.vtx.size(); ++ivx) {
      if(tjs.vtx[ivx].NTraj == 0) continue;
      tjIndices.clear();
      nsh = 0;
      for(itj = 0; itj < tjs.allTraj.size(); ++itj) {
        if(tjs.allTraj[itj].AlgMod[kKilled]) continue;
        if(tjs.allTraj[itj].CTP != tjs.vtx[ivx].CTP) continue;
        if(tjs.allTraj[itj].PDGCode == 24) continue;
        if(tjs.allTraj[itj].Vtx[0] == ivx || tjs.allTraj[itj].Vtx[1] == ivx) {
          tjIndices.push_back(itj);
          if(tjs.allTraj[itj].PDGCode == 12) ++nsh;
        }
      } // itj
      if(tjIndices.empty()) continue;
      if(tjIndices.size() != nsh) continue;
      for(auto itj : tjIndices) {
        if(tjs.allTraj[itj].Vtx[0] == ivx) tjs.allTraj[itj].Vtx[0] = -1;
        if(tjs.allTraj[itj].Vtx[1] == ivx) tjs.allTraj[itj].Vtx[1] = -1;
      }
    } // ivx
  } // KillVerticesInShowers

  //////////////////////////////////////////
  void TrajClusterAlg::DefineShowerTraj(unsigned short icot, std::vector<std::vector<unsigned short>> trjintIndices)
  {
    // Try to define a shower trajectory consisting of a single track-like trajectory (the electron/photon)
    // plus a group of shower-like trajectories, using the vector of trjintIndices
    
    unsigned short ii, iTji, i1, ipt1, i2, ipt2, nEndInside, ipt0;
    std::vector<float> x, y, yerr2;
    float arg, fom, pos0, pos1, minsep, dang;
    float showerAngle, cs, sn;
    float intcpt, slope, intcpterr, slopeerr, chidof, shlong;
    unsigned short primTraj, primTrajEnd;
    for(ii = 0; ii < trjintIndices[icot].size(); ++ii) {
      iTji = trjintIndices[icot][ii];
      i1 = trjint[iTji].itj1;
      ipt1 = trjint[iTji].ipt1;
      x.push_back(tjs.allTraj[i1].Pts[ipt1].Pos[0]);
      y.push_back(tjs.allTraj[i1].Pts[ipt1].Pos[1]);
      // weight by the length of both trajectories
      i2 = trjint[iTji].itj2;
      arg = tjs.allTraj[i1].EndPt[1] + tjs.allTraj[i2].EndPt[1];
      yerr2.push_back(arg);
    } // ii
    fLinFitAlg.LinFit(x, y, yerr2, intcpt, slope, intcpterr, slopeerr, chidof);
    showerAngle = atan(slope);
    if(shPrt) mf::LogVerbatim("TC")<<"Fit intcpt "<<intcpt<<" slope "<<slope<<" angle "<<showerAngle<<" chidof "<<chidof;
    // Rotate the intersection points into a coordinate system along the
    // shower direction. We will use this to find the (restricted) longitudinal and
    // transverse extent of the shower
    cs = cos(-showerAngle);
    sn = sin(-showerAngle);
    // min and max of traj intersections along the shower angle direction
    float minshlong = 1E6, maxshlong = -1;
    unsigned short miniTji = 0, maxiTji = 0;
    for(ii = 0; ii < trjintIndices[icot].size(); ++ii) {
      iTji = trjintIndices[icot][ii];
      i1 = trjint[iTji].itj1;
      ipt1 = trjint[iTji].ipt1;
      shlong = cs * tjs.allTraj[i1].Pts[ipt1].Pos[0] - sn * tjs.allTraj[i1].Pts[ipt1].Pos[1];
      //          std::cout<<"i1 "<<i1<<" ipt1 "<<ipt1<<" pos "<<(int)allTraj[i1].Pts[ipt1].Pos[0]<<":"<<(int)allTraj[i1].Pts[ipt1].Pos[1]<<" shlong "<<shlong<<"\n";
      if(shlong < minshlong) { minshlong = shlong; miniTji = iTji; } // shlong < minshlong
      if(shlong > maxshlong) { maxshlong = shlong; maxiTji = iTji; } // shlong > maxshlong
      i2 = trjint[iTji].itj2;
      ipt2 = trjint[iTji].ipt2;
      shlong = cs * tjs.allTraj[i2].Pts[ipt2].Pos[0] - sn * tjs.allTraj[i2].Pts[ipt2].Pos[1];
      //          std::cout<<"i2 "<<i2<<" ipt2 "<<ipt2<<" pos "<<(int)allTraj[i2].Pts[ipt2].Pos[0]<<":"<<(int)allTraj[i2].Pts[ipt2].Pos[1]<<" shlong "<<shlong<<"\n";
      if(shlong < minshlong) { minshlong = shlong; miniTji = iTji; } // shlong < minshlong
      if(shlong > maxshlong) { maxshlong = shlong; maxiTji = iTji; } // shlong > maxshlong
    } // ii
    // reduce the extend by 1 WSE at each end to allow some tolerance
    minshlong += 1;
    maxshlong -= 1;
    if(shPrt) {
      mf::LogVerbatim("TC")<<"minshlong "<<minshlong<<" maxshlong "<<maxshlong;
      mf::LogVerbatim("TC")<<"Primary traj is probably in "<<miniTji<<" or "<<maxiTji;
    }
    // Form a list of candidates
    std::vector<unsigned short> primCandidates(4);
    primCandidates[0] = trjint[miniTji].itj1;
    primCandidates[1] = trjint[miniTji].itj2;
    primCandidates[2] = trjint[maxiTji].itj1;
    primCandidates[3] = trjint[maxiTji].itj2;
    fom = 999;
    for(ii = 0; ii < 4; ++ii) {
      i1 = primCandidates[ii];
      ipt1 = tjs.allTraj[i1].EndPt[1];
      // reject if both end points are inside the shower boundaries.
      // Rotate the endpoints into the shower coordinate system
      nEndInside = 0;
      shlong = cs * tjs.allTraj[i1].Pts[0].Pos[0] - sn * tjs.allTraj[i1].Pts[0].Pos[1];
      if(shlong > minshlong && shlong < maxshlong) ++nEndInside;
      if(shPrt) mf::LogVerbatim("TC")<<" Candidate min "<<i1<<" shlong "<<shlong<<" nEndInside "<<nEndInside;
      // rotate the other end and test
      shlong = cs * tjs.allTraj[i1].Pts[ipt1].Pos[0] - sn * tjs.allTraj[i1].Pts[ipt1].Pos[1];
      if(shlong > minshlong && shlong < maxshlong) ++nEndInside;
      if(shPrt) mf::LogVerbatim("TC")<<" Candidate max "<<i1<<" shlong "<<shlong<<" nEndInside "<<nEndInside;
      if(nEndInside > 1) continue;
      // average the angle of the TP end points and find the difference
      // wrt the shower angle
      dang = DeltaAngle(0.5 * (tjs.allTraj[i1].Pts[0].Ang + tjs.allTraj[i1].Pts[ipt1].Ang), showerAngle);
//      dang = std::abs(0.5 * (tjs.allTraj[i1].Pts[0].Ang + tjs.allTraj[i1].Pts[ipt1].Ang) - showerAngle);
      arg = 10 * (1 + tjs.allTraj[i1].Pass) * dang * tjs.allTraj[i1].EndPt[1];
//      mf::LogVerbatim("TC")<<"Candidate "<<i1<<" nCloseEnd "<<nCloseEnd[i1][0]<<" "<<nCloseEnd[i1][1]<<" "<<nCloseEnd[i1][2]<<" pass "<<allTraj[i1].Pass<<" dang "<<dang<<" arg "<<arg;
      if(arg < fom) {
        fom = arg;
        primTraj = i1;
      }
    } // ii
    // determine which end is closest to the shower end
    // distance between TP0 and the shower min position
//    ipt1 = tjs.allTraj[primTraj].Pts.size()-1;
    ipt0 = tjs.allTraj[primTraj].EndPt[0];
    ipt1 = tjs.allTraj[primTraj].EndPt[1];
    pos0 = cs * tjs.allTraj[primTraj].Pts[ipt0].Pos[0] - sn * tjs.allTraj[primTraj].Pts[ipt0].Pos[1];
    pos1 = cs * tjs.allTraj[primTraj].Pts[ipt1].Pos[0] - sn * tjs.allTraj[primTraj].Pts[ipt1].Pos[1];
    minsep = std::abs(pos0 - minshlong);
    primTrajEnd = 0;
    //        mf::LogVerbatim("TC")<<"0min "<<pos0<<" minsep "<<minsep<<" end "<<primTrajEnd;
    arg = std::abs(pos1 - minshlong);
    if(arg < minsep) { minsep = arg;  primTrajEnd = ipt1; }
    //        mf::LogVerbatim("TC")<<"1min "<<pos1<<" arg "<<arg<<" end "<<primTrajEnd;
    arg = std::abs(pos0 - maxshlong);
    if(arg < minsep) { minsep = arg;  primTrajEnd = ipt0; }
    //        mf::LogVerbatim("TC")<<"0max "<<pos0<<" minsep "<<minsep<<" end "<<primTrajEnd;
    arg = std::abs(pos1 - maxshlong);
    if(arg < minsep) { minsep = arg;  primTrajEnd = ipt1; }
    //        mf::LogVerbatim("TC")<<"1max "<<pos1<<" arg "<<arg<<" end "<<primTrajEnd;
    if(shPrt) mf::LogVerbatim("TC")<<" set Primary traj = "<<primTraj<<" end "<<primTrajEnd;
    TagShowerTraj(icot, primTraj, primTrajEnd, showerAngle);

  }

  //////////////////////////////////////////
  void TrajClusterAlg::TagShowerTraj(unsigned short icot, unsigned short primTraj, unsigned short primTrajEnd, float showerAngle)
  {
    // Tag a track-like trajectory using primTraj and a shower-like cluster consisting
    // of all other trajectories in ClsOfTrj[icot]
    
    // set the PDGCode for all TJ's to shower-like
    unsigned short ii, itj;
    for(ii = 0; ii < ClsOfTrj[icot].size(); ++ii) {
      itj = ClsOfTrj[icot][ii];
      tjs.allTraj[itj].PDGCode = 12;
      tjs.allTraj[itj].ParentTrajID = primTraj + 1;
    }
    
    tjs.allTraj[primTraj].PDGCode = 13;
    tjs.allTraj[primTraj].ParentTrajID = 0;
    
  } // MakeShowerClusters
  
  //////////////////////////////////////////
  void TrajClusterAlg::FindClustersOfTrajectories(std::vector<std::vector<unsigned short>>& trjintIndices)
  {
    // Associate trajectories that are close to each into Clusters Of Trajectories (COTs)
    
    mf::LogVerbatim myprt("TC");

    if(trjint.empty()) return;
    ClsOfTrj.clear();
    trjintIndices.clear();
    std::vector<unsigned short> tmp;
    std::vector<unsigned short> imp;
    std::vector<bool> inCOT(tjs.allTraj.size(), false);
    bool tjAdded;
    unsigned short iTji, itj1, itj2;
    for(unsigned short nit = 0; nit < 100; ++nit) {
      tmp.clear();
      imp.clear();
      // look for trajectories that haven't been assigned to a COT
      for(iTji = 0; iTji < trjint.size(); ++iTji) {
        itj1 = trjint[iTji].itj1;
        itj2 = trjint[iTji].itj2;
        if(!inCOT[itj1]) {
          tmp.push_back(itj1);
          imp.push_back(iTji);
          inCOT[itj1] = true;
          break;
        }
        if(!inCOT[itj2]) {
          tmp.push_back(itj2);
          imp.push_back(iTji);
          inCOT[itj2] = true;
          break;
        }
      } // itji
      // no new un-assigned trajectories?
      if(tmp.empty()) break;
       // try to add more TJ's to the COT
      tjAdded = true;
      while(tjAdded) {
        tjAdded = false;
        for(iTji = 0; iTji < trjint.size(); ++iTji) {
          itj1 = trjint[iTji].itj1;
          itj2 = trjint[iTji].itj2;
          // itj2 available &&  found itj1 in tmp
          if(!inCOT[itj2] && (std::find(tmp.begin(), tmp.end(), itj1) != tmp.end())) {
            // add itj2 to tmp
            tmp.push_back(itj2);
            inCOT[itj2] = true;
            imp.push_back(iTji);
            tjAdded = true;
          } // !inCOT[itj1]
          // itj1 available &&  found itj2 in tmp
          if(!inCOT[itj1] && (std::find(tmp.begin(), tmp.end(), itj2) != tmp.end())) {
            // add itj1 to tmp
            tmp.push_back(itj1);
            inCOT[itj1] = true;
            imp.push_back(iTji);
            tjAdded = true;
          } // !inCOT[itj1]
        } // iTji
      } // tjAdded
      ClsOfTrj.push_back(tmp);
      trjintIndices.push_back(imp);
    } // nit
    
    if(shPrt) {
      myprt<<"FindClustersOfTrajectories list of COTs\n";
      for(unsigned short ii = 0; ii < ClsOfTrj.size(); ++ii) {
        myprt<<ii<<" COT ";
        for(unsigned short jj = 0; jj < ClsOfTrj[ii].size(); ++jj) myprt<<" "<<ClsOfTrj[ii][jj];
        myprt<<"\n";
      } // ii
      myprt<<"not in a COT ";
      for(itj1 = 0; itj1 < inCOT.size(); ++itj1) {
        if(!inCOT[itj1] && tjs.allTraj[itj1].CTP == fPlane) myprt<<" "<<itj1;
      }
    } // shPrt
    
  } // FindClustersOfTrajectories
*/

  ////////////////////////////////////////////////
  void TrajClusterAlg::AddLAHits(Trajectory& tj, unsigned short ipt, bool& sigOK)
  {
    // Large Angle version of AddHits

    TrajPoint& tp = tj.Pts[ipt];
    tp.Hits.clear();
    tp.UseHit.clear();
    sigOK = false;

    // look at adjacent wires for larger angle trajectories
    // We will check the most likely wire first
    std::vector<unsigned int> wires(1);
    wires[0] = std::nearbyint(tp.Pos[0]);
    if(wires[0] > tjs.LastWire[fPlane]-1) return;
    
    unsigned short angRange = AngleRange(tp);
    if(angRange == 0) {
      mf::LogVerbatim("TC")<<"AddLAHits called with AngleRange = 0. Don't do this";
      return;
    }
    // and the adjacent wires next in the most likely order only
    // after the first point has been defined
    if(ipt > 0 && angRange > 1) {
      if(wires[0] > 0) wires.push_back(wires[0] - 1);
      if(wires[0] < tjs.LastWire[fPlane]-1) wires.push_back(wires[0] + 1);
    } // ipt > 0 ...

    if(prt) {
      mf::LogVerbatim myprt("TC");
      myprt<<" AddLAHits: AngleRange "<<angRange<<" Wires under consideration";
      for(auto& wire : wires) myprt<<" "<<wire;
    }

    float trng = 1000;
    if(std::abs(tp.Dir[0]) > 0.001) trng = 2 / (std::abs(tp.Dir[0]) * tjs.UnitsPerTick);
    unsigned short tickRange = trng;
//    std::cout<<"chk "<<std::fixed<<std::setprecision(3)<<tp.Dir[0]<<" "<<(int)tickRange<<" "<<tjs.UnitsPerTick<<"\n";

    // Put the existing hits, used and unused, in a vector to search
    std::vector<unsigned int> tjHits;
    PutTrajHitsInVector(tj, false, tjHits);

    std::vector<unsigned int> hitsInMultiplet;
//    TrajPoint ltp;
    
    for(unsigned short ii = 0; ii < wires.size(); ++ii) {
      unsigned int wire = wires[ii];
      if(wire > tjs.LastWire[fPlane]) continue;
      // Assume a signal exists on a dead wire
      if(tjs.WireHitRange[fPlane][wire].first == -1) sigOK = true;
      if(tjs.WireHitRange[fPlane][wire].first < 0) continue;
      raw::TDCtick_t rawProjTick = tp.Pos[1];
      if(std::abs(tp.Dir[0]) > 1E-4) rawProjTick = (raw::TDCtick_t)(tp.Pos[1] + ((float)wire - tp.Pos[0]) * tp.Dir[1] / tp.Dir[0]);
      rawProjTick /= tjs.UnitsPerTick;
      if(prt) mf::LogVerbatim("TC")<<"Wire "<<wire<<" rawProjTick "<<rawProjTick;
      unsigned int firstHit = (unsigned int)tjs.WireHitRange[fPlane][wire].first;
      unsigned int lastHit = (unsigned int)tjs.WireHitRange[fPlane][wire].second;
      if(firstHit > tjs.fHits.size() - 1 || lastHit > tjs.fHits.size()) {
        mf::LogError("TC")<<"Bad hits "<<firstHit<<" "<<lastHit<<" "<<tjs.fHits.size()<<"\n";
        fQuitAlg = true;
        return;
      }
      for(unsigned int iht = firstHit; iht < lastHit; ++iht) {
        // ensure that it isn't already in this tj
        if(std::find(tjHits.begin(), tjHits.end(), iht) != tjHits.end()) {
          sigOK = true;
          continue;
        }
        // ignore this hit if it was part of the multiplet found in the previous iteration
        if(hitsInMultiplet.size() > 1 && std::find(hitsInMultiplet.begin(), hitsInMultiplet.end(), iht) != hitsInMultiplet.end()) {
          sigOK = true;
          continue;
        }
        GetHitMultiplet(iht, hitsInMultiplet);
        raw::TDCtick_t loTick = tjs.fHits[iht]->StartTick();
        raw::TDCtick_t hiTick = tjs.fHits[iht]->EndTick();
        // sums for hits that are available
        float sum = 0;
        float sumt = 0;
        for(auto& jht : hitsInMultiplet) {
          float chg = tjs.fHits[jht]->Integral();
          sum += chg;
          sumt += chg * tjs.fHits[jht]->PeakTime();
          if(tjs.fHits[jht]->StartTick() < loTick) loTick = tjs.fHits[jht]->StartTick();
          if(tjs.fHits[jht]->EndTick() > hiTick) hiTick = tjs.fHits[jht]->EndTick();
        } // jht
        if(prt && abs(tjs.fHits[iht]->PeakTime() - rawProjTick) < 500) {
          mf::LogVerbatim myprt("TC");
          myprt<<" hitsInMultiplet ";
          for(auto& jht : hitsInMultiplet) myprt<<" "<<PrintHit(tjs.fHits[jht])<<"_"<<tjs.inTraj[jht];
        }
        if(rawProjTick > loTick && rawProjTick < hiTick) sigOK = true;
        if(sum == 0) continue;
        float mPos1 = sumt / sum;
        // estimate the RMS with a Gaussian assumption that the low and high ticks
        // are at the + and - 2.5 sigma points
        float mPos1RMS = (float)(hiTick - loTick) / 5;
        // projection in the coordinate system along the trajectory
        mPos1RMS *= tp.Dir[0];
        // convert to WSE
        mPos1 *= tjs.UnitsPerTick;
        mPos1RMS *= tjs.UnitsPerTick;
        if(mPos1RMS < 0.1) mPos1RMS = 0.1;
        float mDelta = PointTrajDOCA(tjs, wire, mPos1, tp);
        float mPull = mDelta / mPos1RMS;
        if(prt && abs(mPos1/tjs.UnitsPerTick - rawProjTick) < 500) {
          mf::LogVerbatim myprt("TC");
          myprt<<" LAPos "<<tjs.fHits[iht]->WireID().Plane<<":"<<wire<<":"<<rawProjTick;
          myprt<<" dTick "<<hiTick - loTick<<" tickRange "<<tickRange;
          myprt<<" Mult "<<hitsInMultiplet.size();
          myprt<<" mPos1_ticks "<<(int)(mPos1/tjs.UnitsPerTick);
          myprt<<" mDelta "<<std::fixed<<std::setprecision(1)<<mPull;
          myprt<<" mPos1RMS_ticks "<<std::setprecision(1)<<mPos1RMS/tjs.UnitsPerTick;
          myprt<<" mPull "<<std::setprecision(1)<<mPull;
          myprt<<" Signal? "<<sigOK;
          myprt<<" mHits ";
          // only print the hits that will be used
          for(auto& jht : hitsInMultiplet) if(tjs.inTraj[jht] == 0) myprt<<" "<<PrintHit(tjs.fHits[jht])<<"_"<<tjs.inTraj[jht];
        }
        if(mPull > 3) continue;
        sigOK = true;
        if(prt) {
          std::cout<<"OK "<<wire<<":"<<(int)(mPos1/tjs.UnitsPerTick)<<" dTick "<<hiTick - loTick<<" tickRange "<<tickRange;
          std::cout<<" mPos1RMS_ticks "<<std::setprecision(1)<<mPos1RMS/tjs.UnitsPerTick<<"\n";
        }
        // Associate all hits in the multiplet with the TP
        for(auto& jht : hitsInMultiplet) {
          // ensure that it isn't already in this tj
          if(std::find(tjHits.begin(), tjHits.end(), jht) != tjHits.end()) continue;
          tp.Hits.push_back(jht);
          tjHits.push_back(jht);
        } // jht
      } // iht
    } // ii
    
    // no hits found
    if(tp.Hits.empty()) {
      if(prt) mf::LogVerbatim("TC")<<" AddLAHits: No hits found ";
     return;
    }
    
    // Use all of the hits that are available
    tp.UseHit.resize(tp.Hits.size(), false);
    for(unsigned short ii = 0; ii < tp.Hits.size(); ++ii) {
      unsigned int iht = tp.Hits[ii];
      if(tjs.inTraj[iht] == 0) {
        tp.UseHit[ii] = true;
        tjs.inTraj[iht] = tj.ID;
      }
    } // ii
    DefineHitPos(tp);
    if(prt) {
      mf::LogVerbatim myprt("TC");
      myprt<<" HitPos "<<std::fixed<<std::setprecision(1)<<tp.HitPos[0]<<" "<<(int)tp.HitPos[1]/tjs.UnitsPerTick;
      myprt<<" using hits";
      for(auto& iht : tp.Hits) myprt<<" "<<PrintHit(tjs.fHits[iht]);
    }
    SetEndPoints(tjs, tj);
 
  } // AddLAHits

  ////////////////////////////////////////////////
  void TrajClusterAlg::AddHits(Trajectory& tj, unsigned short ipt, bool& sigOK)
  {
    // Try to add hits to the trajectory point ipt on the supplied
    // trajectory
    
    if(fPlane > tjs.FirstWire.size() - 1) {
      mf::LogWarning("TC")<<"AddHits called with invalid fPlane "<<fPlane;
      fQuitAlg = true;
      return;
    }

    if(tj.Pts.empty()) return;
    if(ipt > tj.Pts.size() - 1) return;
    
    // testing
    if(AngleRange(tj.Pts[ipt]) > 0) {
      AddLAHits(tj, ipt, sigOK);
      return;
    }
    
    std::vector<unsigned int> closeHits;
    unsigned int wire, loWire, hiWire, iht, firstHit, lastHit;

    unsigned int lastPtWithUsedHits = tj.EndPt[1];
    TrajPoint& tp = tj.Pts[ipt];

    loWire = std::nearbyint(tp.Pos[0]);
    hiWire = loWire + 1;
    // Move the TP to this wire
    MoveTPToWire(tp, (float)loWire);
    if(hiWire > tjs.LastWire[fPlane]) hiWire = tjs.LastWire[fPlane];
    if(loWire == hiWire) return;
    
    float fwire, ftime, delta;
    
    // find the projection error to this point. Note that if this is the first
    // TP, lastPtWithUsedHits = 0, so the projection error is 0
    float dw = tp.Pos[0] - tj.Pts[lastPtWithUsedHits].Pos[0];
    float dt = tp.Pos[1] - tj.Pts[lastPtWithUsedHits].Pos[1];
    float dpos = sqrt(dw * dw + dt * dt);
    float projErr = dpos * tj.Pts[lastPtWithUsedHits].AngErr;
    // Add this to the Delta RMS factor and construct a cut
    float deltaCut = 3 * (projErr + tp.DeltaRMS);
    if(deltaCut > 5) deltaCut = 5;
    // loosen up a bit if we just passed a block of dead wires
    if(abs(dw) > 20 && DeadWireCount(tp.Pos[0], tj.Pts[lastPtWithUsedHits].Pos[0], tj.CTP) > 10) deltaCut *= 2;
    
    deltaCut *= fProjectionErrFactor;
    
    float bigDelta = 2 * deltaCut;
    // don't let it get outrageously large
    if(bigDelta > 5) bigDelta = 5;
    unsigned int imBig = UINT_MAX;
    
    // projected time in ticks for testing the existence of a hit signal
    raw::TDCtick_t rawProjTick = (float)(tp.Pos[1] / tjs.UnitsPerTick);
    // assume failure
    sigOK = false;
    if(prt) {
      mf::LogVerbatim("TC")<<" AddHits: loWire "<<loWire<<" tp.Pos[0] "<<tp.Pos[0]<<" hiWire "<<hiWire<<" projTick "<<rawProjTick<<" deltaRMS "<<tp.DeltaRMS<<" tp.Dir[0] "<<tp.Dir[0]<<" deltaCut "<<deltaCut<<" dpos "<<dpos<<" projErr "<<projErr;
    }
    
    std::vector<unsigned int> hitsInMultiplet;
    unsigned short localIndex;
    
    for(wire = loWire; wire < hiWire; ++wire) {
      // Assume a signal exists on a dead wire
      if(tjs.WireHitRange[fPlane][wire].first == -1) sigOK = true;
      if(tjs.WireHitRange[fPlane][wire].first < 0) continue;
      firstHit = (unsigned int)tjs.WireHitRange[fPlane][wire].first;
      lastHit = (unsigned int)tjs.WireHitRange[fPlane][wire].second;
      fwire = wire;
      for(iht = firstHit; iht < lastHit; ++iht) {
        if(tjs.inTraj[iht] == tj.ID) continue;
        // ensure that it isn't associated with any existing points. Determine how many
        // of the previous points to check. Check all previous points for LA trajectories
        short nPtsToCheck = 3;
        if(HitIsInTj(tj, iht, nPtsToCheck)) continue;
        if(rawProjTick > tjs.fHits[iht]->StartTick() && rawProjTick < tjs.fHits[iht]->EndTick()) sigOK = true;
        if(tjs.fHits[iht]->Integral() < 1) continue;
        ftime = tjs.UnitsPerTick * tjs.fHits[iht]->PeakTime();
        delta = PointTrajDOCA(tjs, fwire, ftime, tp);
        float dt = std::abs(ftime - tp.Pos[1]);
        GetHitMultiplet(iht, hitsInMultiplet, localIndex);
        if(prt && delta < 100 && dt < 100) {
          mf::LogVerbatim myprt("TC");
          myprt<<"  iht "<<iht;
          myprt<<" "<<tjs.fHits[iht]->WireID().Plane<<":"<<PrintHit(tjs.fHits[iht]);
          myprt<<" delta "<<std::fixed<<std::setprecision(2)<<delta<<" deltaCut "<<deltaCut<<" dt "<<dt;
          myprt<<" BB Mult "<<hitsInMultiplet.size()<<" localIndex "<<localIndex<<" RMS "<<std::setprecision(1)<<tjs.fHits[iht]->RMS();
          myprt<<" Chi "<<std::setprecision(1)<<tjs.fHits[iht]->GoodnessOfFit();
          myprt<<" tjs.inTraj "<<tjs.inTraj[iht];
          myprt<<" Chg "<<(int)tjs.fHits[iht]->Integral();
          myprt<<" Signal? "<<sigOK;
        }
        if(delta < bigDelta) {
          bigDelta = delta;
          imBig = iht;
        }
        if(delta > deltaCut) continue;
        if(std::find(closeHits.begin(), closeHits.end(), iht) != closeHits.end()) continue;
        closeHits.push_back(iht);
        if(hitsInMultiplet.size() > 1) {
          // include all the hits in a multiplet for not large angle TPs
          for(auto& jht : hitsInMultiplet) {
            if(tjs.inTraj[jht] == tj.ID) continue;
            if(std::find(closeHits.begin(), closeHits.end(), jht) != closeHits.end()) continue;
            // ensure that it isn't associated with any existing points
            if(HitIsInTj(tj, jht, nPtsToCheck)) continue;
            closeHits.push_back(jht);
          } // jht
        } // multiplicity > 1
      } // iht
    } // wire
    if(prt) {
      mf::LogVerbatim myprt("TC");
      myprt<<"closeHits ";
      for(auto iht : closeHits) myprt<<" "<<PrintHit(tjs.fHits[iht]);
      if(imBig < tjs.fHits.size()) {
        myprt<<" imBig "<<PrintHit(tjs.fHits[imBig]);
      } else {
        myprt<<" imBig "<<imBig;
      }
    }
    if(closeHits.empty() && imBig == UINT_MAX) {
      if(prt) mf::LogVerbatim("TC")<<" no signal on any wire at tp.Pos "<<tp.Pos[0]<<" "<<tp.Pos[1]<<" tick "<<(int)tp.Pos[1]/tjs.UnitsPerTick<<" closeHits size "<<closeHits.size();
      return;
    }
    if(imBig < tjs.fHits.size() && closeHits.empty()) {
      GetHitMultiplet(imBig, hitsInMultiplet, localIndex);
      for(auto jht : hitsInMultiplet) {
        if(std::find(closeHits.begin(), closeHits.end(), jht) != closeHits.end()) continue;
        if(tjs.inTraj[jht] == tj.ID) continue;
        closeHits.push_back(jht);
      } // jht
      if(prt) mf::LogVerbatim("TC")<<" Added bigDelta hit "<<PrintHit(tjs.fHits[imBig])<<" w delta = "<<bigDelta;
    }
    if(!closeHits.empty()) sigOK = true;
    if(!sigOK) return;
    tp.Hits.insert(tp.Hits.end(), closeHits.begin(), closeHits.end());
    // resize the UseHit vector and assume that none of these hits will be used (yet)
    tp.UseHit.resize(tp.Hits.size(), false);
    // decide which of these hits should be used in the fit. Use a generous maximum delta
    // and require a charge check if we'not just starting out
    bool useChg = true;
    if(ipt > 3) useChg = false;
//    float maxDelta = 10 * tp.DeltaRMS;
    FindUseHits(tj, ipt, 10, useChg);
    DefineHitPos(tp);
    SetEndPoints(tjs, tj);
    if(prt) mf::LogVerbatim("TC")<<" number of close hits "<<closeHits.size()<<" used hits "<<NumUsedHits(tp);
  } // AddHits
  
  ////////////////////////////////////////////////
  void TrajClusterAlg::ReleaseWorkHits()
  {
    // Sets tjs.inTraj[] = 0 and UseHit false for all TPs in work. Called when abandoning work
    for(unsigned short ipt = 0; ipt < work.Pts.size(); ++ipt)  UnsetUsedHits(work.Pts[ipt]);
  } // ReleaseWorkHits
/*
  //////////////////////////////////////////
  void TrajClusterAlg::SetAllHitsUsed(TrajPoint& tp)
  {
    // Sets UseHit true for all availalable hits in tp. Ensure
    // that hits on the opposite side of a used hit (belonging to
    // a finished trajectory) are not set used
    unsigned short ii, lo = 0, hi = tp.Hits.size();
    for(ii = 0; ii < tp.Hits.size(); ++ii) {
      if(tjs.inTraj[tp.Hits[ii]] > 0) {
        // found a used hit. See which side the TP position (hopefully
        // from a good previous fit) is. Let's hope that there is only
        // one used hit...
        float hitpos = tjs.fHits[tp.Hits[ii]]->PeakTime() * tjs.UnitsPerTick;
        if(tp.Pos[1] < hitpos) {
          hi = ii;
        } else {
          lo = ii + 1;
        }
        break;
      } // tjs.inTraj[tp.Hits[ii]] > 0
    } // ii
    for(ii = lo; ii < hi; ++ii) {
      if(tjs.inTraj[tp.Hits[ii]] > 0) continue;
      tp.UseHit[ii] = true;
    } // ii
    DefineHitPos(tp);
  } // SetAllHitsUsed
*/
  //////////////////////////////////////////
  void TrajClusterAlg::UnsetUsedHits(TrajPoint& tp)
  {
    // Sets tjs.inTraj = 0 and UseHit false for all used hits in tp
    for(unsigned short ii = 0; ii < tp.Hits.size(); ++ii) {
      if(tp.UseHit[ii]) {
        tjs.inTraj[tp.Hits[ii]] = 0;
        tp.UseHit[ii] = false;
      } // UseHit
    } // ii
    tp.Chg = 0;
  } // UnsetUsedHits

  //////////////////////////////////////////
  void TrajClusterAlg::FindUseHits(Trajectory& tj, unsigned short ipt, float maxDelta, bool useChg)
  {
    // Decide which hits to use to determine the trajectory point
    // fit, charge, etc. This is done by setting UseHit true and
    // setting tjs.inTraj < 0.
    
    if(ipt > tj.Pts.size() - 1) return;
    TrajPoint& tp = tj.Pts[ipt];
    
    if(tp.Hits.empty()) return;

    // Use everything (unused) for large angle TPs as long
    // as the multiplet doesn't include a hit used in another
    // trajectory

    if(prt) {
      mf::LogVerbatim("TC")<<"FUH:  maxDelta "<<maxDelta<<" useChg requested "<<useChg<<" TPHitsRMS "<<TPHitsRMSTick(tjs, tp, false)<<" AngleRange "<<AngleRange(tp);
    }
    float chgPullCut = 1000;
    if(useChg) chgPullCut = fChgPullCut;
    
    // large angle or maybe starting out a large angle trajectory
    geo::PlaneID iplID = DecodeCTP(tj.CTP);
    unsigned short ipl = iplID.Plane;
    bool fatHit = (TPHitsRMSTick(tjs, tp, false) > 4 * fAveHitRMS[ipl]);
    if(AngleRange(tp) > 0 || (fatHit && tj.Pts.size() < 4)) {
      for(unsigned short ii = 0; ii < tp.Hits.size(); ++ii) {
        unsigned int iht = tp.Hits[ii];
        if(tjs.inTraj[iht] > 0) continue;
        if(tjs.inTraj[iht] == tj.ID) continue;
        tp.UseHit[ii] = true;
        tjs.inTraj[iht] = tj.ID;
      } // ii
      if(prt) mf::LogVerbatim("TC")<<"FUH: isLA or short and fat. Using all hits ";
      return;
    } // IsLargeAngle

    // Find the hit that has the smallest delta
    tp.Delta = maxDelta;
    float delta;
    unsigned short imBest = USHRT_MAX;
    std::vector<float> deltas(tp.Hits.size(), 100);
    // keep track of the best delta - even if it is used
    float bestDelta = maxDelta;
    unsigned short nAvailable = 0;
    for(unsigned short ii = 0; ii < tp.Hits.size(); ++ii) {
      tp.UseHit[ii] = false;
      unsigned int iht = tp.Hits[ii];
      delta = PointTrajDOCA(tjs, iht, tp);
      if(delta < bestDelta) bestDelta = delta;
      if(tjs.inTraj[iht] > 0) continue;
      ++nAvailable;
      if(prt) {
        if(useChg) {
          if(prt) mf::LogVerbatim("TC")<<ii<<"  "<<PrintHit(tjs.fHits[iht])<<" delta "<<delta<<" Chg "<<(int)tjs.fHits[iht]->Integral();
        } else {
          if(prt) mf::LogVerbatim("TC")<<ii<<"  "<<PrintHit(tjs.fHits[iht])<<" delta "<<delta;
        }
      } // prt
      deltas[ii] = delta;
      if(delta < tp.Delta) {
        tp.Delta = delta;
        imBest = ii;
      }
    } // ii
    
    if(prt) mf::LogVerbatim("TC")<<" imBest available "<<imBest<<" single hit. tp.Delta "<<tp.Delta<<" bestDelta "<<bestDelta;
    if(imBest == USHRT_MAX) return;
    unsigned int bestHit = tp.Hits[imBest];
    
    // don't use the best UNUSED hit if the best delta is for a USED hit and it is much better
    if(bestDelta < 0.5 * tp.Delta) return;
    
    if(!useChg || (useChg && (tj.AveChg == 0 || tj.ChgRMS == 0))) {
      // necessary quantities aren't available for more carefull checking
      if(prt) mf::LogVerbatim("TC")<<" tj.AveChg "<<tj.AveChg<<" or tj.ChgRMS "<<tj.ChgRMS<<" not defined yet. Use the best hit";
      tp.UseHit[imBest] = true;
      tjs.inTraj[bestHit] = tj.ID;
      return;
    }
    
    // calculate the charge pull
    
    float bestHitChgPull = (tjs.fHits[bestHit]->Integral() / tj.AveChg - 1) / tj.ChgRMS;
    
    if(prt) mf::LogVerbatim("TC")<<" bestHit "<<PrintHit(tjs.fHits[bestHit])<<" Delta "<<tp.Delta<<" Charge "<<(int)tjs.fHits[bestHit]->Integral()<<" ChgPull "<<bestHitChgPull<<" nAvailable "<<nAvailable<<" tj.AveChg "<<tj.AveChg<<" tj.ChgRMS "<<tj.ChgRMS;
    
    // always use the best hit if the charge pull is OK
    if(bestHitChgPull < chgPullCut) {
      tp.UseHit[imBest] = true;
      tjs.inTraj[bestHit] = tj.ID;
    } // good charge
     else if(nAvailable == 1 && tj.PDGCode == 13 && tp.Delta < 2 * tp.DeltaRMS && bestHitChgPull < 2 * chgPullCut) {
       // special handling for muons. Allow higher charge if the delta is very good
       tp.UseHit[imBest] = true;
       tjs.inTraj[bestHit] = tj.ID;
    }
      
    // nothing fancy if there is only one hit available or if we are just starting out
    if(nAvailable == 1 || tj.Pts.size() < 4) {
      if(prt) mf::LogVerbatim("TC")<<" One hit available. Use it? "<<tp.UseHit[imBest];
      return;
    } // single hit
    
    // calculate some quantities for selecting the best hit
    float bestHitDeltaErr = std::abs(tp.Dir[1]) * 0.17 + std::abs(tp.Dir[0]) * HitTimeErr(bestHit);
    float arg = deltas[imBest] /  bestHitDeltaErr;
    float tpDRMS2 = tp.DeltaRMS * tp.DeltaRMS;
    float bestHitFOM = sqrt(arg * arg + tpDRMS2);
    // scale by charge pull if > 1
    if(useChg && bestHitChgPull > 1) bestHitFOM *= bestHitChgPull;
    
    // find the closest unused neighbor - the second best available hit
    unsigned short secondBest = USHRT_MAX;
    float secondBestDelta = 5;
    for(unsigned short ii = 0; ii < tp.Hits.size(); ++ii) {
      if(ii == imBest) continue;
      unsigned int iht = tp.Hits[ii];
      if(tjs.inTraj[iht] > 0) continue;
      if(deltas[ii] < secondBestDelta) {
        secondBestDelta = deltas[ii];
        secondBest = ii;
      }
    } // ii
    if(secondBest == USHRT_MAX) return;
    
    // determine if the second best hit should be considered with the
    // first as a multiplet. Find the hit separation significance.
    unsigned int secondBestHit = tp.Hits[secondBest];
    float dtick = std::abs(tjs.fHits[bestHit]->PeakTime() - tjs.fHits[secondBestHit]->PeakTime());
    float rms = tjs.fHits[bestHit]->RMS();
    if(tjs.fHits[secondBestHit]->RMS() > rms) rms = tjs.fHits[secondBestHit]->RMS();
    if(dtick / rms > fMultHitSep) {
      if(prt)  mf::LogVerbatim("TC")<<" secondBestHit separation too large. Use the single hit ";
      return;
    }
    
    // calculate some quantities for selecting the second best hit
    float secondBestHitChgPull = (tjs.fHits[secondBestHit]->Integral() / tj.AveChg - 1) / tj.ChgRMS;
    float secondBestHitDeltaErr = std::abs(tp.Dir[1]) * 0.17 + std::abs(tp.Dir[0]) * HitTimeErr(secondBestHit);
    arg = deltas[secondBest] / secondBestHitDeltaErr;
    float secondBestHitFOM = sqrt(arg * arg + tpDRMS2);
    // scale by charge pull if > 1
    if(useChg && secondBestHitChgPull > 1) secondBestHitFOM *= secondBestHitChgPull;
    
    // calculate doublet quantities
    float doubletChg = tjs.fHits[bestHit]->Integral() + tjs.fHits[secondBestHit]->Integral();
    float doubletChgPull = (doubletChg / tj.AveChg - 1) / tj.ChgRMS;
    float doubletTime = (tjs.fHits[bestHit]->Integral() * tjs.fHits[bestHit]->PeakTime() + tjs.fHits[secondBestHit]->Integral() * tjs.fHits[secondBestHit]->PeakTime()) / doubletChg;
    doubletTime *= tjs.UnitsPerTick;
    // Square of delta here
    float doubletDelta2 = PointTrajDOCA2(tjs, tp.Pos[0], doubletTime, tp);
    float doubletTimeErr = (tjs.fHits[bestHit]->Integral() * tjs.fHits[bestHit]->RMS() + tjs.fHits[secondBestHit]->Integral() * tjs.fHits[secondBestHit]->RMS()) / doubletChg;
    doubletTimeErr *= tjs.UnitsPerTick;
    arg = doubletDelta2 / (doubletTimeErr * doubletTimeErr);
    // note that arg is already squared
    float doubletFOM = sqrt(arg + tpDRMS2);
    // scale by charge pull if > 1
    if(useChg && doubletChgPull > 1) doubletFOM *= doubletChgPull;
    
    if(prt) mf::LogVerbatim("TC")<<" bestHit FOM "<<bestHitFOM<<" secondBestHit "<<PrintHit(tjs.fHits[secondBestHit])<<" FOM "<<secondBestHitFOM<<" Doublet FOM "<<doubletFOM;
    
    // Only consider 1 or 2 of the available hits
    // don't use either hit if both are inconsistent with the average charge
    if(useChg && bestHitChgPull > chgPullCut && secondBestHitChgPull > chgPullCut) return;
    // Have 3 choices: the best hit, the second best hit or both combined (doublet)
    // calculate the combined charge pull and delta
    if(doubletChgPull > chgPullCut) {
      // doublet doesn't work. choose the best or second best
      if(bestHitFOM < secondBestHitFOM) {
        tp.UseHit[imBest] = true;
        tjs.inTraj[bestHit] = tj.ID;
        return;
      } else {
        tp.UseHit[secondBest] = true;
        tjs.inTraj[secondBestHit] = tj.ID;
        return;
      }
    } else {
      // consider the doublet. Assume the bestHit is the best
      float fom = bestHitFOM;
      unsigned short jj = imBest;
      unsigned int jhit = bestHit;
      if(secondBestHitFOM < fom) {
        // second best is better
        fom = secondBestHitFOM;
        jj = secondBest;
        jhit = secondBestHit;
      } // secondBestFOM < fom
      if(doubletFOM < fom) {
        // doublet is best
        tp.UseHit[imBest] = true;
        tjs.inTraj[bestHit] = tj.ID;
        tp.UseHit[secondBest] = true;
        tjs.inTraj[secondBestHit] = tj.ID;
        return;
      } // doubletFOM < fom
      // if we got here the jj and jhit variables make sense
      tp.UseHit[jj] = true;
      tjs.inTraj[jhit] = tj.ID;
    } // consider the doublet
 
  } //  FindUseHits

  //////////////////////////////////////////
  void TrajClusterAlg::SetPoorUsedHits(Trajectory& tj, unsigned short ipt)
  {
    // Try to use the hits on this TP by reducing the number of points fitted. This
    // should only be done for reasonably long TJ's
    if(tj.Pts.size() < 2 * fMinPtsFit[fPass]) return;
    
    // Restrict to TPs with only one hit for now
    if(tj.Pts[ipt].Hits.size() != 1) return;
    
    // Count the number of previously added TPs that were not used in the fit starting at ipt
    unsigned short jpt, nNotUsed = 0;
    for(jpt = ipt; jpt > 1; --jpt) {
      if(tj.Pts[jpt].Chg > 0) break;
      ++nNotUsed;
    }
    // Missed 0 or 1 TP. Don't mess with the fit
    if(nNotUsed < 1) return;
    
    // put the hit into the fit and see how it goes
    TrajPoint& tp = tj.Pts[ipt];
    unsigned int iht = tp.Hits[0];
    tp.UseHit[0] = true;
    tjs.inTraj[iht] = tj.ID;
    unsigned short nptsFit = tp.NTPsFit;
    
    // Temp until we get this sorted out. Make sure that tj == work.
    if(tj.Pts.size() != work.Pts.size() || ipt != work.Pts.size() - 1) {
      mf::LogWarning("TC")<<"SetPoorUsedHits: tj != work. Fix this code";
      fQuitAlg = true;
      return;
    }

    // increment the number of points fit ala UpdateWork
    ++tp.NTPsFit;
    FitWork();
    if(tp.FitChi < 2) {
      // Looks like this will work. Return victorious after
      // decrementing NTPsFit
      --tp.NTPsFit;
      return;
    }
    while(tp.FitChi > 2 && tp.NTPsFit > fMinPtsFit[fPass]) {
      if(tp.NTPsFit > 15) {
        nptsFit = 0.7 * nptsFit;
      } else if(tp.NTPsFit > 4) {
        nptsFit -= 2;
      } else {
        nptsFit -= 1;
      }
      if(nptsFit < 3) nptsFit = 2;
      tp.NTPsFit = nptsFit;
      FitWork();
      if(prt) mf::LogVerbatim("TC")<<"  SetPoorUsedHits: FitChi "<<tp.FitChi<<" after reducing NTPsFit to "<<tp.NTPsFit;
      if(tp.NTPsFit <= fMinPtsFit[work.Pass]) break;
    } // reducing number of fitted points
    
    if(tp.FitChi > 2) {
      // Well that didn't work. Undo UseHit and return
      tp.UseHit[0] = false;
      tjs.inTraj[iht] = 0;
    }
    
  } // SetPoorUsedHits

  //////////////////////////////////////////
  void TrajClusterAlg::DefineHitPos(TrajPoint& tp)
  {
    // defines HitPos, HitPosErr2 and Chg for the used hits in the trajectory point
    
    tp.Chg = 0;
    if(tp.Hits.empty()) return;
    if(tp.Hits.size() != tp.UseHit.size()) {
      mf::LogWarning("TC")<<" Hits - UseHit size mis-match";
      fQuitAlg = true;
      return;
    }
    
    unsigned short nused = 0;
    unsigned int iht = 0;
    for(unsigned short ii = 0; ii < tp.UseHit.size(); ++ii) {
      if(tp.UseHit[ii]) {
        iht = tp.Hits[ii];
        ++nused;
      }
    }
    if(nused == 0) return;
    
    // don't bother with rest of this if there is only one hit since it can
    // only reside on one wire
    if(nused == 1) {
      tp.Chg = tjs.fHits[iht]->Integral();
      tp.HitPos[0] = tjs.fHits[iht]->WireID().Wire;
      tp.HitPos[1] = tjs.fHits[iht]->PeakTime() * tjs.UnitsPerTick;
      float wireErr = tp.Dir[1] * 0.289;
      float timeErr = tp.Dir[0] * HitTimeErr(iht);
      tp.HitPosErr2 = wireErr * wireErr + timeErr * timeErr;
      if(prt) mf::LogVerbatim("TC")<<"DefineHitPos: singlet "<<std::fixed<<std::setprecision(1)<<tp.Pos[0]<<":"<<tp.Pos[1]<<" HitPosErr "<<sqrt(tp.HitPosErr2);
      return;
    } // nused == 1
    
    // multiple hits possibly on different wires
    std::vector<unsigned int> hitVec;
    tp.Chg = 0;
    std::array<float, 2> newpos;
    float chg;
    newpos[0] = 0;
    newpos[1] = 0;
    // Find the wire range for hits used in the TP
    unsigned int loWire = INT_MAX;
    unsigned int hiWire = 0;
    for(unsigned short ii = 0; ii < tp.Hits.size(); ++ii) {
      if(!tp.UseHit[ii]) continue;
      unsigned int iht = tp.Hits[ii];
      chg = tjs.fHits[iht]->Integral();
      unsigned int wire = tjs.fHits[iht]->WireID().Wire;
      if(wire < loWire) loWire = wire;
      if(wire > hiWire) hiWire = wire;
      newpos[0] += chg * wire;
      newpos[1] += chg * tjs.fHits[iht]->PeakTime();
      tp.Chg += chg;
      hitVec.push_back(iht);
    } // ii
 
    if(tp.Chg == 0) return;
    
    tp.HitPos[0] = newpos[0] / tp.Chg;
    tp.HitPos[1] = newpos[1] * tjs.UnitsPerTick / tp.Chg;
    
    // Error is the wire error (1/sqrt(12))^2 if all hits are on one wire.
    // Scale it by the wire range
    float dWire = 1 + hiWire - loWire;
    float wireErr = tp.Dir[1] * dWire * 0.289;
    float timeErr2 = tp.Dir[0] * tp.Dir[0] * HitsTimeErr2(hitVec);
    tp.HitPosErr2 = wireErr * wireErr + timeErr2;
    if(prt) mf::LogVerbatim("TC")<<"DefineHitPos: multiplet "<<std::fixed<<std::setprecision(1)<<tp.Pos[0]<<":"<<tp.Pos[1]<<" HitPosErr "<<sqrt(tp.HitPosErr2);

  } // HitPosErr2

  //////////////////////////////////////////
  float TrajClusterAlg::HitTimeErr(const unsigned int iht)
  {
    return tjs.fHits[iht]->RMS() * tjs.UnitsPerTick * fHitErrFac * tjs.fHits[iht]->Multiplicity();
  } // HitTimeErr
  
  //////////////////////////////////////////
  float TrajClusterAlg::HitsTimeErr2(const std::vector<unsigned int>& hitVec)
  {
    // Estimates the error^2 of the time using all hits in hitVec
    if(hitVec.empty()) return 0;
    float err = fHitErrFac * HitsRMSTime(tjs, hitVec);
    return err * err;
  } // HitsTimeErr2
  
  //////////////////////////////////////////
  void TrajClusterAlg::SplitTrajCrossingVertices()
  {
    // This is kind of self-explanatory...
    
    if(tjs.vtx.empty()) return;
    if(tjs.allTraj.empty()) return;
    
    vtxPrt = (debug.Plane == (int)fPlane && debug.Tick < 0);
    if(vtxPrt) mf::LogVerbatim("TC")<<"vtxPrt set for plane "<<fPlane<<" in SplitTrajCrossingVertices";
//    if(vtxPrt) PrintAllTraj("STCV", tjs, debug, USHRT_MAX, 999);
    
    // Splits trajectories in tjs.allTraj that cross a vertex
    unsigned short itj, iv, nTraj = tjs.allTraj.size();
    unsigned short tPass, closePt;
    float doca;
    for(itj = 0; itj < nTraj; ++itj) {
      // obsolete trajectory
      if(tjs.allTraj[itj].AlgMod[kKilled]) continue;
      tPass = tjs.allTraj[itj].Pass;
      for(iv = 0; iv < tjs.vtx.size(); ++iv) {
        // obsolete vertex
        if(tjs.vtx[iv].NTraj == 0) continue;
        // not in the cryostat/tpc/plane
        if(tjs.allTraj[itj].CTP != tjs.vtx[iv].CTP) continue;
        // already assigned to this vertex
//        if(tjs.allTraj[itj].Vtx[0] == iv) continue;
//        if(tjs.allTraj[itj].Vtx[1] == iv) continue;
        // too short
        if(tjs.allTraj[itj].EndPt[1] < 6) continue;
        TrajClosestApproach(tjs.allTraj[itj], tjs.vtx[iv].Pos[0], tjs.vtx[iv].Pos[1], closePt, doca);
        if(vtxPrt)  mf::LogVerbatim("TC")<<" doca "<<doca<<" btw traj "<<itj<<" and tjs.vtx "<<iv<<" closePt "<<closePt<<" in plane "<<fPlane<<" CTP "<<tjs.vtx[iv].CTP;
        if(doca > fMaxVertexTrajSep[tPass]) continue;
        if(vtxPrt)  {
          mf::LogVerbatim("TC")<<"Good doca "<<doca<<" btw traj "<<itj<<" and tjs.vtx "<<iv<<" closePt "<<closePt<<" in plane "<<fPlane<<" CTP "<<tjs.vtx[iv].CTP;
          PrintTrajPoint("STCV", tjs, closePt, 1, tPass, tjs.allTraj[itj].Pts[closePt]);
        }
      } // iv
    } // itj
    
  } // SplitTrajCrossingVertices

  //////////////////////////////////////////
  void TrajClusterAlg::EndMerge()
  {
    // Merge Trajectorys in the order tj1 end = 1 to tj2 end = 0
    if(tjs.allTraj.size() < 2) return;
    if(!fUseAlg[kEndMerge]) return;

    unsigned short tj1, tj2, ipt;
    float chg1rms, chg2rms, chgpull;
    bool notJunk;

    mrgPrt = (debug.Plane == (int)fPlane && debug.Wire < 0);
    if(mrgPrt) mf::LogVerbatim("TC")<<"inside EndMerge on plane "<<fPlane;
    
    // Try several passes if there was a merge
    for(unsigned short nit = 0; nit < 4; ++nit) {
      bool didMerge = false;
      unsigned short tjsize = tjs.allTraj.size();
      for(tj1 = 0; tj1 < tjsize; ++tj1) {
        if(tjs.allTraj[tj1].AlgMod[kKilled]) continue;
        if(tjs.allTraj[tj1].CTP != fCTP) continue;
        // no merge if there is a vertex at the end
        if(tjs.allTraj[tj1].VtxID[1] > 0) continue;
        // ignore bad trajectories
        if(tjs.allTraj[tj1].MCSMom < 5) continue;
        ipt = tjs.allTraj[tj1].EndPt[1];
        TrajPoint& tp1 = tjs.allTraj[tj1].Pts[ipt];
        for(tj2 = 0; tj2 < tjsize; ++tj2) {
          if(tjs.allTraj[tj1].AlgMod[kKilled]) continue;
          if(tj1 == tj2) continue;
          if(tjs.allTraj[tj2].StepDir != tjs.allTraj[tj1].StepDir) continue;
          if(tjs.allTraj[tj2].AlgMod[kKilled]) continue;
          if(tjs.allTraj[tj2].CTP != fCTP) continue;
          if(tjs.allTraj[tj2].VtxID[0] > 0) continue;
          // ignore bad trajectories
          if(tjs.allTraj[tj2].MCSMom < 5) continue;
          ipt = tjs.allTraj[tj2].EndPt[0];
          TrajPoint& tp2 = tjs.allTraj[tj2].Pts[ipt];
          float doca = PointTrajDOCA(tjs, tp1.Pos[0], tp1.Pos[1], tp2);
          if(doca > 50) continue;
          float dang = DeltaAngle(tp1.Ang, tp2.Ang);
          float dwc = DeadWireCount(tp1, tp2);
          float ptSep0;
          if(tjs.allTraj[tj1].StepDir > 0) {
            ptSep0 = tp2.Pos[0] - tp1.Pos[0] - dwc;
          } else {
            ptSep0 = tp1.Pos[0] - tp2.Pos[0] - dwc;
          }
          bool twoMuons = tjs.allTraj[tj1].PDGCode == 13 && tjs.allTraj[tj2].PDGCode == 13;
          float maxWireSkip = fMaxWireSkipNoSignal;
          float tpSep2 = PosSep2(tp1.Pos, tp2.Pos);
          if(mrgPrt) {
            mf::LogVerbatim("TC")<<"Candidate tj1-tj2 "<<tjs.allTraj[tj1].ID<<"_1"<<"-"<<tjs.allTraj[tj2].ID<<"_0"<<" dang "<<dang<<" DeadWireCount "<<(int)dwc<<" ptSep0 "<<ptSep0<<" maxWireSkip "<<maxWireSkip<<" twoMuons? "<<twoMuons<<" doca "<<doca<<" tp1 "<<PrintPos(tjs, tp1)<<" tp2 "<<PrintPos(tjs, tp2)<<" tpSep2 "<<tpSep2;
          }
          // maximum overlap && maximum missed (live) wires
          if(ptSep0 < -2 || ptSep0 > maxWireSkip) continue;
          // Merge short trajectories, regardless of charge as long as the separation is small
          bool isClose = tpSep2 < 15 && dang < 1 && doca < 2;
          bool shortTraj = NumPtsWithCharge(tjs.allTraj[tj1], false) < 15 && NumPtsWithCharge(tjs.allTraj[tj2], false) < 15;
          bool OneIsDeltaRay = tjs.allTraj[tj1].PDGCode == 12 || tjs.allTraj[tj2].PDGCode == 12;
          if(!twoMuons && isClose && (shortTraj || OneIsDeltaRay)) {
            if(mrgPrt) mf::LogVerbatim("TC")<<" EM short tj1-tj2 "<<tjs.allTraj[tj1].ID<<"_1"<<"-"<<tjs.allTraj[tj2].ID<<"_0"<<" tp1 "<<PrintPos(tjs, tp1)<<" tp2 "<<PrintPos(tjs, tp2)<<" doca "<<doca;
            didMerge = MergeAndStore(tj1, tj2);
            if(fQuitAlg) return;
            if(!didMerge) continue;
            tjsize = tjs.allTraj.size();
            tjs.allTraj[tjsize-1].AlgMod[kEndMerge] = true;
            if(OneIsDeltaRay) tjs.allTraj[tjsize-1].PDGCode = 12;
            continue;
          }
          // handle muons here + uB problem with uncharacterized dead wires
          if(twoMuons && dang < 0.4 && doca < 10) {
            if(mrgPrt) mf::LogVerbatim("TC")<<" merge muon tj1-tj2 "<<tjs.allTraj[tj1].ID<<"_1"<<"-"<<tjs.allTraj[tj2].ID<<"_0"<<" tp1 "<<PrintPos(tjs, tp1)<<" tp2 "<<PrintPos(tjs, tp2)<<" doca "<<doca;
            didMerge = MergeAndStore(tj1, tj2);
            if(fQuitAlg) return;
            if(!didMerge) continue;
            tjsize = tjs.allTraj.size();
            tjs.allTraj[tjsize-1].AlgMod[kEndMerge] = true;
            continue;
          }
          if(dang > 0.4) continue;
          // Inflate the doca cut if we are bridging a block of dead wires
          float docaCut = 1;
          if(dwc > 10) docaCut *= 2;
          //        doca = PointTrajDOCA(tjs, tp1.Pos[0], tp1.Pos[1], tp2);
          if(mrgPrt) mf::LogVerbatim("TC")<<" EM "<<tjs.allTraj[tj1].ID<<"_1"<<"-"<<tjs.allTraj[tj2].ID<<"_0 DOCA "<<doca<<" docaCut "<<docaCut<<" dang "<<dang;
          if(doca > docaCut) continue;
          notJunk = (!tjs.allTraj[tj1].AlgMod[kJunkTj] && !tjs.allTraj[tj1].AlgMod[kJunkTj]);
          if(doca > 10) continue;
          // check the charge?
          chgpull = 0;
          if(notJunk && tjs.allTraj[tj1].AveChg > 0 && tjs.allTraj[tj2].AveChg > 0) {
            // calculate charge pull
            chg1rms = tjs.allTraj[tj1].ChgRMS * tjs.allTraj[tj1].AveChg;
            chg2rms = tjs.allTraj[tj2].ChgRMS * tjs.allTraj[tj2].AveChg;
            if(chg2rms > chg1rms) chg1rms = chg2rms;
            if(chg1rms < 1) chg1rms = 0.15 * (tjs.allTraj[tj1].AveChg + tjs.allTraj[tj2].AveChg);
            chgpull = std::abs(tjs.allTraj[tj1].AveChg - tjs.allTraj[tj2].AveChg) / chg1rms;
            if(mrgPrt) mf::LogVerbatim("TC")<<"   chk tp1 chg "<<tp1.AveChg<<" tj1 chg "<<tjs.allTraj[tj1].AveChg<<" rms "<<tjs.allTraj[tj1].ChgRMS<<" tp2 chg "<<tp2.AveChg<<" tj2 chg "<<tjs.allTraj[tj2].AveChg<<" rms "<<tjs.allTraj[tj2].ChgRMS;
          } // charge is known.
          if(mrgPrt) mf::LogVerbatim("TC")<<" chgpull "<<chgpull<<" require < 4";
          if(chgpull > 4) continue;
          // time to merge them
          didMerge = MergeAndStore(tj1, tj2);
          if(fQuitAlg) return;
          if(!didMerge) continue;
          tjsize = tjs.allTraj.size();
          tjs.allTraj[tjsize-1].AlgMod[kEndMerge] = true;
          // flag the killed trajectories as well to aid in debugging
          tjs.allTraj[tj1].AlgMod[kEndMerge] = true;
          tjs.allTraj[tj2].AlgMod[kEndMerge] = true;
          if(mrgPrt) mf::LogVerbatim("TC")<<" merged "<<tjs.allTraj[tj1].ID<<"_1"<<"-"<<tjs.allTraj[tj2].ID<<"_0"<<" dang "<<dang<<" ptSep0 "<<ptSep0<<" doca "<<doca<<" tp1 "<<PrintPos(tjs, tp1)<<" tp2 "<<PrintPos(tjs, tp2)<<" -> tjsize "<<tjsize;
        } // tj2
      } // tj1
      if(!didMerge) break;
    } // nit

  }  // EndMerge
  
  //////////////////////////////////////////
  void TrajClusterAlg::Find2DVertices()
  {
    // Self-explanatory
    // fVertex2DCuts fcl input usage
    // 0 = maximum length of a short trajectory
    // 1 = max vertex - trajectory separation for short trajectories
    // 2 = max vertex - trajectory separation for long trajectories
    // 3 = max position pull for adding TJs to a vertex
    // 4 = max allowed vertex position error
    
    if(fVertex2DCuts[0] <= 0) return;

    if(tjs.allTraj.size() < 2) return;
    
    vtxPrt = (debug.Plane == (int)fPlane && debug.Tick < 0);
    if(vtxPrt) {
      mf::LogVerbatim("TC")<<"vtxPrt set for plane "<<fPlane<<" in Find2DVertices";
      PrintAllTraj("F2DVi", tjs, debug, USHRT_MAX, tjs.allTraj.size());
    }
    
    unsigned short maxShortTjLen = fVertex2DCuts[0];

    for(unsigned short tj1 = 0; tj1 < tjs.allTraj.size() - 1; ++tj1) {
      if(tjs.allTraj[tj1].AlgMod[kKilled]) continue;
      if(tjs.allTraj[tj1].CTP != fCTP) continue;
      if(tjs.allTraj[tj1].PDGCode == 12) continue;
      // First try to attach it to an existing vertex
      if(AttachTrajToAnyVertex(tj1)) continue;
      bool tj1Short = (tjs.allTraj[tj1].EndPt[1] - tjs.allTraj[tj1].EndPt[0] < maxShortTjLen);
      for(unsigned short end1 = 0; end1 < 2; ++end1) {
        // vertex assignment exists?
        if(tjs.allTraj[tj1].VtxID[end1] > 0) continue;
        unsigned short endPt1, oendPt1;
        if(end1 == 0) {
          endPt1 = tjs.allTraj[tj1].EndPt[0];
          oendPt1 = tjs.allTraj[tj1].EndPt[1];
        } else {
          endPt1 = tjs.allTraj[tj1].EndPt[1];
          oendPt1 = tjs.allTraj[tj1].EndPt[0];
        }
        TrajPoint& tp1 = tjs.allTraj[tj1].Pts[endPt1];
        for(unsigned short tj2 = tj1 + 1; tj2 < tjs.allTraj.size(); ++tj2) {
          if(tjs.allTraj[tj2].AlgMod[kKilled]) continue;
          if(tjs.allTraj[tj2].CTP != fCTP) continue;
          if(tjs.allTraj[tj2].PDGCode == 12) continue;
          // First try to attach it to an existing vertex
          if(AttachTrajToAnyVertex(tj2)) continue;
          bool tj2Short = (tjs.allTraj[tj2].EndPt[1] - tjs.allTraj[tj2].EndPt[0] < maxShortTjLen);
          for(unsigned short end2 = 0; end2 < 2; ++end2) {
            unsigned short endPt2, oendPt2;
            if(end2 == 0) {
              endPt2 = tjs.allTraj[tj2].EndPt[0];
              oendPt2 = tjs.allTraj[tj2].EndPt[1];
            } else {
              endPt2 = tjs.allTraj[tj2].EndPt[1];
              oendPt2 = tjs.allTraj[tj2].EndPt[0];
            }
            if(tjs.allTraj[tj2].VtxID[end2] > 0) continue;
            TrajPoint& tp2 = tjs.allTraj[tj2].Pts[endPt2];
            // Rough first cut on the separation between the end points of the
            // two trajectories
            float sepCut = 20;
            if(std::abs(tp1.Pos[0] - tp2.Pos[0]) > sepCut) continue;
            if(std::abs(tp1.Pos[1] - tp2.Pos[1]) > sepCut) continue;
            float wint, tint;
            TrajIntersection(tp1, tp2, wint, tint);
            // make sure this is inside the TPC
            if(wint < 0 || wint > tjs.MaxPos0[fPlane]) continue;
            if(tint < 0 || tint > tjs.MaxPos1[fPlane]) continue;
            // Next cut on separation between the TPs and the intersection point
            if(tj1Short || tj2Short) { sepCut = fVertex2DCuts[1]; } else { sepCut = fVertex2DCuts[2]; }
            float dw1 = wint - tp1.Pos[0];
            if(std::abs(dw1) > sepCut) continue;
            float dt1 = tint - tp1.Pos[1];
            if(std::abs(dt1) > sepCut) continue;
            float dw2 = wint - tp2.Pos[0];
            if(std::abs(dw2) > sepCut) continue;
            float dt2 = tint - tp2.Pos[1];
            if(std::abs(dt2) > sepCut) continue;
            if(vtxPrt) mf::LogVerbatim("TC")<<"F2DV candidate tj1-tj2 "<<tjs.allTraj[tj1].ID<<"_"<<end1<<"-"<<tjs.allTraj[tj2].ID<<"_"<<end2<<" tjs.vtx pos "<<(int)wint<<":"<<(int)(tint/tjs.UnitsPerTick)<<" tp1 "<<PrintPos(tjs, tp1)<<" tp2 "<<PrintPos(tjs, tp2);
            // make sure that the other end isn't closer
            if(PointTrajDOCA2(tjs, wint, tint, tp1) > PointTrajDOCA2(tjs, wint, tint, tjs.allTraj[tj1].Pts[oendPt1])) continue;
            if(PointTrajDOCA2(tjs, wint, tint, tp2) > PointTrajDOCA2(tjs, wint, tint, tjs.allTraj[tj2].Pts[oendPt2])) continue;
            float dang = DeltaAngle(tp1.Ang, tp2.Ang);
            if(vtxPrt) mf::LogVerbatim("TC")<<"  dang "<<dang;
            if(dang < 0.15) continue;
            if(!SignalAtPos(wint, tint, fCTP)) {
              // try nudging it by +/- 1 wire
              wint += 1;
              if(!SignalAtPos(wint, tint, fCTP)) {
                wint -= 2;
                if(!SignalAtPos(wint, tint, fCTP)) continue;
              } // nudge +1
            } // SignalAtPos
            // Ensure that the vertex position is close to an end
            unsigned short closePt1;
            float doca;
            TrajClosestApproach(tjs.allTraj[tj1], wint, tint, closePt1, doca);
            short dpt = abs((short)endPt1 - (short)closePt1);
            if(tjs.allTraj[tj1].EndPt[1] > 4) {
              if(dpt > 3) continue;
            } else {
              // tighter cut for short trajectories
              if(dpt > 0) continue;
            }
            unsigned short closePt2;
            TrajClosestApproach(tjs.allTraj[tj2], wint, tint, closePt2, doca);
            dpt = abs((short)endPt2 - (short)closePt2);
            if(tjs.allTraj[tj2].EndPt[1] > 4) {
              if(dpt > 3) continue;
            } else {
              // tighter cut for short trajectories
              if(dpt > 0) continue;
            }
            if(vtxPrt) mf::LogVerbatim("TC")<<" wint:tint "<<(int)wint<<":"<<(int)tint<<" ticks "<<(int)(tint/tjs.UnitsPerTick)<<" dang "<<dang;
            // make a new temporary vertex
            VtxStore aVtx;
            aVtx.Pos[0] = wint;
            aVtx.Pos[1] = tint;
            aVtx.NTraj = 0;
            aVtx.Pass = fPass;
            aVtx.Topo = end1 + end2;
            aVtx.ChiDOF = 0;
            aVtx.CTP = fCTP;
            aVtx.Fixed = false;
            // try to fit it. We need to give it an ID to do that. Take the next
            // available ID
            unsigned short newVtxID = tjs.vtx.size() + 1;
            aVtx.ID = newVtxID;
            tjs.allTraj[tj1].VtxID[end1] = newVtxID;
            tjs.allTraj[tj2].VtxID[end2] = newVtxID;
            if(!FitVertex(tjs, aVtx, fVertex2DCuts, vtxPrt)) {
              tjs.allTraj[tj1].VtxID[end1] = 0;
              tjs.allTraj[tj2].VtxID[end2] = 0;
              continue;
            }
            // Save it
            tjs.vtx.push_back(aVtx);
            // Try to attach other tjs to it
            unsigned short ivx = tjs.vtx.size() - 1;
            AttachAnyTrajToVertex(ivx);
          } // end2
        } // tj2
      } // end1
    } // tj1

    // Split trajectories that cross a vertex
//    SplitTrajCrossingVertices();
//    FindHammerVertices();
//    FindHammerVertices2();
    
//    for(auto& vx : tjs.vtx) FitVertex(tjs, vx, fVertex2DCuts, vtxPrt);

    if(vtxPrt) PrintAllTraj("F2DVo", tjs, debug, USHRT_MAX, USHRT_MAX);

  } // Find2DVertices
  
  //////////////////////////////////////////
  void TrajClusterAlg::FindHammerVertices2()
  {
    // Variant of FindHammerVertices with slightly different requirements:
    // 1) tj1 is a straight trajectory with most of the points fit
    // 2) No angle requirement between tj1 and tj2
    // 3) Large charge near the intersection point X on tj2
    // tj2       ---X---
    // tj1         /
    // tj1        /
    // tj1       /
    // minimum^2 DOCA of tj1 endpoint with tj2
    
    if(!fUseAlg[kHammerVx2]) return;
   
    unsigned short itj1, end1, endPt1, itj2, closePt2, ipt, ivx;
    unsigned short tjSize = tjs.allTraj.size();
    unsigned short maxPtsFit = 0;
    unsigned short numPtsWithCharge1, numPtsWithCharge2;
    float doca, minDOCA = 3, nPtsFitFrac, intChg, cnt, chgPull;
    bool didaSplit;
    float wint, tint;
    for(itj1 = 0; itj1 < tjSize; ++itj1) {
      if(tjs.allTraj[itj1].AlgMod[kKilled]) continue;
      Trajectory& tj1 = tjs.allTraj[itj1];
      numPtsWithCharge1 = NumPtsWithCharge(tj1, false);
      if(numPtsWithCharge1 < 6) continue;
      // Require that most of the points in tj1 are fitted to a straight line
      maxPtsFit = 0;
      for(ipt = 0; ipt < tj1.Pts.size(); ++ipt) if(tj1.Pts[ipt].Chg > 0 && tj1.Pts[ipt].NTPsFit > maxPtsFit) maxPtsFit = tj1.Pts[ipt].NTPsFit;
      nPtsFitFrac = (float)maxPtsFit / (float)numPtsWithCharge1;
//      std::cout<<"FHV2 "<<tjs.allTraj[itj1].ID<<" "<<maxPtsFit<<" "<<numPtsWithCharge1<<" frac "<<nPtsFitFrac<<"\n";
      if(nPtsFitFrac < 0.8) continue;
      if(vtxPrt) mf::LogVerbatim("TC")<<"FindHammerVertices2: tj1 "<<tj1.ID;
      // Check each end of tj1
      didaSplit = false;
      for(end1 = 0; end1 < 2; ++end1) {
        // vertex assignment exists?
        if(tj1.VtxID[end1] > 0) continue;
        if(end1 == 0) {
          endPt1 = tj1.EndPt[0];
        } else {
          endPt1 = tj1.EndPt[1];
        }
        for(itj2 = 0; itj2 < tjSize; ++itj2) {
          if(itj1 == itj2) continue;
          Trajectory& tj2 = tjs.allTraj[itj2];
          if(tj2.AlgMod[kKilled]) continue;
          // require that both be in the same CTP
          if(tj2.CTP != tj1.CTP) continue;
          numPtsWithCharge2 = NumPtsWithCharge(tj2, true);
          if(numPtsWithCharge2 < 6) continue;
          // ignore if tj1 is a lot shorter than tj2
          // ignore if ChgRMS isn't known
          if(tjs.allTraj[itj2].ChgRMS == 0) continue;
          if(vtxPrt) mf::LogVerbatim("TC")<<" Candidate? "<<tj1.ID<<"  NPWC1 "<<numPtsWithCharge1<<" NPWC2 "<<numPtsWithCharge2;
          if(numPtsWithCharge1 < 0.2 * numPtsWithCharge2) continue;
          // Find the minimum separation between tj1 and tj2
          doca = 5;
          TrajPointTrajDOCA(tjs, tj1.Pts[endPt1], tj2, closePt2, doca);
          if(vtxPrt) mf::LogVerbatim("TC")<<" Candidate? "<<tj1.ID<<"  "<<tj2.ID<<" doca "<<doca<<" tj2.EndPt[0] "<<tj2.EndPt[0]<<" closePt2 "<<closePt2<<" tj2.EndPt[1] "<<tj2.EndPt[1];
          if(doca == minDOCA) continue;
          // ensure that the closest point is not near an end
          if(closePt2 < tj2.EndPt[0] + 3) continue;
          if(closePt2 > tj2.EndPt[1] - 3) continue;
          // Find the intersection point between the tj1 end and tj2 closest Pt
          TrajIntersection(tj1.Pts[endPt1], tj2.Pts[closePt2], wint, tint);
          if(vtxPrt) mf::LogVerbatim("TC")<<" intersection W:T "<<wint<<"  "<<tint<<" ticks "<<tint/tjs.UnitsPerTick;
          // Find the point on tj2 that is closest to this point, overwriting closePt
          doca = minDOCA;
          TrajClosestApproach(tj2, wint, tint, closePt2, doca);
          if(vtxPrt) mf::LogVerbatim("TC")<<" closePt2 on tj2 "<<closePt2<<" doca "<<doca;
          if(doca == minDOCA) continue;
          // require a lot of charge in tj2 in this vicinity compared with the average.
          intChg = 0;
          cnt = 0;
          for(ipt = closePt2 - 2; ipt < closePt2 + 2; ++ipt) {
            if(tjs.allTraj[itj2].Pts[ipt].Chg == 0) continue;
            intChg += tjs.allTraj[itj2].Pts[ipt].Chg;
            ++cnt;
          } // ipt
          if(cnt > 0) {
            intChg /= cnt;
            chgPull = (intChg - tjs.allTraj[itj2].AveChg) / tjs.allTraj[itj2].ChgRMS;
            if(vtxPrt) mf::LogVerbatim("TC")<<" chgPull at intersection point "<<chgPull;
            if(chgPull < 10) continue;
          }
          // we have a winner
          // create a new vertex
          VtxStore aVtx;
          aVtx.Pos = tj2.Pts[closePt2].Pos;
          aVtx.NTraj = 3;
          aVtx.Pass = fPass;
          aVtx.Topo = 6;
          aVtx.ChiDOF = 0;
          aVtx.CTP = fCTP;
          aVtx.Fixed = false;
          tjs.vtx.push_back(aVtx);
          ivx = tjs.vtx.size() - 1;
          tjs.vtx[ivx].ID = ivx + 1;
          if(!SplitAllTraj(tjs, itj2, closePt2, ivx, vtxPrt)) {
            if(vtxPrt) mf::LogVerbatim("TC")<<"FindHammerVertices2: Failed to split trajectory";
            tjs.vtx.pop_back();
            continue;
          }
          tjs.allTraj[itj1].VtxID[end1] = tjs.vtx[ivx].ID;
          tjs.allTraj[itj1].AlgMod[kHammerVx2] = true;
          tj2.AlgMod[kHammerVx] = true;
          tjs.allTraj[tjs.allTraj.size()-1].AlgMod[kHammerVx2] = true;
          didaSplit = true;
          break;
        } // itj2
        if(didaSplit) break;
      } // end1
    } // itj1
  } // FindHammerVertices2
  
  //////////////////////////////////////////
  void TrajClusterAlg::FindHammerVertices()
  {
    // Look for a trajectory that intersects another. Split
    // the trajectory and make a vertex. The convention used
    // is shown pictorially here. Trajectory tj1 must be longer
    // than tj2
    // tj2       ------
    // tj1         /
    // tj1        /
    // tj1       /
    
    if(!fUseAlg[kHammerVx]) return;
    
    unsigned short itj1, end1, endPt1;
    unsigned short itj2, closePt2, end20, end21, ivx;
    unsigned short tjSize = tjs.allTraj.size();
    
    // minimum^2 DOCA of tj1 endpoint with tj2
    float doca, minDOCA = 3, dang;
    bool didaSplit;
    unsigned short tj1len, tj2len;
    
    for(itj1 = 0; itj1 < tjSize; ++itj1) {
      if(tjs.allTraj[itj1].AlgMod[kKilled]) continue;
      // minimum length requirements
      tj1len = tjs.allTraj[itj1].EndPt[1] - tjs.allTraj[itj1].EndPt[0];
      if(tj1len < 6) continue;
      // Check each end of tj1
      didaSplit = false;
      for(end1 = 0; end1 < 2; ++end1) {
        // vertex assignment exists?
        if(tjs.allTraj[itj1].VtxID[end1] > 0) continue;
        if(end1 == 0) {
          endPt1 = tjs.allTraj[itj1].EndPt[0];
        } else {
          endPt1 = tjs.allTraj[itj1].EndPt[1];
        }
        for(itj2 = 0; itj2 < tjSize; ++itj2) {
          if(itj1 == itj2) continue;
          Trajectory& tj2 = tjs.allTraj[itj2];
          if(tj2.AlgMod[kKilled]) continue;
          // require that both be in the same CTP
          if(tj2.CTP != tjs.allTraj[itj1].CTP) continue;
          // length of tj2 cut
          tj2len = tj2.EndPt[1] - tj2.EndPt[0];
          if(tj2len < 6) continue;
          // ignore if tj1 is a lot shorter than tj2
          if(tj1len < 0.5 * tj2len) continue;
          // ignore very long straight trajectories (probably a cosmic muon)
          end20 = tj2.EndPt[0];
          end21 = tj2.EndPt[1];
          if(tj2len > 100 && DeltaAngle(tj2.Pts[end20].Ang, tj2.Pts[end21].Ang) < 0.2) continue;
          // Require no vertex associated with itj2
          if(tj2.VtxID[0] > 0 || tj2.VtxID[1] > 0) continue;
          doca = minDOCA;
          TrajPointTrajDOCA(tjs, tjs.allTraj[itj1].Pts[endPt1], tj2, closePt2, doca);
//          std::cout<<"FHV "<<tj2.CTP<<" "<<tjs.allTraj[itj1].ID<<" "<<tj2.ID<<" doca "<<doca<<"\n";
          if(doca == minDOCA) continue;
          // ensure that the closest point is not near an end
          if(vtxPrt) mf::LogVerbatim("TC")<<"FindHammerVertices: Candidate "<<tjs.allTraj[itj1].ID<<"  "<<tj2.ID<<" doca "<<doca<<" tj2.EndPt[0] "<<tj2.EndPt[0]<<" closePt2 "<<closePt2<<" tj2.EndPt[1] "<<tj2.EndPt[1];
          if(closePt2 < tj2.EndPt[0] + 3) continue;
          if(closePt2 > tj2.EndPt[1] - 3) continue;
          // make an angle cut
          dang = DeltaAngle(tjs.allTraj[itj1].Pts[endPt1].Ang, tj2.Pts[closePt2].Ang);
          if(vtxPrt) mf::LogVerbatim("TC")<<" dang "<<dang<<" fKinkAngCut "<<fKinkAngCut;
          if(dang < fKinkAngCut) continue;
          // we have a winner
          // create a new vertex
          VtxStore aVtx;
          aVtx.Pos = tj2.Pts[closePt2].Pos;
          aVtx.NTraj = 3;
          aVtx.Pass = fPass;
          aVtx.Topo = 6;
          aVtx.ChiDOF = 0;
          aVtx.CTP = fCTP;
          aVtx.Fixed = false;
          tjs.vtx.push_back(aVtx);
          ivx = tjs.vtx.size() - 1;
          tjs.vtx[ivx].ID = ivx + 1;
          if(!SplitAllTraj(tjs, itj2, closePt2, ivx, vtxPrt)) {
            if(vtxPrt) mf::LogVerbatim("TC")<<"FindHammerVertices: Failed to split trajectory";
            tjs.vtx.pop_back();
            continue;
          }
          tjs.allTraj[itj1].VtxID[end1] = tjs.vtx[ivx].ID;
          tjs.allTraj[itj1].AlgMod[kHammerVx] = true;
          tj2.AlgMod[kHammerVx] = true;
          tjs.allTraj[tjs.allTraj.size()-1].AlgMod[kHammerVx] = true;
          didaSplit = true;
          break;
        } // tj2
        if(didaSplit) break;
      } // end1
    } // tj1
    
  } // FindHammerVertices
  
  //////////////////////////////////////////
  bool TrajClusterAlg::AttachAnyTrajToVertex(unsigned short ivx)
  {
    
    if(ivx > tjs.vtx.size() - 1) return false;
    if(tjs.vtx[ivx].NTraj == 0) return false;
    if(fVertex2DCuts[0] < 0) return false;
    
    VtxStore& vx = tjs.vtx[ivx];

    unsigned short nadd = 0;
    for(unsigned short itj = 0; itj < tjs.allTraj.size(); ++itj) {
      Trajectory& tj = tjs.allTraj[itj];
      if(tj.AlgMod[kKilled]) continue;
      if(tj.CTP != vx.CTP) continue;
      if(tj.VtxID[0] == vx.ID || tj.VtxID[1] == vx.ID) continue;
      if(AttachTrajToVertex(tjs, tj, vx, fVertex2DCuts, vtxPrt)) ++nadd;
    } // itj
    if(nadd == 0) return false;
    return true;
    
  } // AttachAnyTrajToVertex
  
  //////////////////////////////////////////
  bool TrajClusterAlg::AttachTrajToAnyVertex(unsigned short itj)
  {
    
    if(itj > tjs.allTraj.size() - 1) return false;
    if(fVertex2DCuts[0] < 0) return false;
    if(tjs.vtx.size() == 0) return false;
    
    Trajectory& tj = tjs.allTraj[itj];
    
    unsigned short nadd = 0;
    for(unsigned short ivx = 0; ivx < tjs.vtx.size(); ++ivx) {
      VtxStore& vx = tjs.vtx[ivx];
      if(vx.NTraj == 0) continue;
      if(vx.CTP != tj.CTP) continue;
      if(tj.VtxID[0] == vx.ID || tj.VtxID[1] == vx.ID) continue;
      if(AttachTrajToVertex(tjs, tj, vx, fVertex2DCuts, vtxPrt)) ++nadd;
    } // ivx
    if(nadd == 0) return false;
    return true;
    
  } // AttachAnyTrajToVertex
  
  //////////////////////////////////////
  void TrajClusterAlg::Find3DVertices(geo::TPCID const& tpcid)
  {
    // Create 3D vertices from 2D vertices. 3D vertices that are matched
    // in all three planes have Ptr2D >= 0 for all planes
    
    
    if(fVertex3DChiCut < 0) return;
    if(tjs.vtx.size() < 2) return;
    
    geo::TPCGeo const& TPC = geom->TPC(tpcid);
    const detinfo::DetectorProperties* detprop = lar::providerFrom<detinfo::DetectorPropertiesService>();
    
    const unsigned int cstat = tpcid.Cryostat;
    const unsigned int tpc = tpcid.TPC;

    // create a array/vector of 2D vertex indices in each plane
    std::vector<std::vector<unsigned short>> vIndex(3);
    unsigned short ipl;
    for(unsigned short ivx = 0; ivx < tjs.vtx.size(); ++ivx) {
      if(tjs.vtx[ivx].NTraj == 0) continue;
      geo::PlaneID iplID = DecodeCTP(tjs.vtx[ivx].CTP);
      if(iplID.TPC != tpc || iplID.Cryostat != cstat) continue;
      ipl = iplID.Plane;
      if(ipl > 2) continue;
      vIndex[ipl].push_back(ivx);
    }
    
    unsigned short vtxInPln = 0;
    for(unsigned short ipl = 0; ipl < TPC.Nplanes(); ++ipl) if(vIndex[ipl].size() > 0) ++vtxInPln;
    if(vtxInPln < 2) return;

    // Y,Z limits of the detector
    double local[3] = {0.,0.,0.};
    double world[3] = {0.,0.,0.};
    
    const geo::TPCGeo &thetpc = geom->TPC(tpc, cstat);
    thetpc.LocalToWorld(local,world);
    // reduce the active area of the TPC by 1 cm to prevent wire boundary issues
    float YLo = world[1]-geom->DetHalfHeight(tpc,cstat) + 1;
    float YHi = world[1]+geom->DetHalfHeight(tpc,cstat) - 1;
    float ZLo = world[2]-geom->DetLength(tpc,cstat)/2 + 1;
    float ZHi = world[2]+geom->DetLength(tpc,cstat)/2 - 1;
    
    vtxPrt = (debug.Plane >= 0) && (debug.Tick == 6666);
    
    if(vtxPrt) {
      mf::LogVerbatim("CC")<<"Inside Find3DVertices";
      PrintAllTraj("F3DV", tjs, debug, USHRT_MAX, tjs.allTraj.size());
    }
    
    // wire spacing in cm
    float wirePitch = geom->WirePitch(0, 1, 0, tpcid.TPC, tpcid.Cryostat);
    
    size_t vsize = tjs.vtx.size();
    // vector of 2D vertices -> 3D vertices.
    std::vector<short> vPtr(vsize, -1);
    // fill temp vectors of 2D vertex X and X errors
    std::vector<float> vX(vsize);
    std::vector<float> vXsigma(vsize);
    float vXp;
    
    double ticks;
    for(unsigned short ivx = 0; ivx < vsize; ++ivx) {
      if(tjs.vtx[ivx].NTraj == 0) continue;
      geo::PlaneID iplID = DecodeCTP(tjs.vtx[ivx].CTP);
      if(iplID.TPC != tpc || iplID.Cryostat != cstat) continue;
      // Convert 2D vertex time error to X error
      ticks = tjs.vtx[ivx].Pos[1] / tjs.UnitsPerTick;
      vX[ivx]  = detprop->ConvertTicksToX(ticks, (int)iplID.Plane, (int)tpc, (int)cstat);
      ticks = (tjs.vtx[ivx].Pos[1] + tjs.vtx[ivx].PosErr[1]) / tjs.UnitsPerTick;
      vXp = detprop->ConvertTicksToX(ticks, (int)iplID.Plane, (int)tpc, (int)cstat);
      vXsigma[ivx] = fabs(vXp - vX[ivx]);
    } // ivx
    
    // temp vector of all 2D vertex matches
    std::vector<Vtx3Store> v3temp;
    
    double y = 0, z = 0;
    TVector3 WPos = {0, 0, 0};
    TrajPoint tp;
    // i, j, k indicates 3 different wire planes
    unsigned short ii, jpl, jj, kpl, kk, ivx, jvx, kvx, i3t;
//    unsigned int iWire, jWire;
    float kX, kWire, kChi, dX, dXChi, dXSigma, dW;
    bool gotit, sigOK;
    // compare vertices in each view
    for(ipl = 0; ipl < 2; ++ipl) {
      for(ii = 0; ii < vIndex[ipl].size(); ++ii) {
        ivx = vIndex[ipl][ii];
        if(ivx > tjs.vtx.size() - 1) {
          mf::LogError("CC")<<"Find3DVertices: bad ivx "<<ivx;
          return;
        }
        // vertex has been matched already
        if(vPtr[ivx] >= 0) continue;
        unsigned int iWire = std::nearbyint(tjs.vtx[ivx].Pos[0]);
        for(jpl = ipl + 1; jpl < 3; ++jpl) {
          for(jj = 0; jj < vIndex[jpl].size(); ++jj) {
            jvx = vIndex[jpl][jj];
            if(jvx > tjs.vtx.size() - 1) {
              mf::LogError("CC")<<"Find3DVertices: bad jvx "<<jvx;
              return;
            }
            // vertex has been matched already
            if(vPtr[jvx] >= 0) continue;
            unsigned int jWire = std::nearbyint(tjs.vtx[jvx].Pos[0]);
            // new stuff
            dX = fabs(vX[ivx] - vX[jvx]);
            dXSigma = sqrt(vXsigma[ivx] * vXsigma[ivx] + vXsigma[jvx] * vXsigma[jvx]);
            dXChi = dX / dXSigma;
            
            if(vtxPrt) mf::LogVerbatim("CC")<<"Find3DVertices: ipl "<<ipl<<" ivx "<<ivx<<" ivX "<<vX[ivx]
              <<" jpl "<<jpl<<" jvx "<<jvx<<" jvX "<<vX[jvx]<<" W:T "<<(int)tjs.vtx[jvx].Pos[0]<<":"<<(int)tjs.vtx[jvx].Pos[1]<<" dXChi "<<dXChi<<" fVertex3DChiCut "<<fVertex3DChiCut;
            
            if(dXChi > fVertex3DChiCut) continue;
            if (geom->HasWire(geo::WireID(cstat, tpc, ipl, iWire))&&
                geom->HasWire(geo::WireID(cstat, tpc, jpl, jWire))){
              geom->IntersectionPoint(iWire, jWire, ipl, jpl, cstat, tpc, y, z);
            }
            else continue;
            if(y < YLo || y > YHi || z < ZLo || z > ZHi) continue;
            WPos[1] = y;
            WPos[2] = z;
            kpl = 3 - ipl - jpl;
            kX = 0.5 * (vX[ivx] + vX[jvx]);
            kWire = -1;
            if(TPC.Nplanes() > 2) {
              kWire = geom->NearestWire(WPos, kpl, tpc, cstat);
              tp.Pos[0] = kWire;
              // See if there is a wire signal nearby in kpl
              tp.Pos[1] = detprop->ConvertXToTicks(kX, kpl, fTpc, fCstat) * tjs.UnitsPerTick;
              tp.CTP = EncodeCTP(fCstat, fTpc, kpl);
              sigOK = SignalAtTp(tp);
              if(vtxPrt) mf::LogVerbatim("TC")<<" 3rd plane pos "<<kWire<<" "<<tp.Pos[1]<<" sig present? "<<sigOK;
              if(!sigOK) continue;
            }
            kpl = 3 - ipl - jpl;
            // save this incomplete 3D vertex
            Vtx3Store v3d;
            v3d.Ptr2D[ipl] = ivx;
            v3d.Ptr2D[jpl] = jvx;
            v3d.Ptr2D[kpl] = -1;
            // see if this is already in the list
            gotit = false;
            for(i3t = 0; i3t < v3temp.size(); ++i3t) {
              if(v3temp[i3t].Ptr2D[0] == v3d.Ptr2D[0] && v3temp[i3t].Ptr2D[1] == v3d.Ptr2D[1] && v3temp[i3t].Ptr2D[2] == v3d.Ptr2D[2]) {
                gotit = true;
                break;
              }
            } // i3t
            if(gotit) continue;
            v3d.X = kX;
            // Use XErr to store dXChi
            v3d.XErr = dXChi;
//            v3d.XErr = dXSigma;
            v3d.Y = y;
            float yzSigma = wirePitch * sqrt(tjs.vtx[ivx].PosErr[0] * tjs.vtx[ivx].PosErr[0] + tjs.vtx[jvx].PosErr[0] * tjs.vtx[jvx].PosErr[0]);
            v3d.YErr = yzSigma;
            v3d.Z = z;
            v3d.ZErr = yzSigma;
            v3d.Wire = kWire;
            v3d.CStat = cstat;
            v3d.TPC = tpc;
            // push the incomplete vertex onto the list
            v3temp.push_back(v3d);
            
            if(vtxPrt) mf::LogVerbatim("CC")<<"Find3DVertices: 2 Plane match ivx "<<ivx<<" P:W:T "<<ipl<<":"<<(int)tjs.vtx[ivx].Pos[0]<<":"<<(int)tjs.vtx[ivx].Pos[1]<<" jvx "<<jvx<<" P:W:T "<<jpl<<":"<<(int)tjs.vtx[jvx].Pos[0]<<":"<<(int)tjs.vtx[jvx].Pos[1]<<" dXChi "<<dXChi<<" yzSigma "<<yzSigma;
            
            if(TPC.Nplanes() == 2) continue;

            // See if the expected position of the vertex in the 3rd view
            // look for a 3 plane match
            for(kk = 0; kk < vIndex[kpl].size(); ++kk) {
              kvx = vIndex[kpl][kk];
              if(vPtr[kvx] >= 0) continue;
              // Wire difference error
              dW = wirePitch * (tjs.vtx[kvx].Pos[0] - kWire) / yzSigma;
              // X difference error
              dX = (vX[kvx] - kX) / dXSigma;
              kChi = 0.5 * sqrt(dW * dW + dX * dX);
              if(kChi < fVertex3DChiCut) {
                // push all complete vertices onto the list
                v3d.X = (vX[kvx] + 2 * kX) / 3;
                v3d.XErr = kChi;
                v3d.Ptr2D[kpl] = kvx;
                // see if this is already in the list
                gotit = false;
                for(i3t = 0; i3t < v3temp.size(); ++i3t) {
                  if(v3temp[i3t].Ptr2D[0] == v3d.Ptr2D[0] && v3temp[i3t].Ptr2D[1] == v3d.Ptr2D[1] && v3temp[i3t].Ptr2D[2] == v3d.Ptr2D[2]) {
                    gotit = true;
                    break;
                  }
                } // i3t
                if(gotit) continue;
                v3temp.push_back(v3d);
                if(vtxPrt) mf::LogVerbatim("CC")<<" kvx "<<kvx<<" kpl "<<kpl
                  <<" wire "<<(int)tjs.vtx[kvx].Pos[0]<<" kTime "<<(int)tjs.vtx[kvx].Pos[1]<<" kChi "<<kChi<<" dW "<<tjs.vtx[kvx].Pos[0] - kWire;
              } // kChi < best
            } // kk
          } // jj
        } // jpl
      } // ii
    } // ipl
    
    if(v3temp.empty()) return;
    
    if(vtxPrt) {
      mf::LogVerbatim("TC")<<"v3temp list";
      for(auto& v3d : v3temp) {
        mf::LogVerbatim("TC")<<v3d.Ptr2D[0]<<" "<<v3d.Ptr2D[1]<<" "<<v3d.Ptr2D[2]<<" wire "<<v3d.Wire<<" "<<v3d.XErr;
      } // v3d
    }
    
    // find the best 3 plane matches
    if(TPC.Nplanes() > 2) {
      float best;
      unsigned short imbest;
      bool gotone = true;
      std::vector<Vtx3Store> v3temp2;
      while(gotone) {
        gotone = false;
        best = fVertex3DChiCut;
        imbest = USHRT_MAX;
        for(ivx = 0; ivx < v3temp.size(); ++ivx) {
          if(v3temp[ivx].Wire < 0) continue;
          if(v3temp[ivx].XErr > best) continue;
          best = v3temp[ivx].XErr;
          imbest = ivx;
        } // ivx
        if(imbest == USHRT_MAX) break;
        // stash the best one temporarily in a second vector
        v3temp2.push_back(v3temp[imbest]);
        // set the XErr large so it isn't checked again
        v3temp[imbest].XErr = 9999;
        // look for these trajectory IDs in other vertices (complete or incomplete)
        for(ivx = 0; ivx < v3temp.size(); ++ivx) {
          if(ivx == imbest) continue;
          for(ipl = 0; ipl < 3; ++ipl) {
            if(v3temp[ivx].Ptr2D[ipl] == v3temp[imbest].Ptr2D[ipl]) {
              v3temp[ivx].XErr = 9999;
              gotone = true;
            }
          } // ipl
        } // ivx
      } // gotone
      // concatenate the two vectors
      v3temp.insert(v3temp.end(), v3temp2.begin(), v3temp2.end());
    } // TPC.Nplanes() > 2

    if(vtxPrt) {
      mf::LogVerbatim("TC")<<"v3temp list after";
      for(auto& v3d : v3temp) {
        if(v3d.XErr > fVertex3DChiCut) continue;
        mf::LogVerbatim("TC")<<v3d.Ptr2D[0]<<" "<<v3d.Ptr2D[1]<<" "<<v3d.Ptr2D[2]<<" wire "<<v3d.Wire<<" "<<v3d.XErr;
      } // v3d
    }

    // Store the vertices
    unsigned short ninc = 0;
    for(auto& v3d : v3temp) {
      if(v3d.XErr > fVertex3DChiCut) continue;
      if(TPC.Nplanes() == 2) {
        v3d.Ptr2D[2] = 666;
      } else {
        if(v3d.Wire >= 0) ++ninc;
      }
      if(vtxPrt) mf::LogVerbatim("CC")<<"3D vtx "<<tjs.vtx3.size()<<" Ptr2D "<<v3d.Ptr2D[0]<<" "<<v3d.Ptr2D[1]<<" "<<v3d.Ptr2D[2]
        <<" wire "<<v3d.Wire;
      tjs.vtx3.push_back(v3d);
    } // ivx

    // Try to complete incomplete vertices
//    if(ninc > 0) CompleteIncomplete3DVertices(tpcid);
    
  } // Find3DVertices
  
  //////////////////////////////////////////
  void TrajClusterAlg::CompleteIncomplete3DVertices(geo::TPCID const& tpcid)
  {
    // Look for trajectories in a plane that lack a 2D vertex as listed in
    // Ptr2D that are near the projected wire. This may trigger splitting trajectories,
    // assigning them to a new 2D vertex and completing 3D vertices

    geo::TPCGeo const& TPC = geom->TPC(tpcid);
    const detinfo::DetectorProperties* detprop = lar::providerFrom<detinfo::DetectorPropertiesService>();

    unsigned short ipl, itj, mPlane, closePt;
    unsigned short nEndPt, ii, end, aVtxIndx;
    unsigned short dpt;
    CTP_t mCTP;
    // A TP for the missing 2D vertex
    TrajPoint tp;
    // 2D vertex
    VtxStore aVtx;
    float doca, maxdoca = 6;
    // list of candidate trajectory indices and ipt index
    std::vector<std::pair<unsigned short, unsigned short>> mTjs;
    for(auto& vx3 : tjs.vtx3) {
      // check for a completed 3D vertex
      if(vx3.Wire < 0) continue;
      mPlane = USHRT_MAX;
      for(ipl = 0; ipl < TPC.Nplanes(); ++ipl) {
        if(vx3.Ptr2D[ipl] >= 0) continue;
        mPlane = ipl;
        break;
      } // ipl
      if(mPlane == USHRT_MAX) continue;
      mCTP = EncodeCTP(vx3.CStat, vx3.TPC, mPlane);
      // X position of the purported missing vertex
      tp.Pos[0] = vx3.Wire;
      tp.Pos[1] = detprop->ConvertXToTicks(vx3.X, mPlane, vx3.TPC, vx3.CStat) * tjs.UnitsPerTick;
      for(itj = 0; itj < tjs.allTraj.size(); ++itj) {
        if(tjs.allTraj[itj].CTP != mCTP) continue;
        if(tjs.allTraj[itj].AlgMod[kKilled]) continue;
        doca = maxdoca;
        // find the closest distance between the vertex and the trajectory
        TrajPointTrajDOCA(tjs, tp, tjs.allTraj[itj], closePt, doca);
        if(prt) mf::LogVerbatim("TC")<<"CI3DV itj ID "<<tjs.allTraj[itj].ID<<" closePT "<<closePt<<" doca "<<doca;
        if(closePt > tjs.allTraj[itj].EndPt[1]) continue;
        mTjs.push_back(std::make_pair(itj, closePt));
      } // itj
      // handle the case where there are one or more TJs with TPs near the ends
      // that make a vertex (a failure by Find2DVertices)
      if(mTjs.empty()) continue;
      aVtxIndx = tjs.vtx.size();
      aVtx.CTP = mCTP;
      aVtx.Topo = 6;
      aVtx.NTraj = mTjs.size();
      // Give it a bogus pass to indicate it wasn't created while stepping
      aVtx.Pass = 9;
      aVtx.Pos = tp.Pos;
      nEndPt = 0;
      for(ii = 0; ii < mTjs.size(); ++ii) {
        itj = mTjs[ii].first;
        closePt = mTjs[ii].second;
        // determine which end is the closest
        if(fabs(closePt - tjs.allTraj[itj].EndPt[0]) < fabs(closePt - tjs.allTraj[itj].EndPt[1])) {
          // closest to the beginning
          end = 0;
        } else {
          // closest to the end
          end = 1;
        }// closest to the end
        dpt = fabs(closePt - tjs.allTraj[itj].EndPt[end]);
        if(dpt < 4) {
          // close to an end
          tjs.allTraj[itj].VtxID[end] = tjs.vtx[aVtxIndx].ID;
          tjs.allTraj[itj].AlgMod[kComp3DVx] = true;
          ++nEndPt;
        } else {
          // closePt is not near an end, so split the trajectory
          if(!SplitAllTraj(tjs, itj, closePt, aVtxIndx, vtxPrt)) {
            mf::LogVerbatim("TC")<<"SplitAllTraj failed. Traj ID "<<tjs.allTraj[itj].ID;
            // failure occurred. Recover
            for(auto mTj : mTjs) {
              unsigned short jtj = mTj.first;
              if(tjs.allTraj[jtj].VtxID[0] == tjs.vtx[aVtxIndx].ID) tjs.allTraj[jtj].VtxID[0] = 0;
              if(tjs.allTraj[jtj].VtxID[1] == tjs.vtx[aVtxIndx].ID) tjs.allTraj[jtj].VtxID[1] = 0;
            } // itj
            continue;
          } // !SplitAllTraj
        } // closePt is not near an end, so split the trajectory
        tjs.allTraj[itj].AlgMod[kComp3DVx] = true;
        itj = tjs.allTraj.size() - 1;
        tjs.allTraj[itj].AlgMod[kComp3DVx] = true;
      } // ii
      tjs.vtx.push_back(aVtx);
      unsigned short ivx = tjs.vtx.size() - 1;
      tjs.vtx[ivx].ID = ivx + 1;
      std::cout<<"CIC new vtx "<<tjs.vtx[ivx].ID<<"\n";
      vx3.Ptr2D[mPlane] = aVtxIndx;
      vx3.Wire = -1;
      if(prt) mf::LogVerbatim("TC")<<"CompleteIncomplete3DVertices: new 2D tjs.vtx "<<aVtxIndx<<" points to 3D tjs.vtx ";
    } // vx3
  } // CompleteIncomplete3DVertices
  
  //////////////////////////////////////////
  short TrajClusterAlg::TPNearVertex(const TrajPoint& tp)
  {
    // Returns the index of a vertex if tp is heading towards it
    for(unsigned short ivx = 0; ivx < tjs.vtx.size(); ++ivx) {
      if(tjs.vtx[ivx].NTraj == 0) continue;
      if(tjs.vtx[ivx].CTP != tp.CTP) continue;
      if(std::abs(tjs.vtx[ivx].Pos[0] - tp.Pos[0]) > 3) continue;
      if(std::abs(tjs.vtx[ivx].Pos[1] - tp.Pos[1]) > 3) continue;
      if(PointTrajDOCA2(tjs, tjs.vtx[ivx].Pos[0], tjs.vtx[ivx].Pos[1], tp) > 3) continue;
      // see if the TP points to the vertex and not away from it
      if(tp.Pos[0] < tjs.vtx[ivx].Pos[0] && tp.Dir[0] > 0) return (short)ivx;
      if(tp.Pos[0] > tjs.vtx[ivx].Pos[0] && tp.Dir[0] < 0) return (short)ivx;
    } // ivx
    return -1;
  } // TPNearVertex


  //////////////////////////////////////////
  void TrajClusterAlg::StepCrawl()
  {
    // Crawl along the direction specified in the traj vector in steps of size step
    // (wire spacing equivalents). Find hits between the last trajectory point and
    // the last trajectory point + step. A new trajectory point is added if hits are
    // found. Crawling continues until no signal is found for two consecutive steps
    // or until a wire or time boundary is reached.
    
    fGoodWork = false;
    fTryWithNextPass = false;
    if(work.Pts.empty()) return;
 
    unsigned short iwt, lastPt;
    unsigned short lastPtWithUsedHits = work.EndPt[1];
    unsigned short lastPtWithHits;
    if(lastPtWithUsedHits != work.Pts.size() - 1) {
      mf::LogWarning("TC")<<"StepCrawl: Starting trajectory has no hits on the leading edge. Assume this is an error and quit.";
      PrintTrajectory("SC", tjs, work, USHRT_MAX);
      return;
    }
    lastPt = lastPtWithUsedHits;
    // Construct a local TP from the last work TP that will be moved on each step.
    // Only the Pos and Dir variables will be used
    TrajPoint ltp;
    ltp.CTP = work.CTP;
    ltp.Pos = work.Pts[lastPt].Pos;
    ltp.Dir = work.Pts[lastPt].Dir;
    // A second TP is cloned from the leading TP of work, updated with hits, fit
    // parameters,etc and possibly pushed onto work as the next TP
    TrajPoint tp ;
    
    // assume it is good from here on
    fGoodWork = true;
    
    unsigned int step;
    float stepSize;
    unsigned short nMissedSteps = 0;

    bool sigOK, keepGoing;
    unsigned short killPts;
    for(step = 1; step < 10000; ++step) {
      unsigned short angRange = AngleRange(ltp);
      if(angRange > 1) { stepSize = 2; } else { stepSize = std::abs(1/ltp.Dir[0]); }
      // make a copy of the previous TP
      lastPt = work.Pts.size() - 1;
      tp = work.Pts[lastPt];
      ++tp.Step;
      // move the local TP position by one step in the right direction
      for(iwt = 0; iwt < 2; ++iwt) ltp.Pos[iwt] += ltp.Dir[iwt] * stepSize;
      if(TPNearVertex(ltp) >= 0) {
        work.AlgMod[kStopAtVtx] = true;
        break;
      }
      // Special handling of very long straight trajectories, e.g. uB cosmic rays
      if(fMuonTag[0] >= 0 && work.PDGCode == 0 && work.Pts.size() > (unsigned short)fMuonTag[0]) {
        work.MCSMom = MCSMom(tjs, work);
        if(work.MCSMom > fMuonTag[1]) work.PDGCode = 13;
        if(prt) mf::LogVerbatim("TC")<<" Check MCSMom "<<work.MCSMom<<" PDGCode "<<work.PDGCode;
      }

      // anything really really long must be a muon
      if(work.Pts.size() > 200) work.PDGCode = 13;
      // copy this position into tp
      tp.Pos = ltp.Pos;
      tp.Dir = ltp.Dir;
      if(prt) {
        mf::LogVerbatim("TC")<<"StepCrawl "<<step<<" Pos "<<tp.Pos[0]<<" "<<tp.Pos[1]<<" Dir "<<tp.Dir[0]<<" "<<tp.Dir[1]<<" stepSize "<<stepSize<<" AngleRange "<<angRange;
      }
      // hit the boundary of the TPC?
      if(tp.Pos[0] < 0 || tp.Pos[0] > tjs.MaxPos0[fPlane]) break;
      if(tp.Pos[1] < 0 || tp.Pos[1] > tjs.MaxPos1[fPlane]) break;
      // remove the old hits and other stuff
      tp.Hits.clear();
      tp.UseHit.clear();
      tp.FitChi = 0; tp.Chg = 0;
      // append to the work trajectory
      work.Pts.push_back(tp);
      // update the index of the last TP
      lastPt = work.Pts.size() - 1;
      // look for hits
      AddHits(work, lastPt, sigOK);
      // If successfull, AddHits has defined UseHit for this TP,
      // set the trajectory endpoints, and defined HitPos.
      lastPtWithUsedHits = work.EndPt[1];
      if(work.Pts[lastPt].Hits.empty()) {
        // Require three points with charge on adjacent wires for small angle
        // stepping.
        if(angRange == 0 && lastPt == 2) return;
//        if(!isLA && lastPt == 2) return;
        // No close hits added.
        ++nMissedSteps;
        // First check for no signal in the vicinity
        if(lastPt > 0) {
          // Ensure that there is a signal here after missing a number of steps on a LA trajectory
          if(angRange > 0 && nMissedSteps > 4 && !SignalAtTp(ltp)) break;
          // the last point with hits (used or not) is the previous point
          lastPtWithHits = lastPt - 1;
          float tps = TrajPointSeparation(work.Pts[lastPtWithHits], ltp);
          float dwc = DeadWireCount(ltp, work.Pts[lastPtWithHits]);
          float nMissedWires = tps * std::abs(ltp.Dir[0]) - dwc;
          float maxWireSkip = fMaxWireSkipNoSignal;
          if(work.PDGCode == 13) maxWireSkip = fMuonTag[2];
          if(prt) mf::LogVerbatim("TC")<<" StepCrawl: no signal at ltp "<<PrintPos(tjs, ltp)<<" nMissedWires "<<std::fixed<<std::setprecision(1)<<nMissedWires<<" dead wire count "<<dwc<<" maxWireSkip "<<maxWireSkip<<" work.PGDCode "<<work.PDGCode;
          if(nMissedWires > maxWireSkip) break;
        }
        // no sense keeping this TP on work if no hits were added
        work.Pts.pop_back();
        continue;
      } // work.Pts[lastPt].Hits.empty()
      // Found hits at this location so reset the missed steps counter
      nMissedSteps = 0;
      if(work.Pts[lastPt].Chg == 0) {
        // There are points on the trajectory by none used in the last step. See
        // how long this has been going on
        float tps = TrajPointSeparation(work.Pts[lastPtWithUsedHits], ltp);
        float dwc = DeadWireCount(ltp, work.Pts[lastPtWithUsedHits]);
        float nMissedWires = tps * std::abs(ltp.Dir[0]) - dwc;
        if(prt)  mf::LogVerbatim("TC")<<" Hits exist on the trajectory but are not used. Missed wires "<<nMissedWires;
        // Keep stepping
        if(prt) PrintTrajectory("SC", tjs, work, lastPt);
        if(nMissedWires > fMaxWireSkipWithSignal) break;
        continue;
      } // tp.Hits.empty()
      // Update the last point fit, etc using the just added hit(s)
      UpdateTraj(work);
      // a failure occurred
      if(!fUpdateTrajOK) return;
      // Quit if we are starting out poorly. This can happen on the 2nd trajectory point
      // when a hit is picked up in the wrong direction
      if(work.Pts.size() == 2 && std::abs(work.Pts[1].Dir[0]) > 0.01 && std::signbit(work.Pts[1].Dir[0]) != std::signbit(work.Pts[0].Dir[0])) {
        if(prt) mf::LogVerbatim("TC")<<" Picked wrong hit on 2nd traj point. Drop trajectory.";
        return;
      }
      if(work.Pts.size() == 3) {
        // ensure that the last hit added is in the same direction as the first two.
        // This is a simple way of doing it
        if(PosSep2(work.Pts[0].HitPos, work.Pts[2].HitPos) < PosSep2(work.Pts[0].HitPos, work.Pts[1].HitPos)) return;
        // ensure that this didn't start as a small angle trajectory and immediately turn
        // into a large angle one
        if(angRange > fMaxAngleRange[work.Pass]) {
          if(prt) mf::LogVerbatim("TC")<<" Wandered into an invalid angle range. Quit stepping.";
          fGoodWork = false;
          return;
        }
      } // work.Pts.size() == 3
      // Ensure that the trajectory meets the angle requirements now that the direction is well known
      if(work.Pts.size() == 4 && AngleRange(work.Pts[lastPt]) > fMaxAngleRange[work.Pass]) return;
      // Update the local TP with the updated position and direction
      ltp.Pos = work.Pts[lastPt].Pos;
      ltp.Dir = work.Pts[lastPt].Dir;
      if(fMaskedLastTP) {
        // see if TPs have been masked off many times and if the
        // environment is clean. If so, return and try with next pass
        // cuts
        if(!MaskedWorkHitsOK()) {
          if(prt) PrintTrajectory("SC", tjs, work, lastPt);
          return;
        }
        // Don't bother with the rest of the checking below if we
        // set all hits not used on this TP
        if(prt) PrintTrajectory("SC", tjs, work, lastPt);
        continue;
      }
      // We have added a TP with hits
      // assume that we aren't going to kill the point we just added, or any
      // of the previous points...
      killPts = 0;
      // assume that we should keep going after killing points
      keepGoing = true;
      // check for a kink. Stop crawling if one is found
      GottaKink(work, killPts);
      if(work.AlgMod[kGottaKink]) keepGoing = false;
      // See if the Chisq/DOF exceeds the maximum.
      // UpdateTraj should have reduced the number of points fit
      // as much as possible for this pass, so this trajectory is in trouble.
      if(killPts == 0 &&  work.Pts[lastPt].FitChi > fMaxChi && work.PDGCode != 13) {
        if(prt) mf::LogVerbatim("TC")<<"   bad FitChi "<<work.Pts[lastPt].FitChi<<" cut "<<fMaxChi;
        fGoodWork = (NumPtsWithCharge(work, true) > fMinPtsFit[fPass]);
        return;
      }
      // print the local tp unless we have killing to do
      if(killPts == 0) {
        if(prt) PrintTrajectory("SC", tjs, work, lastPt);
      } else {
        MaskTrajEndPoints(work, killPts);
        if(!fGoodWork) return;
        unsigned int onWire = (float)(std::nearbyint(work.Pts[lastPt].Pos[0]));
        float nSteps = (float)(step - work.Pts[lastPt - killPts].Step);
        if(prt) mf::LogVerbatim("TC")<<"TRP   killing "<<killPts<<" after "<<nSteps<<" steps from prev TP.  Current tp.Pos "<<tp.Pos[0]<<" "<<tp.Pos[1];
        // move the position
        work.Pts[lastPt].Pos[0] += nSteps * work.Pts[lastPt].Dir[0];
        work.Pts[lastPt].Pos[1] += nSteps * work.Pts[lastPt].Dir[1];
        if(angRange == 0) {
          // put the TP at the wire position prior to the move
          float dw = onWire - work.Pts[lastPt].Pos[0];
          work.Pts[lastPt].Pos[0] = onWire;
          work.Pts[lastPt].Pos[1] += dw * work.Pts[lastPt].Dir[1] / work.Pts[lastPt].Dir[0];
        }
        // copy to the local trajectory point
        ltp.Pos = work.Pts[lastPt].Pos;
        ltp.Dir = work.Pts[lastPt].Dir;
        if(prt) mf::LogVerbatim("TC")<<"  New ltp.Pos     "<<ltp.Pos[0]<<" "<<ltp.Pos[1]<<" ticks "<<(int)ltp.Pos[1]/tjs.UnitsPerTick;
        if(!keepGoing) break;
      }
    } // step
    
    if(prt) mf::LogVerbatim("TC")<<"End StepCrawl with "<<step<<" steps. work size "<<work.Pts.size()<<" fGoodWork = "<<fGoodWork<<" with fTryWithNextPass "<<fTryWithNextPass;

    if(fGoodWork && fTryWithNextPass) {
      mf::LogVerbatim("TC")<<"StepCrawl: Have fGoodWork && fTryWithNextPass true. This shouldn't happen. Fixing it.";
      fTryWithNextPass = false;
    }

  } // StepCrawl
/*
  ////////////////////////////////////////////////
  void TrajClusterAlg::ModifyShortTraj(Trajectory& tj)
  {
    // See if most of the hits in the short trajectory are doublets and
    // we are only using single hits in each TP. If so, see if we would do
    // better by using all of the hits

    if(tj.Pts.size() != 4) return;
    unsigned short ndouble = 0, nsingle = 0;
    for(auto& tp : tj.Pts) {
      if(tp.Hits.size() == 2) {
        ++ndouble;
        if(NumUsedHits(tp) == 1) ++nsingle;
      }
    } // tp
    
    if(ndouble < 2) return;
    if(nsingle < 2) return;
    
    if(prt) {
      mf::LogVerbatim("TC")<<"Inside ModifyShortTraj. ndouble "<<ndouble<<" nsingle "<<nsingle<<" Starting trajectory";
      PrintTrajectory(tjs, tj, USHRT_MAX);
    }
    
    // clone the trajectory, use all the hits in doublets and check the fit
    Trajectory ntj = tj;
    unsigned short ii;
    unsigned int iht;
    std::vector<unsigned int> inTrajHits;
    for(auto& tp : ntj.Pts) {
      if(tp.Hits.size() == 2 && NumUsedHits(tp) == 1) {
        for(ii = 0; ii < tp.Hits.size(); ++ii) {
          if(tp.UseHit[ii]) continue;
          tp.UseHit[ii] = true;
          iht = tp.Hits[ii];
          tjs.inTraj[iht] = ntj.ID;
          // save the hit indices in case we need to undo this
          inTrajHits.push_back(iht);
          mf::LogVerbatim("TC")<<"Use hit "<<PrintHit(tjs.fHits[iht]);
        } // ii
        mf::LogVerbatim("TC")<<" HitPos1 "<<tp.HitPos[1];
        DefineHitPos(tp);
        mf::LogVerbatim("TC")<<" new HitPos1 "<<tp.HitPos[1];
      } // tp.Hits.size() == 2 && NumUsedHits(tp) == 1
    } // tp
    // do the fit
    unsigned short originPt = ntj.Pts.size() - 1;
    unsigned short npts = ntj.Pts[originPt].NTPsFit;
    TrajPoint tpFit;
    unsigned short fitDir = -1;
    FitTraj(ntj, originPt, npts, fitDir, tpFit);
    if(prt) {
      mf::LogVerbatim("TC")<<"tpFit trajectory";
      PrintTrajectory(tjs, ntj, USHRT_MAX);
      mf::LogVerbatim("TC")<<"compare "<<tj.Pts[originPt].FitChi<<" "<<ntj.Pts[originPt].FitChi<<"\n";
    }

    if(ntj.Pts[originPt].FitChi < 1.2 * tj.Pts[originPt].FitChi) {
      // Use the modified trajectory
      tj = ntj;
      tj.AlgMod[kModifyShortTraj] = true;
      if(prt) mf::LogVerbatim("TC")<<"ModifyShortTraj: use modified trajectory";
    } else {
      // Use the original trajectory
      for(auto iht : inTrajHits) tjs.inTraj[iht] = 0;
      if(prt) mf::LogVerbatim("TC")<<"ModifyShortTraj: use original trajectory";
    }
    
  } // ModifyShortTraj
*/
  ////////////////////////////////////////////////
  bool TrajClusterAlg::IsGhost(std::vector<unsigned int>& tHits, unsigned short& ofTraj)
  {
    // Called by FindJunkTraj to see if the passed hits are close to an existing
    // trajectory and if so, they will be used in that other trajectory
    
    ofTraj = USHRT_MAX;
    
    if(!fUseAlg[kUseGhostHits]) return false;
    
    if(tHits.size() < 2) return false;
    // find all nearby hits
    std::vector<unsigned int> hitsInMuliplet, nearbyHits;
    for(auto iht : tHits) {
      GetHitMultiplet(iht, hitsInMuliplet);
      // prevent double counting
      for(auto mht : hitsInMuliplet) {
        if(std::find(nearbyHits.begin(), nearbyHits.end(), mht) == nearbyHits.end()) {
          nearbyHits.push_back(mht);
        }
      } // mht
    } // iht
    
    // vectors of traj IDs, and the occurrence count
    std::vector<unsigned short> tID, tCnt;
    unsigned short itj, indx;
    for(auto iht : nearbyHits) {
      if(tjs.inTraj[iht] <= 0) continue;
      itj = tjs.inTraj[iht];
      for(indx = 0; indx < tID.size(); ++indx) if(tID[indx] == itj) break;
      if(indx == tID.size()) {
        tID.push_back(itj);
        tCnt.push_back(1);
      }  else {
        ++tCnt[indx];
      }
    } // iht
    if(tCnt.empty()) return false;
    
    // Call it a ghost if > 50% of the hits are used by another trajectory
    unsigned short tCut = 0.5 * tHits.size();
    unsigned short ii, jj;
    itj = USHRT_MAX;
    
    if(prt) {
      mf::LogVerbatim myprt("TC");
      myprt<<"IsGhost tHits size "<<tHits.size()<<" cut fraction "<<tCut<<" tID_tCnt";
      for(ii = 0; ii < tCnt.size(); ++ii) myprt<<" "<<tID[ii]<<"_"<<tCnt[ii];
    } // prt
    
    for(ii = 0; ii < tCnt.size(); ++ii) {
      if(tCnt[ii] > tCut) {
        itj = tID[ii] - 1;
        break;
      }
    } // ii
    if(itj == USHRT_MAX) return false;
    
    if(prt) mf::LogVerbatim("TC")<<"is ghost of trajectory "<<tjs.allTraj[itj].ID;

    // Use all hits in tHits that are found in itj
    unsigned int iht, tht;
    for(auto& tp : tjs.allTraj[itj].Pts) {
      for(ii = 0; ii < tp.Hits.size(); ++ii) {
        iht = tp.Hits[ii];
        if(tjs.inTraj[iht] != 0) continue;
        for(jj = 0; jj < tHits.size(); ++jj) {
          tht = tHits[jj];
          if(tht != iht) continue;
          tp.UseHit[ii] = true;
          tjs.inTraj[iht] = tjs.allTraj[itj].ID;
          break;
        } // jj
      } // ii
    } // tp
    tjs.allTraj[itj].AlgMod[kUseGhostHits] = true;
    ofTraj = itj;
    return true;
    
  } // IsGhost
/*
  ////////////////////////////////////////////////
  void TrajClusterAlg::MaybeDeltaRay(Trajectory& tj, bool doMerge)
  {
    // See if the trajectory appears to be a delta ray. This is characterized by a significant fraction of hits
    // in the trajectory belonging to an existing trajectory. This may also flag ghost trajectories...
    // Merge the hits in this trajectory (if it is the work trajectory) into the parent trajectory if doMerge is true
    
    tj.AlgMod[kGhost] = false;
    // vectors of traj IDs, and the occurrence count
    std::vector<unsigned short> tID, tCnt;
    unsigned short itj, indx;
    unsigned short tCut = 0.5 * tj.Pts.size();
    for(auto& tp : tj.Pts) {
      for(auto iht : tp.Hits) {
        if(tjs.inTraj[iht] <= 0) continue;
        itj = tjs.inTraj[iht];
        for(indx = 0; indx < tID.size(); ++indx) if(tID[indx] == itj) break;
        if(indx == tID.size()) {
          tID.push_back(itj);
          tCnt.push_back(1);
        }  else {
          ++tCnt[indx];
        }
      } // iht
    } // tp
    if(tCnt.empty()) return;
    for(indx = 0; indx < tCnt.size(); ++indx) if(tCnt[indx] > tCut) tj.AlgMod[kGhost] = true;
    
    if(!tj.AlgMod[kGhost]) return;
    if(!doMerge) return;
    
    // Merge the hits only if there is just one parent trajectory
    if(tCnt.size() > 1) return;
    
    // put the hits for the input trajectory into tHits
    std::vector<unsigned int> tHits;
    PutTrajHitsInVector(tj, true, tHits);
    if(tHits.empty()) return;
    
    std::cout<<"MaybeDeltaRay needs work. Skipping it\n";
    return;
    
    itj = tID[0] - 1;
    Trajectory& oldtj = tjs.allTraj[itj];
    unsigned short ii;
    unsigned int iht;
    for(auto tht : tHits) {
      // look for this hit in itj. Don't bother updating the trajectory Pos or HitPos
      for(auto& tp : oldtj.Pts) {
        for(ii = 0; ii < tp.Hits.size(); ++ii) {
          iht = tp.Hits[ii];
          if(iht != tht) continue;
          tp.UseHit[ii] = true;
          tjs.inTraj[iht] = oldtj.ID;
          break;
        } // ii
        if(tjs.inTraj[iht] == oldtj.ID) break;
      } // tp
    } // iht
    oldtj.AlgMod[kUseGhostHits] = true;
    fGoodWork = false;
    
  } // MaybeDeltaRay
*/
  ////////////////////////////////////////////////
  void TrajClusterAlg::CheckTraj(Trajectory& tj)
  {
    // Check the quality of the trajectory and possibly trim it.
    
    if(!fGoodWork) return;
    
    fTryWithNextPass = false;

    // ensure that the end points are defined
    SetEndPoints(tjs, tj);
    if(tj.EndPt[0] == tj.EndPt[1]) return;
    
    if(prt) {
      mf::LogVerbatim("TC")<<"inside CheckTraj";
    }
    
    // Ensure that a hit only appears once in the TJ
    if(HasDuplicateHits(work)) {
      if(prt) mf::LogVerbatim("TC")<<" HasDuplicateHits ";
      fGoodWork = false;
      return;
    }
    
    unsigned short newSize;
    unsigned short ipt = tj.EndPt[1];
//    bool isLA = IsLargeAngle(tj.Pts[ipt]);
    unsigned short angRange = AngleRange(tj.Pts[ipt]);
    
    // First remove any TPs at the end that have no hits
    // TODO This shouldn't be done but first check to see what code will break
    // if we don't do it.
    tj.Pts.resize(tj.EndPt[1] + 1);
/*
    for(unsigned short ipt = tj.Pts.size() - 1; ipt > tj.EndPt[0]; --ipt) {
      if(tj.Pts[ipt].Chg > 0) break;
      tj.Pts.pop_back();
    }
*/
    if(AngleRange(tj.Pts[tj.EndPt[1]]) > 0 && HasDuplicateHits(tj)) {
      fGoodWork = false;
      return;
    }
    
    // Fill in any gaps with hits that were skipped, most likely delta rays on muon tracks
    if(angRange == 0) FillGaps(tj);
    
    CalculateQuality(tj);
    if(prt) mf::LogVerbatim("TC")<<" CheckTraj MCSMom "<<tj.MCSMom;
    
    // Update the trajectory parameters at the beginning of the trajectory
    FixTrajBegin(tj);

    // check the fraction of the trajectory points that have hits
    if(fUseAlg[kTrimHits] && NumPtsWithCharge(tj, false) > fMinPts[tj.Pass]) {
      // first ensure that there are at least 2 good TPs at the end after a dead wire section.
      unsigned short dwc = DeadWireCount(tj.Pts[tj.EndPt[1]-1], tj.Pts[tj.EndPt[1]]);
      while(dwc > 0 && NumPtsWithCharge(tj, false) > fMinPts[tj.Pass]) {
        MaskTrajEndPoints(tj, 1);
        if(!fGoodWork) return;
        // clobber the TPs (most of which are presumed to be on dead wires) after
        // the last TP that has used hits
        tj.Pts.resize(tj.EndPt[1]+1);
        dwc = DeadWireCount(tj.Pts[tj.EndPt[1]-1], tj.Pts[tj.EndPt[1]]);
        if(prt) mf::LogVerbatim("TC")<<"kTrimHits: trimmed single hit after dead wire section. new dead wire count "<<dwc;
        tj.AlgMod[kTrimHits] = true;
      }
      // ensure that the distance between the last two points is not more than 1 wire
      unsigned short endPt = tj.EndPt[1];
      if(std::abs(tj.Pts[endPt].Pos[0] - tj.Pts[endPt-1].Pos[0]) > 1.5) {
        if(prt) mf::LogVerbatim("TC")<<"kTrimHits: Masking end point at pos "<<PrintPos(tjs, tj.Pts[endPt]);
        MaskTrajEndPoints(tj, 1);
        if(!fGoodWork) return;
        tj.AlgMod[kTrimHits] = true;
      }
      float nPts = tj.EndPt[1] - tj.EndPt[0] + 1;
      dwc = DeadWireCount(tj.Pts[tj.EndPt[0]], tj.Pts[tj.EndPt[1]]);
      float nPtsWithCharge = NumPtsWithCharge(tj, false);
      float ptFrac = (nPtsWithCharge + dwc) /(nPts + dwc);
      if(prt) mf::LogVerbatim("TC")<<"kTrimHits: nPts "<<(int)nPts<<" DeadWireCount "<<(int)dwc<<" nPtsWithCharge "<<(int)nPtsWithCharge<<" ptFrac "<<ptFrac;
      while(ptFrac < 0.6 && nPts > 1) {
        // mask off points until this condition is satisfied
        UnsetUsedHits(tj.Pts[tj.EndPt[1]]);
        SetEndPoints(tjs, tj);
        nPts = tj.EndPt[1] - tj.EndPt[0] + 1;
        dwc = DeadWireCount(tj.Pts[tj.EndPt[0]], tj.Pts[tj.EndPt[1]]);
        nPtsWithCharge = NumPtsWithCharge(tj, false);
        if(nPtsWithCharge < fMinPts[tj.Pass]) return;
        ptFrac = (nPtsWithCharge + dwc) /(nPts + dwc);
        tj.AlgMod[kTrimHits] = true;
      } // ptFrac < 0.6 && nPts > 1
      if(prt) mf::LogVerbatim("TC")<<" after trim nPts "<<(int)nPts<<" DeadWireCount "<<(int)dwc<<" nPtsWithCharge "<<(int)nPtsWithCharge<<" ptFrac "<<ptFrac;
    } // fUseAlg[kTrimHits]

    // See if this looks like a ghost trajectory and if so, merge the
    // hits and kill this one
    if(fUseAlg[kUseGhostHits]) {
      std::vector<unsigned int> tHits;
      PutTrajHitsInVector(tj, true, tHits);
      unsigned short ofTraj = USHRT_MAX;
      if(IsGhost(tHits, ofTraj)) {
        fGoodWork = false;
        return;
      }
    } // fUseAlg[kUseGhostHits]
    // ignore short trajectories
    if(tj.EndPt[1] < 4) return;
    
    if(angRange == 0) {
      // Not large angle checks

      // remove the last used hit if there is a gap before it
      unsigned short ipt = tj.EndPt[1] - 1;
      if(tj.Pts[ipt].Chg == 0) {
        UnsetUsedHits(tj.Pts[tj.EndPt[1]]);
        SetEndPoints(tjs, tj);
      }

      if(fUseAlg[kCWKink] && tj.EndPt[1] > 8) {
        // look for the signature of a kink near the end of the trajectory.
        // These are: Increasing chisq for the last few hits. Presence of
        // a removed hit near the end. A sudden decrease in the number of
        // TPs in the fit. A change in the average charge of hits. These may
        // not all be present in every situation.
//        if(prt) PrintTrajectory("CT", tjs, tj, USHRT_MAX);
        unsigned short tpGap = USHRT_MAX;
        unsigned short nBigRat = 0;
        float chirat;
        for(unsigned short ii = 1; ii < 3; ++ii) {
          ipt = tj.EndPt[1] - 1 - ii;
          if(tj.Pts[ipt].Chg == 0) {
            tpGap = ipt;
            break;
          }
          chirat = tj.Pts[ipt+1].FitChi / tj.Pts[ipt].FitChi;
          if(chirat > 1.5) ++nBigRat;
          if(prt) mf::LogVerbatim("TC")<<"CheckTraj: chirat "<<ipt<<" chirat "<<chirat<<" "<<tj.Pts[ipt-1].FitChi<<" "<<tj.Pts[ipt].FitChi<<" "<<tj.Pts[ipt+1].FitChi;
        } // ii
        if(prt) mf::LogVerbatim("TC")<<"CheckTraj: nBigRat "<<nBigRat<<" tpGap "<<tpGap;
        if(tpGap != USHRT_MAX && nBigRat > 0) {
          if(tpGap != USHRT_MAX) {
            newSize = tpGap;
          } else {
            newSize = tj.Pts.size() - 3;
          }
          if(prt) mf::LogVerbatim("TC")<<"  Setting tj UseHits from "<<newSize<<" to "<<tj.Pts.size()-1<<" false";
          for(ipt = newSize; ipt < tj.Pts.size(); ++ipt) UnsetUsedHits(tj.Pts[ipt]);
          SetEndPoints(tjs, tj);
          tj.Pts.resize(newSize);
          tj.AlgMod[kCWKink] = true;
          return;
        }
      } // fUseAlg[kCWKink]

      if(fUseAlg[kCWStepChk]) {
        // Compare the number of steps taken per TP near the beginning and
        // at the end
        short nStepBegin = tj.Pts[2].Step - tj.Pts[1].Step;
        short nStepEnd;
        unsigned short lastPt = tj.Pts.size() - 1;
        newSize = tj.Pts.size();
        for(ipt = lastPt; ipt > lastPt - 2; --ipt) {
          nStepEnd = tj.Pts[ipt].Step - tj.Pts[ipt - 1].Step;
          if(nStepEnd > 3 * nStepBegin) newSize = ipt;
        }
        if(prt) mf::LogVerbatim("TC")<<"CheckTraj: check number of steps. newSize "<<newSize<<" tj.Pts.size() "<<tj.Pts.size();
        if(newSize < tj.Pts.size()) {
          for(ipt = newSize; ipt < tj.Pts.size(); ++ipt) UnsetUsedHits(tj.Pts[ipt]);
          SetEndPoints(tjs, tj);
          tj.AlgMod[kCWStepChk] = true;
          tj.Pts.resize(newSize);
          return;
        } // newSize < tj.Pts.size()
      } // fUseAlg[kCWStepChk]
    } // angRange == 0
    
    // Check either large angle or not-large angles
    CheckHiDeltas(tj);
    
    CheckHiMultUnusedHits(tj);
    if(!fGoodWork || fQuitAlg) return;
    
    // lop off high multiplicity hits at the end
    CheckHiMultEndHits(tj);
    
  } // CheckTraj
  
  ////////////////////////////////////////////////
  void TrajClusterAlg::FixTrajBegin(Trajectory& tj)
  {
    // Update the parameters at the beginning of the trajectory. The first few
    // points may not belong to this trajectory since they were added when there was
    // little information. This information may be updated later if ReversePropagate is used
    
    if(!fUseAlg[kFixBegin]) return;
    // only do this once
    if(tj.AlgMod[kFixBegin]) return;
    // ignore short trajectories
    unsigned short npwc = NumPtsWithCharge(tj, false);
    if(npwc < 6) return;
    // ignore somewhat longer trajectories that are curly
    if(npwc < 10 && tj.MCSMom < 100) return;
    // ignore shower-like trajectories
    if(tj.PDGCode == 12) return;
    // ignore junk trajectories
    if(tj.AlgMod[kJunkTj]) return;
    
    // assume that all points in the trajectory were fitted to a line
    unsigned short lastPtFit = tj.EndPt[1];
    
    // Find the last point that includes the first point in the fit
    for(unsigned short ii = 0; ii < tj.Pts.size(); ++ii) {
      unsigned short ipt = tj.EndPt[1] - 1 - ii;
      unsigned short firstPtFit = ipt + 1 - tj.Pts[ipt].NTPsFit;
      if(ipt == 0) break;
      if(firstPtFit > 1) continue;
      lastPtFit= ipt;
      break;
    }
    
    unsigned short firstPt = tj.EndPt[0];
    if(prt) {
      mf::LogVerbatim("TC")<<"FixTrajBegin: lastPtFit "<<lastPtFit<<" firstPt "<<firstPt;
    }
    
    if(lastPtFit == tj.EndPt[0]) return;

    // Find the point with the highest charge at the beginning
    float beginChg = tj.Pts[firstPt].AveChg;
    for(unsigned short ipt = firstPt; ipt < firstPt + 3; ++ipt) {
      if(tj.Pts[ipt].Chg > beginChg) beginChg = tj.Pts[ipt].Chg;
    }
    float chgRat = beginChg / tj.Pts[lastPtFit].AveChg;
    bool stoppingTraj = (chgRat > 2);
    
    if(prt) mf::LogVerbatim("TC")<<" chgRat "<<chgRat<<" stoppingTraj? "<<stoppingTraj;
    
    // update the trajectory for all the intervening points
    for(unsigned short ipt = firstPt; ipt < lastPtFit; ++ipt) {
      TrajPoint& tp = tj.Pts[ipt];
      tp.Dir = tj.Pts[lastPtFit].Dir;
      tp.Ang = tj.Pts[lastPtFit].Ang;
      tp.AngErr = tj.Pts[lastPtFit].AngErr;
      // Correct the projected time to the wire
      float dw = tp.Pos[0] - tj.Pts[lastPtFit].Pos[0];
      if(tp.Dir[0] != 0) tp.Pos[1] = tj.Pts[lastPtFit].Pos[1] + dw * tp.Dir[1] / tp.Dir[0];
      // Try this out. Drop the TPs with charge and re-find them unless they are large angle
      bool newHits = false;
      if(tp.Chg > 0 && AngleRange(tp) == 0) {
        float chgIn = tp.Chg;
        if(stoppingTraj && ipt < firstPt + 4 && tp.Hits.size() > 1) {
          // Pick up all of the hits near the end of a stopping TJ. Start by finding
          // a hit that is used in this point
          unsigned int myht = INT_MAX;
          for(unsigned short ii = 0; ii < tp.Hits.size(); ++ii) {
            if(!tp.UseHit[ii]) continue;
            myht = tp.Hits[ii];
            break;
          } // ii
          if(myht == INT_MAX) {
            mf::LogWarning("TC")<<"FixTrajBegin: Didn't find myht";
            fQuitAlg = true;
            return;
          }
          // next find all hits in the multiplet in which myht resides
          std::vector<unsigned int> hitsInMuliplet;
          GetHitMultiplet(myht, hitsInMuliplet);
          // Use the hits in the multiplet if they are associated with the TP and are available
          for(unsigned short ii = 0; ii < tp.Hits.size(); ++ii) {
            unsigned int iht = tp.Hits[ii];
            if(tjs.inTraj[iht] > 0) continue;
            if(std::find(hitsInMuliplet.begin(), hitsInMuliplet.end(), iht) == hitsInMuliplet.end()) continue;
            tp.UseHit[ii] = true;
            tjs.inTraj[iht] = tj.ID;
          } // ii
        } else {
          float maxDelta = 5 * tj.Pts[tj.EndPt[1]].DeltaRMS;
          if(tj.Pts[ipt].Hits.size() == 1) maxDelta *= 1.5;
          UnsetUsedHits(tp);
          FindUseHits(tj, ipt, maxDelta, true);
        }
        DefineHitPos(tj.Pts[ipt]);
        if(tp.Chg != chgIn) newHits = true;
      }
      tj.Pts[ipt].Delta = PointTrajDOCA(tjs, tj.Pts[ipt].HitPos[0], tj.Pts[ipt].HitPos[1], tj.Pts[ipt]);
      tj.Pts[ipt].DeltaRMS = tj.Pts[lastPtFit].DeltaRMS;
      tj.Pts[ipt].NTPsFit = tj.Pts[lastPtFit].NTPsFit;
      tj.Pts[ipt].FitChi = tj.Pts[lastPtFit].FitChi;
      tj.Pts[ipt].AveChg = tj.Pts[lastPtFit].AveChg;
      tj.Pts[ipt].ChgPull = (tj.Pts[ipt].Chg / tj.AveChg - 1) / tj.ChgRMS;
      if(prt) {
        if(newHits) {
          PrintTrajectory("FTB", tjs, tj, ipt);
        } else {
          PrintTrajectory("ftb", tjs, tj, ipt);
        }
      }
    } // ipt
    tj.AlgMod[kFixBegin] = true;

  } // FixTrajBegin

  ////////////////////////////////////////////////
  void TrajClusterAlg::FillGaps(Trajectory& tj)
  {
    // Fill in any gaps in the trajectory with close hits regardless of charge
   
    if(!fUseAlg[kFillGap]) return;
    
    // start with the first point that has charge
    unsigned short firstPtWithChg = tj.EndPt[0];
    bool first = true;
    float maxDelta = 1;
    while(firstPtWithChg < tj.EndPt[1]) {
      unsigned short nextPtWithChg = firstPtWithChg + 1;
      // find the next point with charge
      for(nextPtWithChg = firstPtWithChg + 1; nextPtWithChg < tj.EndPt[1]; ++nextPtWithChg) {
        if(tj.Pts[nextPtWithChg].Chg > 0) break;
      } // nextPtWithChg
      if(nextPtWithChg == firstPtWithChg + 1) {
        // the next point has charge
        ++firstPtWithChg;
        continue;
      }
      // Found a gap. Make a bare trajectory point at firstPtWithChg that points to nextPtWithChg
      TrajPoint tp;
      MakeBareTrajPoint(tjs, tj.Pts[firstPtWithChg], tj.Pts[nextPtWithChg], tp);
      // Find the maximum delta between hits and the trajectory Pos for all
      // hits on this trajectory
      if(first) {
        maxDelta = MaxHitDelta(tj);
        first = false;
      } // first
      // fill in the gap
      for(unsigned short mpt = firstPtWithChg + 1; mpt < nextPtWithChg; ++mpt) {
        if(tj.Pts[mpt].Chg > 0) {
          mf::LogWarning("TC")<<"FillGaps coding error: firstPtWithChg "<<firstPtWithChg<<" mpt "<<mpt<<" nextPtWithChg "<<nextPtWithChg;
          fQuitAlg = true;
          return;
        }
        bool filled = false;
        for(unsigned short ii = 0; ii < tj.Pts[mpt].Hits.size(); ++ii) {
          unsigned int iht = tj.Pts[mpt].Hits[ii];
          if(tjs.inTraj[iht] > 0) continue;
          float delta = PointTrajDOCA(tjs, iht, tp);
          if(delta > maxDelta) continue;
          if(tj.Pts[mpt].UseHit[ii]) {
            mf::LogWarning("TC")<<"FillGaps: Found UseHit true on TP with no charge "<<tj.ID<<" mpt "<<mpt<<" hit "<<PrintHit(tjs.fHits[iht]);
            fQuitAlg = true;
            return;
          }
          tj.Pts[mpt].UseHit[ii] = true;
          tjs.inTraj[iht] = tj.ID;
          filled = true;
        } // ii
        if(filled) {
          DefineHitPos(tj.Pts[mpt]);
          tj.AlgMod[kFillGap] = true;
          if(prt) PrintTrajPoint("FG", tjs, mpt, tj.StepDir, tj.Pass, tj.Pts[mpt]);
        } // filled
      } // mpt
      firstPtWithChg = nextPtWithChg;
    } // firstPtWithChg
    
  } // FillGaps

  ////////////////////////////////////////////////
  float TrajClusterAlg::MaxHitDelta(Trajectory& tj)
  {
    float delta, md = 0;
    unsigned short ii;
    unsigned int iht;
    for(auto& tp : tj.Pts) {
      for(ii = 0; ii < tp.Hits.size(); ++ii) {
        if(!tp.UseHit[ii]) continue;
        iht = tp.Hits[ii];
        delta = PointTrajDOCA(tjs, iht, tp);
        if(delta > md) md = delta;
      } // ii
    } // pts
    return md;
  } // MaxHitDelta
 
  
  ////////////////////////////////////////////////
  void TrajClusterAlg::CheckHiDeltas(Trajectory& tj)
  {
    // Mask off hits at the beginning and end of the trajectory if
    // Delta is too high
    
    if(!fUseAlg[kHiEndDelta]) return;

    // don't bother with LA trajectories
    if(AngleRange(tj.Pts[tj.EndPt[1]]) > 0) return;
    
    float drms, pull;
    bool didit;
    unsigned short ipt, usePt;

    // Don't check the end if this appears to be a stopping particle since
    // there can be a lot of scatter near the stopping point.

    // Check the beginning first.
    unsigned short endPt = tj.EndPt[0];
    bool checkEnd = (endPt > 0 && !tj.Pts[0].Hits.empty());
    if(checkEnd) {
      // find the first value of delta RMS that is not the default value
      didit = false;
      usePt = USHRT_MAX;
      for(ipt = tj.EndPt[0] + fNPtsAve + 3; ipt < tj.EndPt[1]; ++ipt) {
        if(tj.Pts[ipt].Chg == 0) continue;
        // ignore the default value
        if(tj.Pts[ipt].DeltaRMS == 0.02) continue;
        usePt = ipt;
        break;
      } // ipt
      if(usePt < tj.EndPt[1]) {
        // Scan from usePt back to the beginning. Stop if delta looks suspiciously large
        // and remove all points from the beginning to this point
        unsigned short ii;
        drms = tj.Pts[usePt].DeltaRMS;
        if(drms < 0.02) drms = 0.02;
        for(ii = 0; ii < tj.Pts.size(); ++ii) {
          ipt = usePt - ii;
          pull = tj.Pts[ipt].Delta / drms;
          if(prt) mf::LogVerbatim("TC")<<"CHD begin "<<ipt<<" "<<fPlane<<":"<<PrintPos(tjs, tj.Pts[ipt])<<" Delta "<<tj.Pts[ipt].Delta<<" pull "<<pull<<" usePt "<<usePt;
          if(pull > 5) {
            // unset all TPs from here to the beginning
            for(unsigned short jpt = 0; jpt < ipt + 1; ++jpt) UnsetUsedHits(tj.Pts[jpt]);
            tj.AlgMod[kHiEndDelta] = true;
            didit = true;
            break;
          } // pull > 5
          if(ipt == 0) break;
          if(didit) break;
        } // ii
        if(tj.AlgMod[kHiEndDelta] == true) SetEndPoints(tjs, tj);
      } // usePt != USHRT_MAX
    }

    // now check the other end using a similar procedure
    endPt = tj.EndPt[1];
    checkEnd = (endPt < tj.Pts.size() - 1 && !tj.Pts[endPt + 1].Hits.empty());
    if(checkEnd) {
      usePt = USHRT_MAX;
      didit = false;
      unsigned short cnt = 0;
      for(ipt = tj.EndPt[1]; ipt > tj.EndPt[0]; --ipt) {
        if(tj.Pts[ipt].Chg == 0) continue;
        if(tj.Pts[ipt].DeltaRMS < 0.02) continue;
        usePt = ipt;
        ++cnt;
        if(ipt == 0) break;
        if(cnt > 8) break;
      } // ipt
      if(usePt == USHRT_MAX) return;
      drms = tj.Pts[usePt].DeltaRMS;
      if(drms < 0.02) drms = 0.02;
      if(prt) mf::LogVerbatim("TC")<<"CHD end usePt "<<usePt<<" drms "<<drms;
      if(usePt == USHRT_MAX) return;
      for(ipt = usePt; ipt < tj.EndPt[1] + 1; ++ipt) {
        pull = tj.Pts[ipt].Delta / drms;
        if(prt) mf::LogVerbatim("TC")<<"CHD end "<<ipt<<" "<<fPlane<<":"<<PrintPos(tjs, tj.Pts[ipt])<<" Delta "<<tj.Pts[ipt].Delta<<" pull "<<pull;
        if(pull > 5) {
          // unset all TPs from here to the end
          for(unsigned short jpt = ipt; jpt < tj.EndPt[1] + 1; ++jpt) UnsetUsedHits(tj.Pts[jpt]);
          didit = true;
          tj.AlgMod[kHiEndDelta] = true;
          break;
        } // pull > 5
        if(didit) break;
      } // ipt
    }

    if(tj.AlgMod[kHiEndDelta] == true) SetEndPoints(tjs, tj);
    
  } // CheckHiDeltas
  
  ////////////////////////////////////////////////
  void TrajClusterAlg::CheckHiMultUnusedHits(Trajectory& tj)
  {
    // Check for many unused hits in high multiplicity TPs in work and try to use them
    
    
    if(!fUseAlg[kChkHiMultHits]) return;
    
    // This code might do bad things to short trajectories
    if(NumPtsWithCharge(tj, true) < 6) return;
    if(tj.EndPt[0] == tj.EndPt[1]) return;
    
    // count the number of unused hits multiplicity > 1 hits and decide
    // if the unused hits should be used. This may trigger another
    // call to StepCrawl
    unsigned short ii, stopPt;
    // Use this to see if the high multiplicity Pts are mostly near
    // the end or further upstream
    unsigned short lastMult1Pt = USHRT_MAX;
    // the number of TPs with > 1 hit (HiMult)
    unsigned short nHiMultPt = 0;
    // the total number of hits associated with HiMult TPs
    unsigned short nHiMultPtHits = 0;
    // the total number of used hits associated with HiMult TPs
    unsigned short nHiMultPtUsedHits = 0;
    unsigned int iht;
    // start counting at the leading edge and break if a hit
    // is found that is used in a trajectory
    bool doBreak = false;
    unsigned short jj;
    for(ii = 1; ii < tj.Pts.size(); ++ii) {
      stopPt = tj.EndPt[1] - ii;
      for(jj = 0; jj < tj.Pts[stopPt].Hits.size(); ++jj) {
        iht = tj.Pts[stopPt].Hits[jj];
        if(tjs.inTraj[iht] > 0) {
          doBreak = true;
          break;
        }
      } // jj
      if(doBreak) break;
      // require 2 consecutive multiplicity = 1 points
      if(lastMult1Pt == USHRT_MAX && tj.Pts[stopPt].Hits.size() == 1 && tj.Pts[stopPt-1].Hits.size() == 1) lastMult1Pt = stopPt;
      if(tj.Pts[stopPt].Hits.size() > 1) {
        ++nHiMultPt;
        nHiMultPtHits += tj.Pts[stopPt].Hits.size();
        nHiMultPtUsedHits += NumUsedHits(tj.Pts[stopPt]);
      } // high multiplicity TP
      // stop looking when two consecutive single multiplicity TPs are found
      if(lastMult1Pt != USHRT_MAX) break;
      if(stopPt == 1) break;
    } // ii
    // Don't do this if there aren't a lot of high multiplicity TPs
    float fracHiMult = (float)nHiMultPt / (float)ii;
    if(lastMult1Pt != USHRT_MAX) {
      float nchk = tj.EndPt[1] - lastMult1Pt + 1;
      fracHiMult = (float)nHiMultPt / nchk;
    } else {
      fracHiMult = (float)nHiMultPt / (float)ii;
    }
    float fracHitsUsed = 0;
    if(nHiMultPt > 0 && nHiMultPtHits > 0) fracHitsUsed = (float)nHiMultPtUsedHits / (float)nHiMultPtHits;
    // Use this to limit the number of points fit for trajectories that
    // are close the LA tracking cut
    ii = tj.EndPt[1];
//    bool sortaLargeAngle = (std::abs(tj.Pts[ii].Dir[0]) < fLargeAngle + 0.1);
    bool sortaLargeAngle = (AngleRange(tj.Pts[ii]) == 1);

    if(prt) mf::LogVerbatim("TC")<<"CHMUH: First tjs.inTraj stopPt "<<stopPt<<" fracHiMult "<<fracHiMult<<" fracHitsUsed "<<fracHitsUsed<<" lastMult1Pt "<<lastMult1Pt<<" sortaLargeAngle "<<sortaLargeAngle;
    if(fracHiMult < 0.3) return;
    if(fracHitsUsed > 0.98) return;
    
    float maxDelta = 2.5 * MaxHitDelta(tj);

    if(prt) {
      mf::LogVerbatim("TC")<<" Pts size "<<tj.Pts.size()<<" nHiMultPt "<<nHiMultPt<<" nHiMultPtHits "<<nHiMultPtHits<<" nHiMultPtUsedHits "<<nHiMultPtUsedHits<<" sortaLargeAngle "<<sortaLargeAngle<<" maxHitDelta "<<maxDelta;
    }

    // Use next pass cuts if available
    if(sortaLargeAngle && tj.Pass < fMinPtsFit.size()-1) ++tj.Pass;

    // Make a copy of tj in case something bad happens
    Trajectory TjCopy = tj;
    // and the list of used hits
    std::vector<unsigned int> inTrajHits;
    PutTrajHitsInVector(tj, true, inTrajHits);
    unsigned short ipt;

    // unset the used hits from stopPt + 1 to the end
    for(ipt = stopPt + 1; ipt < tj.Pts.size(); ++ipt) UnsetUsedHits(tj.Pts[ipt]);
    SetEndPoints(tjs, tj);
    unsigned short killPts;
    float delta;
    bool added;
    for(ipt = stopPt + 1; ipt < tj.Pts.size(); ++ipt) {
      // add hits that are within maxDelta and re-fit at each point
      added = false;
      for(ii = 0; ii < tj.Pts[ipt].Hits.size(); ++ii) {
        iht = tj.Pts[ipt].Hits[ii];
        if(prt) mf::LogVerbatim("TC")<<" ipt "<<ipt<<" hit "<<PrintHit(tjs.fHits[iht])<<" inTraj "<<tjs.inTraj[iht]<<" delta "<<PointTrajDOCA(tjs, iht, tj.Pts[ipt]);
        if(tjs.inTraj[iht] > 0) continue;
        delta = PointTrajDOCA(tjs, iht, tj.Pts[ipt]);
        if(delta > maxDelta) continue;
        tj.Pts[ipt].UseHit[ii] = true;
        tjs.inTraj[iht] = tj.ID;
        added = true;
      } // ii
      if(added) DefineHitPos(tj.Pts[ipt]);
      if(tj.Pts[ipt].Chg == 0) continue;
      tj.EndPt[1] = ipt;
      // This will be incremented by one in UpdateTraj
      if(sortaLargeAngle) tj.Pts[ipt].NTPsFit = 2;
      UpdateTraj(tj);
      if(!fUpdateTrajOK) {
        if(prt) mf::LogVerbatim("TC")<<"UpdateTraj failed on point "<<ipt;
        // Clobber the used hits from the corrupted points in tj
        for(unsigned short jpt = stopPt + 1; jpt <= ipt; ++jpt) {
          for(unsigned short jj = 0; jj < tj.Pts[jpt].Hits.size(); ++jj) {
            if(tj.Pts[jpt].UseHit[jj]) tjs.inTraj[tj.Pts[jpt].Hits[jj]] = 0;
          } // jj
        } // jpt
        // restore the original trajectory
        tj = TjCopy;
        // restore the hits
        for(unsigned short jpt = stopPt + 1; jpt <= ipt; ++jpt) {
          for(unsigned short jj = 0; jj < tj.Pts[jpt].Hits.size(); ++jj) {
            if(tj.Pts[jpt].UseHit[jj]) tjs.inTraj[tj.Pts[jpt].Hits[jj]] = tj.ID;
          } // jj
        } // jpt
        return;
      }
      GottaKink(tj, killPts);
      if(killPts > 0) {
        MaskTrajEndPoints(tj, killPts);
        if(!fGoodWork) return;
        break;
      }
      if(prt) PrintTrajectory("CHMUH", tjs, tj, ipt);
    } // ipt
    // if we made it here it must be OK
    SetEndPoints(tjs, tj);
    // Try to extend it, unless there was a kink
    if(tj.AlgMod[kGottaKink]) return;
    // trim the end points although this shouldn't happen
    if(tj.EndPt[1] != tj.Pts.size() - 1) tj.Pts.resize(tj.EndPt[1] + 1);
    tj.AlgMod[kChkHiMultHits] = true;
    // Don't try to extend trajectories that were trimmed
    if(tj.AlgMod[kTrimHits]) return;
    if(prt) mf::LogVerbatim("TC")<<"TRP CheckHiMultUnusedHits successfull. Calling StepCrawl to extend it";
    StepCrawl();
    
  } // CheckHiMultUnusedHits

  
  ////////////////////////////////////////////////
  void TrajClusterAlg::CheckHiMultEndHits(Trajectory& tj)
  {
    // mask off high multiplicity TPs at the end
    if(!fUseAlg[kChkHiMultEndHits]) return;
    if(tj.Pts.size() < 10) return;
    // find the average multiplicity in the first half
    unsigned short aveMult= 0;
    unsigned short ipt, nhalf = tj.Pts.size() / 2;
    unsigned short cnt = 0;
    for(auto& tp : tj.Pts) {
      if(tp.Chg == 0) continue;
      aveMult += tp.Hits.size();
      ++cnt;
      if(cnt == nhalf) break;
    } //  pt
    if(cnt == 0) return;
    aveMult /= cnt;
    if(aveMult == 0) aveMult = 1;
    // convert this into a cut
    aveMult *= 3;
    cnt = 0;
    for(ipt = tj.EndPt[1]; ipt > tj.EndPt[0]; --ipt) {
      if(tj.Pts[ipt].Chg == 0) continue;
      if(tj.Pts[ipt].Hits.size() > aveMult) {
        UnsetUsedHits(tj.Pts[ipt]);
        ++cnt;
        continue;
      }
      break;
    } // ipt
    if(prt) mf::LogVerbatim("TC")<<"CHMEH multiplicity cut "<<aveMult<<" number of TPs masked off "<<cnt;
    if(cnt > 0) {
      tj.AlgMod[kChkHiMultEndHits] = true;
      SetEndPoints(tjs, tj);
    }
  } // CheckHiMultEndHits

    ////////////////////////////////////////////////
  bool TrajClusterAlg::MaskedWorkHitsOK()
  {
    // The hits in the TP at the end of the trajectory were masked off. Decide whether to continue stepping with the
    // current configuration or whether to stop and possibly try with the next pass settings
    unsigned short lastPt = work.EndPt[1];
//    if(NumUsedHits(work.Pts[lastPt]) > 0) return true;
    if(work.Pts[lastPt].Chg > 0) return true;
    
    // count the number of points w/o used hits and the number of close hits
    unsigned short nClose = 0, nMasked = 0;
    for(unsigned short ii = 1; ii < work.Pts.size(); ++ii) {
      unsigned short ipt = work.Pts.size() - ii;
      if(work.Pts[ipt].Chg > 0) break;
      ++nMasked;
      nClose += work.Pts[ipt].Hits.size();
    } // ii
    
    if(nMasked == 0) return true;
    
    if(prt) mf::LogVerbatim("TC")<<"MaskedWorkHitsOK:  nMasked "<<nMasked<<" nClose "<<nClose<<" fMaxWireSkipWithSignal "<<fMaxWireSkipWithSignal;
    
    // Check for many masked hits and nearby hits to see if they can be included
    // Only allow this to happen once
    if(!work.AlgMod[kUnMaskHits] && lastPt > 10 && nMasked > 4 && nMasked == nClose) {
      std::cout<<"Trying it "<<work.ID<<"\n";
      float maxDelta = 4 * work.Pts[lastPt].DeltaRMS;
      for(unsigned short ii = 1; ii < work.Pts.size(); ++ii) {
        unsigned short ipt = work.Pts.size() - ii;
        if(work.Pts[ipt].Chg > 0) break;
        if(work.Pts[ipt].Delta > maxDelta) continue;
        if(work.Pts[ipt].Hits.size() != 1) continue;
        unsigned int iht = work.Pts[ipt].Hits[0];
        if(tjs.inTraj[iht] > 0) continue;
        work.Pts[ipt].UseHit[0] = true;
        tjs.inTraj[iht] = work.ID;
        DefineHitPos(work.Pts[ipt]);
      } // ii
      SetEndPoints(tjs, work);
      // try to fit them all
      work.Pts[lastPt].NTPsFit = NumPtsWithCharge(work, false);
      if(prt) mf::LogVerbatim("TC")<<" Add close hits and try UpdateTraj ";
      UpdateTraj(work);
      work.AlgMod[kUnMaskHits] = true;
      return true;
    }
/*
    // See if we have masked off several TPs and there are a number of close hits that haven't
    // been added. This indicates that maybe we should be using the next pass cuts. The approach
    // here is to add the (previously excluded) hits in the fit and re-fit with next pass settings
    if(fUseAlg[kMaskHits] && work.Pass < (fMinPtsFit.size()-1) && work.Pts.size() > 10 && nMasked > 1 && nClose < 3 * nMasked) {
      // set all of the close hits used for the last TPs. This is a bit scary since we
      // aren't sure that all of these hits are the right ones
      for(ii = 0; ii < work.Pts.size(); ++ii) {
        indx = work.Pts.size() - 1 - ii;
        if(NumUsedHits(work.Pts[indx]) > 0) break;
        unsigned int iht;
        for(unsigned short jj = 0; jj < work.Pts[indx].Hits.size(); ++jj) {
          iht = work.Pts[indx].Hits[jj];
          if(tjs.inTraj[iht] > 0) continue;
          work.Pts[indx].UseHit[jj] = true;
          tjs.inTraj[iht] = work.ID;
        } // jj
      } // ii
      SetEndPoints(tjs, work);
      PrepareWorkForNextPass();
      if(prt) mf::LogVerbatim("TC")<<"MaskedWorkHitsOK: Try next pass with too many missed close hits "<<fTryWithNextPass;
      work.AlgMod[kMaskHits] = true;
      return false;
    }
*/
    // Be a bit more lenient with short trajectories on the first pass if
    // the FitChi is not terribly bad and there is ony one hit associated with the last TP
    if(work.Pass < (fMinPtsFit.size()-1) && work.Pts.size() > 5 && work.Pts.size() < 15 && nMasked < 4
       && work.Pts[lastPt].FitChi < 2 * fMaxChi && work.Pts[lastPt].Hits.size() == 1) {
      // set this hit used if it is available
      unsigned int iht = work.Pts[lastPt].Hits[0];
      if(tjs.inTraj[iht] <= 0) {
        work.Pts[lastPt].UseHit[0] = true;
        tjs.inTraj[iht] = work.ID;
      }
      SetEndPoints(tjs, work);
      PrepareWorkForNextPass();
      if(prt) mf::LogVerbatim("TC")<<"MaskedWorkHitsOK: Try next pass with kinda short, kinda bad trajectory. "<<fTryWithNextPass;
      return false;
    }

    // OK if we haven't exceeded the user cut
    if(nMasked < fMaxWireSkipWithSignal) return true;
    
    // not a lot of close hits try with the next pass settings
    if(nClose < 1.5 * nMasked) {
      // trim the trajectory
      unsigned short newSize = work.Pts.size() - nMasked;
      if(prt) mf::LogVerbatim("TC")<<"MaskedWorkHitsOK:  Trimming work to size "<<newSize;
      work.Pts.resize(newSize);
      PrepareWorkForNextPass();
    }
    return false;
    
  } // MaskedWorkHitsOK
/*
  ////////////////////////////////////////////////
  unsigned short TrajClusterAlg::NumGoodWorkTPs()
  {
    unsigned short cnt = 0;
    for(unsigned short ipt = 0; ipt < work.Pts.size(); ++ipt) {
      if(work.Pts[ipt].Chg == 0) continue;
      ++cnt;
    }
    return cnt;
  } // NumGoodWorkTPs
*/
  ////////////////////////////////////////////////
  void TrajClusterAlg::PrepareWorkForNextPass()
  {
    // Any re-sizing should have been done by the calling routine. This code updates the Pass and adjusts the number of
    // fitted points to get FitCHi < 2
    
    fTryWithNextPass = false;

    // See if there is another pass available
    if(work.Pass > fMinPtsFit.size()-2) return;
    ++work.Pass;
    
    unsigned short lastPt = work.Pts.size() - 1;
    // Return if the last fit chisq is OK
    if(work.Pts[lastPt].FitChi < 1.5) {
      fTryWithNextPass = true;
      return;
    }
    TrajPoint& lastTP = work.Pts[lastPt];
    unsigned short newNTPSFit = lastTP.NTPsFit;
    // only give it a few tries before giving up
    unsigned short nit = 0;

    while(lastTP.FitChi > 1.5 && lastTP.NTPsFit > 2) {
      if(lastTP.NTPsFit > 3) newNTPSFit -= 2;
      else if(lastTP.NTPsFit == 3) newNTPSFit = 2;
      lastTP.NTPsFit = newNTPSFit;
      FitWork();
      if(prt) mf::LogVerbatim("TC")<<"PrepareWorkForNextPass: FitChi is > 1.5 "<<lastTP.FitChi<<" Reduced NTPsFit to "<<lastTP.NTPsFit<<" work.Pass "<<work.Pass;
      if(lastTP.NTPsFit <= fMinPtsFit[work.Pass]) break;
      ++nit;
      if(nit == 3) break;
    }
    // decide if the next pass should indeed be attempted
    if(lastTP.FitChi > 2) return;
    fTryWithNextPass = true;
    
  } // PrepareWorkForNextPass
  
  ////////////////////////////////////////////////
  void TrajClusterAlg::FindHit(std::string someText, unsigned int iht)
  {
    // finds a hit in work
    for(unsigned short ipt = 0; ipt < work.Pts.size(); ++ipt) {
      TrajPoint& tp = work.Pts[ipt];
      if(std::find(tp.Hits.begin(), tp.Hits.end(), iht) != tp.Hits.end()) {
        mf::LogVerbatim("TC")<<someText<<" Found hit "<<work.CTP<<":"<<PrintHit(tjs.fHits[iht])<<" in work. tjs.inTraj = "<<tjs.inTraj[iht]<<" work trajectory below ";
        PrintTrajectory("FH", tjs, work, USHRT_MAX);
        return;
      }
    } // ipt
    // look in tjs.allTraj
    for(unsigned short itj = 0; itj < tjs.allTraj.size(); ++itj) {
      for(unsigned short ipt = 0; ipt < tjs.allTraj[itj].Pts.size(); ++ipt) {
        TrajPoint& tp = tjs.allTraj[itj].Pts[ipt];
        if(std::find(tp.Hits.begin(), tp.Hits.end(), iht) != tp.Hits.end()) {
          mf::LogVerbatim("TC")<<someText<<" Found hit "<<tjs.allTraj[itj].CTP<<" "<<PrintHit(tjs.fHits[iht])<<" tjs.inTraj "<<tjs.inTraj[iht]<<" tjs.allTraj ID "<<tjs.allTraj[itj].ID<<" trajectory below ";
          PrintTrajectory("FH", tjs, tjs.allTraj[itj], USHRT_MAX);
          return;
        }
      } // ipt
    } // itj
  } // FindHit
  
  ////////////////////////////////////////////////
  void TrajClusterAlg::GottaKink(Trajectory& tj, unsigned short& killPts)
  {
    // This routine requires that EndPt is defined
    killPts = 0;

    unsigned short lastPt = tj.EndPt[1];
    if(lastPt < 6) return;
    if(tj.Pts[lastPt].Chg == 0) return;
    
    // A simple check when there are few points being fit
    if(tj.Pts[lastPt].NTPsFit < 6) {
      unsigned short ii, prevPtWithHits = USHRT_MAX;
      unsigned short ipt;
      for(ii = 1; ii < tj.Pts.size(); ++ii) {
        ipt = lastPt - ii;
        if(tj.Pts[ipt].Chg > 0) {
          prevPtWithHits = ipt;
          break;
        }
        if(ipt == 0) break;
      } // ii
      if(prevPtWithHits == USHRT_MAX) return;
      float dang = DeltaAngle(tj.Pts[lastPt].Ang, tj.Pts[prevPtWithHits].Ang);
      if(prt) mf::LogVerbatim("TC")<<"GottaKink Simple check lastPt "<<lastPt<<" prevPtWithHits "<<prevPtWithHits<<" dang "<<dang<<" cut "<<fKinkAngCut;
      // need to be more generous since the angle error isn't being considered - BAD IDEA
      if(dang > fKinkAngCut) {
        killPts = 1;
        tj.AlgMod[kGottaKink] = true;
      }
      // Another case where there are few hits fit just prior to a dead wire
      // section or there were no hits added for several steps or due to a large
      // value of fMaxWireSkipNoSignal. We just added a bogus hit just after this section
      // so the trajectory angle change will be small. Find the angle between the previous
      // point fitted angle and the angle formed by the last two TPs
      if(std::abs(tj.Pts[lastPt-1].Pos[0] - tj.Pts[lastPt].Pos[0]) > 3) {
        TrajPoint tmp;
        MakeBareTrajPoint(tjs, tj.Pts[lastPt-1], tj.Pts[lastPt], tmp);
        dang = DeltaAngle(tmp.Ang, tj.Pts[prevPtWithHits].Ang);
        if(prt) mf::LogVerbatim("TC")<<"GottaKink Simple check after gap lastPt "<<lastPt<<" prevPtWithHits "<<prevPtWithHits<<" dang "<<dang<<" cut "<<fKinkAngCut;
        if(dang > 1.5 * fKinkAngCut) {
          killPts = 1;
          tj.AlgMod[kGottaKink] = true;
        }
      }
      // Few points fit in a long trajectory, indicating that chisq went to pot
      // maybe because we just passed through a bad section. Go back a few points
      // and check
      if(tj.Pts.size() < 10) return;
      if(dang < 0.5 * fKinkAngCut) return;
      for(ii = 1; ii < 6; ++ii) {
        ipt = lastPt - ii;
        if(tj.Pts[ipt].Chg == 0) {
          killPts = ii;
          break;
        }
        if(ipt == 0) break;
      } // ii
      if(killPts > 0) tj.AlgMod[kGottaKink] = true;
      return;
    } // tj.NTPSFit < 4

    
    if(tj.EndPt[1] < 10) return;
    
    unsigned short kinkPt = USHRT_MAX;
    unsigned short ii, ipt, cnt, nHiMultPt;
    
    // Find the kinkPt which is the third Pt from the end that has charge
    cnt = 0;
    nHiMultPt = 0;
    for(ii = 1; ii < lastPt; ++ii) {
      ipt = lastPt - ii;
      if(tj.Pts[ipt].Chg == 0) continue;
      ++cnt;
      if(tj.Pts[ipt].Hits.size() > 1) ++nHiMultPt;
      if(cnt == 3) {
        kinkPt = ipt;
        break;
      }
      if(ipt == 0) break;
    } // ii
    if(kinkPt == USHRT_MAX) return;

    TrajPoint tpFit;
    unsigned short npts = 4;
    unsigned short fitDir = -1;
    FitTraj(tj, lastPt, npts, fitDir, tpFit);
    if(tpFit.FitChi > 900) return;
 
    float dang = DeltaAngle(tj.Pts[kinkPt].Ang, tpFit.Ang);

    float kinkSig = dang / tpFit.AngErr;
    
    if(dang > fKinkAngCut && kinkSig > 3) {
      // Kink larger than hard user cut
      killPts = 3;
    } else if(dang > 0.8 * fKinkAngCut && kinkSig > 10 && tpFit.FitChi < 2) {
      // Found a very significant smaller angle kink
      killPts = 3;
    }
    
    if(killPts > 0) {
      // This could be two crossing trajectories, in which case calling
      // this is a kink would be wrong. Instead we should kill some points
      // and keep going. The signature of crossing trajectories is a multiplicity > 1
      // near the purported kink point and/or increased charge and a long trajectory
      if(tj.Pts.size() < 100 && nHiMultPt == 0) tj.AlgMod[kGottaKink] = true;
      // This could be a stopping track which can wander a bit right at the end. Check for increasing
      // charge from beginning to end
      if(dang < 1.5 * fKinkAngCut && nHiMultPt == 0) {
        float chgrat = 1;
        unsigned short firstPt = tj.EndPt[0];
        if(tj.Pts[firstPt].AveChg > 0) chgrat = tj.Pts[lastPt].AveChg / tj.Pts[firstPt].AveChg;
        if(prt) mf::LogVerbatim("TC")<<" Stopping track? end/begin chgrat "<<chgrat;
        if(chgrat > 1.1) {
          // check for no signal at the next step???
          killPts = 0;
          tj.AlgMod[kGottaKink] = false;
        } // chgrat > 1.1
      } // nHiMultPt == 0
    }
    if(prt) mf::LogVerbatim("TC")<<"GottaKink kinkPt "<<kinkPt<<" Pos "<<PrintPos(tjs, tj.Pts[kinkPt])<<" dang "<<dang<<" cut "<<fKinkAngCut<<" kinkSig "<<kinkSig<<" tpFit chi "<<tpFit.FitChi<<" nHiMultPt "<<nHiMultPt<<" killPts "<<killPts<<" GottaKink? "<<tj.AlgMod[kGottaKink];
    
  } // GottaKink

  //////////////////////////////////////////
  void TrajClusterAlg::UpdateTraj(Trajectory& tj)
  {
    // Updates the last added trajectory point fit, average hit rms, etc.

    fUpdateTrajOK = false;
    fMaskedLastTP = false;
    
    if(tj.EndPt[1] < 1) return;
    unsigned int lastPt = tj.EndPt[1];
    TrajPoint& lastTP = tj.Pts[lastPt];

    // find the previous TP that has hits (and was therefore in the fit)
    unsigned short ii, prevPtWithHits = USHRT_MAX;
    unsigned short firstFitPt = tj.EndPt[0];
    unsigned short ipt, ndead;
    for(ii = 1; ii < tj.Pts.size(); ++ii) {
      ipt = lastPt - ii;
      if(tj.Pts[ipt].Chg > 0) {
        prevPtWithHits = ipt;
        break;
      }
      if(ipt == 0) break;
    } // ii
    if(prevPtWithHits == USHRT_MAX) return;
    
    // define the FitChi threshold above which something will be done
    float maxChi = 2;
    unsigned short minPtsFit = fMinPtsFit[tj.Pass];
    // just starting out?
    if(lastPt < 6) minPtsFit = 2;
    if(tj.PDGCode == 13) {
      // Fitting a muon
      maxChi = fMaxChi;
      minPtsFit = lastPt / 3;
    }

    if(prt) mf::LogVerbatim("TC")<<"UpdateTraj: lastPt "<<lastPt<<" previous point with hits "<<prevPtWithHits<<" tj.Pts size "<<tj.Pts.size()<<" AngleRange "<<AngleRange(lastTP)<<" PDGCode "<<tj.PDGCode<<" maxChi "<<maxChi<<" minPtsFit "<<minPtsFit;
    
    // Set the lastPT delta before doing the fit
    lastTP.Delta = PointTrajDOCA(tjs, lastTP.HitPos[0], lastTP.HitPos[1], lastTP);
    
    UpdateAveChg(tj);

    if(lastPt == 1) {
      // Handle the second trajectory point. No error calculation. Just update
      // the position and direction
      lastTP.NTPsFit = 2;
      FitTraj(tj);
      lastTP.FitChi = 0.01;
      lastTP.AngErr = tj.Pts[0].AngErr;
      if(prt) mf::LogVerbatim("TC")<<"UpdateTraj: Second traj point pos "<<lastTP.Pos[0]<<" "<<lastTP.Pos[1]<<"  dir "<<lastTP.Dir[0]<<" "<<lastTP.Dir[1];
      fUpdateTrajOK = true;
      return;
    }
    
    if(lastPt == 2) {
      // Third trajectory point. Keep it simple
      lastTP.NTPsFit = 3;
      FitTraj(tj);
      fUpdateTrajOK = true;
      // Define the 3 point end trajectory. This copies the direction and angle
      tj.EndTP[0] = lastTP;
      // clobber the hits because we don't need them
      tj.EndTP[0].Hits.clear(); tj.EndTP[0].UseHit.clear();
      // set the position to be the hit position of the first TP
      tj.EndTP[0].Pos = tj.Pts[firstFitPt].HitPos;
      if(prt) mf::LogVerbatim("TC")<<"UpdateTraj: Third traj point fit "<<lastTP.FitChi;
      return;
    }
    
    // Fit with > 2 TPs
    // Keep adding hits until Chi/DOF exceeds 1
    if(tj.Pts[prevPtWithHits].FitChi < 1) lastTP.NTPsFit += 1;
    // Reduce the number of points fit if the trajectory is long and chisq is getting a bit larger
    if(lastPt > 20 && tj.Pts[prevPtWithHits].FitChi > 1.5 && lastTP.NTPsFit > minPtsFit) lastTP.NTPsFit -= 2;
    // Reduce the number of fitted points for long muons by 5% if chisq starts to get a bit large
    // but keep it at least 50% of the total length
//    if(tj.PDGCode == 13 && lastTP.NTPsFit > minPtsFit && tj.Pts[prevPtWithHits].FitChi > 1) lastTP.NTPsFit *= 0.95;
    
    FitTraj(tj);
    
    // don't get too fancy when we are starting out
    if(lastPt < 6 && lastTP.FitChi < 2) {
      fUpdateTrajOK = true;
      return;
    }
    
    fMaskedLastTP = false;
    // find the first point that was fit.
    unsigned short cnt = 0;
    for(ii = 0; ii < tj.Pts.size(); ++ii) {
      ipt = lastPt - ii;
      if(tj.Pts[ipt].Chg > 0) {
        firstFitPt = ipt;
        ++cnt;
      }
      if(cnt == lastTP.NTPsFit) break;
      if(ipt == 0) break;
    }
    
    if(lastTP.FitChi > maxChi && tj.Pts.size() > 6) {
      // A large chisq jump can occur if we just jumped a large block of dead wires. In
      // this case we don't want to mask off the last TP but reduce the number of fitted points
      // This count will be off if there a lot of dead or missing wires...
      ndead = 0;
      if(lastTP.FitChi > 3 && firstFitPt < lastPt) ndead = DeadWireCount(lastTP.HitPos[0], tj.Pts[firstFitPt].HitPos[0], fCTP);
      // reduce the number of points significantly
      if(ndead > 5) {
        if(lastTP.NTPsFit > 5) lastTP.NTPsFit = 5;
      } else {
        // Have a longish trajectory and chisq was a bit large.
        // Was this a sudden occurrence and the fraction of TPs are included
        // in the fit? If so, we should mask off this
        // TP and keep going. If these conditions aren't met, we
        // should reduce the number of fitted points
        float chirat = 0;
        if(prevPtWithHits != USHRT_MAX) chirat = lastTP.FitChi / tj.Pts[prevPtWithHits].FitChi;
        fMaskedLastTP = (chirat > 1.5 && lastTP.NTPsFit > 0.3 * NumPtsWithCharge(tj, false));
        if(prt) {
          mf::LogVerbatim("TC")<<" First fit chisq too large "<<lastTP.FitChi<<" prevPtWithHits chisq "<<tj.Pts[prevPtWithHits].FitChi<<" chirat "<<chirat<<" NumPtsWithCharge "<<NumPtsWithCharge(tj, false)<<" fMaskedLastTP "<<fMaskedLastTP;
        }
        // we should also mask off the last TP if there aren't enough hits
        // to satisfy the minPtsFit constraint
        if(!fMaskedLastTP && NumPtsWithCharge(tj, true) < minPtsFit) fMaskedLastTP = true;
      } // few dead wires
    } // lastTP.FitChi > 2 ...
    
    // Deal with a really long trajectory that is in trouble (uB cosmic).
    if(tj.PDGCode == 13 && lastTP.FitChi > fMaxChi) {
      if(lastTP.NTPsFit > 1.3 * fMuonTag[0]) {
        lastTP.NTPsFit *= 0.8;
        if(prt) mf::LogVerbatim("TC")<<" Muon - Reduce NTPsFit "<<lastPt;
      } else {
        fMaskedLastTP = true;
        if(prt) mf::LogVerbatim("TC")<<" Muon - mask last point "<<lastPt;
      }
    }
    
    if(prt) mf::LogVerbatim("TC")<<"UpdateTraj: First fit "<<lastTP.Pos[0]<<" "<<lastTP.Pos[1]<<"  dir "<<lastTP.Dir[0]<<" "<<lastTP.Dir[1]<<" FitChi "<<lastTP.FitChi<<" NTPsFit "<<lastTP.NTPsFit<<" fMaskedLastTP "<<fMaskedLastTP;
    if(fMaskedLastTP) {
      UnsetUsedHits(lastTP);
      DefineHitPos(lastTP);
      SetEndPoints(tjs, tj);
      lastPt = tj.EndPt[1];
      lastTP.NTPsFit -= 1;
      FitTraj(tj);
      fUpdateTrajOK = true;
      return;
    }  else {
      // a more gradual increase in chisq. Reduce the number of points
      unsigned short newNTPSFit = lastTP.NTPsFit;
      // reduce the number of points fit to keep Chisq/DOF < 2 adhering to the pass constraint
      // and also a minimum number of points fit requirement for long muons
      while(lastTP.FitChi > 1.5 && lastTP.NTPsFit > minPtsFit) {
        if(lastTP.NTPsFit > 15) {
          newNTPSFit = 0.7 * newNTPSFit;
        } else if(lastTP.NTPsFit > 4) {
          newNTPSFit -= 2;
        } else {
          newNTPSFit -= 1;
        }
        if(lastTP.NTPsFit < 3) newNTPSFit = 2;
        if(newNTPSFit < minPtsFit) newNTPSFit = minPtsFit;
        lastTP.NTPsFit = newNTPSFit;
        if(prt) mf::LogVerbatim("TC")<<"  Bad FitChi "<<lastTP.FitChi<<" Reduced NTPsFit to "<<lastTP.NTPsFit<<" Pass "<<tj.Pass;
        FitTraj(tj);
        if(lastTP.NTPsFit == minPtsFit) break;
      } // lastTP.FitChi > 2 && lastTP.NTPsFit > 2
    }
    // last ditch attempt. Drop the last hit
    if(tj.Pts.size() > fMinPtsFit[tj.Pass] && lastTP.FitChi > maxChi) {
      if(prt) mf::LogVerbatim("TC")<<"  Last try. Drop last TP "<<lastTP.FitChi<<" NTPsFit "<<lastTP.NTPsFit;
      UnsetUsedHits(lastTP);
      DefineHitPos(lastTP);
      SetEndPoints(tjs, tj);
      lastPt = tj.EndPt[1];
      FitTraj(tj);
      fUpdateTrajOK = true;
      fMaskedLastTP = true;
    }
    if(prt) mf::LogVerbatim("TC")<<"  Fit done. Chi "<<lastTP.FitChi<<" NTPsFit "<<lastTP.NTPsFit;

    if(tj.EndPt[0] == tj.EndPt[1]) {
      fUpdateTrajOK = false;
      return;
    }
    
    // Don't let the angle error get too small too soon. Stepping would stop if the first
    // few hits on a low momentum wandering track happen to have a very good fit to a straight line.
    // We will do this by averaging the default starting value of AngErr of the first TP with the current
    // value from FitTraj.
    if(lastPt < 14) {
      float defFrac = 1 / (float)(tj.EndPt[1]);
      lastTP.AngErr = defFrac * tj.Pts[0].AngErr + (1 - defFrac) * lastTP.AngErr;
    }

    UpdateDeltaRMS(tj);

    fUpdateTrajOK = true;
    return;

  } // UpdateWork

  //////////////////////////////////////////
  void TrajClusterAlg::UpdateDeltaRMS(Trajectory& tj)
  {
    // Estimate the Delta RMS of the TPs on the end of tj.
    
    unsigned int lastPt = tj.EndPt[1];
    TrajPoint& lastTP = tj.Pts[lastPt];
    
    if(lastTP.Chg == 0) return;
    if(lastPt < 6) return;

    unsigned short ii, ipt, cnt = 0;
    float sum = 0;
    for(ii = 1; ii < tj.Pts.size(); ++ii) {
      ipt = lastPt - ii;
      if(ipt > tj.Pts.size() - 1) break;
      if(tj.Pts[ipt].Chg == 0) continue;
      sum += PointTrajDOCA(tjs, tj.Pts[ipt].Pos[0], tj.Pts[ipt].Pos[1], lastTP);
      ++cnt;
      if(cnt == lastTP.NTPsFit) break;
      if(ipt == 0) break;
    }
    if(cnt < 3) return;
    // RMS of Gaussian distribution is ~1.2 x the average
    // of a one-sided Gaussian distribution (since Delta is > 0)
    lastTP.DeltaRMS = 1.2 * sum / (float)cnt;
    if(lastTP.DeltaRMS < 0.02) lastTP.DeltaRMS = 0.02;

  } // UpdateDeltaRMS
  
  //////////////////////////////////////////
  void TrajClusterAlg::FitWork()
  {
    // Jacket around FitTraj to fit the leading edge of the supplied trajectory
    unsigned short originPt = work.Pts.size() - 1;
    unsigned short npts = work.Pts[originPt].NTPsFit;
    TrajPoint tpFit;
    unsigned short fitDir = -1;
    FitTraj(work, originPt, npts, fitDir, tpFit);
    work.Pts[originPt] = tpFit;
    
  } // FitWork
  
  //////////////////////////////////////////
  void TrajClusterAlg::FitTraj(Trajectory& tj)
  {
    // Jacket around FitTraj to fit the leading edge of the supplied trajectory
//    unsigned short originPt = tj.Pts.size() - 1;
    unsigned short originPt = tj.EndPt[1];
    unsigned short npts = tj.Pts[originPt].NTPsFit;
    TrajPoint tpFit;
    unsigned short fitDir = -1;
    FitTraj(tj, originPt, npts, fitDir, tpFit);
    tj.Pts[originPt] = tpFit;
    
  } // FitTraj

  //////////////////////////////////////////
  void TrajClusterAlg::FitTraj(Trajectory& tj, unsigned short originPt, unsigned short npts, short fitDir, TrajPoint& tpFit)
  {
    // Fit the supplied trajectory using HitPos positions with the origin at originPt.
    // The npts is interpreted as the number of points on each side of the origin
    // The allowed modes are as follows, where i denotes a TP that is included, . denotes
    // a TP with no hits, and x denotes a TP that is not included
    //TP 012345678  fitDir  originPt npts
    //   Oiiixxxxx   1        0       4 << npts in the fit
    //   xi.iiOxxx  -1        5       4
    //   xiiiOiiix   0        4       4 << 2 * npts + 1 points in the fit
    //   xxxiO.ixx   0        4       1
    //   0iiixxxxx   0        0       4
   // This routine puts the results into tp if the fit is successfull. The
    // fit "direction" is in increasing order along the trajectory from 0 to tj.Pts.size() - 1.
    
//    static const float twoPi = 2 * M_PI;
    
    if(originPt > tj.Pts.size() - 1) {
      mf::LogWarning("TC")<<"FitTraj: Requesting fit of invalid TP "<<originPt;
      return;
    }
    
    // copy the origin TP into the fit TP
    tpFit = tj.Pts[originPt];
    // Assume that the fit will fail
    tpFit.FitChi = 999;
    if(fitDir < -1 || fitDir > 1) return;

    std::vector<double> x, y, w, q;
    std::array<float, 2> dir, origin = tj.Pts[originPt].HitPos;
    // Use TP position if there aren't any hits on it
    if(tj.Pts[originPt].Chg == 0) origin = tj.Pts[originPt].Pos;
    double xx, yy, xr, yr;
    double chgWt;

    // Rotate the traj hit position into the coordinate system defined by the
    // originPt traj point, where x = along the trajectory, y = transverse
    double rotAngle = tj.Pts[originPt].Ang;
    double cs = cos(-rotAngle);
    double sn = sin(-rotAngle);

    // enter the originPT hit info if it exists
    if(tj.Pts[originPt].Chg > 0) {
      xx = tj.Pts[originPt].HitPos[0] - origin[0];
      yy = tj.Pts[originPt].HitPos[1] - origin[1];
//      if(prt) std::cout<<" originPT "<<originPt<<" xx "<<xx<<" "<<yy<<" chg "<<tj.Pts[originPt].Chg<<"\n";
      xr = cs * xx - sn * yy;
      yr = sn * xx + cs * yy;
      x.push_back(xr);
      y.push_back(yr);
      chgWt = tj.Pts[originPt].ChgPull;
      if(chgWt < 1) chgWt = 1;
      chgWt *= chgWt;
      w.push_back(chgWt * tj.Pts[originPt].HitPosErr2);
    }
    
    // correct npts to account for the origin point
    if(fitDir != 0) --npts;
    
    // step in the + direction first
    if(fitDir != -1) {
      unsigned short cnt = 0;
      for(unsigned short ipt = originPt + 1; ipt < tj.Pts.size(); ++ipt) {
        if(tj.Pts[ipt].Chg == 0) continue;
        xx = tj.Pts[ipt].HitPos[0] - origin[0];
        yy = tj.Pts[ipt].HitPos[1] - origin[1];
//        if(prt) std::cout<<"ipt "<<ipt<<" xx "<<xx<<" yy "<<yy<<" chg "<<tj.Pts[ipt].Chg<<"\n";
        xr = cs * xx - sn * yy;
        yr = sn * xx + cs * yy;
        x.push_back(xr);
        y.push_back(yr);
        chgWt = tj.Pts[ipt].ChgPull;
        if(chgWt < 1) chgWt = 1;
        chgWt *= chgWt;
        w.push_back(chgWt * tj.Pts[ipt].HitPosErr2);
        ++cnt;
        if(cnt == npts) break;
      } // ipt
    } // fitDir != -1
    
    // step in the - direction next
    if(fitDir != 1 && originPt > 0) {
      unsigned short cnt = 0;
      for(unsigned short ii = 1; ii < tj.Pts.size(); ++ii) {
        unsigned short ipt = originPt - ii;
        if(ipt > tj.Pts.size() - 1) continue;
        if(tj.Pts[ipt].Chg == 0) continue;
        xx = tj.Pts[ipt].HitPos[0] - origin[0];
        yy = tj.Pts[ipt].HitPos[1] - origin[1];
//        if(prt) std::cout<<"ipt "<<ipt<<" xx "<<xx<<" "<<yy<<" chg "<<tj.Pts[ipt].Chg<<"\n";
        xr = cs * xx - sn * yy;
        yr = sn * xx + cs * yy;
        x.push_back(xr);
        y.push_back(yr);
        chgWt = tj.Pts[ipt].ChgPull;
        if(chgWt < 1) chgWt = 1;
        chgWt *= chgWt;
        w.push_back(chgWt * tj.Pts[ipt].HitPosErr2);
        ++cnt;
        if(cnt == npts) break;
        if(ipt == 0) break;
      } // ipt
    } // fitDir != -1
    
    // Not enough points to define a line?
    if(x.size() < 2) return;
    
//    if(prt) std::cout<<"FitTraj: npts "<<npts<<" origin "<<origin[0]<<" "<<origin[1]<<" ticks "<<origin[1]/tjs.UnitsPerTick<<" rotAngle "<<rotAngle<<"\n";
    
    double sum = 0.;
    double sumx = 0.;
    double sumy = 0.;
    double sumxy = 0.;
    double sumx2 = 0.;
    double sumy2 = 0.;

    // weight by the charge ratio and accumulate sums
    double wght;
    for(unsigned short ipt = 0; ipt < x.size(); ++ipt) {
      if(w[ipt] < 0.00001) w[ipt] = 0.00001;
      wght = 1 / w[ipt];
      sum   += wght;
      sumx  += wght * x[ipt];
      sumy  += wght * y[ipt];
      sumx2 += wght * x[ipt] * x[ipt];
      sumy2 += wght * y[ipt] * y[ipt];
      sumxy += wght * x[ipt] * y[ipt];
    }
    // calculate coefficients and std dev
    double delta = sum * sumx2 - sumx * sumx;
    if(delta == 0) return;
    // A is the intercept
    double A = (sumx2 * sumy - sumx * sumxy) / delta;
    // B is the slope
    double B = (sumxy * sum  - sumx * sumy) / delta;
    
    // The chisq will be set below if there are enough points. Don't allow it to be 0
    // so we can take Chisq ratios later
    tpFit.FitChi = 0.01;
    double newang = atan(B);
    dir[0] = cos(newang);
    dir[1] = sin(newang);
    // rotate back into the (w,t) coordinate system
    cs = cos(rotAngle);
    sn = sin(rotAngle);
    tpFit.Dir[0] = cs * dir[0] - sn * dir[1];
    tpFit.Dir[1] = sn * dir[0] + cs * dir[1];
    // ensure that the direction is consistent with the originPt direction
    bool flipDir = false;
    if(AngleRange(tj.Pts[originPt]) > 0) {
      flipDir = std::signbit(tpFit.Dir[1]) != std::signbit(tj.Pts[originPt].Dir[1]);
    } else {
      flipDir = std::signbit(tpFit.Dir[0]) != std::signbit(tj.Pts[originPt].Dir[0]);
    }
    if(flipDir) {
      tpFit.Dir[0] = -tpFit.Dir[0];
      tpFit.Dir[1] = -tpFit.Dir[1];
    }
    tpFit.Ang = atan2(tpFit.Dir[1], tpFit.Dir[0]);
//    if(prt) mf::LogVerbatim("TC")<<"FitTraj "<<originPt<<" originPt Dir "<<tj.Pts[originPt].Dir[0]<<" "<<tj.Pts[originPt].Dir[1]<<" rotAngle "<<rotAngle<<" tpFit.Dir "<<tpFit.Dir[0]<<" "<<tpFit.Dir[1]<<" Ang "<<tpFit.Ang<<" flipDir "<<flipDir<<" fit vector size "<<x.size();

    // rotate (0, intcpt) into (W,T) coordinates
    tpFit.Pos[0] = -sn * A + origin[0];
    tpFit.Pos[1] =  cs * A + origin[1];
    // force the origin to be at origin[0]
    MoveTPToWire(tpFit, origin[0]);
    
    if(x.size() < 3) return;
    
    // Calculate chisq/DOF
    double ndof = x.size() - 2;
    double varnce = (sumy2 + A*A*sum + B*B*sumx2 - 2 * (A*sumy + B*sumxy - A*B*sumx)) / ndof;
    if(varnce > 0.) {
      // Intercept error is not used
//      InterceptError = sqrt(varnce * sumx2 / delta);
      double slopeError = sqrt(varnce * sum / delta);
      tpFit.AngErr = std::abs(atan(slopeError));
    } else {
      tpFit.AngErr = 0.01;
    }
    sum = 0;
    // calculate chisq
    double arg;
    for(unsigned short ii = 0; ii < y.size(); ++ii) {
      arg = y[ii] - A - B * x[ii];
      sum += arg * arg / w[ii];
    }
    tpFit.FitChi = sum / ndof;
  
  } // FitTraj
  
  //////////////////////////////////////////
  void TrajClusterAlg::UpdateAveChg(Trajectory& tj)
  {
    if(tj.EndPt[1] == 0) return;
    unsigned short lastPt = tj.EndPt[1];
    tj.AveChg = 0;
    tj.Pts[lastPt].AveChg = 0;

    // calculate ave charge and charge RMS using ALL hits in the trajectory
    unsigned short ii, ipt, cnt = 0;
    float fcnt, sum = 0;
    float sum2 = 0;
    // Don't include the first point in the average. It will be too
    // low if this is a stopping/starting particle
    for(ii = 0; ii < tj.Pts.size(); ++ii) {
      ipt = tj.EndPt[1] - ii;
      if(ipt == 0) break;
      if(tj.Pts[ipt].Chg == 0) continue;
      ++cnt;
      sum += tj.Pts[ipt].Chg;
      sum2 += tj.Pts[ipt].Chg * tj.Pts[ipt].Chg;
      if(cnt == fNPtsAve) break;
    } // iii
    if(cnt == 0) return;
    fcnt = cnt;
    sum /= fcnt;
    tj.AveChg = sum;
    tj.Pts[lastPt].AveChg = sum;
    // define the first point average charge if necessary
    if(tj.Pts[tj.EndPt[0]].AveChg <= 0) tj.Pts[tj.EndPt[0]].AveChg = sum;
    if(cnt > 3) {
      float arg = sum2 - fcnt * sum * sum;
      if(arg < 0) arg = 0;
      float rms = sqrt(arg / (fcnt - 1));
      // convert this to a normalized RMS
      rms /= sum;
      // don't let the calculated charge RMS dominate the default
      // RMS until it is well known. Start with 100% error on the
      // charge RMS
      float defFrac = 1 / (float)(tj.EndPt[1]);
      tj.ChgRMS = defFrac + (1 - defFrac) * rms;
      // don't let it get crazy small
      if(tj.ChgRMS < 0.15) tj.ChgRMS = 0.15;
      tj.Pts[lastPt].ChgPull = (tj.Pts[lastPt].Chg / tj.AveChg - 1) / tj.ChgRMS;
    } // cnt > 3

  } // UpdateAveChg

  ////////////////////////////////////////////////
  void TrajClusterAlg::StartWork(unsigned int fromHit, unsigned int toHit)
  {
    float fromWire = tjs.fHits[fromHit]->WireID().Wire;
    float fromTick = tjs.fHits[fromHit]->PeakTime();
    float toWire = tjs.fHits[toHit]->WireID().Wire;
    float toTick = tjs.fHits[toHit]->PeakTime();
    CTP_t tCTP = EncodeCTP(tjs.fHits[fromHit]->WireID());
    StartWork(fromWire, fromTick, toWire, toTick, tCTP);
  } // StartWork

  ////////////////////////////////////////////////
  void TrajClusterAlg::StartWork(float fromWire, float fromTick, float toWire, float toTick, CTP_t tCTP)
  {
    // Start a simple (seed) trajectory going from a hit to a position (toWire, toTick).
    
    // construct a default trajectory
    Trajectory tj;
    // and use it to blow out work
    work = tj;
    // decrement the work ID so we can use it for debugging problems
    --fWorkID;
    if(fWorkID == SHRT_MIN) fWorkID = -1;
    work.ID = fWorkID;
    work.Pass = fPass;
    work.StepDir = fStepDir;
    work.CTP = tCTP;
    
    // create a trajectory point
    TrajPoint tp;
    MakeBareTrajPoint(tjs, fromWire, fromTick, toWire, toTick, tCTP, tp);

    tp.AngErr = 0.1;
    if(work.ID == debug.WorkID) { prt = true; didPrt = true; debug.Plane = fPlane; TJPrt = work.ID; }
    if(prt) mf::LogVerbatim("TC")<<"StartWork "<<(int)fromWire<<":"<<(int)fromTick<<" -> "<<(int)toWire<<":"<<(int)toTick<<" dir "<<tp.Dir[0]<<" "<<tp.Dir[1]<<" ang "<<tp.Ang<<" AngleRange "<<AngleRange(tp)<<" angErr "<<tp.AngErr;
    work.Pts.push_back(tp);
    
  } // StartWork
  
  //////////////////////////////////////////
  void TrajClusterAlg::ReverseTraj(Trajectory& tj)
  {
    // reverse the trajectory
    if(tj.Pts.empty()) return;
    // reverse the crawling direction flag
    tj.StepDir = -tj.StepDir;
    // Vertices
    short tmp = tj.VtxID[0];
    tj.VtxID[0] = tj.VtxID[1];
    tj.VtxID[0] = tmp;
    // trajectory points
    std::reverse(tj.Pts.begin(), tj.Pts.end());
    // reverse the direction vector on all points
    for(unsigned short ipt = 0; ipt < tj.Pts.size(); ++ipt) {
      if(tj.Pts[ipt].Dir[0] != 0) tj.Pts[ipt].Dir[0] = -tj.Pts[ipt].Dir[0];
      if(tj.Pts[ipt].Dir[1] != 0) tj.Pts[ipt].Dir[1] = -tj.Pts[ipt].Dir[1];
      tj.Pts[ipt].Ang = atan2(tj.Pts[ipt].Dir[1], tj.Pts[ipt].Dir[0]);
      // and the order of hits if more than 1
      if(tj.Pts[ipt].Hits.size() > 1) {
        std::reverse(tj.Pts[ipt].Hits.begin(), tj.Pts[ipt].Hits.end());
        std::reverse(tj.Pts[ipt].UseHit.begin(), tj.Pts[ipt].UseHit.end());
      }
    } // ipt
    // the end TPs
    std::reverse(tj.EndTP.begin(), tj.EndTP.end());
    SetEndPoints(tjs, tj);
  }
  
  ////////////////////////////////////////////////
  void TrajClusterAlg::ChkInTraj(std::string someText)
  {
    // Check tjs.allTraj -> tjs.inTraj associations
    
    if(!fUseAlg[kChkInTraj]) return;
    
    unsigned short tID;
    unsigned int iht;
    unsigned short itj = 0;
    std::vector<unsigned int> tHits;
    std::vector<unsigned int> atHits;
    for(auto& tj : tjs.allTraj) {
      // ignore abandoned trajectories
      if(tj.AlgMod[kKilled]) continue;
      tID = tj.ID;
      for(auto& tp : tj.Pts) {
        if(tp.Hits.size() != tp.UseHit.size()) {
          tj.AlgMod[kKilled] = true;
          mf::LogWarning("TC")<<"ChkInTraj Hits - UseHit mismatched\n";
          fQuitAlg = true;
          return;
        }
      } // tp
      if(tj.AlgMod[kKilled]) {
        std::cout<<someText<<" ChkInTraj hit size mis-match in tj ID "<<tj.ID<<" AlgBitNames";
        for(unsigned short ib = 0; ib < AlgBitNames.size(); ++ib) if(tj.AlgMod[ib]) std::cout<<" "<<AlgBitNames[ib];
        std::cout<<"\n";
        continue;
      }
      PutTrajHitsInVector(tj, true,  tHits);
      if(tHits.size() < 2) {
        mf::LogVerbatim("TC")<<someText<<" ChkInTraj: Insufficient hits in traj "<<tj.ID<<" Killing it";
        tj.AlgMod[kKilled] = true;
        continue;
      }
      std::sort(tHits.begin(), tHits.end());
      atHits.clear();
      for(iht = 0; iht < tjs.inTraj.size(); ++iht) {
        if(tjs.inTraj[iht] == tID) atHits.push_back(iht);
      } // iht
      if(atHits.size() < 2) {
        mf::LogVerbatim("TC")<<someText<<" ChkInTraj: Insufficient hits in atHits in traj "<<tj.ID<<" Killing it";
        tj.AlgMod[kKilled] = true;
        continue;
      }
      if(!std::equal(tHits.begin(), tHits.end(), atHits.begin())) {
        mf::LogVerbatim myprt("TC");
        myprt<<someText<<" ChkInTraj: inTraj - UseHit mis-match for tj ID "<<tID<<" tj.WorkID "<<tj.WorkID<<" atHits size "<<atHits.size()<<" tHits size "<<tHits.size()<<" in CTP "<<tj.CTP<<"\n";
        myprt<<"AlgMods: ";
        for(unsigned short ib = 0; ib < AlgBitNames.size(); ++ib) if(tj.AlgMod[ib]) myprt<<" "<<AlgBitNames[ib];
        myprt<<"\n";
        myprt<<"     inTraj     UseHit \n";
        for(iht = 0; iht < atHits.size(); ++iht) {
          myprt<<"iht "<<iht<<" "<<PrintHit(tjs.fHits[atHits[iht]])<<"_"<<tjs.inTraj[atHits[iht]];
          if(iht < tHits.size()) myprt<<" "<<PrintHit(tjs.fHits[tHits[iht]])<<"_"<<tjs.inTraj[tHits[iht]];
          if(atHits[iht] != tHits[iht]) myprt<<" <<< ";
          myprt<<"\n";
          fQuitAlg = true;
        } // iht
        if(tHits.size() > atHits.size()) {
          for(iht = atHits.size(); iht < atHits.size(); ++iht) {
            myprt<<"atHits "<<iht<<" "<<PrintHit(tjs.fHits[atHits[iht]])<<"\n";
          } // iht
//          PrintAllTraj(tjs, debug, USHRT_MAX, 0);
        } // tHit.size > atHits.size()
      }
      ++itj;
      if(fQuitAlg) return;
    } // tj
    
  } // ChkInTraj

  ////////////////////////////////////////////////
  void TrajClusterAlg::StoreWork()
  {

    if(work.EndPt[1] <= work.EndPt[0]) return;
    if(work.AlgMod[kKilled]) {
      mf::LogWarning("TC")<<"StoreWork: Trying to store a killed trajectory. Work ID "<<work.ID;
      return;
    }
    
    // Fit the last 3 points and stuff it into EndTP[1]
    unsigned short originPt = work.Pts.size() - 1;
    unsigned short npts = 3;
    unsigned short fitDir = -1;
    FitTraj(work, originPt, npts, fitDir, work.EndTP[1]);
    work.EndTP[1].Pos = work.Pts[originPt].HitPos;

    // put trajectories in order of US -> DS
    if(work.StepDir < 0) ReverseTraj(work);
    // This shouldn't be necessary but do it anyway
    SetEndPoints(tjs, work);
    
    // Calculate the charge near the end and beginning if necessary. This must be a short
    // trajectory. Find the average using 4 points
    if(work.Pts[work.EndPt[0]].AveChg <= 0) {
//      std::cout<<"StoreWork AveChg at beginning is 0 on Traj ID "<<work.ID<<"\n";
      unsigned short cnt = 0;
      float sum = 0;
      for(unsigned short ipt = work.EndPt[0] + 1; ipt <= work.EndPt[1]; ++ipt) {
        if(work.Pts[ipt].Chg == 0) continue;
        sum += work.Pts[ipt].Chg;
        ++cnt;
        if(cnt == 4) break;
       }
      work.Pts[work.EndPt[0]].AveChg = sum / (float)cnt;
    }
    if(work.Pts[work.EndPt[1]].AveChg <= 0) {
//      std::cout<<"StoreWork AveChg at end is 0 on Traj ID "<<work.ID<<"\n";
      float sum = 0;
      unsigned short cnt = 0;
      for(unsigned short ii = 1; ii < work.Pts.size(); ++ii) {
        unsigned short ipt = work.EndPt[1] - ii;
        if(work.Pts[ipt].Chg == 0) continue;
        sum += work.Pts[ipt].Chg;
        ++cnt;
        if(cnt == 4) break;
        if(ipt == 0) break;
      } // ii
      work.Pts[work.EndPt[1]].AveChg = sum / (float)cnt;
    } // begin charge == end charge
    work.EndTP[0].AveChg = work.Pts[work.EndPt[0]].AveChg;
    work.EndTP[1].AveChg = work.Pts[work.EndPt[1]].AveChg;
    
    CalculateQuality(work);
    
    short trID = tjs.allTraj.size() + 1;
    for(unsigned short ipt = work.EndPt[0]; ipt < work.EndPt[1] + 1; ++ipt) {
      for(unsigned short ii = 0; ii < work.Pts[ipt].Hits.size(); ++ii) {
        if(work.Pts[ipt].UseHit[ii]) {
          unsigned int iht = work.Pts[ipt].Hits[ii];
          if(tjs.inTraj[iht] > 0) {
            mf::LogWarning("TC")<<"StoreWork: Failed trying to store hit "<<PrintHit(tjs.fHits[iht])<<" in new tjs.allTraj "<<trID<<" but it is used in traj ID = "<<tjs.inTraj[iht]<<" print work and quit";
            PrintTrajectory("SW", tjs, work, USHRT_MAX);
            ReleaseWorkHits();
            fQuitAlg = true;
            return;
          } // error
          tjs.inTraj[iht] = trID;
        }
      } // ii
    } // ipt
    
    // ensure that inTraj is clean for the work ID
    for(unsigned int iht = 0; iht < tjs.fHits.size(); ++iht) {
      if(tjs.inTraj[iht] == work.ID) {
        mf::LogWarning("TC")<<"StoreWork: Hit "<<PrintHit(tjs.fHits[iht])<<" thinks it belongs to work ID "<<work.ID<<" but it wasn't stored\n";
        PrintTrajectory("SW", tjs, work, USHRT_MAX);
        fQuitAlg = true;
        return;
      }
    } // iht
    
    work.WorkID = work.ID;
    work.ID = trID;
    tjs.allTraj.push_back(work);
    if(prt) mf::LogVerbatim("TC")<<"StoreWork trID "<<trID<<" CTP "<<work.CTP<<" EndPts "<<work.EndPt[0]<<" "<<work.EndPt[1];
    if(debug.Hit != UINT_MAX) {
      // print out some debug info
      for(unsigned short ipt = 0; ipt < work.Pts.size(); ++ipt) {
        for(unsigned short ii = 0; ii < work.Pts[ipt].Hits.size(); ++ii) {
          unsigned int iht = work.Pts[ipt].Hits[ii];
          if(iht == debug.Hit) std::cout<<"Debug hit appears in trajectory w WorkID "<<work.WorkID<<" UseHit "<<work.Pts[ipt].UseHit[ii]<<"\n";
        } // ii
      } // ipt
    } // debug.Hit ...
    ChkInTraj("StoreWork");
    
  } // StoreWork
  
  ////////////////////////////////////////////////
  void TrajClusterAlg::CalculateQuality(Trajectory& tj)
  {
    // Calculate a quality metric using the deviations of all hits used in the trajectory
    
    tj.MCSMom = MCSMom(tjs, tj);
    
  } // CalculateQuality
  
  ////////////////////////////////////////////////
  void TrajClusterAlg::MakeAllTrajClusters(bool fMakeNewHits)
  {
    // Make clusters from all trajectories in tjs.allTraj
    
    if(fMakeNewHits) MakeNewHits();
    
    ClusterStore cls;
    tjs.tcl.clear();
    tjs.inClus.resize(tjs.fHits.size());
    unsigned int iht;
    for(iht = 0; iht < tjs.inClus.size(); ++iht) tjs.inClus[iht] = 0;
    
    if(prt) mf::LogVerbatim("TC")<<"MakeAllTrajClusters: tjs.allTraj size "<<tjs.allTraj.size();
    
    ChkInTraj("MATC");
    if(fQuitAlg) return;
    
    unsigned short itj, endPt0, endPt1, ii;
    
    std::vector<unsigned int> tHits;
    
    // Make one cluster for each trajectory. The indexing of trajectory parents
    // should map directly to cluster parents
    short clID = 0;
    for(itj = 0; itj < tjs.allTraj.size(); ++itj) {
      Trajectory& tj = tjs.allTraj[itj];
      if(tj.AlgMod[kKilled]) continue;
      if(tj.StepDir > 0) ReverseTraj(tj);
      // ensure that the endPts are correct
      SetEndPoints(tjs, tj);
      // some sort of error occurred
      if(tj.EndPt[0] >= tj.EndPt[1]) {
        mf::LogWarning("TC")<<"MakeAllTrajClusters failed in SetEndPoints "<<tj.EndPt[0]<<" "<<tj.EndPt[1];
        continue;
      }
      // count AlgMod bits
      for(unsigned short ib = 0; ib < AlgBitNames.size(); ++ib) if(tj.AlgMod[ib]) ++fAlgModCount[ib];
      ++clID;
      cls.ID = clID;
      cls.CTP = tj.CTP;
      cls.PDGCode = tj.PDGCode;
      cls.ParentCluster = tj.ParentTrajID - 1;
      endPt0 = tj.EndPt[0];
      cls.BeginWir = tj.Pts[endPt0].Pos[0];
      cls.BeginTim = tj.Pts[endPt0].Pos[1] / tjs.UnitsPerTick;
      cls.BeginAng = tj.Pts[endPt0].Ang;
      cls.BeginChg = tj.Pts[endPt0].Chg;
      cls.BeginVtx = tj.VtxID[0]-1;
      endPt1 = tj.EndPt[1];
      cls.EndWir = tj.Pts[endPt1].Pos[0];
      cls.EndTim = tj.Pts[endPt1].Pos[1] / tjs.UnitsPerTick;
      cls.EndAng = tj.Pts[endPt1].Ang;
      cls.EndChg = tj.Pts[endPt1].Chg;
      cls.EndVtx = tj.VtxID[1]-1;
      PutTrajHitsInVector(tj, true, tHits);
      for(auto& iht : tHits) if(iht > tjs.newHits.size() - 1) std::cout<<"Bad hit\n";
      if(tHits.empty()) {
        mf::LogWarning("TC")<<"MakeAllTrajClusters: No hits found in trajectory "<<itj<<" so skip it";
        continue;
      } // error
      cls.tclhits = tHits;
      // Set the traj info
      tj.ClusterIndex = tjs.tcl.size();
      tjs.tcl.push_back(cls);
      // do some checking and define tjs.inClus
      geo::PlaneID planeID = DecodeCTP(cls.CTP);
      for(ii = 0; ii < cls.tclhits.size(); ++ii) {
        iht = cls.tclhits[ii];
        if(tjs.newHits.empty()) {
          if(tjs.fHits[iht]->WireID().Plane != planeID.Plane ||
             tjs.fHits[iht]->WireID().Cryostat != planeID.Cryostat ||
             tjs.fHits[iht]->WireID().TPC != planeID.TPC) {
            mf::LogWarning("TC")<<"MakeAllTrajClusters: Bad OLD hit CTP in itj "<<itj<<" hit "<<PrintHit(tjs.fHits[iht])<<" WorkID "<<tjs.allTraj[itj].WorkID<<" Plane "<<tjs.fHits[iht]->WireID().Plane<<" vs "<<planeID.Plane<<" Cstat "<<tjs.fHits[iht]->WireID().Cryostat<<" vs "<<planeID.Cryostat<<" TPC "<<tjs.fHits[iht]->WireID().TPC<<" vs "<<planeID.TPC;
            fQuitAlg = true;
            return;
          }
        } else {
          if(tjs.newHits[iht].WireID().Plane != planeID.Plane ||
             tjs.newHits[iht].WireID().Cryostat != planeID.Cryostat ||
             tjs.newHits[iht].WireID().TPC != planeID.TPC) {
            mf::LogWarning("TC")<<"MakeAllTrajClusters: Bad NEW hit in itj "<<itj<<" WorkID "<<tjs.allTraj[itj].WorkID<<" Plane "<<tjs.newHits[iht].WireID().Plane<<" vs "<<planeID.Plane<<" Cstat "<<tjs.newHits[iht].WireID().Cryostat<<" vs "<<planeID.Cryostat<<" TPC "<<tjs.newHits[iht].WireID().TPC<<" vs "<<planeID.TPC;
            std::cout<<"MakeAllTrajClusters: Bad NEW hit in itj "<<itj<<" WorkID "<<tjs.allTraj[itj].WorkID<<" Plane "<<tjs.newHits[iht].WireID().Plane<<" vs "<<planeID.Plane<<" Cstat "<<tjs.newHits[iht].WireID().Cryostat<<" vs "<<planeID.Cryostat<<" TPC "<<tjs.newHits[iht].WireID().TPC<<" vs "<<planeID.TPC<<"\n";
//            PrintAllTraj("MATC", tjs, debug, itj, USHRT_MAX);
            fQuitAlg = true;
            return;
          }
        }
        tjs.inClus[iht] = clID;
      } //iht
    } // itj

  } // MakeAllTrajClusters

  ////////////////////////////////////////////////
  void TrajClusterAlg::MakeNewHits()
  {
    // Make a new hit collection by merging close hits on each TP. The trajectory point -> hit
    // association tj.Pts[].Hits[] is redirected to tjs.newHits instead of tjs.fHits. Currently this
    // routine is called just before making clusters so there should be little confusion whether to use
    // tjs.newHits or tjs.fHits but one can test tjs.newhits.empty() to be sure.

//    PrintAllTraj("MNH1", tjs, debug, 3, USHRT_MAX);
    
    tjs.newHits.reserve(tjs.fHits.size());
    std::vector<bool> flag(tjs.fHits.size(), false);
    // oldnew points from old hit to new hit, or UINT_MAX for obsolete hits
    std::vector<unsigned int> oldnew(tjs.fHits.size(), UINT_MAX);

    for(unsigned int fht = 0; fht < tjs.fHits.size(); ++fht) {
      // already considered
      if(flag[fht]) continue;
      
      if(tjs.inTraj[fht] <= 0) {
        // unused hit. Just copy it
        tjs.newHits.emplace_back(*tjs.fHits[fht]);
        // flag it as considered
        flag[fht] = true;
        oldnew[fht] = tjs.newHits.size() - 1;
        continue;
      }
      
      // Hit is used in a trajectory. Get the trajectory index from the ID
      unsigned short itj = tjs.inTraj[fht] - 1;
      // Determine which trajectory point uses this hit
      Trajectory& tj = tjs.allTraj[itj];
      unsigned short hitInPt = USHRT_MAX;
      for(unsigned short ipt = tj.EndPt[0]; ipt < tj.EndPt[1] + 1; ++ipt) {
        for(unsigned short ii = 0; ii < tj.Pts[ipt].Hits.size(); ++ii) {
          if(!tj.Pts[ipt].UseHit[ii]) continue;
          if(tj.Pts[ipt].Hits[ii] != fht) continue;
          hitInPt = ipt;
          break;
        } // ii
        if(hitInPt != USHRT_MAX) break;
      } // ipt
      // this should not happen but check anyway
      if(hitInPt == USHRT_MAX) {
        mf::LogWarning("TC")<<"Hit "<<tjs.fHits[fht]->WireID().Plane<<":"<<PrintHit(tjs.fHits[fht])<<" fht "<<fht<<" is inTraj "<<tjs.inTraj[fht]<<" but wasn't found in Pts ";
        fQuitAlg = true;
        return;
      }
      // This is simple if there is only one used hit
      if(NumUsedHits(tj.Pts[hitInPt]) == 1) {
        tjs.newHits.emplace_back(*tjs.fHits[fht]);
        oldnew[fht] = tjs.newHits.size() - 1;
        // flag it used
        flag[fht] = true;
      } else {
        // Merge the charge from all used hits in the TP.
        // The merged hit will be put on the wire that is closest to TP.HitPos[0]. The charge
        // from hits on nearby wires will be added to this hit and those hits will be dropped
        unsigned int mergedHitWire = std::nearbyint(tj.Pts[hitInPt].HitPos[0]);
        // find the index of a hit that lies on this wire so we can use the View, Channel, etc
        // to construct the hit. Assume it is the hit we are considering to start
        unsigned int wireInfoHit = fht;
        // Find the tick range that encompasses all the hits
        raw::TDCtick_t loTick = tjs.fHits[fht]->StartTick();
        raw::TDCtick_t hiTick = tjs.fHits[fht]->EndTick();
        unsigned short ipl = tjs.fHits[fht]->WireID().Plane;
        short maxTime = tjs.MaxPos1[ipl] / tjs.UnitsPerTick;
        for(unsigned short ii = 0; ii < tj.Pts[hitInPt].Hits.size(); ++ii) {
          if(!tj.Pts[hitInPt].UseHit[ii]) continue;
          unsigned int iht = tj.Pts[hitInPt].Hits[ii];
          if(tjs.fHits[iht]->WireID().Wire == mergedHitWire) wireInfoHit = iht;
          if(tjs.fHits[iht]->StartTick() < loTick) loTick = tjs.fHits[iht]->StartTick();
          if(tjs.fHits[iht]->EndTick() > hiTick) hiTick = tjs.fHits[iht]->EndTick();
        }
        // make a temporary signal waveform vector to calculate the merged charge, rms, etc
        std::vector<float> signal(hiTick - loTick, 0);
        // Add the Gaussian charge distribution for each used hit
        float chg = 0;
        for(unsigned short ii = 0; ii < tj.Pts[hitInPt].Hits.size(); ++ii) {
          if(!tj.Pts[hitInPt].UseHit[ii]) continue;
          unsigned int iht = tj.Pts[hitInPt].Hits[ii];
          // flag it used
          flag[iht] = true;
          chg += tjs.fHits[iht]->Integral();
          // add charge in the range +/- 3 sigma
          short loTime = tjs.fHits[iht]->PeakTimeMinusRMS(3);
          if(loTime < 0) loTime = 0;
          short hiTime = tjs.fHits[iht]->PeakTimePlusRMS(3);
          if(hiTime > maxTime) hiTime = maxTime;
          float hitPeakAmp = tjs.fHits[iht]->PeakAmplitude();
          float hitPeakTime = tjs.fHits[iht]->PeakTime();
          float hitRMS = tjs.fHits[iht]->RMS();
          for(short time = loTime; time < hiTime; ++time) {
            unsigned short indx = time - loTick;
            if(indx > signal.size() - 1) continue;
            float arg = (time - hitPeakTime) / hitRMS;
            signal[indx] += hitPeakAmp * exp(-0.5 * arg * arg);
          } // time
        } // ii
        // Find the charge weighted time of the summed signal
        float sumt = 0;
        chg = 0;
        for(unsigned short indx = 0; indx < signal.size(); ++indx) {
          chg  += signal[indx];
          sumt += signal[indx] * indx;
        } // indx
        if(chg == 0) {
          std::cout<<"No charge in signal range. This is really bad...\n";
          continue;
        }
        // aveIndx is the index of the charge-weighted average in the signal vector
        float aveIndx = sumt / chg;
        // find the merged hit RMS
        float mergedHitRMS = 0;
        for(unsigned short indx = 0; indx < signal.size(); ++indx) {
          float dindx = indx - aveIndx;
          mergedHitRMS += signal[indx] * dindx * dindx;
        } // indx
        mergedHitRMS = std::sqrt(mergedHitRMS / chg);
        // Calculate the hit charge normalization factor so that we can make a reasonable
        // estimate of the peak amplitude of the merged hits using a Gaussian approximation
        float chgNorm = tjs.fHits[fht]->PeakAmplitude() * tjs.fHits[fht]->RMS() / tjs.fHits[fht]->Integral();
        float mergedHitPeakAmp = chg * chgNorm / mergedHitRMS;
        float mergedHitPeakTick = loTick + aveIndx;
        tjs.newHits.emplace_back(
                                 tjs.fHits[wireInfoHit]->Channel(),
                                 loTick, hiTick,
                                 mergedHitPeakTick, 0,     // peak time & uncertainty
                                 mergedHitRMS,
                                 mergedHitPeakAmp, 0,      // peak amplitude & uncertainty
                                 chg, chg, 0,              // SummedADC, Integralv & uncertainty
                                 1, 0,                     // Multiplicity, LocalIndex
                                 666, 1,                   // GoodnessOfFit, DOF
                                 tjs.fHits[wireInfoHit]->View(),
                                 tjs.fHits[wireInfoHit]->SignalType(),
                                 tjs.fHits[wireInfoHit]->WireID()
        
        );
        // Correct UseHit for this TP
        for(unsigned short ii = 0; ii < tj.Pts[hitInPt].Hits.size(); ++ii) {
          if(tj.Pts[hitInPt].Hits[ii] == wireInfoHit) {
            tj.Pts[hitInPt].UseHit[ii] = true;
            // define oldnew
            oldnew[wireInfoHit] = tjs.newHits.size() - 1;
          } else {
            // oldnew for obsolete hits was set to UINT_MAX
            tj.Pts[hitInPt].UseHit[ii] = false;
          }
        } // ii
        
      } // NumUsedHits > 1
    } // fht
    
    // assign all TP hits to the new hit collection
    for(unsigned short itj = 0; itj < tjs.allTraj.size(); ++itj) {
      if(tjs.allTraj[itj].AlgMod[kKilled]) continue;
      for(unsigned short ipt = 0; ipt < tjs.allTraj[itj].Pts.size(); ++ipt) {
        for(unsigned short ii = 0; ii < tjs.allTraj[itj].Pts[ipt].Hits.size(); ++ii) {
          unsigned int iht = tjs.allTraj[itj].Pts[ipt].Hits[ii];
          unsigned int nht = oldnew[iht];
          if(nht != UINT_MAX) {
            if(tjs.fHits[iht]->WireID().Plane != tjs.newHits[nht].WireID().Plane || tjs.fHits[iht]->WireID().Wire != tjs.newHits[nht].WireID().Wire) {
              unsigned short ipl = tjs.fHits[iht]->WireID().Plane;
              unsigned short npl = tjs.newHits[nht].WireID().Plane;
              std::cout<<"MakeNewHits: fHits "<<ipl<<":"<<PrintHit(tjs.fHits[iht])<<" inconsistent with newHits "<<npl<<":"<<PrintHit(tjs.newHits[nht])<<"\n";
            }
          }
          tjs.allTraj[itj].Pts[ipt].Hits[ii] = oldnew[iht];
          // double check
          if(tjs.allTraj[itj].Pts[ipt].UseHit[ii] && oldnew[iht] == UINT_MAX) {
            std::cout<<"MakeNewHits UseHits points to an invalid hit\n";
          }
        } // ii
      } // ipt
    } // itj
    
    // QC check
    bool itsBad = false;
    for(unsigned int fht = 0; fht < tjs.fHits.size(); ++fht) {
      unsigned int nht = oldnew[fht];
      if(nht == UINT_MAX) continue;
      unsigned short fpl = tjs.fHits[fht]->WireID().Plane;
      unsigned int fwire = tjs.fHits[fht]->WireID().Wire;
      unsigned short npl = tjs.newHits[nht].WireID().Plane;;
      unsigned int nwire = tjs.newHits[nht].WireID().Wire;
      if(fpl != npl || fwire != nwire) {
        std::cout<<"Oops "<<fpl<<":"<<fwire<<" != "<<npl<<":"<<nwire<<"\n";
        itsBad = true;
      }
    } // fht
    if(itsBad) exit(1);
//    PrintAllTraj("MNH2", tjs, debug, 3, USHRT_MAX);
    
  } // MakeNewHits
  
  ////////////////////////////////////////////////
  void TrajClusterAlg::GetHitMultiplet(unsigned int theHit, std::vector<unsigned int>& hitsInMultiplet)
  {
    unsigned short localIndex;
    GetHitMultiplet(theHit, hitsInMultiplet, localIndex);
  } // GetHitMultiplet
  
  ////////////////////////////////////////////////
  void TrajClusterAlg::GetHitMultiplet(unsigned int theHit, std::vector<unsigned int>& hitsInMultiplet, unsigned short& localIndex)
  {
    hitsInMultiplet.clear();
    if(theHit > tjs.fHits.size() - 1) return;

    hitsInMultiplet.resize(1);
    hitsInMultiplet[0] = theHit;
    
    float hitSep;
    unsigned int theWire = tjs.fHits[theHit]->WireID().Wire;
    unsigned short thePlane = tjs.fHits[theHit]->WireID().Plane;
    float theTime = tjs.fHits[theHit]->PeakTime();
    float theRMS = tjs.fHits[theHit]->RMS();
//    if(prt) mf::LogVerbatim("TC")<<"GetHitMultiplet theHit "<<theHit<<" "<<PrintHit(tjs.fHits[theHit])<<" RMS "<<tjs.fHits[theHit]->RMS();
    // look for hits < theTime but within hitSep
    if(theHit > 0) {
      for(unsigned int iht = theHit - 1; iht != 0; --iht) {
        if(tjs.fHits[iht]->WireID().Wire != theWire) break;
        if(tjs.fHits[iht]->WireID().Plane != thePlane) break;
        if(tjs.fHits[iht]->RMS() > theRMS) {
          hitSep = fMultHitSep * tjs.fHits[iht]->RMS();
          theRMS = tjs.fHits[iht]->RMS();
        } else {
          hitSep = fMultHitSep * theRMS;
        }
        if(theTime - tjs.fHits[iht]->PeakTime() > hitSep) break;
//        if(prt) mf::LogVerbatim("TC")<<" iht- "<<iht<<" "<<tjs.fHits[iht]->WireID().Plane<<":"<<PrintHit(tjs.fHits[iht])<<" RMS "<<tjs.fHits[iht]->RMS()<<" dt "<<theTime - tjs.fHits[iht]->PeakTime()<<" "<<hitSep;
        hitsInMultiplet.push_back(iht);
        theTime = tjs.fHits[iht]->PeakTime();
        if(iht == 0) break;
      } // iht
    } // iht > 0
    localIndex = hitsInMultiplet.size() - 1;
    // reverse the order so that hitsInMuliplet will be
    // returned in increasing time order
    if(hitsInMultiplet.size() > 1) std::reverse(hitsInMultiplet.begin(), hitsInMultiplet.end());
    // look for hits > theTime but within hitSep
    theTime = tjs.fHits[theHit]->PeakTime();
    theRMS = tjs.fHits[theHit]->RMS();
    for(unsigned int iht = theHit + 1; iht < tjs.fHits.size(); ++iht) {
      if(tjs.fHits[iht]->WireID().Wire != theWire) break;
      if(tjs.fHits[iht]->WireID().Plane != thePlane) break;
      if(tjs.fHits[iht]->RMS() > theRMS) {
        hitSep = fMultHitSep * tjs.fHits[iht]->RMS();
        theRMS = tjs.fHits[iht]->RMS();
      } else {
        hitSep = fMultHitSep * theRMS;
      }
      if(tjs.fHits[iht]->PeakTime() - theTime > hitSep) break;
//      if(prt) mf::LogVerbatim("TC")<<" iht+ "<<iht<<" "<<PrintHit(tjs.fHits[iht])<<" dt "<<(theTime - tjs.fHits[iht]->PeakTime())<<" RMS "<<tjs.fHits[iht]->RMS()<<" "<<hitSep;
      hitsInMultiplet.push_back(iht);
      theTime = tjs.fHits[iht]->PeakTime();
    } // iht

  } //GetHitMultiplet

/*
  ////////////////////////////////////////////////
  void TrajClusterAlg::HitMultipletPosition(unsigned int theHit, float& hitTick, float& deltaRms, float& qtot)
  {
    // returns the charge weighted wire, time position of hits in the multiplet which are within
    // fMultHitSep of iht
    
    std::vector<unsigned int> hitsInMultiplet;
    GetHitMultiplet(theHit, hitsInMultiplet);
    qtot = 0;
    hitTick = 0;
    for(auto iht : hitsInMultiplet) {
      qtot += tjs.fHits[iht]->Integral();
      hitTick += tjs.fHits[iht]->Integral() * tjs.fHits[iht]->PeakTime();
    } // iht
    hitTick /= qtot;
    deltaRms = HitsRMS;
//    deltaRms = sqrt(HitsTimeErr2(hitsInMultiplet));
    
  } // HitMultipletPosition
*/
  ////////////////////////////////////////////////
  bool TrajClusterAlg::TrajHitsOK(unsigned int iht, unsigned int jht)
  {
    // Hits (assume to be on adjacent wires with wire A > wire B) have an acceptable signal overlap
    
    if(iht > tjs.fHits.size() - 1) return false;
    if(jht > tjs.fHits.size() - 1) return false;
    
    raw::TDCtick_t hiStartTick = tjs.fHits[iht]->StartTick();
    if(tjs.fHits[jht]->StartTick() > hiStartTick) hiStartTick = tjs.fHits[jht]->StartTick();
    raw::TDCtick_t loEndTick = tjs.fHits[iht]->EndTick();
    if(tjs.fHits[jht]->EndTick() < loEndTick) loEndTick = tjs.fHits[jht]->EndTick();
    // add a tolerance to the StartTick - EndTick overlap
    raw::TDCtick_t tol = 30;
    // expand the tolerance for induction planes
    if(fPlane < geom->Cryostat(fCstat).TPC(fTpc).Nplanes()-1) tol = 40;

    if(tjs.fHits[jht]->PeakTime() > tjs.fHits[iht]->PeakTime()) {
      // positive slope
      if(loEndTick + tol < hiStartTick) {
//          if(prt) mf::LogVerbatim("TC")<<" bad overlap pos Slope "<<loEndTick<<" > "<<hiStartTick;
        return false;
      }
    } else {
      // negative slope
      if(loEndTick + tol < hiStartTick) {
//          if(prt) mf::LogVerbatim("TC")<<" bad overlap neg Slope "<<loEndTick<<" < "<<hiStartTick;
        return false;
      }
    }
    
    return true;
    
  } // TrajHitsOK

  
  ////////////////////////////////////////////////
  void TrajClusterAlg::FillWireHitRange(geo::TPCID const& tpcid)
  {
    // fills the WireHitRange vector. Slightly modified version of the one in ClusterCrawlerAlg.
    // Also fills the WirePtr vector
    
    // determine the number of planes
    art::ServiceHandle<geo::Geometry> geom;
    geo::TPCGeo const& TPC = geom->TPC(tpcid);
    unsigned int cstat = tpcid.Cryostat;
    unsigned int tpc = tpcid.TPC;
    unsigned short nplanes = TPC.Nplanes();
    
    lariov::ChannelStatusProvider const& channelStatus = art::ServiceHandle<lariov::ChannelStatusService>()->GetProvider();
    
    if(!tjs.WireHitRange.empty()) tjs.WireHitRange.clear();
    
    // initialize everything
    tjs.WireHitRange.resize(nplanes);
    tjs.FirstWire.resize(nplanes);
    tjs.LastWire.resize(nplanes);
    tjs.NumWires.resize(nplanes);
    tjs.MaxPos0.resize(nplanes);
    tjs.MaxPos1.resize(nplanes);
    fAveHitRMS.resize(nplanes, 3);
    
    std::pair<int, int> flag;
    flag.first = -2; flag.second = -2;
    
    // Calculate tjs.UnitsPerTick, the scale factor to convert a tick into
    // Wire Spacing Equivalent (WSE) units where the wire spacing in this plane = 1.
    // Strictly speaking this factor should be calculated for each plane to handle the
    // case where the wire spacing is different in each plane. Deal with this later if
    // the approximation used here fails.
    raw::ChannelID_t channel = tjs.fHits[0]->Channel();
    float wirePitch = geom->WirePitch(geom->View(channel));
    float tickToDist = detprop->DriftVelocity(detprop->Efield(),detprop->Temperature());
    tickToDist *= 1.e-3 * detprop->SamplingRate(); // 1e-3 is conversion of 1/us to 1/ns
    tjs.UnitsPerTick = tickToDist / wirePitch;
    
    for(unsigned short ipl = 0; ipl < nplanes; ++ipl) {
      tjs.FirstWire[ipl] = INT_MAX;
      tjs.LastWire[ipl] = 0;
      tjs.NumWires[ipl] = geom->Nwires(ipl, tpc, cstat);
      tjs.WireHitRange[ipl].resize(tjs.NumWires[ipl], flag);
      tjs.MaxPos0[ipl] = (float)(tjs.NumWires[ipl] + 1);
      tjs.MaxPos1[ipl] = (float)detprop->NumberTimeSamples() * tjs.UnitsPerTick;
    }
    
    unsigned int lastwire = 0, lastipl = 0;
    for(unsigned int iht = 0; iht < tjs.fHits.size(); ++iht) {
      if(tjs.fHits[iht]->WireID().Cryostat != cstat) continue;
      if(tjs.fHits[iht]->WireID().TPC != tpc) continue;
      unsigned short ipl = tjs.fHits[iht]->WireID().Plane;
      unsigned int wire = tjs.fHits[iht]->WireID().Wire;
      if(wire > tjs.NumWires[ipl] - 1) {
        mf::LogWarning("TC")<<"FillWireHitRange: Invalid wire number "<<wire<<" > "<<tjs.NumWires[ipl] - 1<<" in plane "<<ipl<<" Quitting";
        fQuitAlg = true;
        return;
      } // too large wire number
      if(ipl == lastipl && wire < lastwire) {
        mf::LogWarning("TC")<<"FillWireHitRange: Hits are not in increasing wire order. Quitting ";
        fQuitAlg = true;
        return;
      } // hits out of order
      lastwire = wire;
      lastipl = ipl;
      if(tjs.FirstWire[ipl] == INT_MAX) tjs.FirstWire[ipl] = wire;
      if(tjs.WireHitRange[ipl][wire].first == -2) tjs.WireHitRange[ipl][wire].first = iht;
      tjs.WireHitRange[ipl][wire].second = iht + 1;
      tjs.LastWire[ipl] = wire + 1;
      if(tjs.fHits[iht]->StartTick() > (int)detprop->NumberTimeSamples() || tjs.fHits[iht]->EndTick() > (int)detprop->NumberTimeSamples()) {
        std::cout<<"Bad StartTick "<<tjs.fHits[iht]->StartTick()<<" or EndTick "<<tjs.fHits[iht]->EndTick()<<" NumberTimeSamples "<<detprop->NumberTimeSamples()<<"\n";
      }
    } // iht
    
    // overwrite with the "dead wires" condition
    flag.first = -1; flag.second = -1;
    for(unsigned short ipl = 0; ipl < nplanes; ++ipl) {
      for(unsigned int wire = 0; wire < tjs.NumWires[ipl]; ++wire) {
        raw::ChannelID_t chan = geom->PlaneWireToChannel((int)ipl, (int)wire, (int)tpc, (int)cstat);
        if(!channelStatus.IsGood(chan)) tjs.WireHitRange[ipl][wire] = flag;
      } // wire
    } // ipl
    
    // do a QC check
    unsigned int firstHit, lastHit;
    TrajPoint tp;
    for(unsigned short ipl = 0; ipl < nplanes; ++ipl) {
      // calculate the average hit width
      float hitWid = 0;
      float sum2 = 0;
      unsigned int cnt = 0;
      for(unsigned int wire = 0; wire < tjs.NumWires[ipl]; ++wire) {
        //        mf::LogVerbatim("TC")<<" wire "<<wire<<" "<<tjs.WireHitRange[ipl][wire].first<<" "<<tjs.WireHitRange[ipl][wire].second;
        // No hits or dead wire
        if(tjs.WireHitRange[ipl][wire].first < 0) continue;
        firstHit = tjs.WireHitRange[ipl][wire].first;
        lastHit = tjs.WireHitRange[ipl][wire].second;
        if(lastHit > tjs.fHits.size()) {
          mf::LogWarning("TC")<<"FillWireHitRange: Invalid lastHit "<<lastHit<<" > fHits.size "<<tjs.fHits.size()<<" in plane "<<ipl;
          fQuitAlg = true;
          return;
        }
        for(unsigned int iht = firstHit; iht < lastHit; ++iht) {
          if(tjs.fHits[iht]->Multiplicity() == 1) {
//            float wid = tjs.fHits[iht]->EndTick() - tjs.fHits[iht]->StartTick();
            float wid = tjs.fHits[iht]->RMS();
            hitWid += wid;
            sum2 += wid * wid;
            ++cnt;
          }
          if(tjs.fHits[iht]->WireID().Plane != ipl) {
            mf::LogWarning("TC")<<"FillWireHitRange: Invalid plane "<<tjs.fHits[iht]->WireID().Plane<<" != "<<ipl;
            fQuitAlg = true;
            return;
          }
          if(tjs.fHits[iht]->WireID().Wire != wire) {
            mf::LogWarning("TC")<<"FillWireHitRange: Invalid wire "<<tjs.fHits[iht]->WireID().Wire<<" != "<<wire<<" in plane "<<ipl;
            fQuitAlg = true;
            return;
          }
        } // iht
      } // wire
      if(cnt > 5) fAveHitRMS[ipl] = hitWid/(float)cnt;
    } // ipl
    
  } // FillWireHitRange
 
  /////////////////////////////////////////
  unsigned short TrajClusterAlg::AngleRange(TrajPoint const& tp)
  {
    // returns the index of the angle range
    float dang = tp.Ang;
    if(dang > M_PI) dang = M_PI;
    if(dang < -M_PI) dang = M_PI;
    if(dang < 0) dang = -dang;
    if(dang > M_PI/2) dang = M_PI - dang;
    for(unsigned short ir = 0; ir < fAngleRanges.size(); ++ir) {
      if(dang < fAngleRanges[ir]) return ir;
    }
    return fAngleRanges.size() - 1;
  } // AngleRange

  /////////////////////////////////////////
  bool TrajClusterAlg::SignalAtTp(TrajPoint const& tp)
  {
    return SignalAtPos(tp.Pos[0], tp.Pos[1], tp.CTP);
  } // SignalAtTp
  
  
  /////////////////////////////////////////
  bool TrajClusterAlg::SignalAtPos(float pos0, float pos1, CTP_t tCTP)
  {
    // Returns true if the TP is within the TPC and within a ROI
    
    if(pos0 < 0) return false;
    if(pos1 < 0) return false;
    unsigned int wire = std::nearbyint(pos0);
    geo::PlaneID planeID = DecodeCTP(tCTP);
    unsigned int ipl = planeID.Plane;
    if(wire >= tjs.NumWires[ipl]) return false;
    if(pos1 > tjs.MaxPos1[ipl]) return false;
    // Assume dead wires have a signal
    if(tjs.WireHitRange[ipl][wire].first == -1) return true;
    raw::TDCtick_t rawProjTick = (float)(pos1 / tjs.UnitsPerTick);
    unsigned int firstHit = (unsigned int)tjs.WireHitRange[ipl][wire].first;
    unsigned int lastHit = (unsigned int)tjs.WireHitRange[ipl][wire].second;
    for(unsigned int iht = firstHit; iht < lastHit; ++iht) {
      if(rawProjTick > tjs.fHits[iht]->StartTick() && rawProjTick < tjs.fHits[iht]->EndTick()) return true;
    } // iht
    return false;
  } // SignalAtPos
  
  //////////////////////////////////////////
  unsigned short TrajClusterAlg::NumPtsWithCharge(Trajectory& tj, bool includeDeadWires)
  {
    unsigned short ntp = 0;
    for(unsigned short ipt = tj.EndPt[0]; ipt < tj.EndPt[1]+1; ++ipt) if(tj.Pts[ipt].Chg > 0) ++ntp;
    // Add the count of deadwires
    unsigned short ipt0 = tj.EndPt[0];
    unsigned short ipt1 = tj.EndPt[1];
    if(ipt1 > tj.Pts.size() - 1) return 0;
    if(includeDeadWires) ntp += DeadWireCount(tj.Pts[ipt0], tj.Pts[ipt1]);
    return ntp;
  } // NumTPsWithCharge
  
  //////////////////////////////////////////
  unsigned short TrajClusterAlg::NumUsedHits(TrajPoint& tp)
  {
    // Counts the number of used hits in tp
    unsigned short nused = 0;
    if(tp.Hits.empty()) return nused;
    for(unsigned short ii = 0; ii < tp.UseHit.size(); ++ii) if(tp.UseHit[ii]) ++nused;
    return nused;
  } // NumUsedHits
  
  //////////////////////////////////////////
  float TrajClusterAlg::DeadWireCount(TrajPoint& tp1, TrajPoint& tp2)
  {
    return DeadWireCount(tp1.Pos[0], tp2.Pos[0], tp1.CTP);
  } // DeadWireCount
  
  //////////////////////////////////////////
  float TrajClusterAlg::DeadWireCount(float inWirePos1, float inWirePos2, CTP_t tCTP)
  {
    if(inWirePos1 < -0.4 || inWirePos2 < -0.4) return 0;
    unsigned int inWire1 = std::nearbyint(inWirePos1);
    unsigned int inWire2 = std::nearbyint(inWirePos2);
    geo::PlaneID planeID = DecodeCTP(tCTP);
    unsigned short plane = planeID.Plane;
    if(inWire1 > tjs.NumWires[plane] || inWire2 > tjs.NumWires[plane]) return 0;
    if(inWire1 > inWire2) {
      // put in increasing order
      unsigned int tmp = inWire1;
      inWire1 = inWire2;
      inWire2 = tmp;
    } // inWire1 > inWire2
    ++inWire2;
    unsigned int wire, ndead = 0;
    for(wire = inWire1; wire < inWire2; ++wire) if(tjs.WireHitRange[plane][wire].first == -1) ++ndead;
    return ndead;
  } // DeadWireCount

  //////////////////////////////////////////
  void TrajClusterAlg::CheckHitClusterAssociations()
  {
    // check hit - cluster associations
    
    if(tjs.fHits.size() != tjs.inClus.size()) {
      mf::LogWarning("TC")<<"CHCA: Sizes wrong "<<tjs.fHits.size()<<" "<<tjs.inClus.size();
      fQuitAlg = true;
      return;
    }
    
    unsigned int iht;
    short clID;
    
    // check cluster -> hit association
    for(unsigned short icl = 0; icl < tjs.tcl.size(); ++icl) {
      if(tjs.tcl[icl].ID < 0) continue;
      clID = tjs.tcl[icl].ID;
      for(unsigned short ii = 0; ii < tjs.tcl[icl].tclhits.size(); ++ii) {
        iht = tjs.tcl[icl].tclhits[ii];
        if(iht > tjs.fHits.size() - 1) {
          mf::LogWarning("CC")<<"CHCA: Bad tclhits index "<<iht<<" tjs.fHits size "<<tjs.fHits.size();
          fQuitAlg = true;
          return;
        } // iht > tjs.fHits.size() - 1
        if(tjs.inClus[iht] != clID) {
          mf::LogError("TC")<<"CHCA: Bad cluster -> hit association. clID "<<clID<<" hit "<<PrintHit(tjs.fHits[iht])<<" tjs.inClus "<<tjs.inClus[iht]<<" CTP "<<tjs.tcl[icl].CTP;
          FindHit("CHCA ", iht);
          fQuitAlg = true;
          return;
        }
      } // ii
    } // icl
    
    // check hit -> cluster association
    unsigned short icl;
    for(iht = 0; iht < tjs.fHits.size(); ++iht) {
      if(tjs.inClus[iht] <= 0) continue;
      icl = tjs.inClus[iht] - 1;
      // see if the cluster is obsolete
      if(tjs.tcl[icl].ID < 0) {
        mf::LogError("TC")<<"CHCA: Hit "<<PrintHit(tjs.fHits[iht])<<" associated with an obsolete cluster tjs.tcl[icl].ID "<<tjs.tcl[icl].ID;
        fQuitAlg = true;
        return;
      }
      if (std::find(tjs.tcl[icl].tclhits.begin(), tjs.tcl[icl].tclhits.end(), iht) == tjs.tcl[icl].tclhits.end()) {
        mf::LogError("TC")<<"CHCA: Hit "<<tjs.tcl[icl].CTP<<":"<<PrintHit(tjs.fHits[iht])<<" -> tjs.inClus "<<tjs.inClus[iht]<<" but isn't in tjs.tcl[icl].ID "<<tjs.tcl[icl].ID<<" list of hits. icl "<<icl<<" iht "<<iht;
        for(unsigned short itj = 0; itj < tjs.allTraj.size(); ++itj) {
          if(tjs.allTraj[itj].ClusterIndex == icl) mf::LogError("TC")<<"CHCA: Cluster index "<<icl<<" found in traj ID "<<tjs.allTraj[itj].ID;
        } // itj
        PrintAllTraj("CHCA", tjs, debug, USHRT_MAX, USHRT_MAX);
        fQuitAlg = true;
        return;
      }
    } // iht
    
  } // CheckHitClusterAssociations()
  
  //////////////////////////////////////////
  void TrajClusterAlg::MaskTrajEndPoints(Trajectory& tj, unsigned short nPts)
  {
    // Masks off (sets all hits not-Used) nPts trajectory points at the leading edge of the
    // trajectory, presumably because the fit including this points is poor. The position, direction
    // and Delta of the last nPts points is updated as well
    
    if(tj.Pts.size() < 3) {
      mf::LogError("TC")<<"MaskTrajEndPoints: Trajectory ID "<<tj.ID<<" too short to mask hits ";
      fGoodWork = false;
      return;
    }
    if(nPts > tj.Pts.size() - 2) {
      mf::LogError("TC")<<"MaskTrajEndPoints: Trying to mask too many points "<<nPts<<" Pts.size "<<tj.Pts.size();
      fGoodWork = false;
      return;
    }
    
    // find the last good point (with charge)
    unsigned short lastGoodPt = USHRT_MAX ;
    for(unsigned short ii = 0; ii < tj.Pts.size(); ++ii) {
      unsigned short ipt = tj.EndPt[1] - nPts - ii;
      if(tj.Pts[ipt].Chg > 0) {
        lastGoodPt = ipt;
        break;
      }
      if(ipt == 0) break;
    } // ii
    if(prt) {
      mf::LogVerbatim("TC")<<"MTEP: lastGoodPt "<<lastGoodPt<<" Pts size "<<tj.Pts.size()<<" fGoodWork "<<fGoodWork;
    }
    if(lastGoodPt == USHRT_MAX) return;
    tj.EndPt[1] = lastGoodPt;
    bool isVLA = (AngleRange(tj.Pts[lastGoodPt]) > 1);
    
    for(unsigned short ii = 0; ii < nPts; ++ii) {
      unsigned short ipt = tj.Pts.size() - 1 - ii;
      // Ensure that the hits are masked off
      for(unsigned short jj = 0; jj < tj.Pts[ipt].Hits.size(); ++jj) {
        tj.Pts[ipt].UseHit[jj] = false;
        unsigned int iht = tj.Pts[ipt].Hits[jj];
        if(tjs.inTraj[iht] == tj.ID) tjs.inTraj[iht] = 0;
      }
      tj.Pts[ipt].Chg = 0;
      // Reset the position and direction of the masked off points
      tj.Pts[ipt].Dir = tj.Pts[lastGoodPt].Dir;
      if(isVLA) {
        // Very large angle: Move by path length
        float path = TrajPointSeparation(tj.Pts[lastGoodPt], tj.Pts[ipt]);
        tj.Pts[ipt].Pos[0] = tj.Pts[lastGoodPt].Pos[0] + path * tj.Pts[ipt].Dir[0];
        tj.Pts[ipt].Pos[1] = tj.Pts[lastGoodPt].Pos[1] + path * tj.Pts[ipt].Dir[1];
      } else {
        // Not large angle: Move by wire
        float dw = work.Pts[ipt].Pos[0] - work.Pts[lastGoodPt].Pos[0];
        // Correct the projected time to the wire
        float newpos = work.Pts[lastGoodPt].Pos[1] + dw * work.Pts[ipt].Dir[1] / work.Pts[ipt].Dir[0];
        if(prt) mf::LogVerbatim("TC")<<"MTEP: ipt "<<ipt<<" Pos[0] "<<tj.Pts[ipt].Pos[0]<<". Move Pos[1] from "<<tj.Pts[ipt].Pos[1]<<" to "<<newpos;
        tj.Pts[ipt].Pos[1] = work.Pts[lastGoodPt].Pos[1] + dw * work.Pts[ipt].Dir[1] / work.Pts[ipt].Dir[0];
      }
      tj.Pts[ipt].Delta = PointTrajDOCA(tjs, tj.Pts[ipt].HitPos[0], tj.Pts[ipt].HitPos[1], tj.Pts[ipt]);
    } // ii
    
  } // MaskTrajEndPoints


} // namespace cluster
