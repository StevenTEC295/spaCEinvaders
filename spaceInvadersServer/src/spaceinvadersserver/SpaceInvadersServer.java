package spaceinvadersserver;

import admin.AdminWindow;
import java.io.IOException;
import javax.swing.SwingUtilities;
import network.GameServer;

public class SpaceInvadersServer {

    public static void main(String[] args) {
        int port = args.length > 0 ? Integer.parseInt(args[0]) : 8080;

        GameServer server = new GameServer(port);

        // pasar el mismo Map que usa el servidor
        SwingUtilities.invokeLater(() -> new AdminWindow(server.getEngines()));

        try {
            server.start();
        } catch (IOException e) {
            System.out.println("Error iniciando servidor: " + e.getMessage());
        }
    }
    
}
