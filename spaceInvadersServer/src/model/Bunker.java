package model;

import java.util.ArrayList;
import java.util.List;

public class Bunker {
    protected int id;
    protected int x, y;
    protected int puntos;
    protected int health;
    protected boolean active;
    
    public Bunker(int id,int x, int y){
        this.id = id;
        this.x = x;
        this.y = y;
        this.health = 100; 
        this.active = true;
    }
    
    public void impacto(){this.health -= 10;}
    public static List<Bunker> createDefault() {
        List<Bunker> bunkers = new ArrayList<>();
        int row = 12;
        bunkers.add(new Bunker(1,1, row));
        bunkers.add(new Bunker(2,5, row));
        bunkers.add(new Bunker(3,9, row));
        bunkers.add(new Bunker(4,13, row));
        return bunkers;
    }
    public boolean isActive(){return active;}
    public void deactivate(){this.active = false;}
    public int getX(){ return x; }
    public int getY(){ return y; }
    public int getHealth(){return health;}
    public int getID(){return id;}
    public void setVida(int health){this.health = health;}
}
