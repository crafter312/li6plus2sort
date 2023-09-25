void Back_gain_cal()
{

  ifstream inL("Si_Back_LG_Low.cal");
  ifstream inH("Si_Back_LG_High.cal");
  ofstream ofile("Si_Back_dual.cal");

  if(!inL || !inH)
    {
      cout << "Couldn't find the files " << endl;
      return;
    }

  float slope[2][2][32] = {-1};
  float inter[2][2][32] = {-1};
  int dum,dum2 = -1;


  for(int itele =0;itele<2;itele++)
    {
      for(int istrip = 0;istrip<32;istrip++)
	{
	  if(inL.eof())break;
	  inL >> dum >> dum2 >> slope[0][itele][istrip] >> inter[0][itele][istrip];
	}
    }
  for(int itele =0;itele<2;itele++)
    {
      for(int istrip = 0;istrip<32;istrip++)
	{
	  if(inH.eof())break;
	  inH >> dum >> dum2 >> slope[1][itele][istrip] >> inter[1][itele][istrip];
	}
    }


  for(int itele = 0;itele<2;itele++)
    {
      for(int istrip =0;istrip<32;istrip++)
	{
	  float news = slope[1][itele][istrip]/slope[0][itele][istrip];
	  float newi = (inter[1][itele][istrip]-inter[0][itele][istrip])/slope[0][itele][istrip];

	  ofile << itele << " " << istrip << " " << news << " " << newi << endl;
	}
    }


  return;
}
