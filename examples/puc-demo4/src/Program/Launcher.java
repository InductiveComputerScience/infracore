package Program;

import ProcessingUnitController.ProcessingUnitControllerStructure;

import static ProcessingUnitController.ProcessingUnitController.*;

public class Launcher {
    public static void main(String[] args) throws InterruptedException {
        final ProcessingUnitControllerStructure puc1;

        Thread [] programs = new Thread[2];

        puc1 = CreatePUC(3, programs);

        int program = 0;
        programs[program] = CreateThread(program, puc1, () -> {
            ProgramA.Program(puc1); return 0;
        });
        program++;
        programs[program] = CreateThread(program, puc1, () -> {
            ProgramB.Program(puc1); return 0;
        });

        for(int i = 0; i < programs.length; i++){
            programs[i].start();
        }

        for(int i = 0; i < programs.length; i++){
            programs[i].join();
        }

        ClosePUC(puc1);
    }
}
