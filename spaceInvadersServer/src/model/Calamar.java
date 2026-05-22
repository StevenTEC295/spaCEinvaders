
package model;

public class Calamar extends Alien
{
    public Calamar(int id, int x, int y, int puntos){
        super(id, x, y);
        this.puntos = puntos;
    
    }
    
    @Override
    public String getType(){return "calamar";}
}
