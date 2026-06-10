
package model;

public class Bala {
    private int id, x, y;
    private String owner;
    private boolean activo;
    
    public Bala(int id, int x, int y, String owner){
        this.id=id;
        this.x = x;
        this.y = y;
        this.owner = owner;
        this.activo = true;
    }
    
    public void mover(){
        
        y += owner.equals("jugador") ? -16:+16;
    }
    public boolean isFueraAlcance(){return y<0 || y>1000;}
    public void desactivar(){this.activo = false;}
    public boolean isActivo(){return activo;}
    public int getX(){return x;}
    public int getY(){return y;}
    public int getId(){return id;}
    public String getOwner(){return owner;}
    
    
}
