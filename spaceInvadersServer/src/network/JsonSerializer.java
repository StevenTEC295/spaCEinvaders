package network;

// network/JsonSerializer.java

import java.util.List;
import java.util.stream.Collectors;
import model.Alien;
import model.Bala;
import model.GameState;

public class JsonSerializer {

    public static String toGameStateJson(GameState s) {
        StringBuilder sb = new StringBuilder();
        sb.append("{\"type\":\"GAME_STATE\",");

        // jugador
        sb.append(String.format(
            "\"jugador\":{\"jugadorID\":\"%s\",\"cannon_x\":%d,\"vidas\":%d,\"score\":%d},",
            s.jugador.getJugadorId(), s.jugador.getCannonX(),
            s.jugador.getVidas(),    s.jugador.getPuntos()
        ));

        // aliens
        sb.append("\"aliens\":[");
        List<String> alienList = s.aliens.stream()
            .filter(Alien::isVivo)
            .map(a -> String.format(
                "{\"id\":%d,\"x\":%d,\"y\":%d,\"type\":\"%s\",\"points\":%d}",
                a.getId(), a.getX(), a.getY(), a.getType(), a.getPuntos()
            ))
            .collect(Collectors.toList());
        sb.append(String.join(",", alienList)).append("],");

        // balas
        sb.append("\"balas\":[");
        List<String> bulletList = s.balas.stream()
            .filter(Bala::isActivo)
            .map(b -> String.format(
                "{\"id\":%d,\"x\":%d,\"y\":%d,\"owner\":\"%s\"}",
                b.getId(), b.getX(), b.getY(), b.getOwner()
            ))
            .collect(Collectors.toList());
        sb.append(String.join(",", bulletList)).append("],");
        // ufo
        if (s.ufo != null){
            sb.append(String.format(
            "\"ufo\":{\"activo\":\"%s\",\"x\":%d,\"points\":%d},",
            s.ufo.isVivo(), s.ufo.getX(), s.ufo.getPuntos()
        ));
        }else{sb.append(String.format("\"ufo\":{},"));}
        
        // bunkers
        sb.append("\"bunkers\":[");
        List<String> bunkerList = s.bunkers.stream()
            .map(b -> String.format(
                "{\"id\":%d,\"x\":%d,\"health\":%d}",
                b.getID(), b.getX(), b.getHealth()
            ))
            .collect(Collectors.toList());
        sb.append(String.join(",", bunkerList)).append("],");

        // metadata
        sb.append(String.format(
            "\"wave\":%d,\"alien_speed\":%d,\"game_status\":\"%s\"}",
            s.wave, s.alienSpeed, s.status
        ));

        return sb.toString();
    }
}
