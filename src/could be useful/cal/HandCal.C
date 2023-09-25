void HandCal()
{

  ifstream infile("peaks.dat");
  ofstream outfile("handCal.Dat");

  int telescope = 2;
  int stripnumber;
  
  double peak1;
  double peak2;

  double alpha1 = 5.4779;
  double alpha2 = 8.6328;

  double slope;
  double intercept;
  
  for(int i = 0; i < 32; i++)
    {
      infile>>stripnumber>>peak1>>peak2;

      slope = (alpha2 - alpha1)/(peak2 - peak1);
      intercept = alpha2 - slope*peak2;

      outfile<<telescope<<" "<<stripnumber<<" "<<slope<<" "<<intercept<<endl;
    }
  infile.close();
  outfile.close();
  return;
}
