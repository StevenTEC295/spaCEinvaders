
package model;


public class Cangrejo extends Alien{
    public Cangrejo(int id, int x, int y, int puntos) {
        super(id, x, y);
        this.puntos = puntos; 
    }
    @Override
    public String getType() { return "cangrejo"; }
}
