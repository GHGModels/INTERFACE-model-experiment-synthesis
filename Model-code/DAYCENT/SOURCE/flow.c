
/*              Copyright 1993 Colorado State University                    */
/*                      All Rights Reserved                                 */

/*****************************************************************************
**
**  FILE:      flow.c
**
**  FUNCTION:  void flow()
**
**  PURPOSE:   Schedules a flow by storing the arguments in a stack which will
**             be accessed by the function 'flowup' which performs the actual
**             flow for all stack elements where 'when' is less than or equal
**             to time.
**  
**  INPUTS:
**    from    - memory location to flow amount from
**    to      - memory location to flow amount to
**    when    - when in simulation time the flow occurred
**    howmuch - amount of element to flow from the from location to the to
**              location
**
**  GLOBAL VARIABLES:
**    flowstack[].from - memory location to flow amount from
**    flowstack[].to   - memory location to flow amount to
**    flowstack[].when - when in simulation time the flow occurred
**    flowstack[].amt  - amount of element to flow from the from location to
**                       the to location
**    LENFST           - maximum number of elements in flowstack array
**
**  LOCAL VARIABLES:
**    None
**
**  OUTPUT:
**    flowstack[] - variables used in Century flow routines
**    nflows      - indicates number of unflowed events stored in flowstack
**
**  CALLED BY:
**    csched()
**    cultiv()
**    cutrtn()
**    dedrem()
**    dshoot()
**    erosn()
**    esched()
**    firrtn()
**    grem()
**    growth()
**    harvst()
**    killrt()
**    litburn()
**    livrem()
**    partit()
**    pschem()
**    respir()
**    simsom()
**    soilos()
**    trees()
**    wdeath()
**
**  CALLS:
**    flow_err() - handles errors generated by the flow functions
**
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "flow.h"

    void flow(double *from, double *to, double *when, double *howmuch)
    {

      /* Increment the number of flows stored in the stack */
      nflows += 1;

      if (nflows > LENFST) {
        /* Stack Overflow */
        flow_err(1, *when);
        exit(1);
      } else {
        /* Store the arguments in the stack */
        flowstack[nflows].from = from;
        flowstack[nflows].to = to;
        flowstack[nflows].when = *when;
        flowstack[nflows].amt = *howmuch;
      }

      return;
    }