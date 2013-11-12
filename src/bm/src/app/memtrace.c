/* use shell command awk to filter all malloced addr and freed addr.
sushaohua@new18:~$ cat system_wid2.log |grep wid_memory_trace|grep malloc|awk '{print $15}' > malloc.log
sushaohua@new18:~$ cat system_wid2.log |grep wid_memory_trace|grep free|awk '{print $13}' > free.log

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 65535
#define FREE_FILE "free.log"
#define MALLOC_FILE "malloc.log"

int read_to_array( char * filename, int *arry)
{
	FILE *fp;
	int len = 0;
	int i=0;
	char buf[MAX_LINE];
	char *ptr;
	if ((fp = fopen(filename, "r")) == NULL )
		{
		printf("open file error.\n");
		exit(1);
		}

	while(fgets(buf,MAX_LINE,fp) != NULL)
	{
	//printf("%s",buf);
	arry[i] = (int)strtol(buf, &ptr, 16);
	//printf("%d,0x%x,i=%d.\n",arry[i],arry[i],i);
	i++;
	
	}
  return 0;


}
void comparemallocfree(int *malloc, int *free)
{ 
	int m;
	int f;
	for (m=0;m< MAX_LINE;m++)
		{
		if(malloc[m] == 0)
			continue;
		
		for(f=0;f< MAX_LINE;f++)
			{
				if (free[f] == 0)
				continue;
				if ( malloc[m] == free[f])
					{
					malloc[m] = 0;
					free[f] = 0;
					break;
					}
				
			}
		}
return;

}

void display_unfree(int * malloc)
{
         int i;
	for(i=0;i<MAX_LINE;i++)
		{
		if(malloc[i] != 0)
			printf("0x%x\n",malloc[i]);
		}



}

void main(void)
{

/*process org lig file*/
     int malloc[MAX_LINE];
    int free[MAX_LINE];
    int i;
	printf("%p,%p\n",malloc,free);
	for(i=0;i<MAX_LINE;i++)
		{
		malloc[i] = 0;
		free[i] = 0;
		}
		
	read_to_array(MALLOC_FILE, malloc);
	read_to_array(FREE_FILE, free);
	comparemallocfree(malloc,free);
	display_unfree(malloc);
    
}

