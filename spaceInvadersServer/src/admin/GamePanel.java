
package admin;


import engine.GameEngine;
import javax.swing.*;
import java.awt.*;
import model.Alien;
import model.GameState;
import patrones.AlienFactory;

public class GamePanel extends JPanel {

    private final String     playerId;
    private final GameEngine engine;
    private volatile boolean activo = true;

    private JLabel  lblStatus;
    private JLabel  lblScore;
    private JLabel  lblLives;
    private JLabel  lblWave;
    private JLabel  lblAliens;



    private JSlider  sliderSpeed;


    private JComboBox<String> cbType;
    private JTextField        tfPoints;
    private JTextField        tfX;
    private JTextField        tfY;

    public GamePanel(String playerId, GameEngine engine) {
        this.playerId = playerId;
        this.engine   = engine;

        setLayout(new BorderLayout(8, 8));
        setBorder(BorderFactory.createTitledBorder(
            BorderFactory.createLineBorder(Color.GRAY),
            "Partida: " + playerId,
            0, 0, new Font("Arial", Font.BOLD, 13)
        ));

        add(buildStatusPanel(),  BorderLayout.NORTH);
        add(buildControlPanel(), BorderLayout.CENTER);
        add(buildAlienPanel(),   BorderLayout.SOUTH);


        iniciarListenerEstado();
    }
    public String getPlayerId() { return playerId; }

    private void iniciarListenerEstado() {
        Thread listener = new Thread(() -> {
            while (activo) {
                GameState state = engine.getLastState();
                if (state != null) {
                    // capturar valores antes de entrar al EDT
                    String status  = state.status;
                    int    score   = state.jugador.getPuntos();
                    int    vidas   = state.jugador.getVidas();
                    int    wave    = state.wave;
                    long   alive   = state.aliens.stream()
                                        .filter(Alien::isVivo).count();
                    SwingUtilities.invokeLater(() -> {
                        lblStatus.setText("Estado: " + status);
                        lblScore.setText("Score: "   + score);
                        lblLives.setText("Vidas: "   + vidas);
                        lblWave.setText("Wave: "     + wave);
                        lblAliens.setText("Aliens: " + alive);

                        Color statusColor = switch (status) {
                            case "RUNNING"  -> new Color(0, 150, 0);
                            case "PAUSED"   -> new Color(200, 130, 0);
                            case "FINISHED" -> Color.RED;
                            default         -> Color.DARK_GRAY;
                        };
                        lblStatus.setForeground(statusColor);
                    });
                }

                try { Thread.sleep(200); }
                catch (InterruptedException e) { Thread.currentThread().interrupt(); break; }
            }
        });

        listener.setDaemon(true);
        listener.setName("listener-" + playerId);
        listener.start();
    }

    
    public void detener() { activo = false; }


    
    private JPanel buildStatusPanel() {
        JPanel p = new JPanel(new GridLayout(1, 5, 8, 0));
        p.setBorder(BorderFactory.createEmptyBorder(4, 4, 4, 4));

        lblStatus = makeLabel("Estado: -");
        lblScore  = makeLabel("Score: 0");
        lblLives  = makeLabel("Vidas: 3");
        lblWave   = makeLabel("Wave: 1");
        lblAliens = makeLabel("Aliens: 55");

        p.add(lblStatus);
        p.add(lblScore);
        p.add(lblLives);
        p.add(lblWave);
        p.add(lblAliens);
        return p;
    }

    
    private JPanel buildControlPanel() {
    JPanel p = new JPanel(new FlowLayout(FlowLayout.LEFT, 8, 4));

    
    JLabel lblSpeed = new JLabel("Velocidad:");
    sliderSpeed = new JSlider(40, 300, 120);
    sliderSpeed.setMajorTickSpacing(60);
    sliderSpeed.setPaintTicks(true);
    sliderSpeed.setPreferredSize(new Dimension(160, 40));

    JLabel lblSpeedVal = new JLabel("120ms");
    sliderSpeed.addChangeListener(e -> {
        int val = sliderSpeed.getValue();
        lblSpeedVal.setText(val + "ms");
        engine.setSpeed(val);
    });

    JButton btnBunker = new JButton("Restaurar bunkers");
    btnBunker.addActionListener(e -> engine.setBunkerHealth(100));

    p.add(lblSpeed);
    p.add(sliderSpeed);
    p.add(lblSpeedVal);
    p.add(Box.createHorizontalStrut(16));
    p.add(btnBunker);
    return p;
}

    
    private JPanel buildAlienPanel() {
        JPanel p = new JPanel(new FlowLayout(FlowLayout.LEFT, 8, 4));
        p.setBorder(BorderFactory.createTitledBorder("Crear alien"));

        cbType   = new JComboBox<>(new String[]{"calamar", "cangrejo", "pulpo"});
        tfPoints = new JTextField("10", 4);
        tfX      = new JTextField("5",  3);
        tfY      = new JTextField("0",  3);

        JButton btnCreate = new JButton("Crear");
        btnCreate.addActionListener(e -> createAlien());

        p.add(new JLabel("Tipo:"));    p.add(cbType);
        p.add(new JLabel("Puntos:"));  p.add(tfPoints);
        p.add(new JLabel("X:"));       p.add(tfX);
        p.add(new JLabel("Y:"));       p.add(tfY);
        p.add(btnCreate);
        return p;
    }

    private void createAlien() {
        try {
            String type   = (String) cbType.getSelectedItem();
            int    points = Integer.parseInt(tfPoints.getText().trim());
            int    x      = Integer.parseInt(tfX.getText().trim());
            int    y      = Integer.parseInt(tfY.getText().trim());
            int    id     = (int)(Math.random() * 9000) + 1000;

            Alien alien = AlienFactory.create(type, id, x, y, points);
            engine.addAlien(alien);

            JOptionPane.showMessageDialog(this,
                "Alien creado: " + type + " (" + points + " pts) en (" + x + "," + y + ")",
                "Éxito", JOptionPane.INFORMATION_MESSAGE);

        } catch (NumberFormatException ex) {
            JOptionPane.showMessageDialog(this,
                "Verificar que puntos, X e Y sean números enteros.",
                "Error", JOptionPane.ERROR_MESSAGE);
        }
    }

    private void refreshStatus() {
        GameState state;
        state = engine.getLastState();
        if (state == null) return;

        lblStatus.setText("Estado: " + state.status);
        lblScore.setText("Score: "  + state.jugador.getPuntos());
        lblLives.setText("Vidas: "  + state.jugador.getVidas());
        lblWave.setText("Wave: "    + state.wave);

        long alive = state.aliens.stream().filter(Alien::isVivo).count();
        lblAliens.setText("Aliens: " + alive);

        // color según estado
        Color statusColor = switch (state.status) {
            case "RUNNING"   -> new Color(0, 150, 0);
            case "PAUSED"    -> new Color(200, 130, 0);
            case "GAME_OVER" -> Color.RED;
            default          -> Color.DARK_GRAY;
        };
        lblStatus.setForeground(statusColor);
    }

    private JLabel makeLabel(String text) {
        JLabel l = new JLabel(text);
        l.setFont(new Font("Monospaced", Font.PLAIN, 12));
        return l;
    }
}