package Program;

import Screen.ScreenStructure;

import static Screen.Screen.*;

public class Launcher {
    public static void main(String[] args) {
        ScreenStructure screen1;

        screen1 = CreateJavaScreen(640, 480);

        Program.Program(screen1);

        CloseJavaScreen(screen1);
    }
}
