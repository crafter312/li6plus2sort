void SiCal()
{
  TFile *file = new TFile("../sort.root");


  ofstream ofile("dASical.dat");

  ostringstream outstring;
  string name;

  int p1= 15, p2=50; //+- fit limits up to 2 peaks. May be different.
  int const num_par = 5; //number of peaks times 2(pol1)+3(gaus).

  int ent = 0;

  TCanvas *mycan = new TCanvas("mycan","",1000,600);

  for(int it = 7;it<8;it++)
    {
      int ic =  11;
//       for(int ic = 0 ;ic<32;ic++)
	{

	  cout << it << " " << ic << endl;
	  file->cd();
	  outstring.str("");
	  outstring << "front/TotCal/FTotalCal" <<it<<"_" << ic;
	  name = outstring.str();
	  TH1I * hist = (TH1I*)file->Get(name.c_str());
	  //hist->Rebin(5);
	  hist->GetXaxis()->SetRangeUser(0,60);
	  ent = hist->GetEntries();
	  
	  cout << ent << endl;
	  hist->Draw();
	  mycan->SetLogy();
	  mycan->Modified();
	  mycan->Update();
	  if(ent <50)
	    {
	      ofile << ic << " " << 0 << " " << 0 << " "  << 0 << endl;
	      cout << ic << " Only has " << ent << " Entries. " << endl;
	      continue;
	    }
	  else
	    {

		  TMarker * mark;
	      ofile << it << " " << ic;
	      for(int p = 0;p<2;p++)
		{
		  ofile << " ";

		  mark=(TMarker*)mycan->WaitPrimitive("TMarker"); //Get the Background limits
		  double bkg_lo = mark->GetX();
		  delete mark;  
		  mark=(TMarker*)mycan->WaitPrimitive("TMarker");
		  double bkg_hi = mark->GetX();
		  delete mark;
		  mark=(TMarker*)mycan->WaitPrimitive("TMarker"); // Get the 1st peak initial guess
		  double peak1 = mark->GetX();
		  delete mark;
		  
		  double par[num_par] = {0.};
		  double out[num_par] = {0.}; 
		  
		  //cout << "peak1 = " << peak1 << endl;
		  double peak1_lo = peak1 - p1, peak1_hi = peak1 + p1; // Peak center and limits
		  
		  TF1 *l1 = new TF1("l1", "pol1", bkg_lo, bkg_hi);
		  TF1 *g1 = new TF1("g1", "gaus", peak1_lo,peak1_hi);
		  
		  TF1 *total = new TF1("total", "pol1(0)+gaus(2)", bkg_lo,bkg_hi);

		  //cout << peak1_lo << " " << peak1_hi << endl;

		  g1->SetParLimits(0,1.,1000.);
		  g1->SetParLimits(1,bkg_lo,bkg_hi);
		  g1->SetParLimits(2,0.1,10.0);
		  
		  

		  hist->Fit(l1,"RQ");
		  hist->Fit(g1,"RQ+");
		  
		  l1->GetParameters(&par[0]);
		  g1->GetParameters(&par[2]);
		  
		  total->SetParameters(par);


		  total->SetParLimits(2,1.,1000.);
		  total->SetParLimits(3,bkg_lo,bkg_hi);
		  total->SetParLimits(4,0.1,10.0);
		  hist->Fit(total,"RQ");
		  total->GetParameters(out);
		  
		  for(int i=0;i<num_par;i++)
		    {
		      cout << i << " " <<out[i] << " "<< par[i]<< endl;
		    }

		  cout << " " << total->GetParameter(3) << endl;
		  ofile << total->GetParameter(3);
		  
		}
	      mark=(TMarker*)mycan->WaitPrimitive("TMarker"); //Get the Background limits

	      ofile << endl;
	      cout << " done " << endl;
	    }
	  
	  cout << "done2" << endl;
	}
      cout << "done3" << endl;
    }
  file->Close();
  ofile.close();
  cout << "done4" << endl;
  return;
}
