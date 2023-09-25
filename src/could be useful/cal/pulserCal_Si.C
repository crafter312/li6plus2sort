void pulserCal_Si()
{

  int channel[14][32][25]={-1};
  int itel,istp,ipeaks =-1;
  float ichan =0.;
  float voltage_full[22] ={0.};
  float voltage_fine[42] ={0.};
  int Peaks[14][32] = {-1};
  ifstream file("Si_Back_Pulser_Full.dat");

  for(int itele =0; itele<14;itele++)
    {
      for(int istrip = 0;istrip<32;istrip++)
	{
	  file >> itel >> istp >> ipeaks;
	  Peaks[itele]=ipeaks;
	  if(itele != itel || istp != istrip)
	    {
	      cout << "Didn't read the file right" << endl;
	      return;
	    }
	  for(int peak =0;peak<ipeaks;peak++)
	    {
	      file >> ichan;
	      if(peak ==0)ped[itele] = ichan;
	      else channel[itele][peak-1] =ichan-ped[itele];
	    }      
	  Peaks[itele] = ipeaks-1;
	}
    }

  for (int ii=1;ii<42;ii++)
    {
      voltage[ii-1]= (4./41.)*ii;
      //    cout << voltage[ii] << endl;
    }

  gROOT->cd();
  TCanvas *mycan1 = (TCanvas*)gROOT->FindObject("mycan1");
  if(!mycan1)
    {
      mycan1 = new TCanvas("mycan1","mycan1");
      mycan1->Divide(5,4);
    }
  TCanvas *mycan2 = (TCanvas*)gROOT->FindObject("mycan2");
  if(!mycan2)
    {
      mycan2 = new TCanvas("mycan2","mycan2");
      mycan2->Divide(5,4);
    }
  TCanvas *mycan3 = (TCanvas*)gROOT->FindObject("mycan3");
  if(!mycan3)
    {
      mycan3 = new TCanvas("mycan3","mycan3");
      mycan3->Divide(5,4);
    }


  for(int itele =0;itele<56;itele++)
    {
      if(itele<20)
	{
	  mycan1->cd(itele+1);
	  TGraph *mygraph = new TGraph(Peaks[itele],channel[itele],voltage);
	  mygraph->SetName(Form("CsI_%i",itele));
	  mygraph->Draw("AP");
	  mygraph->SetMarkerStyle(20);
	  TF1 *myfit = new TF1("myfit","pol1",0,500);
	  myfit->SetLineColor(kRed);
	  mygraph->Fit(myfit);
	}
      else if(itele>=20 && itele<40)
	{
	  mycan2->cd(itele+1-20);
	  TGraph *mygraph = new TGraph(Peaks[itele],channel[itele],voltage);
	  mygraph->SetName(Form("CsI_%i",itele));
	  mygraph->Draw("AP");
	  mygraph->SetMarkerStyle(20);
	  TF1 *myfit = new TF1("myfit","pol1",0,500);
	  myfit->SetLineColor(kRed);
	  mygraph->Fit(myfit);
	}
      else
	{
	  mycan3->cd(itele+1-40);
	  TGraph *mygraph = new TGraph(Peaks[itele],channel[itele],voltage);
	  mygraph->SetName(Form("CsI_%i",itele));
	  mygraph->Draw("AP");
	  mygraph->SetMarkerStyle(20);
	  TF1 *myfit = new TF1("myfit","pol1",0,500);
	  myfit->SetLineColor(kRed);
	  mygraph->Fit(myfit);
	}

    }


  return;
}
