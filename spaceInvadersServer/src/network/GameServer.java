
package network;

// network/GameServer.java

import engine.GameEngine;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

public class GameServer {

    private final int port;
    private final Map<String, GameEngine> engines;

    public GameServer(int port) {
        this.port    = port;
        this.engines = new ConcurrentHashMap<>();
    }

    public void start() throws IOException {
        ServerSocket serverSocket = new ServerSocket(port);
        System.out.println("Servidor escuchando en puerto " + port);

        while (true) {
            Socket client = serverSocket.accept();
            System.out.println("Nueva conexión desde " + client.getInetAddress());
            ClientHandler handler = new ClientHandler(client, engines);
            handler.start();
        }
    }

    public Map<String, GameEngine> getEngines() { return engines; }
}
