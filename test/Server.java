import java.io.*;
import java.net.*;
import java.security.*;

/** Server thread to test connections. */

public class Server {

  private static int port=4444, maxConnections=0;

  // Listen for incoming connections and handle them

  public static void main(String[] args) {
    int i=0;

    try{
      ServerSocket listener = new ServerSocket(port);
      Socket server;

      while((i++ < maxConnections) || (maxConnections == 0)){
        doComms connection;

        server = listener.accept();
        doComms conn_c= new doComms(server);
        Thread t = new Thread(conn_c);
        t.start();
      }
    } catch (IOException ioe) {
      System.out.println("IOException on socket listen: " + ioe);
      ioe.printStackTrace();
    }
  }

}

class doComms implements Runnable {
    private Socket server;
    private String line,input;

    doComms(Socket server) {
      this.server=server;
    }

    public void run () {

      input="";

      try {
        // Get input from the client
        DataInputStream in = new DataInputStream (server.getInputStream());
        PrintStream out = new PrintStream(server.getOutputStream());

        while((line = in.readLine()) != null && !line.equals(".")) {
          input=input + line;
          out.println("Server Recieved:" + line);
        }

        // Now write to the client

        System.out.println("Server Recieved:" + input);
        out.println("Server Recieved:" + input);

        server.close();
      } catch (IOException ioe) {
        System.out.println("IOException on socket listen: " + ioe);
        ioe.printStackTrace();
      }
    }
}
