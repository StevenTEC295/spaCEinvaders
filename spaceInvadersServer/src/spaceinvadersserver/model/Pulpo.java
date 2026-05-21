
package spaceinvadersserver.model;


public class Pulpo extends Alien {
    public Pulpo(int id, int x, int y, int puntos) {
        super(id, x, y);
        this.puntos = puntos; 
    }
    @Override
    public String getType() { return "pulpo"; }
}
