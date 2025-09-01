/*
**      TOPO.CPP
**      Topology functions.
**
**      拓扑分类：《Ｃ语言常用算法程序集》（清华大学出版社）
**
**      ZJian,2000.3.2.
*/
#include "rays.h"
#include <stdlib.h>



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
EXPORT  void    FNBACK  topo_sort(int n,int r[],int m,int p[])
{
    int     *f;     // f[i] = # of the fore things of thing i
    int     *g;     // g[i] = pointer of the link of thing i's rear things #
    int     *s;     // work stack, store the thing ID(s) of no fore things
    int     top,i,j,k,t;
    struct  node 
    {
        int suc;
        int next;
    }   *q;
    
    //(1) allocate memory -------------------------------------------------------
    if(NULL == (q = (struct node *)GlobalAlloc(GPTR, m*sizeof(struct node))) )
        return;
    if(NULL == (f = (int *)GlobalAlloc(GPTR, n*sizeof(int))) )
        return;
    if(NULL == (s = (int *)GlobalAlloc(GPTR, n*sizeof(int))) )
        return;
    if(NULL == (g = (int *)GlobalAlloc(GPTR, n*sizeof(int))) )
        return;
    
    //(2) init datas ------------------------------------------------------------
    top=-1; t=0;
    for (k=0; k<=n-1; k++)
    {
        f[k]=0;
        g[k]=-1;
    }
    
    //(3) set datas -------------------------------------------------------------
    for (k=0; k<=m-1; k++)
    {
        i=r[k+k];            // i <= fore ID
        j=r[k+k+1];          // j <= rear ID
        f[j-1]=f[j-1]+1;     // add (j-1)'s fore things #
        
        q[k].next=g[i-1];    // 
        q[k].suc=j;          //
        g[i-1]=k;            //
    }
    for (k=0; k<=n-1; k++)
    {
        if (f[k]==0)         // ID(k+1) has no fore things
        {
            top=top+1;
            s[top]=k+1;       // push (k+1) to stack s
        }
    }
    
    while (top!=-1)         // is stack s is not empty
    {
        i=s[top];top=top-1;  // pop stack to i
        
        
        p[t]=i;
        t=t+1;
        k=g[i-1];
        while (k!=-1)
        {
            j=q[k].suc;
            f[j-1]=f[j-1]-1;
            if (f[j-1]==0)
            {
                top=top+1;
                s[top]=j;
            }
            k=q[k].next;
        }
    }
    for (k=0; k<=n-1; k++)
    {
        if (f[k]!=0)
        {
            p[t]=-(k+1);
            t=t+1;
        }
    }
    GlobalFree(f);
    GlobalFree(g); 
    GlobalFree(s); 
    GlobalFree(q);
    return;
}

