#!/bin/bash

# Loop from bar 0 to 95
for bar in {0..95}; do
  echo "Processing bar ${bar}..."

  # Construct the cut string dynamically
  CUT="texneut.bar==${bar} && !texneut.isSaturated"

  # Run ROOT in batch mode (-b) and exit upon completion (-q)
  root -b -q -l "PlotFancy.C" -e "PlotFancyHist(\"tpar\", \"texneut.Lnorm:texneut.E_tot>>h(100,0,20000,100,-1.2,1.2)\", \"${CUT}\", \"colz\", \"E_{tot} (arb. units)\", \"Z (arb. units)\");"

  # Rename the generated canvas image to prevent overwriting
  if [ -f "Canvas_1.png" ]; then
    mv Canvas_1.png "Canvas_bar_${bar}.png"
  else
    echo "Warning: Canvas_1.png was not generated for bar ${bar}."
  fi
done

echo "Processing complete."
