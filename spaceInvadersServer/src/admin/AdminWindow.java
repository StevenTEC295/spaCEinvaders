package admin;

import engine.GameEngine;
import javax.swing.*;
import java.awt.*;
import java.util.Map;
import java.util.Arrays;

public class AdminWindow extends JFrame {

    private final Map<String, GameEngine> engines;
    private final JPanel contentPanel;
    private final JLabel lblConnected;

    public AdminWindow(Map<String, GameEngine> engines) {
        this.engines = engines;

        setTitle("Space Invaders — Admin");
        setSize(900, 600);
        setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
        setLayout(new BorderLayout(8, 8));

        
        JPanel topBar = new JPanel(new FlowLayout(FlowLayout.LEFT));
        topBar.setBackground(new Color(30, 56, 100));
        JLabel title = new JLabel("Space Invaders · Panel de Administración");
        title.setForeground(Color.WHITE);
        title.setFont(new Font("Arial", Font.BOLD, 15));
        lblConnected = new JLabel("Partidas activas: 0");
        lblConnected.setForeground(new Color(180, 220, 255));
        lblConnected.setFont(new Font("Arial", Font.PLAIN, 13));
        topBar.add(title);
        topBar.add(Box.createHorizontalStrut(20));
        topBar.add(lblConnected);
        add(topBar, BorderLayout.NORTH);

        
        contentPanel = new JPanel();
        contentPanel.setLayout(new BoxLayout(contentPanel, BoxLayout.Y_AXIS));
        contentPanel.setBorder(BorderFactory.createEmptyBorder(8, 8, 8, 8));
        JScrollPane scroll = new JScrollPane(contentPanel);
        scroll.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED);
        add(scroll, BorderLayout.CENTER);

        
        new Timer(500, e -> checkNuevosJugadores()).start();

        setLocationRelativeTo(null);
        setVisible(true);
    }
    
    private void checkNuevosJugadores() {
    SwingUtilities.invokeLater(() -> {
        engines.forEach((jugadorId, engine) -> {
            boolean yaExiste = false;
            for (Component c : contentPanel.getComponents()) {
                if (c instanceof GamePanel gp && gp.getPlayerId().equals(jugadorId)) {
                    yaExiste = true;
                    break;
                }
            }
            if (!yaExiste && engine.isRunning()) {   // solo crear si está activo
                GamePanel panel = new GamePanel(jugadorId, engine);
                panel.setMaximumSize(new Dimension(Integer.MAX_VALUE, 220));
                contentPanel.add(panel);
                contentPanel.add(Box.createVerticalStrut(8));
                contentPanel.revalidate();
                contentPanel.repaint();
            }
        });

        // Contar solo paneles vivos (GamePanel presentes en el layout)
        long activos = Arrays.stream(contentPanel.getComponents())
                             .filter(c -> c instanceof GamePanel)
                             .count();
        lblConnected.setText("Partidas activas: " + activos);
    });
}
    
}