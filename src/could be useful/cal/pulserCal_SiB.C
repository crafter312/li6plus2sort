void pulserCal_SiB()
{

  float channel[2][32][25]={-1};
  int itel,istp,ipeaks =-1;
  float ichan =0.;
  float voltage[22] ={-1};
  int Peaks[2][32] = {-1};
  ifstream file("Si_Pulser_Back_new.dat");
  // ofstream ofile1("Si_Back_LG_Low.cal");
  //ofstream ofile2("Si_Back_LG_High.cal");

  for(int itele =6; itele<8;itele++)
    {
      for(int istrip = 0;istrip<32;istrip++)
	{
	  file >> itel >> istp >> ipeaks;
	  Peaks[itele-6][istrip]=ipeaks;
	  if(itele != itel || istp != istrip)
	    {
	      cout << "Didn't read the file right" << endl;
	      return;
	    }
	  for(int peak =0;peak<ipeaks;peak++)
	    {
	      file >> ichan;
	      channel[itele-6][istrip][peak] =ichan;
	    }      
	}
    }

  
  for (int ii=1;ii<21;ii++)
    {
      voltage[ii-1]= (10./20.)*ii;
      //cout << voltage[ii-1] << endl;
    }

  gROOT->cd();
  TCanvas *mycan1 = (TCanvas*)gROOT->FindObjectAny("mycan1");
  if(!mycan1)
    {
      mycan1 = new TCanvas("mycan1","mycan1");
    }

  int Switch1,Switch2 =0;
  for(int itele =6;itele<8;itele++)
    {
      for(int istrip = 0;istrip<32;istrip++)
	{
	  mycan1->cd(1);
	  TGraph *mygraph = new TGraph(Peaks[itele-6][istrip],channel[itele-6][istrip],voltage);
	  mygraph->SetName(Form("Si_%i_%i",itele,istrip));
	  mygraph->Draw("AP");
	  mygraph->SetMarkerStyle(20);

	  mycan1->Modified();
	  mycan1->Update();
	  
	  TMarker *mark=(TMarker*)mycan1->WaitPrimitive("TMarker");
	  Switch1 = mark->GetX();
	  delete mark;      
	  
	  mark=(TMarker*)mycan1->WaitPrimitive("TMarker");
	  Switch2 = mark->GetX();
	  delete mark;      


	  
	 
	  TF1 *myfit = new TF1("myfit","pol1",0,Switch1);
	  TF1 *myfit2 = new TF1("myfit2","pol1",Switch1,Switch2);
	  myfit->SetLineColor(kBlue);
	  myfit2->SetLineColor(kRed);
	  mygraph->Fit(myfit,"R");
	  mygraph->Fit(myfit2,"R");
	  myfit->Draw("Same");
	  myfit2->Draw("Same");
	  mycan1->Modified();
	  mycan1->Update();
	  TMarker *mark=(TMarker*)mycan1->WaitPrimitive("TMarker");
	  delete mark;      
	  
	  //ofile1 << itele << " " << istrip << " " << myfit->GetParameter(1);
	  //ofile1 << " " << myfit->GetParameter(0) << endl;
	  //ofile2 << itele << " " << istrip << " " << myfit2->GetParameter(1);
	  //ofile2 << " " << myfit2->GetParameter(0) << endl;

	}
      
    } 
  return;
}
