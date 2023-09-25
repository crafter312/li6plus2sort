
const int Nboards = 8;
const int Nchan = 32;
const int len = 256;//Nboards*Nchan;

//sorts an array into ascending order
//preserves correspondance between the main array and the 'carry' array
void insertion_sort(Double_t array[], Double_t carry[], int len)
{
  for (int i = 1; i < len; i++)
  {
    double val = array[i];
    double carry_val = carry[i];
    for (int j = i - 1; j >= 0; j--)
    {
      if (array[j+1] < array[j])
      {
        array[j+1] = array[j];
        carry[j+1] = carry[j];
        array[j] = val;
        carry[j] = carry_val;
      }
      else
      {
        break;
      }
    }
  }
}//insertion_sort

//loads the histograms from histogram root file
void load_E_histograms(const char histogram_filepath[], TH1I *hist[], int Nb, int Nc) {
  TFile *infile = new TFile(histogram_filepath);
  if (!infile->IsOpen())
  {
    cout << "Error opening infile.root named: " << histogram_filepath << endl;
    abort();
  } 

  for (int ib = 0; ib < Nb; ib++)
  {
    for (int ic = 0; ic < Nc; ic++)
    {
      int loc = ib*32 + ic;
      hist[loc] = (TH1I*)infile->Get(Form("Summary/1dEnergyR/EhighR%i_%i",ib,ic));
    }
  }
}
//loads the histograms from histogram root file
void load_Delta_histograms(const char histogram_filepath[], TH1I *hist[], int Nb, int Nc) {
  TFile *infile = new TFile(histogram_filepath);
  if (!infile->IsOpen())
  {
    cout << "Error opening infile.root named: " << histogram_filepath << endl;
    abort();
  }
  for (int ib = 0; ib < Nb; ib++)
  {
    for (int ic = 0; ic < Nc; ic++)
    {
      int loc = ib*32 + ic;
      hist[loc] = (TH1I*)infile->Get(Form("Summary/1dDeltaR/DeltahighR%i_%i",ib,ic));
    }
  }
}


/* TSpectrum doesn't work well with newer versions of root. need to avoid.
//use alpha source spectra and actual energy peaks, calculates a linear equation
//to calibrate each strip of the silicon detector
void peak_fit(TList *list, TH1I *hist[], Int_t num_peaks_used, Double_t energy[], Double_t energy_er[], Double_t gains[len], Double_t offsets[len])
{

  //arrays to store the centroids of the peaks
  double centroids[len][num_peaks_used+1];
  double centroids_er[len][num_peaks_used+1];

  for (int i = 0; i < len; i++)
  {

    int max_bin = hist[i]->GetXaxis()->GetXmax();
    int min_bin = hist[i]->GetXaxis()->GetXmin();
    //don't want to include pedistals in peak search
    hist[i]->SetAxisRange(50, max_bin-100,"X");

    //roughly locates the peaks in the spectrum
    TSpectrum *spec = new TSpectrum(2 * num_peaks_used);
    int num_found = spec->Search(hist[i], 2, "", 0.5); //hist[i], sigma, option, threshold

    cout << "Found " << num_found << " peaks in histogram." << endl;

    //if too many or too few peaks have been found,
    //something has gone wrong and we move on to the next core
    if (num_found != num_peaks_used)
    {
      for (int j = 0; j < num_peaks_used; j++)
      {
        centroids[i][j] = -1;
      }
      continue;
    }

    //we fit a gaussian distribution around each peak
    //using the TSpectrum's data as a starting point
    TF1 *fit[num_peaks_used];
    double* x_pos_array = spec->GetPositionX();

    for (int j = 0; j < num_found; j++)
    {
      double x_pos = x_pos_array[j];
      int bin = hist[i]->GetXaxis()->FindBin(x_pos);
      double y_pos = (double) hist[i]->GetBinContent(bin);
      //for fitting alpha peaks use equation 1 from
      //https://doi.org/10.1016/j.apradiso.2014.11.023
      //[0]=Area, [1]=peak location, [2]=std of gaussian, [3]=tailing parameter
      fit[j] = new TF1(Form("Fit%i-%i", i, j), 
             "[0]/(2*[3])*exp((x-[1])/[3]+[2]^2/(2*[3]))*ROOT::Math::erfc(((x-[1])/[2]+[2]/[3])/pow(2,0.5))",
              x_pos - 50, x_pos + 50);

      fit[j]->SetParameters(y_pos*4, x_pos, 1, 1);
      fit[j]->SetParLimits(0, 10, 1e6); //Area
      fit[j]->SetParLimits(1, x_pos - 10, x_pos + 10); //centroid
      fit[j]->SetParLimits(2, 0.1, 30); //sigma
      fit[j]->SetParLimits(3, 0.1, 30); //tailing

      //fitting the equation and storing the calculated centroids
      hist[i]->Fit(fit[j], "RQ+");
      centroids[i][j] = fit[j]->GetParameter(1);
      centroids_er[i][j] = 0.2;

      //just for now, fit to origin
      centroids[i][1] = 0;
      centroids_er[i][1] = 0.2;

      //centroids_er[i][j] = fit[j]->GetParError(1);

      cout << fit[j]->GetParameter(2) << endl;
      cout << "CENTROID PROCESSED: Graph " << i << " Guess: " << x_pos << " Actual: " << centroids[i][j] << endl;
    }

    //to make sure the centroids match up to the correct energies
    //we sort the centroids in ascending order
    //making sure the centroids_er keep the correspondance
    insertion_sort(centroids[i], centroids_er[i], num_peaks_used);
  }



  //we graph the centroids vs their corresponding energies
  //and fit a linear equation on the points
  for (int i = 0; i < len; i++)
  {
    //if the histogram is empty, skip this core
    if (centroids[i][0] == -1)
    {
      gains[i] = -1;
      offsets[i] = -1;
      continue;
    }

    TGraphErrors *gr = new TGraphErrors(num_peaks_used, centroids[i], energy, centroids_er[i], energy_er);
    gr->Draw("AP");
    list->Add(gr); //adding the graph to be saved to root file later

    TF1 *coeffFit = new TF1("coeffFit", "[0] + [1]*x");
    //coeffFit->SetParLimits(0, -1000, 1000);
    //coeffFit->SetParLimits(1, -0.5, 3.0);
    gr->Fit(coeffFit, "Q+");
    //storing linear equation parameters
    gains[i] = coeffFit->GetParameter(1);
    offsets[i] = coeffFit->GetParameter(0);
  }
}
*/


//implement my own algorithm for searching for peaks
//take the derivative, then looks to see if it crosses a threshold
int peak_search(TH1I *hist, int threshold, int peak_index[], int Npeaks)
{
  //clear peak_index
  for (int i=0;i<Npeaks;i++) { peak_index[i] = 0;} 
  
  TH1I *hist_smooth = (TH1I*)hist->Clone();
  hist_smooth->Smooth(4);
  
  //take derivative
  int Nbins = hist_smooth->GetNbinsX();
  double deriv[Nbins];
  cout << "Nbins" << Nbins << endl;
  //start at bin 50 to cut out potential pedistals
  for (int i=50; i<Nbins-1;i++)
  {
    deriv[i] = hist_smooth->GetBinContent(i+1) - hist_smooth->GetBinContent(i);
  }

  int pos=50;
  int peaknum = 0;
  while(pos < Nbins-1 && peaknum < Npeaks)
  {
    if (deriv[pos] > threshold)
    {
      //we crossed the threshold, now look forward for the derivative to cross zero
      for (int posforward=pos; posforward < pos+50; posforward++)
      {
        if (deriv[posforward] < 0)
        {
          peak_index[peaknum] = posforward;
          pos = posforward+3; //get a little further
          peaknum++;
          break;
        }
      }

      if (peaknum == Npeaks){ break;} //no more peaks to find    

    }
    else
    {
      pos++;
    }
  }
  return peaknum;
}


//use alpha source spectra and actual energy peaks, calculates a linear equation
//to calibrate each strip of the silicon detector
void peak_fit(TList *list, TH1I *hist[], int num_peaks_used, double energy[], double energy_er[], double gains[], double offsets[], int length)
{

  //arrays to store the centroids of the peaks
  double centroids[length][num_peaks_used+1];
  double centroids_er[length][num_peaks_used+1];
  int peak_index[num_peaks_used];

  for (int i = 0; i < length; i++)
  {
    //skip histograms with low statistics or empty
    if (hist[i]->GetEntries() < 50)
    {
      cout << "hist " << i << " has too few entries, no fit applied" << endl;
      centroids[i][0] = -1;
      continue;
    }


    int max_bin = hist[i]->GetXaxis()->GetXmax();
    int min_bin = hist[i]->GetXaxis()->GetXmin();

    //don't want to include pedistals in peak search
    hist[i]->SetAxisRange(100, max_bin-100,"X");

    cout << "before peak_search" << endl;
    //roughly locates the peaks in the spectrum
    //peak_search(TH1I *hist, int threshold, int peak_index[], int Npeaks)
    int num_found = peak_search(hist[i], 20, peak_index, num_peaks_used);

    cout << "Found " << num_found << " peaks in histogram." << endl;
    cout << "peak located at bin " << peak_index[0] << endl;
  
    //if too many or too few peaks have been found,
    //something has gone wrong and we move on to the next core
    if (num_found != num_peaks_used)
    {
      cout << "!!! for loc " << i << " too few peaks were found" << endl;
      for (int j = 0; j < num_peaks_used; j++)
      {
        centroids[i][j] = -1;
      }
      continue;
    }

    //we fit a gaussian distribution around each peak
    //using the TSpectrum's data as a starting point
    TF1 *fit[num_peaks_used];

    for (int j = 0; j < num_found; j++)
    {
      int bin = peak_index[j];
      double x_pos = (double) hist[i]->GetBinCenter(bin);
      double y_pos = (double) hist[i]->GetBinContent(bin);
      //for fitting alpha peaks use equation 1 from
      //https://doi.org/10.1016/j.apradiso.2014.11.023
      //[0]=Area, [1]=peak location, [2]=std of gaussian, [3]=tailing parameter
      fit[j] = new TF1(Form("Fit%i-%i", i, j), 
             "[0]/(2*[3])*exp((x-[1])/[3]+[2]^2/(2*[3]))*ROOT::Math::erfc(((x-[1])/[2]+[2]/[3])/pow(2,0.5))",
              x_pos - 50, x_pos + 50);

      fit[j]->SetParameters(y_pos*4, x_pos, 1, 1);
      fit[j]->SetParLimits(0, 10, 1e6); //Area
      fit[j]->SetParLimits(1, x_pos - 10, x_pos + 10); //centroid
      fit[j]->SetParLimits(2, 0.1, 30); //sigma
      fit[j]->SetParLimits(3, 0.1, 30); //tailing

      //fitting the equation and storing the calculated centroids
      hist[i]->Fit(fit[j], "RQ+");
      centroids[i][j] = fit[j]->GetParameter(1);
      centroids_er[i][j] = 0.2;

      //just for now, fit to origin
      centroids[i][1] = 0;
      centroids_er[i][1] = 0.2;

      //centroids_er[i][j] = fit[j]->GetParError(1);

      cout << fit[j]->GetParameter(1) << endl;
      cout << "CENTROID PROCESSED: Graph " << i << " Guess: " << x_pos << " Actual: " << centroids[i][j] << endl;
    }

    //to make sure the centroids match up to the correct energies
    //we sort the centroids in ascending order
    //making sure the centroids_er keep the correspondance
    insertion_sort(centroids[i], centroids_er[i], num_peaks_used);
  }



  //we graph the centroids vs their corresponding energies
  //and fit a linear equation on the points
  for (int i = 0; i < length; i++)
  {
    //if the histogram is empty, skip this core
    if (centroids[i][0] == -1)
    {
      gains[i] = -1;
      offsets[i] = -1;
      continue;
    }

    TGraphErrors *gr = new TGraphErrors(num_peaks_used+1, centroids[i], energy, centroids_er[i], energy_er);
    gr->Draw("AP");
    list->Add(gr); //adding the graph to be saved to root file later

    TF1 *coeffFit = new TF1("coeffFit", "[0] + [1]*x");
    //coeffFit->SetParLimits(0, -1000, 1000);
    //coeffFit->SetParLimits(1, -0.5, 3.0);
    gr->Fit(coeffFit, "Q+");
    //storing linear equation parameters
    gains[i] = coeffFit->GetParameter(1)/1000;
    offsets[i] = coeffFit->GetParameter(0)/1000;
  }
}


//main program run in script
void SiCal()
{
  TList *list = new TList;
  TH1I * lin_hist_E[len];
  TH1I * lin_hist_Delta[len/2];

  load_E_histograms("../sort_1006alpha.root", lin_hist_E, Nboards, Nchan);
  //load_Delta_histograms("../sort.root", lin_hist_Delta, Nboards/2, Nchan);

  //linear calibration equation parameters for each core
  Double_t lin_gains[Nboards*Nchan];
  Double_t lin_offsets[Nboards*Nchan];

  Double_t lin_gains_delta[Nboards/2*Nchan];
  Double_t lin_offsets_delta[Nboards/2*Nchan];

  for (int i=0;i<Nboards*Nchan;i++){lin_gains[i] = 0;}
  for (int i=0;i<Nboards*Nchan;i++){lin_offsets[i] = 0;}
  for (int i=0;i<Nboards/2*Nchan;i++){lin_gains_delta[i] = 0;}
  for (int i=0;i<Nboards/2*Nchan;i++){lin_offsets_delta[i] = 0;}


  //adding histograms to be saved later
  for(int ib = 0;ib<Nboards;ib++) //we are using 8 front/Back Energys
  {
    for(int ic = 0 ;ic<Nchan;ic++) //each has 32 chan
    {
      int loc = ib*32+ic;
      cout << "ib " << ib << " ic " << ic << " loc " << loc << endl;
      list->Add(lin_hist_E[loc]);
    }
  }
  //for(int ib = 0;ib<Nboards/2;ib++) //we are using 4 Deltas
  //{
  //  for(int ic = 0;ic<Nchan;ic++) //each has 32 chan
  //  {
  //    int loc = ib*32+ic;
  //    cout << "Deltas: ib " << ib << " ic " << ic << " loc " << loc << endl;
  //    list->Add(lin_hist_Delta[loc]);
  //  }
  //}

  //This is where you can add more peaks to fit
  double Cf249_energy[2] = {5813.3,0};
  double Cf249_err[2] = {1,0};
  double peak4_energy[4] = {3184,5156,5486,5805};
  double peak4_err[4] = {1,1,1,1};
  double Ra226_energy[4] = {4784,5489,6002,7686};
  double Ra226_err[4] = {1,1,1,1};



  //This is where all the magic is done!
  peak_fit(list, lin_hist_E, 4, peak4_energy, peak4_err, lin_gains, lin_offsets, Nboards*Nchan);
  //peak_fit(list, lin_hist_Delta, 1, Cf249_energy, Cf249_err, lin_gains_delta, lin_offsets_delta, Nboards/2*Nchan);

  //write out linear equations parameters to FrontEcal.dat file to be used in analysis
  ofstream ofile("FrontEcal.dat");
  for(int ib=0;ib<8; ib=ib+2) //we are using 8 or 12 boards
  {
    for(int ic=0;ic<Nchan;ic++) //each has 32 chan
    {
      int loc = ib*32+ic;
      ofile << ib/2 << " " << ic << " " << lin_gains[loc] <<  " " << lin_offsets[loc] << endl;
    }
  }
  ofile.close();

  //write out linear equations parameters to BackEcal.dat file to be used in analysis
  ofile.open("BackEcal.dat");
  for(int ib=1;ib<8; ib=ib+2) //we are using 8 or 12 boards
  {
    for(int ic=0;ic<Nchan;ic++) //each has 32 chan
    {
      int loc = ib*32+ic;
      ofile << (ib-1)/2 << " " << ic << " " << lin_gains[loc] <<  " " << lin_offsets[loc] << endl;
    }
  }
  ofile.close();

/*
  //write out linear equations parameters to DeltaEcal.dat file to be used in analysis
  ofile.open("DeltaEcal.dat");
  for(int ib=0;ib<4;ib++) //we are using 8 or 12 boards
  {
    for(int ic=0;ic<Nchan;ic++) //each has 32 chan
    {
      int loc = ib*32+ic;
      ofile << ib << " " << ic << " " << lin_gains_delta[loc] <<  " " << lin_offsets_delta[loc] << endl;
    }
  }
  ofile.close();
*/


  //write out histograms and linear fit graphs for review
  TFile * outfile = new TFile("lin_cal.root", "RECREATE");
  outfile->cd();
  list->Write();
  outfile->Close();

}//SiCal

