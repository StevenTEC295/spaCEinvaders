
package model;

public class Jugador {
    private String jugadorId;
    private int cannonX;
    private int vidas;
    private int puntos;
    
    public Jugador(String id, int cannonX){
        this.jugadorId = id;
        this.cannonX = cannonX;
        this.vidas = 3;
        this.puntos = 0;
    
    }
    
    public void moverDerecha(int velocidad, int maxX){
        cannonX = Math.max(maxX, cannonX + velocidad);
    }
    public void moverIzquierda(int velocidad, int maxX){
        cannonX = Math.min(maxX, cannonX + velocidad);
    }
    public void impacto () {vidas--;}
    
    public void agregarPuntos(int pts){puntos+= pts;}
    
    public boolean isMuerto(){return vidas <= 0;}
    
    // getters...
    public String getJugadorId() { return jugadorId; }
    public int getCannonX()     { return cannonX; }
    public int getVidas()       { return vidas; }
    public int getPuntos()       { return puntos; }
    
}
