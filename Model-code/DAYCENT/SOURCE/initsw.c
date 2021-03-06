
/*              Copyright 1993 Colorado State University                    */
/*                      All Rights Reserved                                 */

/*****************************************************************************
**
**  FILE:      initsw.c
**
**  FUNCTION:  void initsw()
**
**  PURPOSE:   Initialize the soil water model
**
**  INPUTS:
**    sitlat - latitude (degrees)
**
**  GLOBAL VARIABLES:
**    FNSITE   - file name for site specific input parameters (sitepar.in)
**    FNSOIL   - file name for soil layer structure input file (soils.in)
**    MAXLYR   - maximum number of soil water model layers (21)
**    MAXSTLYR - maximum number of 5 centimeter layers for the soil
**               temperature model (200)
**    PI       - pi (3.14159265)
**
**  EXTERNAL VARIABLES:
**    files                  - structure containing information about output
**                             files
**    flags                  - structure containing debugging flags
**    layers                 - soil water soil layer structure
**    layers->numlyrs        - total number of layers in the soil water model
**                             soil profile
**    layers->swcfc[]        - volumetric soil water content at field capacity
**                             for layer (cm H2O/cm of soil)
**    layers->swclimit[]     - minimum volumetric soil water content of a
**                             layer, fraction 0.0 - 1.0
**    layers->swcwp[]        - volumetric soil water content at wilting point
**                             for layer (cm H2O)
**    layers->width[]        - the thickness of soil water model layers (cm)
**    sitepar                - site specific parameters structure for soil
**                             water model
**    sitepar->fswcinit      - initial soil water content, fraction of field
**                             capacity (0.0 - 1.0)
**    sitepar->usexdrvrs     - 0 = use air temperature to drive PET rates
**                             1 = use extra weather drivers (solrad, rhumid,
**                                 windsp) for PET calculation, 
**                             2 = use extra weather drivers (srad, vpd)
**                                 for photosynthesis calculation
**                             3 = use extra drivers for both PET and
**                                 photosynthesis calculations
**    soil                   - soil temperature structure
**
**  LOCAL VARIABLES:
**    errmsg[] - string containing error message
**    ilyr     - current layer in the soil profile
**    latitude - site latitude (decimal degrees)
**    lcnt     - count of number of input file lines read
**    line[]   - buffer containing line read from input file
**    MAXL     - maximum length of line read from input file
**    wrt      - flag, 0 = do not write to output file,
**               1 = do not write to output file
**
**  OUTPUTS:
**    bioabsorp             - litter biomass at full absorption of radiation
**                            (grams biomass)
**    daylength[]           - length of day light (hours)
**    files->fp_bio         - file pointer to bio.out output file
**    files->fp_cflows      - file pointer to cflows.out output file
**    files->fp_co2         - file pointer to co2.out output file
**    files->fp_dcsip       - file pointer to dc_sip.csv output file
**    files->fp_deadc       - file pointer to deadc.out output file
**    files->fp_dels        - file pointer to dels.out output file
**    files->fp_dN2lyr      - file pointer to dN2lyr.out output file
**    files->fp_dN2Olyr     - file pointer to the dN2Olyr.out output file
**    files->fp_gresp       - file pointer to gresp.out output file
**    files->fp_harv        - file pointer to the harvest.csv output file
**    files->fp_livec       - file pointer to livec.out output file
**    files->fp_mresp       - file pointer to mresp.out output file
**    files->fp_outf        - file pointer to outfiles.in input file
**    files->fp_psyn        - file pointer to psyn.out output file
**    files->fp_soilc       - file pointer to soilc.out output file
**    files->fp_soiln       - file pointer to soiln.out output file
**    files->fp_soiltavg    - file pointer to soiltavg.out output file
**    files->fp_soiltmax    - file pointer to soiltmax.out output file
**    files->fp_soiltmin    - file pointer to soiltmin.out output file
**    files->fp_stempdx     - file pointer to stemp_dx.out output file
**    files->fp_swc         - file pointer to vswc.out output file
**    files->fp_sysc        - file pointer to sysc.out output file
**    files->fp_tgmonth     - file pointer to the tgmonth.out output file
**    files->fp_wb          - file pointer to watrbal.out output file
**    files->fp_wflux       - file pointer to wflux.out output file
**    files->fp_wfps        - file pointer to wfps.out output file
**    files->fp_yearsum     - file pointer to year_summary.out output file
**    files->fp_yrcflows    - file pointer to year_cflows.out output file
**    files->write_bio      - flag to indicate if bio.out output file should
**                            be created, 0 = do not create, 1 = create
**    files->write_cflows   - flag to indicate if cflows.out output file
**                            should be created, 0 = do not create, 1 = create
**    files->write_co2      - flag to indicate if co2.out output file should
**                            be created, 0 = do not create, 1 = create
**    files->write_dcsip    - flag to indicate if dc_sip.csv output file
**                            should be created, 0 = do not create, 1 = create
**    files->write_deadc    - flag to indicate if deadc.out output file
**                            should be created, 0 = do not create, 1 = create
**    files->write_dels     - flag to indicate if dels.out output file should
**                            be created, 0 = do not create, 1 = create
**    files->write_dN2lyr   - flag to indicate if dN2lyr.out output file
**                            should be created, 0 = do not create, 1 = create
**    files->write_dN2Olyr  - flag to indicate if dN2Olyr.out output file
**                            should be created, 0 = do not create, 1 = create
**    files->write_gresp    - flag to indicate if gresp.out output file should
**                            be created, 0 = do not create, 1 = create
**    files->write_harvest  - flag to indicate if harvest.csv output file
**                            should be created, 0 = do not create, 1 = create
**    files->write_livec    - flag to indicate if livec.out output file
**                            should be created, 0 = do not create, 1 = create
**    files->write_mresp    - flag to indicate if mresp.out output file should
**                            be created, 0 = do not create, 1 = create
**    files->write_psyn     - flag to indicate if psyn.out output file should
**                            be created, 0 = do not create, 1 = create
**    files->write_soilc    - flag to indicate if soilc.out output file
**                            should be created, 0 = do not create, 1 = create
**    files->write_soiln    - flag to indicate if  output file should
**                            be created, 0 = do not create, 1 = create
**    files->write_soiltavg - flag to indicate if soiltavg.out output file
**                            should be created, 0 = do not create, 1 = create
**    files->write_soiltmax - flag to indicate if soiltmax.out output file
**                            should be created, 0 = do not create, 1 = create
**    files->write_soiltmin - flag to indicate if soiltmin.out output file
**                            should be created, 0 = do not create, 1 = create
**    files->write_stempdx  - flag to indicate if stemp_dx.out output file
**                            should be created, 0 = do not create, 1 = create
**    files->write_swc      - flag to indicate if vswc.out output file should
**                            be created, 0 = do not create, 1 = create
**    files->write_sysc     - flag to indicate if sysc.out output file
**                            should be created, 0 = do not create, 1 = create
**    files->write_tgmonth  - flag to indicate if tgmonth.out output file
**                            should be created, 0 = do not create, 1 = create
**    files->write_wb       - flag to indicate if watrbal.out output file
**                            should be created, 0 = do not create, 1 = create
**    files->write_wflux    - flag to indicate if wflux.out output file should
**                            be created, 0 = do not create, 1 = create
**    files->write_wfps     - flag to indicate if wfps.out output file should
**                            be created, 0 = do not create, 1 = create
**    files->write_yearsum  - flag to indicate if year_summary.out output file
**                            should be created, 0 = do not create, 1 = create
**    files->write_yrcflow  - flag to indicate if year_cflows.out output file
**                            should be created, 0 = do not create, 1 = create
**    flags->debug          - flag to set debugging mode, 0 = off, 1 = on
**    flags->verbose        - flag to set verbose debugging mode, 0 = off,
**                            1 = on
**    layers->minpot[]      - minimum matric potential by layer based on
**                            swcmin (-cm)
**    layers->swc[]         - soil water content by layer (cm H2O)
**    layers->swcmin[]      - lower bound on soil water content by layer
**                            (cm H2O) swc will not be allowed to drop below
**                            this minimum
**    maxphoto              - maximum carbon loss due to photodecomposition
**                            (ug C/KJ srad)
**    numlyrs               - total number of layers in the soil water model
**                            soil profile 
**    sitepar->rlatitude    - latitude of the site (in radians)
**    soil->soiltavg[]      - average soil temperature of layer (degrees C)
**    soil->soiltmax[]      - maximum soil temperature by layer (degrees C)
**    soil->soiltmin[]      - minimum soil temperature by layer (degrees C)
**    soil->stmtemp[]       - the average soil temperature of the soil
**                            temperature model layers (degrees C)
**    sradadj[]             - solar radiation adjustment for cloud cover and
**                            transmission coeffient
**    swcinit[]             - initial soil water content by layer (cm H2O)
**    texture               - texture classification for trace gas model
**                            (1 = coarse, 2 = medium, 3 = fine)
**    tminintercept         - intercept used to adjust minimum temperature
**                            for calculating VPD at dewpoint
**    tminslope             - slope used to adjust minimum temperature
**                            for calculating VPD at dewpoint
**    usexdrvrs             - 0 = use air temperature to drive PET rates
**                            1 = use extra weather drivers (solrad, rhumid,
**                                windsp) for PET calculation, 
**                            2 = use extra weather drivers (srad, vpd)
**                                for photosynthesis calculation
**                            3 = use extra drivers for both PET and
**                                photosynthesis calculations
** 
**  CALLED BY:
**    detiv
**
**  CALLS:
**    initlyrs()  - read in layers of the soil structure for the site
**                  (from soils.in)
**    initsite()  - read in site specific parameters (from sitepar.in)
**    initsrad()  - initialize the solar radiation submodel
**    swpotentl() - given its soil water content calculate the soil water
**                  potential of a soil layer
**
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "soilwater.h"

#define MAXL 150

FLAG_S flagstruct;
FLAG_SPT flags = &flagstruct;
LAYERPAR_S lyrstruct;
LAYERPAR_SPT layers = &lyrstruct;
SITEPAR_S sitestruct;
SITEPAR_SPT sitepar = &sitestruct;
SOIL_S soilstruct;
SOIL_SPT soil = &soilstruct;
FILES_S filestruct;
FILES_SPT files = &filestruct;

    void initsw(double *sitlat, double swcinit[MAXLYR], int *usexdrvrs,
                int *numlyrs, int *texture, 
                double sradadj[NMONTH], double *tminslope, double *tminintercept,
                double *maxphoto, double *bioabsorp)
    {

      int wrt, ilyr, lcnt;
      char errmsg[100], line[MAXL];
      double latitude;
      double daylength[NDAY];

      flags->debug = 1;
      flags->verbose = 1;

      initlyrs(FNSOIL, layers, flags, sitepar);
      initsite(FNSITE, sitepar, layers, flags, sradadj, tminslope,
               tminintercept, maxphoto, bioabsorp);
      latitude = *sitlat;
      initsrad(sitepar->elevation, latitude, sitepar->slope, sitepar->aspect,
               sitepar->ehoriz, sitepar->whoriz, daylength);

      sitepar->rlatitude = *sitlat * (PI/180.0);
      *texture = sitepar->texture;
   
      for (ilyr=0; ilyr<layers->numlyrs; ilyr++) {

        layers->swc[ilyr] = sitepar->fswcinit * layers->swcfc[ilyr];
        swcinit[ilyr] = layers->swc[ilyr];
   
        /* Set the lower limit on soil water content and potential. */ 

        layers->swcmin[ilyr] = 0.01;
        layers->minpot[ilyr] = 0;
      }

      for (ilyr=0; ilyr<MAXLYR; ilyr++) {
        soil->soiltavg[ilyr] = 0.0;
        soil->soiltmin[ilyr] = 0.0;
        soil->soiltmax[ilyr] = 0.0;
      }

      for (ilyr=0; ilyr<MAXSTLYR; ilyr++) {
        soil->stmtemp[ilyr] = 0.0;
      }
   
      layers->swc[layers->numlyrs] = 0.0;
      swcinit[layers->numlyrs] = layers->swc[layers->numlyrs];
      *usexdrvrs = sitepar->usexdrvrs;
      *numlyrs = layers->numlyrs;

      if ((files->fp_outf = fopen("outfiles.in", "r")) == NULL) {
        sprintf(errmsg, "Cannot open file %s\n", "outfiles.in");
        perror(errmsg);
        exit(1);
      }

      files->write_bio = 0;
      files->write_soiln = 0;
      files->write_soiltavg = 0;
      files->write_soiltmax = 0;
      files->write_soiltmin = 0;
      files->write_stempdx = 0;
      files->write_swc = 0;
      files->write_wb = 0;
      files->write_wfps = 0;
      files->write_co2 = 0;
      files->write_wflux = 0;
      files->write_mresp = 0;
      files->write_yearsum = 0;
      files->write_livec = 0;
      files->write_deadc = 0;
      files->write_soilc = 0;
      files->write_sysc = 0;
      files->write_tgmonth = 0;
      files->write_dN2lyr = 0;
      files->write_dN2Olyr = 0;
      files->write_gresp = 0;
      files->write_dels = 0;
      files->write_dcsip = 0;
      files->write_harvest = 0;
      files->write_cflows = 0;
      files->write_yrcflows = 0;
      files->write_psyn = 0;

      lcnt = 0;
      while( fgets(line, MAXL, files->fp_outf) != NULL) {
        printf("%s", line);
        lcnt++;
        if (lcnt > 1) {
          sscanf(line, "%d", &wrt);
          printf("wrt = %d\n", wrt);
        }
        if (lcnt == 2)  files->write_bio = wrt;
        if (lcnt == 3)  files->write_soiln = wrt;
        if (lcnt == 4)  files->write_soiltavg = wrt;
        if (lcnt == 5)  files->write_soiltmax = wrt;
        if (lcnt == 6)  files->write_soiltmin = wrt;
        if (lcnt == 7)  files->write_stempdx = wrt;
        if (lcnt == 8)  files->write_swc = wrt;
        if (lcnt == 9)  files->write_wb = wrt;
        if (lcnt == 10) files->write_wfps = wrt;
        if (lcnt == 11) files->write_co2 = wrt;
        if (lcnt == 12) files->write_wflux = wrt;
        if (lcnt == 13) files->write_mresp = wrt;
        if (lcnt == 14) files->write_yearsum = wrt;
        if (lcnt == 15) files->write_livec = wrt;
        if (lcnt == 16) files->write_deadc = wrt;
        if (lcnt == 17) files->write_soilc = wrt;
        if (lcnt == 18) files->write_sysc = wrt;
        if (lcnt == 19) files->write_tgmonth = wrt;
        if (lcnt == 20) files->write_dN2lyr = wrt;
        if (lcnt == 21) files->write_dN2Olyr = wrt;
        if (lcnt == 22) files->write_gresp = wrt;
        if (lcnt == 23) files->write_dels = wrt;
        if (lcnt == 24) files->write_dcsip = wrt;
        if (lcnt == 25) files->write_harvest = wrt;
        if (lcnt == 26) files->write_cflows = wrt;
        if (lcnt == 27) files->write_yrcflows = wrt;
        if (lcnt == 28) files->write_psyn = wrt;
      }

      if (files->write_soiln) {
        files->fp_soiln = fopen("soiln.csv", "w"); 
        fprintf(files->fp_soiln, "%8s,%8s,%12s,%12s,%12s,%12s,%12s,",
                "time", "dayofyr", "ammonium", "NO3_ppm[0]", "NO3_ppm[1]",
                "NO3_ppm[2]", "NO3_ppm[3]");
        fprintf(files->fp_soiln, "%12s,%12s,%12s,%12s,%12s,%12s,%12s\n",
                "NO3_ppm[4]", "NO3_ppm[5]", "NO3_ppm[6]", "NO3_ppm[7]",
                "NO3_ppm[8]", "NO3_ppm[9]", "etc...");
      }

      if (files->write_co2) {
        files->fp_co2 = fopen("co2.csv", "w"); 
        fprintf(files->fp_co2, "%8s,%8s,%12s,%12s,%12s,%12s,%12s,",
                "time", "dayofyr", "CO2_ppm[0]", "CO2_ppm[1]", "CO2_ppm[2]",
                "CO2_ppm[3]", "CO2_ppm[4]");
        fprintf(files->fp_co2, "%12s,%12s,%12s,%12s,%12s,%12s\n",
                "CO2_ppm[5]", "CO2_ppm[6]", "CO2_ppm[7]", "CO2_ppm[8]",
                "CO2_ppm[9]", "etc...");
      }

      if (files->write_yearsum) {
        files->fp_yearsum = fopen("year_summary.csv", "w"); 
        fprintf(files->fp_yearsum, "%8s,%12s,%12s,%12s,%12s,%12s",
                "time", "N2Oflux", "NOflux", "N2flux", "CH4", "NIT,");
        fprintf(files->fp_yearsum, "%12s\n", "ANNPPT");
      }

      if (files->write_soilc) {
        files->fp_soilc = fopen("soilc.csv", "w"); 
        fprintf(files->fp_soilc, "%8s,%8s,%10s,%10s,%10s,%10s,", "time",
                "dayofyr", "metabc(2)", "strucc(2)", "som1c(1)", "som1c(2)");
        fprintf(files->fp_soilc, "%10s,%10s,%10s\n",
                "som2c(1)", "som2c(2)", "som3c");
      }

      if (files->write_tgmonth) {
        files->fp_tgmonth = fopen("tgmonth.csv", "w"); 
        fprintf(files->fp_tgmonth, "%8s, %12s, %12s, %12s, %12s, %12s\n",
                "time", "N2Oflux", "NOflux", "N2flux", "CH4", "NIT");
      }

      if (files->write_dN2lyr) {
        printf("Open dN2lyr.csv\n");
        files->fp_dN2lyr = fopen("dN2lyr.csv", "w"); 
        fprintf(files->fp_dN2lyr, "%8s,%8s,%12s,%12s,%12s,%12s,%12s,",
                "time", "dayofyr", "dN2_g/m2[0]", "dN2_g/m2[1]", "dN2_g/m2[2]",
                "dN2_g/m2[3]", "dN2_g/m2[4]");
        fprintf(files->fp_dN2lyr, "%12s,%12s,%12s,%12s,%12s,%12s\n",
                "dN2_g/m2[5]", "dN2_g/m2[6]", "dN2_g/m2[7]", "dN2_g/m2[8]",
                "dN2_g/m2[9]", "etc...");
      }

      if (files->write_dN2Olyr) {
        files->fp_dN2Olyr = fopen("dN2Olyr.csv", "w"); 
        fprintf(files->fp_dN2Olyr, "%8s,%8s,%12s,%12s,%12s,%12s,%12s,",
                "time", "dayofyr", "dN2O_g/m2[0]", "dN2O_g/m2[1]",
                "dN2O_g/m2[2]", "dN2O_g/m2[3]", "dN2O_g/m2[4]");
        fprintf(files->fp_dN2Olyr, "%12s,%12s,%12s,%12s,%12s,%12s\n",
                "dN2O_g/m2[5]", "dN2O_g/m2[6]", "dN2O_g/m2[7]",
                "dN2O_g/m2[8]", "dN2O_g/m2[9]", "etc...");
      }

      if (files->write_cflows) {
        files->fp_cflows = fopen("cflows.csv", "w");
        fprintf(files->fp_cflows, "%8s,%8s,%10s,%10s,%10s,%10s,%10s,",
                "time", "dayofyr", "som11tosom21", "som12tosom22",
                "som12tosom3", "som21tosom11", "som21tosom22");
        fprintf(files->fp_cflows, "%10s,%10s,%10s,%10s,%10s,",
                "som22tosom12", "som22tosom3", "som3tosom12", "metc1tosom11",
                "metc2tosom12");
        fprintf(files->fp_cflows, "%10s,%10s,%10s,%10s,",
                "struc1tosom11", "struc1tosom21", "struc2tosom12",
                "struc2tosom22");
        fprintf(files->fp_cflows, "%10s,%10s,%10s,%10s,",
                "wood1tosom11", "wood1tosom21", "wood2tosom11",
                "wood2tosom21");
        fprintf(files->fp_cflows, "%10s,%10s\n",
                "wood3tosom12", "wood3tosom22");
      }

      if (files->write_yrcflows) {
        files->fp_yrcflows = fopen("year_cflows.csv", "w"); 
        fprintf(files->fp_yrcflows, "%8s,%10s,%10s,%10s,%10s,%10s,",
                "time", "asom11tosom21", "asom12tosom22", "asom12tosom3",
                "asom21tosom11", "asom21tosom22");
        fprintf(files->fp_yrcflows, "%10s,%10s,%10s,%10s,%10s,",
                "asom22tosom12", "asom22tosom3", "asom3tosom12",
                "ametc1tosom11", "ametc2tosom12");
        fprintf(files->fp_yrcflows, "%10s,%10s,%10s,%10s,",
                "astruc1tosom11", "astruc1tosom21", "astruc2tosom12",
                "astruc2tosom22");
        fprintf(files->fp_yrcflows, "%10s,%10s,%10s,%10s,",
                "awood1tosom11", "awood1tosom21", "awood2tosom11",
                "awood2tosom21");
        fprintf(files->fp_yrcflows, "%10s,%10s\n",
                "awood3tosom12", "awood3tosom22");
      }

      fclose(files->fp_outf);

      if (flags->debug) {
        printf("Exitting initsw...\n");
      }

      return;
    }
