package com.exquance.metta.mockups;

import java.util.ArrayList;
import java.util.List;

import processing.core.PApplet;
import processing.core.PFont;

@SuppressWarnings("serial")
public class Sketch extends PApplet/*, Component*/ {
    
    public static boolean OPENGL_ON = false;
    public static boolean FONTS_ON = true;
    
    public static int WIDTH = -1;
    public static int HEIGHT = -1;
    
    public float center_x = -1;
    public float center_y = -1;
    
    private PFont cur_font; 
    
    protected List<Component> components;
    
    public Sketch() {
        components = new ArrayList<Component>();
    }
    
    protected void add_component(Component component) {
        components.add(component);
    }
    
    public void use_client_size() {
        int width = (WIDTH > 0) ? WIDTH : (screen.width - 100);
        int height = (HEIGHT > 0) ? HEIGHT : (screen.height - 100);        
        if (OPENGL_ON) {
            size(width, height, OPENGL);
        } else {
            size(width, height);
        }
    }
    
    public void set_cur_font(PFont font) {
        this.cur_font = font;
    }
    
    public PFont get_cur_font() {
        return cur_font;        
    }
    
    @Override
    public void setup() {
        for (Component component: components) {
             component.prepare(this);
        }
    }
    
    @Override
    public void draw() {
        for (Component component: components) {
            if (component.shown()) {
                component.mouse_move(mouseX, mouseY);
                component.update(this);
            }
        }        
    }
    
    @Override
    public void mousePressed() {  
        for (Component component: components) {
            if (component.shown()) {
                component.mouse_click(mouseX, mouseY);            
            }
        }
    } 
    
    // just the methods used in the sketches are turned off
    
    @Override
    public void size(final int iwidth, final int iheight,
            String irenderer, String ipath) {
        center_x = iwidth / 2;
        center_y = iheight / 2;
        super.size(iwidth, iheight, irenderer, ipath);
    }
    
    @Override
    public PFont loadFont(String filename) {
        return FONTS_ON ? super.loadFont(filename) : null; 
    }
    
    @Override
    public void textFont(PFont which) { if (FONTS_ON) super.textFont(which); }    
    
    @Override
    public void textFont(PFont which, float size) { if (FONTS_ON) super.textFont(which, size); }
    
    @Override
    public void text(String str, float x, float y) { if (FONTS_ON) super.text(str, x, y); }
}
