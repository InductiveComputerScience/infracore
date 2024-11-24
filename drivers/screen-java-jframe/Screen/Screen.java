package Screen;

import java.awt.Graphics;
import java.awt.image.BufferedImage;
import javax.swing.JFrame;
import javax.swing.JPanel;

public class Screen extends JPanel{
    private BufferedImage buffer;
    private JFrame window;

    public static ScreenStructure CreateJavaScreen(int w, int h){
        ScreenStructure screenStructure = new ScreenStructure();

        screenStructure.javaScreen = new Screen(w, h);
        screenStructure.last = System.currentTimeMillis();
        screenStructure.w = w;
        screenStructure.h = h;

        return screenStructure;
    }

    public static void CloseJavaScreen(ScreenStructure screen){
        screen.javaScreen.window.dispose();
    }

    public static void GetScreenSpecifications(ScreenStructure screen, NumberReference width, NumberReference height, NumberReference density) {
        width.numberValue = screen.w;
        height.numberValue = screen.h;
        density.numberValue = 1;
    }

    public Screen(int w, int h) {
        window = new JFrame("Java Virtual Screen");
        window.setSize(w, h+30);
        window.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        window.setLayout(null);
        setBounds(0, 0, w, h);
        window.add(this);
        window.setVisible(true);
        buffer = new BufferedImage(w, h, BufferedImage.TYPE_INT_RGB);
    }

    @Override
    public void paint(Graphics gr) {
        gr.drawImage(buffer, 0, 0, this);
    }

    public static void Display(ScreenStructure screen, int [] pixelsP){
        int w = screen.w;
        int h = screen.h;

        for(int j = 0; j < h; j++){
            for(int i = 0; i < w; i++){
                int color = pixelsP[j * w + i];
                screen.javaScreen.buffer.setRGB(i, j, color);
            }
        }

        screen.javaScreen.window.repaint();
    }

    public static boolean Sync(ScreenStructure screen){
        long diff = System.currentTimeMillis() - screen.last;

        long sleep = Math.max(16 - diff, 0);

        try {
            Thread.sleep(sleep);
        } catch (InterruptedException e) {
        }

        screen.last = System.currentTimeMillis();

        return true;
    }
}