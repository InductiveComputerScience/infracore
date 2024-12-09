package Program;

import ProcessingUnitController.ProcessingUnitControllerStructure;

import static ProcessingUnitController.ProcessingUnitController.WasException;

public class ProgramA {
    static int i = 0;

    public static void Program(ProcessingUnitControllerStructure puc){
        boolean ex;

        i++;

        System.out.println("Program A round " + i);

        try {
            Thread.sleep(500);
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        }

        ex = WasException(puc);

        if(ex){
            System.out.println("Program A: Last execution caused exception.");
        }

        if(i == 1){
            double i = 1 / 0;
        }else if(i == 2){
            Object o = null;
            o.toString();
        }
    }
}
