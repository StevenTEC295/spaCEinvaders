package spaceinvadersserver;

import java.io.IOException;
import network.GameServer;

public class SpaceInvadersServer {

    public static void main(String[] args) throws IOException {
        int port = args.length > 0 ? Integer.parseInt(args[0]) : 8080;

        GameServer server = new GameServer(port);

        // consola admin en hilo separado, con referencia al mapa de engines
        /*AdminConsole admin = new AdminConsole(server.getEngines());
        admin.setDaemon(true); // se cierra cuando cierra el servidor
        admin.start();*/

        // bloquea aquí aceptando conexiones
        server.start();
    }
    
}
