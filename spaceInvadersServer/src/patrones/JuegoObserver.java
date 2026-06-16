
package patrones;
import model.GameState;

public interface JuegoObserver {
    void onGameStateChanged(GameState state);
    void onAlienKilled(int alienId, int points, int newScore);
    void onPlayerHit(int livesRemaining, int cannonX);
    void onBunkerHit(int bunkerId, int health);
    void onGameOver(String reason, int finalScore);
    void onGameWon(int wave, int finalScore);
    void onWaveStart(int wave, int alienCount, int alienSpeed);
    void onWaveClear(int wave);
    void onUfoKilled(int points, int newScore);
}
