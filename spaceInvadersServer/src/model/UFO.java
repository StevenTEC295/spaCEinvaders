
package model;
import java.util.Random;
public class UFO {    
    protected int x, y;
    protected int puntos;
    protected boolean vivo;

    
    public UFO(){
        Random random = new Random();
        this.vivo = true;
        this.puntos = random.nextInt(10, 100);
    }
    
    public void matar(){this.vivo = false;}
    public int getX(){ return x; }
    public int getY(){ return y; }
    public int getPuntos(){ return puntos; }
    public boolean isVivo(){ return vivo; }
    public void setX(int x){this.y = y;}
    public void setY(int y){this.x = x;}

}
