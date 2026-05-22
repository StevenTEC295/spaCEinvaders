package engine;
import java.util.ArrayList;
import java.util.List;
import model.Alien;
import model.UFO;
import model.Bala;
import model.Bunker;
import model.Jugador;
import model.GameState;
import patrones.AlienFactory;
import patrones.JuegoObserver;


public class GameEngine extends Thread {
    
    private Jugador jugador;
    private List<Alien> aliens;
    private List<Bala> balas;
    private List<Bunker> bunkers;
    private UFO ufo;
    private List<JuegoObserver>  observers;
    private int wave;
    private int velocidad;
    private volatile boolean running;
    
    public GameEngine(String jugadorId){
        this.jugador = new Jugador(jugadorId, 400);
        this.aliens = AlienFactory.createWave(1);
        this.bunkers = Bunker.createDefault();
        this.balas = new ArrayList<>();
        this.wave=1;
        this.velocidad = 120;
        this.running = true;
    }
    @Override
    public void run(){
        while(running){
            update();
            checkCollisions();
            notifyStateChanged();
            checkWaveComplete();
            try { Thread.sleep(velocidad); }
            catch (InterruptedException e) { Thread.currentThread().interrupt(); }
        
        
        
        }
    
    }
    
    
    private void update(){
        moveAliens();
        moveBalas();
        updateUFO();
    
    }
    
    private int alienDirection = 1;
    private void moveAliens(){
        
        //Se mueven en bloque: todos a la derecha hasta el borde, luego bajan y van a la izquierda
        boolean hitWall = aliens.stream()
                .filter(Alien::isVivo)
                .anyMatch(a->a.getX()>=10 || a.getX()<=0);
        if (hitWall){
            aliens.stream().filter(Alien::isVivo)
                    .forEach(a->a.setY(a.getY()+1));
            alienDirection+=-1;
        }

    }
    private void moveBalas(){
        balas.forEach(Bala::mover);
        balas.removeIf(b-> !b.isActivo() || b.isFueraAlcance());

    }
    
    private void updateUFO() {

        // Si no existe, posibilidad de aparecer
        if (ufo == null) {

            double chance = Math.random();

            // 1% de probabilidad por update
            if (chance < 0.01) {

                // aparece desde la izquierda
                ufo = new UFO(0, 1);
            }

            return;
        }

        // mover UFO
        ufo.setX(ufo.getX() + 1);

        // eliminar si sale de pantalla
        if (ufo.getX() > 20) {
            ufo = null;
        }
    }
    private void checkCollisions(){}
    private void notifyStateChanged(){}
    private void checkWaveComplete(){}
    
    
    
    
}
