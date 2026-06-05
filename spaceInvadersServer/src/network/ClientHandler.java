
package network;
import org.json.JSONObject;
import engine.GameEngine;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.Map;
import model.GameState;
import patrones.JuegoObserver;

public class ClientHandler extends Thread implements JuegoObserver{
    
    private Socket socket;
    private PrintWriter out;
    private BufferedReader in;
    private GameEngine engine;
    private String role;
    private String jugadorID;
    private Map<String, GameEngine> engines;
    
    public ClientHandler(Socket socket, Map<String, GameEngine> engines) {
        this.socket  = socket;
        this.engines = engines;
    }
    @Override
    public void run(){
        try{
            out = new PrintWriter(socket.getOutputStream(), true);
            in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            
            String joinMsg = in.readLine();
            handleJoin(joinMsg);
            
            //loop de mensajes
            String msg;
            while ((msg = in.readLine())!=null){
                if(role.equals("JUGADOR")) handleJugadorMessage(msg);
            
            }
        }catch (IOException e){
                    System.out.println("Cliente desconectado: " + jugadorID);}
            finally{
                    if (engine != null) engine.detener(); engine.removeObserver((JuegoObserver) this);
                        try {socket.close();}catch (IOException ignored){}
                        }
    
    
    
    }
    private void handleJoin(String json) {
        // parsear con org.json o gson
        JSONObject obj = new JSONObject(json);
        this.role = obj.getString("role");
        System.out.println(this.role);
        
        if (role.equals("JUGADOR")) {
            this.jugadorID = obj.getString("jugador_id");
            System.out.println(this.jugadorID);
            this.engine   = new GameEngine(jugadorID);
            engines.put(jugadorID, engine);
            engine.addObserver(this);
            engine.start();

            send(String.format(
                "{\"type\":\"JOIN_ACK\",\"role\":\"JUGADOR\",\"jugador_id\":\"%s\",\"cannon_x\":400}",
                jugadorID
            ));

        } else if (role.equals("SPECTATOR")) {
            String watchPlayer = obj.getString("watch_player");
            GameEngine target  = engines.get(watchPlayer);

            if (target == null) {
                send("{\"type\":\"ERROR\",\"message\":\"Partida no encontrada\"}");
            } else {
                this.engine = target;
                target.addObserver(this);
                send(String.format(
                    "{\"type\":\"JOIN_ACK\",\"role\":\"SPECTATOR\",\"watching\":\"%s\"}",
                    watchPlayer
                ));
            }
        }
    }
    private void handleJugadorMessage(String json) {
        JSONObject obj = new JSONObject(json);
        String type    = obj.getString("type");
        System.out.println("Mensaje cliente: "+type);
        switch (type) {
            case "MOVE"  -> engine.moverJugador(obj.getString("direction"));
            case "SHOOT" -> engine.shoot();
        }
    }

    @Override
    public void onGameStateChanged(GameState state) {
        send(JsonSerializer.toGameStateJson(state));
    }

    @Override
    public void onAlienKilled(int alienId, int points, int newScore) {
        send(String.format(
            "{\"type\":\"ALIEN_KILLED\",\"alien_id\":%d,\"points\":%d,\"new_score\":%d}",
            alienId, points, newScore
        ));
    }

    @Override
    public void onPlayerHit(int livesRemaining, int cannonX) {
        send(String.format(
            "{\"type\":\"PLAYER_HIT\",\"lives_remaining\":%d,\"cannon_x\":%d}",
            livesRemaining, cannonX
        ));
    }

    @Override
    public void onBunkerHit(int bunkerId, int health) {
        send(String.format("{\"type\":\"BUNKER_HIT\",\"bunker_id\":%d,\"health\":%d}", bunkerId, health));
    }

    @Override
    public void onGameOver(String reason, int finalScore) {
        send(String.format(
            "{\"type\":\"GAME_OVER\",\"reason\":\"%s\",\"final_score\":%d}",
            reason, finalScore
        ));
    }

    @Override
    public void onGameWon(int wave, int finalScore) {
        send(String.format(
            "{\"type\":\"GAME_WON\",\"wave\":%d,\"final_score\":%d}",
            wave, finalScore
        ));
    }

    @Override
    public void onWaveStart(int wave, int alienCount, int alienSpeed) {
        send(String.format(
                "{\"type\":\"WAVE__START\",\"wave\":%d,\"alien_count\":%d,\"alienspeed\":%d}",
                wave, alienCount, alienSpeed
        ));
    }

    @Override
    public void onWaveClear(int wave) {
        send(String.format("{\"type\":\"WAVE_CLEAR\",\"wave\":%d}", wave));
    }

    

    @Override
    public void onUfoKilled(int points, int newScore) {
        send(String.format(
                "{\"type\":\"UFO_KILLED\",\"points\":%d,\"new_score\":%d}",
                points, newScore
        ));
    }
    
    private synchronized void send(String json) {
        if (out != null) out.println(json);
    }
    
    
    
}
