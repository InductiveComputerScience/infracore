#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
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
};

typedef struct ProcessingUnitControllerProcessesStructure ProcessingUnitControllerProcessesStructure;

static void SetExceptionHandler(void (*Program)());
void ProgramException(int x);

static sigjmp_buf sigbuf;
static bool wasException;

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

	// Start PUs
	for(i = 0; i < programsLength; i++){
		pucS->ppid[i] = fork();

		if(pucS->ppid[i] == 0){
			SetExceptionHandler(ProgramException);
			wasException = false;
			raise(SIGSTOP);
			sret = sigsetjmp(sigbuf, 1); // savesigs is true
			programs[i]();
			exit(0);
		}
		waitpid(pucS->ppid[i], NULL, WSTOPPED);
	}
}

void CloseProcessingUnitControllerProcesses(ProcessingUnitControllerStructure *puc){
	ProcessingUnitControllerProcessesStructure *pucS;
	pucS = (ProcessingUnitControllerProcessesStructure *)puc->p;

	for(int i = 0; i < pucS->programsLength; i++){
		kill(pucS->pupid[i], SIGKILL);
	}
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

static void SetExceptionHandler(void (*Program)()){
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
}

void ProgramException(int x){
	//printf("Got signal: %d\n", x);
	wasException = true;
	siglongjmp(sigbuf, 0);
}

bool WasException(ProcessingUnitControllerStructure *puc){
	return wasException;
}

























