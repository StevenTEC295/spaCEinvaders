
package model;

public class Jugador {
    private String jugadorId;
    private int cannonX;
    private int vidas;
    private int puntos;
    private int cannonY;
    
    public Jugador(String id, int cannonX){
        this.jugadorId = id;
        this.cannonX = cannonX;
        this.cannonY = 817;
        this.vidas = 3;
        this.puntos = 0;
    
    }
    
    public void moverDerecha(){
        if (cannonX < 800){
        cannonX+=50;
        }
        
    }
    public void moverIzquierda(){
        if (cannonX > 100){
            cannonX-=50;
        }
            
    }
    public void impacto () {vidas--;}
    
    public void agregarPuntos(int pts){puntos+= pts;}
    
    public boolean isMuerto(){return vidas <= 0;}
    
    // getters...
    public String getJugadorId() { return jugadorId; }
    public int getCannonX()     { return cannonX; }
    public int getCannonY()     { return cannonY; }
    public int getVidas()       { return vidas; }
    public int getPuntos()       { return puntos; }
    
}
