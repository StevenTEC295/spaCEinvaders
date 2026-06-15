
package patrones;
import java.util.ArrayList;
import java.util.List;
import model.Alien; 
import model.Calamar;
import model.Cangrejo;
import model.Pulpo;



public class AlienFactory {

    public static Alien create(String type, int id, int x, int y, int points) {
        return switch (type.toLowerCase()) {
            case "calamar"   -> new Calamar(id, x, y, points);
            case "cangrejo"    -> new Cangrejo(id, x, y, points);
            case "pulpo" -> new Pulpo(id, x, y, points);
            default -> throw new IllegalArgumentException("Tipo de alien desconocido: " + type);
        };
    }

    // Crea la formación inicial completa de una oleada
    public static List<Alien> createWave(int wave) {
        List<Alien> aliens = new ArrayList<>();
        int id = 1;
        int[][] formacion = {
            {40},  
            {20},
            {10}
        };
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 5; col++) {
                int puntos = formacion[row][0] + (wave - 1) * 5; 
                String type = row == 0 ? "pulpo" : (row < 3 ? "cangrejo" : "calamar");
                aliens.add(create(type, id++, col, row, puntos));
            }
        }
        return aliens;
    }
}
