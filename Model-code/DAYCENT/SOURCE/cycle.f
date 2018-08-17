
c               Copyright 1993 Colorado State University
c                       All Rights Reserved


      subroutine cycle()

      implicit none
      include 'const.inc'
      include 'dovars.inc'
      include 'plot2.inc'
      include 'plot3.inc'

c ... This routine is called once a day by Daily century
c ...   Call scheduling routine
c ...   Initialize production accumulators
c ...   If planting or growing month, calculate growing season precipitation
c ...   Initialize mineralization accumulators
c ...   Compute cultivation effects
c ...
c ... Difference from monthly version of Century
c ...   tave, rain, tmxs, tmns, stemp, tfunc, wfunc, anerb calculations removed
c ...   call to irrigation routine removed
c ...   calls to potential production routines removed
c ...   call to h2oloss removed
      
c ... Local variables
      integer      iel, lyr

c ... Initialize the mineralization accumulators for each element.
      do 30 iel = 1, MAXIEL
        do 25 lyr = 1, 2
          strmnr(lyr,iel) = 0.0
          metmnr(lyr,iel) = 0.0
          s1mnr(lyr,iel) = 0.0
          s2mnr(lyr,iel) = 0.0
25      continue
        s3mnr(iel) = 0.0
        gromin(iel) = 0.0
        w1mnr(iel) = 0.0
        w2mnr(iel) = 0.0
        w3mnr(iel) = 0.0
30    continue

      return
      end
