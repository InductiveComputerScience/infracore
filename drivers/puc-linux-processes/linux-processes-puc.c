#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <setjmp.h>

#include <ProcessingUnitController.h>

#include "linux-processes-puc.h"

struct ProcessingUnitControllerProcessesStructure{
	double pus;
	double programsLength;
	pid_t *ppid; // 1 pid per program
	pid_t *pupid; // 1 pid per PU, pid must be in PU to run
	sigjmp_buf *sigbufs;
};

typedef struct ProcessingUnitControllerProcessesStructure ProcessingUnitControllerProcessesStructure;

static void SetExceptionHandler(void (*Program)());
void Program1Exception(int x);
void Program2Exception(int x);

void CreateProcessingUnitControllerProcesses(ProcessingUnitControllerStructure **puc, double pus, void (**programs)(), double programsLength){
	ProcessingUnitControllerProcessesStructure *pucS;
	int sret;
	int i;

	// Create Structure
	*puc = malloc(sizeof(ProcessingUnitControllerStructure)); // Alloc #1
	pucS = malloc(sizeof(ProcessingUnitControllerProcessesStructure)); // Alloc #2
	(*puc)->p = pucS;

	pucS->pus = pus;
	pucS->programsLength = programsLength;
	pucS->ppid = (pid_t*)malloc(pus * sizeof(pid_t));
	pucS->pupid = (pid_t*)malloc(programsLength * sizeof(pid_t));
	pucS->sigbufs = (sigjmp_buf*)malloc(programsLength * sizeof(sigjmp_buf));

	// Start PUs
	for(i = 0; i < programsLength; i++){
		pucS->ppid[i] = fork();

		if(pucS->ppid[i] == 0){
			//SetExceptionHandler(Program1Exception);
			raise(SIGSTOP);
			sret = sigsetjmp(pucS->sigbufs[i], 1); // savesigs is true
			if(sret == 0){
				programs[i]();
			}else{
				programs[i]();
			}
			exit(0);
		}
		waitpid(pucS->ppid[i], NULL, WSTOPPED);
	}
}

void CloseProcessingUnitControllerProcesses(ProcessingUnitControllerStructure *puc){
}

void GetProcessingUnitsAndPrograms(ProcessingUnitControllerStructure *puc, int64_t *rpus, int64_t *rps){
	ProcessingUnitControllerProcessesStructure *pucS;
	pucS = (ProcessingUnitControllerProcessesStructure *)puc->p;

	*rpus = pucS->pus;
	*rps = pucS->programsLength;
}

void Start(ProcessingUnitControllerStructure *puc, int64_t n){
	ProcessingUnitControllerProcessesStructure *pucS;
	pucS = (ProcessingUnitControllerProcessesStructure *)puc->p;

	kill(pucS->pupid[n], SIGCONT);
	waitpid(pucS->pupid[n], NULL, WCONTINUED);
}

void Stop(ProcessingUnitControllerStructure *puc, int64_t n){
	ProcessingUnitControllerProcessesStructure *pucS;
	pucS = (ProcessingUnitControllerProcessesStructure *)puc->p;

	kill(pucS->pupid[n], SIGSTOP);
	waitpid(pucS->pupid[n], NULL, WSTOPPED);
}

void SetProgram(ProcessingUnitControllerStructure *puc, int64_t n, int64_t p){
	ProcessingUnitControllerProcessesStructure *pucS;
	pucS = (ProcessingUnitControllerProcessesStructure *)puc->p;

	pucS->pupid[n] = pucS->ppid[p];
}

/*static void SetExceptionHandler(void (*Program)()){
	struct sigaction *sa;

	sa = (struct sigaction *)malloc(sizeof(struct sigaction));
	memset(sa, 0, sizeof(struct sigaction));
	sigemptyset(&sa->sa_mask);
	sa->sa_handler = Program;
	sa->sa_flags = 0;

	sigaction(SIGILL, sa, NULL);
	sigaction(SIGFPE, sa, NULL);
	sigaction(SIGSEGV, sa, NULL);
	sigaction(SIGBUS, sa, NULL);
	sigaction(SIGTRAP, sa, NULL);
}*/

/*void Program1Exception(int x){
	siglongjmp(sigbufs[0], 0);
}*/

/*void Program2Exception(int x){
	siglongjmp(sigbufs[1], 0);
}*/



























