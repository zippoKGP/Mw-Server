/*
**      TOPO.H
**      Topology functions.
**
**      ZJian,2000.3.2.
*/
#ifndef TOPO_H_INCLUDE
#define TOPO_H_INCLUDE


/***************************************************************************/
/*        FUNC :  Topology Sort                                            */
/*        IN   :  int n   -> # of problems                                 */
/*                int r[] -> 2 dimensions array,store relations            */
/*                                eg. {1,3} = do 1 first, then 3           */
/*                int m   -> # of 2 dimensions in the relations r          */
/*                int p[] -> 1 dimension array (length n), will store      */
/*                           the returned sorted data.                     */
/*                           If there is a data i is less than zero,       */
/*                           then problem |i| can't be sorted.             */
/*        OUT  :  NONE                                                     */
/***************************************************************************/
EXPORT  void    FNBACK  topo_sort(int n,int r[],int m,int p[]);


#endif//TOPO_H_INCLUDE

