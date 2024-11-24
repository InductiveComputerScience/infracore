package Program;

import Screen.Screen.NumberReference;
import Screen.Screen.ScreenStructure;

import static Screen.Screen.Screen.*;

public class Program {
    public static void Program(ScreenStructure screen1){
        int [] image;
        NumberReference widthRef, heightRef, densityRef;
        int width, height, w, h, i, r, g, b;

        widthRef = new NumberReference();
        heightRef = new NumberReference();
        densityRef = new NumberReference();

        GetScreenSpecifications(screen1, widthRef, heightRef, densityRef);

        width = (int)widthRef.numberValue;
        height = (int)heightRef.numberValue;

        image = new int[width*height];

        // Clear screen
        for(w = 0; w < 640; w = w + 1) {
            for(h = 0; h < 480; h = h + 1) {
                image[h*640 + w] = 0;
            }
        }

        for(i = 0; i < 200; i = i + 1) {
            r = 255;
            g = 255;
            b = 255;

            image[i * width + i] = r << 16 | g << 8 | b;

            Sync(screen1);
            Display(screen1, image);
        }
    }
}
