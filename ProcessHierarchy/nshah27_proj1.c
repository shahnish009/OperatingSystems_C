#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
	if (argc != 4){printf("\n\nPlease enter './nshah27_proj1 H C 0' to execute\n\n");}
	else
	{
		int H = atoi(argv[1]);
		int C = atoi(argv[2]);
		int flag = atoi(argv[3]);
		pid_t pid[C];

		for(int i = 0; i < flag; i++)
		{
			printf("->\t");
		}
		printf("%d: Process starting\t%d: Parent's id = %d\t%d: Height in the tree = %d\t%d: Creating %d children at height %d\n", getpid(), getpid(), getppid(), getpid(), H, getpid(), C, (H-1));

		if (H > 1)
		{
			int i;
			for(i = 0; i < C; i++)
			{
				if ((pid[i] = fork()) == -1)
				{
					fprintf(stderr, "\nfork failed\n");
					exit (1);
				}
				else if (pid[i] == 0)
				{
					char strH[20];
                			char strC[20];
                			char strflag[20];
					snprintf(strH, 20, "%d", H-1);
					snprintf(strC, 20, "%d", C);
					snprintf(strflag, 20, "%d", flag+1);
					if (execlp("./nshah27_proj1", "./nshah27_proj1", strH, strC, strflag, (char *)NULL) == -1)
					{fprintf(stderr, "\nexec failed\n");}
					exit(0);
				}
			}
			int status = 0, wpid;
			while(( wpid = wait(&status)) > 0);
			printf("\n%d: Terminating at height %d\n", getpid(), H);
			exit(0);
		}
	}
}
