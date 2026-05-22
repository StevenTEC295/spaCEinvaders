
package model;
import java.util.Random;
public class UFO {    
    protected int x, y;
    protected int puntos;
    protected boolean vivo;
    protected boolean activo;
    
    public UFO(int x, int y){
        Random random = new Random();
        this.x = x;
        this.y = y;
        this.vivo = true;
        this.activo = true;
        this.puntos = random.nextInt(10, 100);
    }
    
    public void matar(){this.vivo = false;}
    public boolean isActivo(){return activo;}
    public void desactivar(){this.activo = false;}
    public int getX(){ return x; }
    public int getY(){ return y; }
    public int getPuntos(){ return puntos; }
    public boolean isVivo(){ return vivo; }
    public void setX(int y){this.y = y;}

}
