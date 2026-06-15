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
import java.util.concurrent.CopyOnWriteArrayList;

// Agregar campo al inicio de la clase

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
    private int ufoDirection;
    private int tickCount;        
    private int alienMovEach;  
    private int ufoMovEach;
    private int ufoTickCount;

    // agregar con los demás campos al inicio de la clase
    private int alienShootEach = 4; // dispara cada 15 ticks
    private int alienShootTick = 0;
    private Thread balasThread;
    public GameEngine(String jugadorId){
        this.jugador = new Jugador(jugadorId, 400);
        this.aliens = AlienFactory.createWave(1);
        this.bunkers = Bunker.createDefault();
        this.balas = new CopyOnWriteArrayList<>();
        this.observers  = new ArrayList<>(); 
        this.ufo = null;
        this.wave=1;
        this.velocidad = 400;
        this.running = true;
        this.alienDirection = 1;
        this.ufoDirection = 1;
        this.tickCount = 0;
        this.alienMovEach =4;
        this.ufoTickCount = 0;
        this.ufoMovEach = 2;
        balasThread = new Thread(() -> {
        while (running) {
            synchronized (this) {
                balas.forEach(Bala::mover);
                balas.removeIf(b -> !b.isActivo() || b.isFueraAlcance());
            }
            try { Thread.sleep(60); }  // balas a 50ms independiente del game loop
            catch (InterruptedException e) { Thread.currentThread().interrupt(); break; }
        }
        });
        balasThread.setDaemon(true);

        
    }
    private volatile GameState lastState;
    
    @Override
    public void run(){
        balasThread.start();
        while(running){
            update();
            checkCollisions();
            notifyStateChanged();
            checkWaveComplete();
            lastState = buildState();
            try {Thread.sleep(velocidad);}
            catch (InterruptedException e) {Thread.currentThread().interrupt();}
        
        
        
        }
    }
    private void update(){
        moveAliens();
        //moveBalas();
        updateUFO();
        alienesDisparan();
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
        UFO ufoLocal = this.ufo; 
        if (ufo == null) {
            
            double chance = Math.random();
            // 1% de probabilidad por update
            if (chance < 0.01) {
                // aparece desde la izquierda
                ufo = new UFO();
                ufo.setX(0);
                ufo.setY(1);
                System.out.println("Se creo UFO");
            }
            return;
        }
        else{
        
        ufoTickCount++;
        if (ufoTickCount < ufoMovEach) return; // no mover todavía
        ufoTickCount = 0;  
        
        if(ufoDirection ==1){
            ufo.setX(ufo.getX() + 1);
            System.out.println("UFO se movió 1 posición");
        }
        else{
            ufo.setX(ufo.getX()-1);
        }

        
        if (ufo.getX() > 20) {
            ufo.matar();
            ufo = null;
            System.out.println("El UFO se eliminó");
            this.ufoDirection = -1;
        }
        else if(ufo.getX() < 0){
            ufo.matar();
            ufo = null;
            System.out.println("El UFO se eliminó");
            this.ufoDirection = 1;
        }
        }
    }
    private synchronized void checkCollisions(){
    List<Bala> balasJugador = balas.stream().filter(b->b.getOwner().equals("jugador") && b.isActivo()).collect(Collectors.toList());
    for(Bala b:balasJugador){
        for (Alien a:aliens){
            if (a.isVivo() && overlaps(b,a)){
                a.matar();
                b.desactivar();
                System.out.println("Alien muerto:"+a.getId());
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
                
                triggerGameOver("SIN VIDAS");
                
                detener();
            
            }
        
        }
    } 
    if (ufo != null){
        for(Bala b:balasJugador){
            if (overlapsUFO(b,ufo)){
                ufo.matar();
                b.desactivar();
                jugador.agregarPuntos(ufo.getPuntos());
                notifyUFOMuerto(ufo.getPuntos(), jugador.getPuntos());
                ufo = null;
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
    boolean overlapsFondo = aliens.stream().filter(Alien::isVivo).anyMatch(a -> a.getY() >= 17);

        if (overlapsFondo) {
            
            triggerGameOver("LOS ALIENS TE ALCANZARON");
            detener();
        }
    for (Bala b : balas) {

            if (!b.isActivo()) continue;
            for (Bunker bunker : bunkers) {

                if (bunker.isActive() && overlapsBunker(b, bunker)) {

                    bunker.impacto();
                    b.desactivar();
                    notifyBunkerHit(bunker.getID(), bunker.getHealth());

                    if (bunker.getHealth() <= 0) {
                        bunker.deactivate();
                    }
                }
            }
        }
    }
    private void alienesDisparan() {
    alienShootTick++;
    if (alienShootTick < alienShootEach) return;
    alienShootTick = 0;

    
    List<Alien> vivos = aliens.stream()
            .filter(Alien::isVivo)
            .collect(Collectors.toList());

    if (vivos.isEmpty()) return;

   
    Alien tirador = vivos.get((int)(Math.random() * vivos.size()));

    
    int id = balas.size() + 1;
    int bx = tirador.getX() * 75 + 37;
    int by = tirador.getY() * 55 + 55;
    balas.add(new Bala(id, bx, by, "alien"));
}
    
    
    
    
    
    private void checkWaveComplete(){
        boolean allDead = aliens.stream().noneMatch(Alien::isVivo);
        if (allDead) {
            observers.forEach(o -> o.onWaveClear(wave));
            wave++;
            aliens    = AlienFactory.createWave(wave);
            velocidad     = Math.max(40, velocidad - 10); // se acelera cada oleada
        }
        //Para versiones posteriores, incrementar la cantidad de oleadas
        /*
        if (wave >= 3){ 
            notifyGameWon(wave, jugador.getPuntos());
            try { Thread.sleep(100); } catch (InterruptedException ignored) {}
            detener();
        }*/
    }

    private void notifyAlienMuerto(int id, int puntos, int jugadorPuntos) {
        observers.forEach(o -> o.onAlienKilled(id, puntos, jugadorPuntos));
    }

    private boolean overlapsJugador(Bala b) {
        int bx = b.getX(), by = b.getY();
        int cx = jugador.getCannonX();
        int cy = jugador.getCannonY();
        return bx >= cx && bx <= cx + 75 && by >= cy && by <= cy+60;
    }

    private boolean overlaps(Bala b, Alien a) {
    int bx = b.getX(), by = b.getY();
    int ax = a.getX() * 75, ay = a.getY() * 55;
    return bx >= ax && bx <= ax + 75 && by >= ay && by <= ay + 55;
}

    private boolean overlapsUFO(Bala b, UFO ufo) {
        int bx = b.getX(), by = b.getY();
        int ufox = ufo.getX() * 75, ufoy = ufo.getY() * 55;
        return bx >= ufox && bx <= ufox +110  && by >= ufoy && by <= ufoy + 55;
        }
    
private boolean overlapsBunker(Bala b, Bunker bunker) {
        int bx = b.getX(), by = b.getY();
        int bunkx = bunker.getX() * 75;
        int bunky = bunker.getY() * 55;

        return bx >= bunkx &&
               bx <= bunkx+120  &&
               by >= bunky &&
               by <= bunky+90;
    }
public synchronized void moverJugador(String direction) {
        if (direction.equals("LEFT"))  jugador.moverIzquierda();
        if (direction.equals("RIGHT")) jugador.moverDerecha();
    }
public synchronized void shoot() {
        int id = balas.size() + 1;
        balas.add(new Bala(id, jugador.getCannonX(), jugador.getCannonY(), "jugador"));
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
        observers.forEach(o-> o.onUfoKilled(ufoPts, jugadorPts));
    }
    private void notifyBunkerHit(int id, int health) {
        observers.forEach(o-> o.onBunkerHit(id, health));
    }

    private void notifyGameOver(String reason, int puntos) {
        observers.forEach(o -> o.onGameOver(reason, puntos));
    }
    
    /*private void notifyGameWon(int wave, int puntos) {
        observers.forEach(o->o.onGameWon(wave, puntos));
    }*/
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
        s.status     = running ? "RUNNING" : "FINISHED";
        return s;
    }  
private void triggerGameOver(String reason) {
    notifyGameOver(reason, jugador.getPuntos());
    try { Thread.sleep(100); } catch (InterruptedException ignored) {} 
    detener();
}
public GameState getLastState() { return lastState; }
    
public synchronized void detener() {
    this.running = false;
    if (balasThread != null) balasThread.interrupt();
}

public boolean isRunning(){
    return running;
}

    
}
