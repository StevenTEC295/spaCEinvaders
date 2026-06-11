
package model;

import java.util.List;


public class GameState {
    public Jugador        jugador;
    public List<Alien>   aliens;
    public List<Bala>  balas;
    public List<Bunker>  bunkers;
    public UFO           ufo;
    public int           wave;
    public int           alienSpeed;
    public String        status; // RUNNING, PAUSED, GAME_OVER, WON
}