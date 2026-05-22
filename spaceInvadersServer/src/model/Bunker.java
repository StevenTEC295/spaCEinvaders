package model;

import java.util.ArrayList;
import java.util.List;

public class Bunker {
    protected int x, y;
    protected int puntos;
    protected int health;
    
    
    public Bunker(int x, int y){
        
        this.x = x;
        this.y = y;
        this.health = 100; 
    }
    
    public void impacto(){this.health -= 10;}
    public static List<Bunker> createDefault() {

        List<Bunker> bunkers = new ArrayList<>();

        int row = 12;

        bunkers.add(new Bunker(2, row));
        bunkers.add(new Bunker(6, row));
        bunkers.add(new Bunker(10, row));
        bunkers.add(new Bunker(14, row));

        return bunkers;
    }
    public int getX(){ return x; }
    public int getY(){ return y; }
    public int getHealth(){return health;}
}
