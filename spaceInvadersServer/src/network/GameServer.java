
package network;



import engine.GameEngine;
import java.io.IOException;
import java.net.Inet4Address;
import java.net.NetworkInterface;
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
        NetworkInterface.getNetworkInterfaces().asIterator().forEachRemaining(ni -> {
        ni.getInetAddresses().asIterator().forEachRemaining(addr -> {
            if (!addr.isLoopbackAddress() && addr instanceof Inet4Address) {
                System.out.println("IP para conectarse: " + addr.getHostAddress());
            }
        });
    });

        while (true) {
            Socket client = serverSocket.accept();
            System.out.println("Nueva conexión desde " + client.getInetAddress());
            ClientHandler handler = new ClientHandler(client, engines);
            handler.start();
        }
    }

    public Map<String, GameEngine> getEngines() { return engines; }
}
