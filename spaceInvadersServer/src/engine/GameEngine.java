package engine;
import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;
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
    private int alienDirection;
    private int tickCount;        
    private int alienMovEach;     
    public GameEngine(String jugadorId){
        this.jugador = new Jugador(jugadorId, 400);
        this.aliens = AlienFactory.createWave(1);
        this.bunkers = Bunker.createDefault();
        this.balas = new ArrayList<>();
        this.observers  = new ArrayList<>(); 
        this.ufo = new UFO();
        this.wave=1;
        this.velocidad = 120;
        this.running = true;
        this.alienDirection = 1;
        this.tickCount = 0;
        this.alienMovEach = 4;
        
        
    }
    @Override
    public void run(){
        while(running){
            update();
            checkCollisions();
            notifyStateChanged();
            checkWaveComplete();
            try {Thread.sleep(velocidad);}
            catch (InterruptedException e) {Thread.currentThread().interrupt();}
        
        
        
        }
    }
    private void update(){
        moveAliens();
        moveBalas();
        updateUFO();
    }
    
    
    private void moveAliens(){
        
        tickCount++;
        if (tickCount < alienMovEach) return; // no mover todavía
        tickCount = 0;                         // reiniciar contador

        boolean hitWall = aliens.stream()
                .filter(Alien::isVivo)
                .anyMatch(a -> (a.getX() >= 13 && alienDirection == 1) || (a.getX() <= 0 && alienDirection == -1));

        if (hitWall) {
            aliens.stream().filter(Alien::isVivo)
                    .forEach(a -> a.setY(a.getY() + 1));
            alienDirection *= -1;
        } else {
            aliens.stream().filter(Alien::isVivo)
                    .forEach(a -> a.setX(a.getX() + alienDirection));
        }
    }

    
    private void moveBalas(){
        balas.forEach(Bala::mover);
        balas.removeIf(b-> !b.isActivo() || b.isFueraAlcance());

    }
    
    private void updateUFO() {
        // Si no existe, posibilidad de aparecer
        if (ufo.isVivo()) {
            double chance = Math.random();
            // 1% de probabilidad por update
            if (chance < 0.01) {
                // aparece desde la izquierda
                ufo = new UFO();
                ufo.setX(0);
                ufo.setY(1);
            }
            return;
        }
        // mover UFO
        ufo.setX(ufo.getX() + 1);

        // eliminar si sale de pantalla
        if (ufo.getX() > 20) {
            ufo.matar();
        }
    }
    private void checkCollisions(){
    List<Bala> balasJugador = balas.stream().filter(b->b.getOwner().equals("jugador") && b.isActivo()).collect(Collectors.toList());
    for(Bala b:balasJugador){
        for (Alien a:aliens){
            if (a.isVivo() && overlaps(b,a)){
                a.matar();
                b.desactivar();
                jugador.agregarPuntos(a.getPuntos());
                notifyAlienMuerto(a.getId(), a.getPuntos(), jugador.getPuntos());
            }
        }
    }
    
    
    
    for (Bala b: balas){
    
        if (b.getOwner().equals("alien")&&b.isActivo() && overlapsJugador(b)){
            b.desactivar();
            jugador.impacto();
            notifyJugadorImpacto(jugador.getVidas(), jugador.getCannonX());
            if(jugador.isMuerto()){
                running = false;
                notifyGameOver("SIN VIDAS", jugador.getPuntos());
            
            }
        
        }
    } 
    if (ufo.isVivo()){
        for(Bala b:balasJugador){
            if (overlapsUFO(b,ufo)){
                ufo.matar();
                b.desactivar();
                jugador.agregarPuntos(ufo.getPuntos());
                notifyUFOMuerto(ufo.getPuntos(), jugador.getPuntos());
            }
        }
    }
    for(Bala b:balasJugador){
        for (Alien a:aliens){
            if (a.isVivo() && overlaps(b,a)){
                a.matar();
                b.desactivar();
                jugador.agregarPuntos(a.getPuntos());
                notifyAlienMuerto(a.getId(), a.getPuntos(), jugador.getPuntos());
            }
        }
    }
    boolean overlapsFondo = aliens.stream().filter(Alien::isVivo).anyMatch(a -> a.getY() >= 10);

        if (overlapsFondo) {
            running = false;
            notifyGameOver("LOS ALIENS TE ALCANZARON", jugador.getPuntos());
        }
    for (Bala b : balas) {

            if (!b.isActivo()) continue;

            for (Bunker bunker : bunkers) {

                if (bunker.isActive() && overlapsBunker(b, bunker)) {

                    bunker.impacto();
                    b.desactivar();

                    if (bunker.getHealth() <= 0) {
                        bunker.deactivate();
                    }
                }
            }
        }
    }
    
    
    
    
    
    private void checkWaveComplete(){
        boolean allDead = aliens.stream().noneMatch(Alien::isVivo);
        if (allDead) {
            observers.forEach(o -> o.onWaveClear(wave));
            wave++;
            aliens    = AlienFactory.createWave(wave);
            velocidad     = Math.max(40, velocidad - 10); // se acelera cada oleada
        }
    }

    private void notifyAlienMuerto(int id, int puntos, int jugadorPuntos) {
        observers.forEach(o -> o.onAlienKilled(id, puntos, jugadorPuntos));
    }

    private boolean overlapsJugador(Bala b) {
        int bx = b.getX(), by = b.getY();
        int cx = jugador.getCannonX();
        return bx >= cx - 20 && bx <= cx + 20 && by >= 540 && by <= 580;
    }

    private boolean overlaps(Bala b, Alien a) {
        int bx = b.getX(), by = b.getY();
        int ax = a.getX() * 32, ay = a.getY() * 32;
        return bx >= ax && bx <= ax + 28 && by >= ay && by <= ay + 28;
    }
    private boolean overlapsUFO(Bala b, UFO ufo) {
        int bx = b.getX(), by = b.getY();
        int ufox = ufo.getX() * 32, ufoy = ufo.getY() * 32;
        return bx >= ufox && bx <= ufox + 28 && by >= ufoy && by <= ufoy + 28;
        }
    
private boolean overlapsBunker(Bala b, Bunker bunker) {
        int bx = b.getX(), by = b.getY();
        int bunkx = bunker.getX() * 32;
        int bunky = bunker.getY() * 32;

        return bx >= bunkx &&
               bx <= bunkx + 28 &&
               by >= bunky &&
               by <= bunky + 28;
    }
public synchronized void moverJugador(String direction) {
        if (direction.equals("LEFT"))  jugador.moverIzquierda(5, 0);
        if (direction.equals("RIGHT")) jugador.moverDerecha(5, 760);
    }
public synchronized void shoot() {
        boolean alreadyShooting = balas.stream()
            .anyMatch(b -> b.getOwner().equals("jugador") && b.isActivo());
        if (!alreadyShooting) {
            int id = balas.size() + 1;
            balas.add(new Bala(id, jugador.getCannonX(), 540, "jugador"));
        }
    }
    

    // ── admin ────────────────────────────────────────────────
    public synchronized void addAlien(Alien a) { aliens.add(a); }
    public synchronized void setSpeed(int ms)  { this.velocidad = ms; }
    public synchronized void setBunkerHealth(int pct) {
        bunkers.forEach(b -> b.setVida(pct));
    }
    private void notifyJugadorImpacto(int vidas, int cannonX) {
        observers.forEach(o -> o.onPlayerHit(vidas, cannonX));
    }
    private void notifyUFOMuerto(int ufoPts, int jugadorPts) {
        throw new UnsupportedOperationException("Not supported yet."); 
    }

    private void notifyGameOver(String reason, int puntos) {
        observers.forEach(o -> o.onGameOver(reason, puntos));
    }
    public synchronized void addObserver(JuegoObserver o) { observers.add(o); }
    public synchronized void removeObserver(JuegoObserver o) { observers.remove(o); }
    private void notifyStateChanged() {
        GameState state = buildState();
        observers.forEach(o -> o.onGameStateChanged(state));
    }
    
    private GameState buildState() {
        GameState s = new GameState();
        s.jugador     = this.jugador;
        s.aliens     = new ArrayList<>(this.aliens.stream().filter(a->a.isVivo()).collect(Collectors.toList()));
        s.balas    = new ArrayList<>(this.balas);
        s.bunkers    = new ArrayList<>(this.bunkers);
        s.ufo        = this.ufo;
        s.wave       = this.wave;
        s.alienSpeed = this.velocidad;
        s.status     = running ? "FINISHED" : "RUNNING";
        return s;
    }   
}
