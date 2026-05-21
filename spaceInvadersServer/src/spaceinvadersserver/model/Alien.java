
package spaceinvadersserver.model;

public abstract class Alien {
    protected int id;
    protected int x, y;
    protected int puntos;
    protected boolean vivo;
    
    public Alien(int id, int x, int y){
        this.id = id;
        this.x = x;
        this.y = y;
        this.vivo = true;
    }
    
    public abstract String getType();
    public int getId()     { return id; }
    public int getX()      { return x; }
    public int getY()      { return y; }
    public int getPuntos() { return puntos; }
    public boolean isVivo() { return vivo; }
    public void matar()     { this.vivo = false; }
    public void setX(int x){ this.x = x; }
    public void setY(int y){ this.y = y; }

    
}
    

