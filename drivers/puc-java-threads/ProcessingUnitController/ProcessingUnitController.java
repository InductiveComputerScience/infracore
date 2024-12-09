package ProcessingUnitController;

import java.util.concurrent.Callable;

public class ProcessingUnitController{
    public static ProcessingUnitControllerStructure CreatePUC(double pus, Thread [] programs) {
        ProcessingUnitControllerStructure puc;
        int i;

        puc = new ProcessingUnitControllerStructure();

        puc.pu = new PUStruct[(int)pus];
        for(i = 0; i < pus; i = i + 1){
            puc.pu[i] = new PUStruct();
            puc.pu[i].hasProgram = false;
            puc.pu[i].program = 0;
        }
        puc.programs = programs;

        puc.wasException = new boolean[programs.length];
        for(i = 0; i < programs.length; i = i + 1){
            puc.wasException[i] = false;
        }

        return puc;
    }

    public static void ClosePUC(ProcessingUnitControllerStructure puc1) {

    }

    public static void PUSpecs(ProcessingUnitControllerStructure puc, NumberReference pus, NumberReference ps){
        pus.numberValue = puc.pu.length;
        ps.numberValue = puc.programs.length;
    }

    public static void Start(ProcessingUnitControllerStructure puc, double n){
        int p;

        p = (int)puc.pu[(int)n].program;
        puc.pu[(int)n].started = true;

        puc.programs[p].resume();
    }

    public static void Stop(ProcessingUnitControllerStructure puc, double n){
        int p;

        p = (int)puc.pu[(int)n].program;

        puc.programs[(int)p].suspend();

        puc.pu[(int)n].started = false;
    }

    public static void SetProgram(ProcessingUnitControllerStructure puc, double n, double p){
        if(!puc.pu[(int)n].started) {
            puc.pu[(int)n].hasProgram = true;
            puc.pu[(int)n].program = p;
        }
    }

    public static boolean WasException(ProcessingUnitControllerStructure puc) {
        boolean wasException = false;

        Thread thread = Thread.currentThread();
        for(int i = 0; i < puc.programs.length; i++){
            if(puc.programs[i] == thread){
                wasException = puc.wasException[i];
            }
        }

        return wasException;
    }

    public static Thread CreateThread(int nr, ProcessingUnitControllerStructure puc, Callable<Integer> task){
        Thread thread = new Thread() {
            @Override
            public void run() {
                boolean ex;
                ex = true;
                for (; ex; ) {
                    ex = false;

                    try {
                        task.call();
                    } catch (Exception | Error e) {
                        ex = true;
                        puc.wasException[nr] = true;
                    }
                }
            }
        };

        return thread;
    }
}