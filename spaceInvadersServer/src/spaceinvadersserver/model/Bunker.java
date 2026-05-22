package spaceinvadersserver.model;

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
    public int getX(){ return x; }
    public int getY(){ return y; }
    public int getHealth(){return health;}
}
